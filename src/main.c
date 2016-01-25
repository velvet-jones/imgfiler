/*
 * A utility for filing photos based on the photo's metadata.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "main.h"
#include "args.h"
#include "exif.h"
#include "sha.h"
#include "common.h"

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

  process_dir (args->src_dir);

  if (args->verbose)
  {
    printf ("Total files: %ld\n",counters.total_files);
    printf ("Total dirs: %ld\n",counters.total_dirs);
    printf ("With date: %ld\n",counters.total_files-counters.missing_exif_date);
    printf ("Without date: %ld\n",counters.missing_exif_date);
    printf ("Duplicates: %ld\n",counters.duplicates);
  }
  return 0;
}

// handles dateless, duplicates and normal destination
void process_operation(const char* src_fqpn, const char* dst_dir, const char* dst_name)
{
//  snprintf (dst_fqpn, PATH_MAX,"%s/%s", dir, dst_name);

  switch (args->operation)
  {
    case OPERATION_NOP:
      printf ("Suggest copy/move %s -> %s/%s\n",src_fqpn,dst_dir,dst_name);
    break;

    case OPERATION_COPY:
      printf ("Copy %s -> %s/%s\n",src_fqpn,dst_dir,dst_name);
    break;

    case OPERATION_MOVE:
      printf ("Move %s -> %s/%s\n",src_fqpn,dst_dir,dst_name);
    break;
  }
}

bool to_long (const char* s, long* l)
{
  char dummy = '\0';
  char* lastValid = &dummy;
  errno = 0;  // we must use errno, since we have to set it to 0 before calling strtoul
  *l = strtoul (s,&lastValid,10);

  if (!lastValid || *lastValid != 0 || errno != 0)
    return false;
  else
    return true;
}

bool format_dst (const char* base_dir, const char* date, const char* dst_name, char* dst_dir, char* dst_fqpn)
{
  memset (dst_dir,0,PATH_MAX);
  struct tm ft;

  // exif date format is 'YYYY:MM:DD HH:MM:SS'
  if (sscanf(date, "%d:%d:%d %d:%d:%d",&ft.tm_year,&ft.tm_mon,&ft.tm_mday,&ft.tm_hour,&ft.tm_min,&ft.tm_sec) != 6)
    return false;

  /* Uncomment the following to make the struct tm valid*/
//  ft.tm_isdst = -1;  // no dst data available
//  ft.tm_mon -= 1;    // zero-based months
//  ft.tm_year -= 1900;  // years are offset from 1900

  if (!ft.tm_year || !ft.tm_mon || !ft.tm_mday)
    return false;

  snprintf (dst_dir,PATH_MAX-1,"%s/%04u/%02u-%02u",base_dir,ft.tm_year,ft.tm_mon,ft.tm_mday);
  snprintf (dst_fqpn,PATH_MAX-1,"%s/%s",dst_dir,dst_name);
  return true;
}

void process_file (const char* src_dir, const char* src_name, const char* src_fqpn)
{
  // first compute the sha1 of the source file
  char dst_name[PATH_MAX];
  if (!compute_sha1(src_fqpn,dst_name))
    return;

  // now attempt to get an exif date
  char date[1024];
  memset (&date,0,sizeof(date));

  if (exif_date (src_fqpn,date,sizeof(date)))
  {
    // format a destination dir and fqpn
    char dst_dir[PATH_MAX];
    char dst_fqpn[PATH_MAX];
    if (!format_dst (args->dst_dir,date,dst_name,dst_dir,dst_fqpn))
    {
      fprintf (stderr,"Invalid date format '%s' in file %s.\n",date,src_fqpn);
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
  else
  {
    // no exif date; send this file to the dateless dir
    process_operation (src_fqpn,args->dateless_dir,dst_name);
    counters.missing_exif_date++;
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
        process_dir (path);
      }
    }

    // if it's a regular file...
    if (entry->d_type & DT_REG) {
      counters.total_files++;
      process_file (dir,d_name,path);
    }
  }

  if (closedir (d)) {
    fprintf (stderr, "Failed to close %s: %s.\n", dir, strerror (errno));
    exit (1);
  }
}
