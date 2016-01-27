/* Copyright (C) Bud Millwood, 2016.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"
#include "args.h"
#include "hash.h"
#include "extract.h"
#include "exif.h"

counters_t counters;
const args_t* args = 0;

int main (int argc, char **argv)
{
  memset (&counters,0,sizeof(counters_t));

  args = get_args (argc,argv);

  set_sig_handlers();

  if (!init_extractor())
  {
    fprintf (stderr,"Failed to initialize the extractor.\n");
    exit (1);
  }

  process_dir (args->src_dir);
  close_extractor();

  if (args->verbose)
  {
    printf ("Total files: %ld\n",counters.total_files);
    printf ("Dateless: %ld\n",counters.missing_date);
    printf ("Duplicates: %ld\n",counters.duplicates);
    printf ("Total dirs: %ld\n",counters.total_dirs);
  }
  return 0;
}

void perform_delete_src(const char* src_fqpn, const char* src_sha, const char* dst_fqpn)
{
  char dst_sha[PATH_MAX];

  switch (args->operation)
  {
    case OPERATION_NOP: // simply suggest what we would do
      printf ("Suggest delete %s, duplicate of %s\n",src_fqpn,dst_fqpn);
    break;

    case OPERATION_MOVE:
      if (args->verbose)
        printf ("Delete %s, duplicate of %s\n",src_fqpn,dst_fqpn);

      if (!compute_hash(args,dst_fqpn,dst_sha,sizeof(dst_sha)))
      {
        fprintf (stderr,"Failed to compute hash for %s: %s.\n",dst_fqpn,strerror (errno));
        return;
      }

      if (strcmp (src_sha,dst_sha) != 0)
      {
        fprintf (stderr,"Refusing to delete: %s != %s!\n",src_fqpn,dst_fqpn);
        return;
      }
      if (unlink (src_fqpn) != 0)
        fprintf (stderr,"Failed to delete %s: %s.\n",src_fqpn,strerror (errno));
    break;
  }
}

// handles dateless, duplicates and normal destination
void perform_move_src(const char* src_fqpn, const char* dst_dir, const char* dst_name)
{
  char dst_fqpn[PATH_MAX];

  switch (args->operation)
  {
    case OPERATION_NOP: // simply suggest what we would do
      printf ("Suggest move %s -> %s/%s\n",src_fqpn,dst_dir,dst_name);
    break;

    case OPERATION_MOVE:
      if (!create_directory_if (dst_dir))
      {
        fprintf (stderr,"Failed to create directory %s: %s.\n",dst_dir,strerror (errno));
        return;
      }
      snprintf (dst_fqpn,PATH_MAX,"%s/%s",dst_dir,dst_name);

      if (args->verbose)
        printf ("Move %s -> %s\n",src_fqpn,dst_fqpn);

      // fails gracefully if the two are not on the same file system
      if (rename (src_fqpn,dst_fqpn) != 0)
        fprintf (stderr,"Failed to move %s -> %s: %s.\n",src_fqpn,dst_fqpn,strerror (errno));
    break;
  }
}

bool format_dst (const char* base_dir, const date_t* date, const char* dst_name, char* dst_dir, char* dst_fqpn)
{
  memset (dst_dir,0,PATH_MAX);

  snprintf (dst_dir,PATH_MAX-1,"%s/%04u/%02u-%02u",base_dir,date->tm.tm_year+1900,date->tm.tm_mon+1,date->tm.tm_mday);
  snprintf (dst_fqpn,PATH_MAX-1,"%s/%s",dst_dir,dst_name);
  return true;
}

void process_file (const char* src_dir, const char* src_name, const char* src_fqpn)
{
//  if (args->verbose)
//    printf ("Processing file %s.\n",src_fqpn);

  // first compute the hash of the source file
  char dst_name[PATH_MAX];
  if (!compute_hash(args,src_fqpn,dst_name,sizeof(dst_name)))
  {
    fprintf (stderr,"Failed to compute hash for %s: %s.\n",src_fqpn,strerror (errno));
    return;
  }

  // now attempt to get 'date' metadata from the file
  date_t date;
  if (!extract_date (src_fqpn,&date) && !exif_date (src_fqpn,&date))
  {
    // no date; send this file to the dateless dir
    if (*args->dateless_dir == 0)
    {
      if (args->verbose)
        printf ("Skipping dateless file %s; no dateless directory specified.\n",src_fqpn);
      return;
    }
    perform_move_src (src_fqpn,args->dateless_dir,dst_name);
    counters.missing_date++;
    return;
  }

  // format a destination dir and fqpn
  char dst_dir[PATH_MAX];
  char dst_fqpn[PATH_MAX];
  if (!format_dst (args->dst_dir,&date,dst_name,dst_dir,dst_fqpn))
  {
    if (*args->dateless_dir == 0)
    {
      if (args->verbose)
        printf ("Skipping dateless file %s; no dateless directory specified.\n",src_fqpn);
      return;
    }
    // no valid date; send this file to the dateless dir
    perform_move_src (src_fqpn,args->dateless_dir,dst_name);
    counters.missing_date++;  // date format is incorrect
    return;
  }

  struct stat st_src;
  int ret = stat(src_fqpn,&st_src);
  if (ret != 0)
  {
    fprintf (stderr,"Failed to stat source file %s: %s.\n",src_fqpn,strerror (errno));
    return;
  }

  struct stat st_dst;
  ret = stat(dst_fqpn,&st_dst);
  if (ret == 0) // dst file exists already
  {
    if (!same_file (&st_src,&st_dst))
    {
      if (*args->dup_dir == 0)
        perform_delete_src (src_fqpn,dst_name,dst_fqpn);
      else
        perform_move_src(src_fqpn,args->dup_dir,dst_name);  // send file to dup dir; what if it already exists here? has diff content?
    }
    else
    {
      if (args->verbose)
        printf ("Nothing to do for %s.\n",src_fqpn);
    }
  }
  else
  {
    int err = errno;
    if (err == ENOENT || err == ENOTDIR)
    {
      // destination file doesn't exist, move the source file to the destination file
      perform_move_src (src_fqpn,dst_dir,dst_name);
    }
    else
    {
      // this error means that a real error occurred, not just "the file doesn't exist"
      fprintf (stderr,"Failed to stat destination file %s: %s.\n",dst_fqpn,strerror (errno));
    }
  }
}

void process_dir (const char* dir)
{
  DIR* d;
  d = opendir (dir);

  if (!d)
  {
    fprintf (stderr, "Failed to open directory %s: %s.\n",dir,strerror (errno));
    exit (1);
  }

  counters.total_dirs++;

  while (1)
  {
    struct dirent* entry;
    const char* d_name;

    entry = readdir (d);
    if (!entry)
      break; // no more

    d_name = entry->d_name;
    int path_length;
    char path[PATH_MAX];
    path_length = snprintf (path, PATH_MAX,"%s/%s", dir, d_name);

    // if it's a directory...
    if (entry->d_type & DT_DIR)
    {
      if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0)
      {
        if (path_length >= PATH_MAX)
        {
          fprintf (stderr, "Path too long.\n");
          exit (1);
        }

        // recursively process this directory
        if (d_name[0] != '.')  // we ignore hidden directories
          process_dir (path);
        else
        {
          if (args->verbose)
            printf ("Skipping hidden directory %s.\n",path);
        }
      }
    }

    // if it's a regular file...
    if (entry->d_type & DT_REG)
    {
      counters.total_files++;
      if (d_name[0] != '.')  // we ignore hidden files
        process_file (dir,d_name,path);
      else
      {
        if (args->verbose)
          printf ("Skipping hidden file %s.\n",path);
      }
    }
  }

  if (closedir (d))
  {
    fprintf (stderr, "Failed to close %s: %s.\n", dir, strerror (errno));
    exit (1);
  }
}

void set_sig_handlers ()
{
  struct sigaction sa;
  memset (&sa,0,sizeof(sa));
  sa.sa_handler = SIG_IGN;

  // handle signals
  if (sigaction (SIGTTOU,&sa,0) < 0 ||   // allow the app writes to stdout while running in the background
      sigaction (SIGPIPE,&sa,0) < 0 ||   // libextractor requires this for its plugins
      sigaction (SIGHUP,&sa,0) < 0  ||   // nothing to do for this
      sigaction (SIGUSR1,&sa,0) < 0 ||   // .. or this
      sigaction (SIGUSR2,&sa,0) < 0)     // ..or this
  {
    fprintf (stderr, "Failed to set signal masks.\n");
    exit (1);
  }
}
