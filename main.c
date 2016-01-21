/*
 * A utility for filing photos by date based on the photo's metadata.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <libexif/exif-data.h>

void process_dir (const char* name);

int main (int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf (stderr, "Usage: %s DIRECTORY\n", argv[0]);
    exit (1);
  }

  process_dir (argv[1]);

  return 0;
}

void process_dir (const char* name)
{
  DIR* d;
  d = opendir (name);

  if (!d) {
    fprintf (stderr, "Cannot open directory '%s': %s\n",name,strerror (errno));
    exit (1);
  }

  while (1) {
    struct dirent* entry;
    const char* d_name;

    entry = readdir (d);
    if (!entry) {
      break; // no more
    }

    d_name = entry->d_name;
    if (!(entry->d_type & DT_DIR)) {
      printf ("%s/%s\n", name, d_name);
    }

    if (entry->d_type & DT_DIR) {
      if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) {
        int path_length;
        char path[PATH_MAX];
        path_length = snprintf (path, PATH_MAX,"%s/%s", name, d_name);
        printf ("%s\n", path);
        if (path_length >= PATH_MAX) {
          fprintf (stderr, "Path too long.\n");
          exit (1);
        }

        // recursively process this directory
        process_dir (path);
      }
    }
  }

  if (closedir (d)) {
    fprintf (stderr, "Failed to close '%s': %s\n", name, strerror (errno));
    exit (1);
  }
}
