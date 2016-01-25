#ifndef INC_COMMON_H
#define INC_COMMON_H

#include <stdbool.h>
#include <sys/stat.h>

#define APP_VERSION "1.0"

typedef struct {
  long  total_files;
  long  total_dirs;
  long  duplicates;
  long  missing_exif;
  long  missing_date;
} counters_t;

extern counters_t counters;

void right_trim (char* buf);
bool same_file (struct stat* st1, struct stat* st2);

#endif // INC_COMMON_H
