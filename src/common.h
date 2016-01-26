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
const char* get_extension (const char* fqpn);
bool validate_dir (const char* dir);
bool create_directory_if (const char* dir);

#endif // INC_COMMON_H
