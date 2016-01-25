/*
 * A utility for filing photos based on the photo's metadata.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include "args.h"
#include "exif.h"
#include "sha.h"
#include "common.h"

void process_dir (const char* dir);
void process_file (const char* dir, const char* name, const char* fqpn);

counters_t counters;

int main (int argc, char **argv)
{
  memset (&counters,0,sizeof(counters_t));

  const args_t* args = get_args (argc,argv);

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
    printf ("With exif: %ld\n",counters.total_files-counters.missing_exif);
    printf ("Without exif: %ld\n",counters.missing_exif);
    printf ("With date: %ld\n",counters.total_files-counters.missing_exif-counters.missing_date);
    printf ("Without date: %ld\n",counters.missing_date);
    printf ("Duplicates: %ld\n",counters.duplicates);
  }
  return 0;
}

void process_file (const char* dir, const char* name, const char* fqpn)
{
  char date[1024];
  memset (&date,0,sizeof(date));

  if (exif_date (fqpn,date))
  {
    char dst[PATH_MAX];
    memset (dst,0,sizeof(dst));

    // time_t t = convert_exif_date (buf);

    struct stat st_src;
    int ret = stat(fqpn,&st_src);
    if (ret != 0)
    {
      fprintf (stderr,"Failed to stat source file '%s': %s.\n",fqpn,strerror (errno));
      return;
    }

    char mds[SHA1_SBUF_LEN];
    if (compute_sha1(fqpn,mds))
      printf ("%s  %s\n",mds,fqpn);

    struct stat st_dst;
    ret = stat(dst,&st_dst);
    if (ret == 0) // dst file exists already - // FLAG: Does it have the same content??
      if (!same_file (&st_src,&st_dst))
      ;
//        process_duplicate();
      else
      ;
//        skip; // src=dst already
    else
    ;
//      copy_or_move (fqpn,dst); // dst does not exist, perform operation
  }
  else
    counters.missing_date++;
}

void process_dir (const char* dir)
{
  DIR* d;
  d = opendir (dir);

  if (!d) {
    fprintf (stderr, "Failed to open directory '%s': %s.\n",dir,strerror (errno));
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
    fprintf (stderr, "Failed to close '%s': %s.\n", dir, strerror (errno));
    exit (1);
  }
}
