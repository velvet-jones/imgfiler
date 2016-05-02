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
#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"
#include "args.h"
#include "hash.h"
#include "extract.h"
#include "exif.h"

counters_t counters;
const args_t* args = 0;

int main (int argc, char **argv)
{
  umask (022);

  memset (&counters,0,sizeof(counters_t));

  args = get_args (argc,argv);

  set_sig_handlers();

  if (!init_extractor())
  {
    fprintf (stderr,"Failed to initialize the extractor.\n");
    exit (1);
  }

  if (args->src_is_file == 1)
    map_and_process_file (args->src_dir);
  else
    process_dir (args->src_dir);

  close_extractor();

  if (args->verbose)
  {
    printf ("Total files: %ld\n",counters.total_files);
    printf ("Dateless: %ld\n",counters.missing_date);
    printf ("Duplicates: %ld\n",counters.duplicates);
    printf ("Skipped: %ld\n",counters.skipped);
    printf ("Total dirs: %ld\n",counters.total_dirs);
  }
  return 0;
}

void perform_delete_src(file_t* src_file, const char* dst_fqpn)
{
  file_t* dst_file = 0;

  switch (args->operation)
  {
    case OPERATION_NOP: // simply suggest what we would do
      printf ("Suggest delete %s, duplicate of %s\n",src_file->fqpn,dst_fqpn);
    break;

    case OPERATION_MOVE:
      if (args->verbose)
        printf ("Delete %s, duplicate of %s\n",src_file->fqpn,dst_fqpn);

      dst_file = map_file (dst_fqpn);
      if (!dst_file)
      {
        fprintf (stderr,"Failed to open file %s: %s.\n",dst_fqpn,strerror (errno));
        return;
      }

      if (!compute_hash(args,dst_file))
      {
        fprintf (stderr,"Failed to compute hash for %s: %s.\n",dst_file->fqpn,strerror (errno));
        return;
      }

      if (strcmp (src_file->hash,dst_file->hash) != 0)
      {
        fprintf (stderr,"Refusing to delete: %s != %s!\n",src_file->fqpn,dst_file->fqpn);
        return;
      }
      if (unlink (src_file->fqpn) != 0)
        fprintf (stderr,"Failed to delete %s: %s.\n",src_file->fqpn,strerror (errno));
    break;
  }
}

// handles dateless, duplicates and normal destination
void perform_move_src(file_t* src_file, const char* dst_dir)
{
  char dst_fqpn[PATH_MAX];

  switch (args->operation)
  {
    case OPERATION_NOP: // simply suggest what we would do
      printf ("Suggest move %s -> %s/%s\n",src_file->fqpn,dst_dir,src_file->hash);
    break;

    case OPERATION_MOVE:
      if (!create_directory_if (dst_dir))
      {
        fprintf (stderr,"Failed to create directory %s: %s.\n",dst_dir,strerror (errno));
        return;
      }
      snprintf (dst_fqpn,PATH_MAX,"%s/%s",dst_dir,src_file->hash);

      if (args->verbose)
        printf ("Move %s -> %s\n",src_file->fqpn,dst_fqpn);

      // fails gracefully if the two are not on the same file system
      if (rename (src_file->fqpn,dst_fqpn) != 0)
        fprintf (stderr,"Failed to move %s -> %s: %s.\n",src_file->fqpn,dst_fqpn,strerror (errno));
    break;
  }
}

bool format_dst (const char* base_dir, const date_t* date, const char* dst_name, char* dst_dir, char* dst_fqpn)
{
  memset (dst_dir,0,PATH_MAX);

  int ret = snprintf (dst_dir,PATH_MAX-1,"%s/%04u/%02u-%02u",base_dir,date->tm.tm_year+1900,date->tm.tm_mon+1,date->tm.tm_mday);
  if (ret < 0 || ret >= PATH_MAX-1)
    return false;  // failed to properly format

  ret = snprintf (dst_fqpn,PATH_MAX-1,"%s/%s",dst_dir,dst_name);
  if (ret < 0 || ret >= PATH_MAX-1)
    return false;  // failed to properly format

  return true;
}

void map_and_process_file(const char* path)
{
  // map the file
  file_t* src_file = map_file (path);
  if (!src_file)
  {
    fprintf (stderr,"Failed to open file %s: %s.\n",path,strerror (errno));
    return;
  }

  process_file (src_file);
  unmap_file (src_file);
}

void process_file (file_t* src_file)
{
  // compute the hash of the source file
  if (!compute_hash(args,src_file))
  {
    fprintf (stderr,"Failed to compute hash for %s: %s.\n",src_file->fqpn,strerror (errno));
    return;
  }

  // now attempt to get 'date' metadata from the file
  date_t date;
  if (!exif_date (src_file,&date) && !extract_date (src_file->fqpn,&date))
  {
    // no date; send this file to the dateless dir
    if (*args->dateless_dir == 0)
    {
      if (args->verbose)
        printf ("Skipping dateless file %s; no dateless directory specified.\n",src_file->fqpn);
      return;
    }
    perform_move_src (src_file,args->dateless_dir);
    counters.missing_date++;
    return;
  }

  // format a destination dir and fqpn
  char dst_dir[PATH_MAX];
  char dst_fqpn[PATH_MAX];
  if (!format_dst (args->dst_dir,&date,src_file->hash,dst_dir,dst_fqpn))
  {
    fprintf (stderr,"Failed to format destination directory for file %s: %s.\n",src_file->fqpn,strerror (errno));
    counters.skipped++;  // probably the resulting path is too long
    return;
  }

  struct stat st_dst;
  int ret = stat(dst_fqpn,&st_dst);
  if (ret == 0) // dst file exists already
  {
    if (!same_file (&src_file->st,&st_dst))
    {
      counters.duplicates++;
      if (*args->dup_dir == 0)
        perform_delete_src (src_file,dst_fqpn);
      else
        perform_move_src(src_file,args->dup_dir);  // send file to dup dir; what if it already exists here? has diff content?
    }
    else
    {
      if (args->verbose)
        printf ("Nothing to do for %s.\n",src_file->fqpn);
    }
  }
  else
  {
    int err = errno;
    if (err == ENOENT || err == ENOTDIR)
    {
      // destination file doesn't exist, move the source file to the destination file
      perform_move_src (src_file,dst_dir);
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
      {
        map_and_process_file (path);
      }
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
