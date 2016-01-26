/*
 * A utility for filing photos based on the photo's metadata.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>

#include "main.h"
#include "args.h"
#include "extract.h"
#include "sha.h"

counters_t counters;
const args_t* args = 0;

int main (int argc, char **argv)
{
  memset (&counters,0,sizeof(counters_t));

  args = get_args (argc,argv);

  // if no dateless_dir, we just leave the original file as is
  if (!*args->src_dir && !*args->dst_dir || !*args->dup_dir)
  {
    show_help(argv[0]);
    exit(1);
  }

  if (!validate_dir(args->src_dir))
  {
    fprintf (stderr,"Source directory '%s' does not exist.\n",args->src_dir);
    exit (1);
  }

  if (!validate_dir(args->dst_dir))
  {
    fprintf (stderr,"Destination directory '%s' does not exist.\n",args->dst_dir);
    exit (1);
  }

  if (!validate_dir(args->dup_dir))
  {
    fprintf (stderr,"Duplicates directory '%s' does not exist.\n",args->dup_dir);
    exit (1);
  }

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
    printf ("Total dirs: %ld\n",counters.total_dirs);
    printf ("With date: %ld\n",counters.total_files-counters.missing_date);
    printf ("Without date: %ld\n",counters.missing_date);
    printf ("Duplicates: %ld\n",counters.duplicates);
  }
  return 0;
}

// handles dateless, duplicates and normal destination
void process_operation(const char* src_fqpn, const char* dst_dir, const char* dst_name)
{
  char dst_fqpn[PATH_MAX];

  if (strlen(dst_dir) == 0)
  {
    if (args->verbose)
      printf ("Skipping dateless file %s; no dateless directory specified.\n",src_fqpn);
    return;  // do nothing if there is no destination directory defined (dateless can be empty)
  }

  switch (args->operation)
  {
    case OPERATION_NOP: // simply suggest what we would do
      printf ("Suggest move %s -> %s/%s\n",src_fqpn,dst_dir,dst_name);
    break;

    case OPERATION_MOVE:
      if (!create_directory_if (dst_dir))
      {
        fprintf (stderr,"Failed to create directory '%s': %s.\n",dst_dir,strerror (errno));
        return;
      }
      snprintf (dst_fqpn,PATH_MAX,"%s/%s",dst_dir,dst_name);
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
  // first compute the sha1 of the source file
  char dst_name[PATH_MAX];
  if (!compute_sha1(src_fqpn,dst_name))
    return;

  // now attempt to get 'date' metadata from the file
  date_t date;
  if (!extract_date (src_fqpn,&date))
  {
    // no date; send this file to the dateless dir
    process_operation (src_fqpn,args->dateless_dir,dst_name);
    counters.missing_date++;
    return;
  }

  // format a destination dir and fqpn
  char dst_dir[PATH_MAX];
  char dst_fqpn[PATH_MAX];
  if (!format_dst (args->dst_dir,&date,dst_name,dst_dir,dst_fqpn))
  {
    // no date; send this file to the dateless dir
    process_operation (src_fqpn,args->dateless_dir,dst_name);
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
  if (ret == 0) // dst file exists already - // FLAG: Does it have the same content??
  {
    if (!same_file (&st_src,&st_dst))
      process_operation(src_fqpn,args->dup_dir,dst_name);  // send file to dup dir
    else
    {
      if (args->verbose)
        printf ("Nothing to do for %s.\n",src_fqpn);
    }
  }
  else
  {
    // Could not stat dst, file does not exist
    process_operation (src_fqpn,dst_dir,dst_name); // send file to formatted dst_dir
  }
}

void process_dir (const char* dir)
{
  DIR* d;
  d = opendir (dir);

  if (!d) {
    fprintf (stderr, "Failed to open directory %s: %s.\n",dir,strerror (errno));
    exit (1);
  }

  counters.total_dirs++;

  while (1) {
    struct dirent* entry;
    const char* d_name;

    entry = readdir (d);
    if (!entry) {
      break; // no more
    }

    d_name = entry->d_name;
    int path_length;
    char path[PATH_MAX];
    path_length = snprintf (path, PATH_MAX,"%s/%s", dir, d_name);

    // if it's a directory...
    if (entry->d_type & DT_DIR) {
      if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) {
        if (path_length >= PATH_MAX) {
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
    if (entry->d_type & DT_REG) {
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

  if (closedir (d)) {
    fprintf (stderr, "Failed to close %s: %s.\n", dir, strerror (errno));
    exit (1);
  }
}
