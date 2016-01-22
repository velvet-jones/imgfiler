#define APP_VERSION "1.0"

typedef struct {
  long  total_files;
  long  total_dirs;
  long  duplicates;
  long  missing_exif;
  long  missing_date;
} counters_t;

extern counters_t counters;