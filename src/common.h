#ifndef INC_COMMON_H
#define INC_COMMON_H

#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

#define APP_VERSION "1.0"

typedef struct {
  long  total_files;
  long  total_dirs;
  long  duplicates;
  long  missing_date;
} counters_t;

extern counters_t counters;

typedef struct {
  struct tm   tm;
  bool        valid; // false if 'date' is invalid
} date_t;

void right_trim (char* buf);
bool same_file (struct stat* st1, struct stat* st2);
bool to_long (const char* s, long* l);
bool validate_dir (const char* dir);
bool create_directory_if (const char* dir);

#endif // INC_COMMON_H
