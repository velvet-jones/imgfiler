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

typedef struct {
  long  total_files;
  long  total_dirs;
  long  duplicates;
  long  missing_date;
  long  skipped;
} counters_t;

extern counters_t counters;

typedef struct {
  struct tm   tm;
  bool        valid; // false if 'date' is invalid
} date_t;

void right_trim (char* buf, char c);
const char* contains (const char* s, char c);
const char* read_integer (const char* buf,const char* delimiters,int max_chars,int* result);
const char* read_long (const char* buf,const char* delimiters,int max_chars,long* result);
bool same_file (struct stat* st1, struct stat* st2);
bool to_long (const char* s, long* l);
const char* get_extension (const char* fqpn);
bool validate_name (const char* name, mode_t mode);
bool create_directory_if (const char* dir);

#endif // INC_COMMON_H
