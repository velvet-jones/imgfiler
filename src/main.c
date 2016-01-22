/*
 * A utility for filing photos based on the photo's metadata.
 *
 * 1. Get arguments:
 *     - source directory
 *     - destination directory/output format
 *     - duplicates directory (/dev/null to remove duplicates)
 *     - copy or move file?
 *     - verbose?
 * 2. Scan for image files
 * 3. Run sha checksum
 * 4. Copy or move to destination directory; if dest exists, move to duplicates directory
 * 5. Remove empty directories
 *
 * Notes:
 *      - Keep sequence numbers if possible so photos are in order
 *      - Group by user if possible (two users in different places on the same day)
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include "args.h"
#include "common.h"
#include <libexif/exif-data.h>

void process_dir (const char* dir);
void process_file (const char* fqpn);
void trim_spaces(char *buf);
void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag);

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

/* Remove spaces on the right of the string */
void trim_spaces(char* buf)
{
  char* s = buf-1;
  for (; *buf; ++buf) {
    if (*buf != ' ')
      s = buf;
  }
  *++s = 0; /* nul terminate the string on the first of the final spaces */
}

/* Show the tag name and contents if the tag exists */
void show_tag(ExifData* d, ExifIfd ifd, ExifTag tag)
{
  /* See if this tag exists */
  ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
  if (entry) {
    char buf[1024];
    /* Get the contents of the tag in human-readable form */
    exif_entry_get_value(entry, buf, sizeof(buf));
    /* Don't bother printing it if it's entirely blank */
    trim_spaces(buf);
    if (*buf) {
      printf("%s: %s\n", exif_tag_get_name_in_ifd(tag,ifd), buf);
    }
  }
  else
    counters.missing_date++;
}

void process_file (const char* fqpn)
{
  ExifData *ed;
  ExifEntry *entry;

  /* Load an ExifData object from an EXIF file */
  ed = exif_data_new_from_file(fqpn);
  if (!ed)
  {
    counters.missing_exif++;
    return;  // no exif data in this file
  }

  show_tag(ed, EXIF_IFD_0, EXIF_TAG_DATE_TIME);
  show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);

//  show_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_USER_COMMENT);
//  show_tag(ed, EXIF_IFD_0, EXIF_TAG_IMAGE_DESCRIPTION);
//  show_tag(ed, EXIF_IFD_1, EXIF_TAG_IMAGE_DESCRIPTION);

  /* Free the EXIF data */
  exif_data_unref(ed);
}

void process_dir (const char* dir)
{
  DIR* d;
  d = opendir (dir);

  if (!d) {
    fprintf (stderr, "Cannot open directory '%s': %s\n",dir,strerror (errno));
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
/*    if (!(entry->d_type & DT_DIR)) {
      printf ("%s/%s\n", dir, d_name);
    }
*/
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
      process_file (path);
    }
  }

  if (closedir (d)) {
    fprintf (stderr, "Failed to close '%s': %s\n", dir, strerror (errno));
    exit (1);
  }
}
