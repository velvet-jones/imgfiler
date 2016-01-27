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

#ifndef INC_FILE_H
#define INC_FILE_H

#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

typedef struct
{
  unsigned char*  addr;
  char            fqpn[PATH_MAX];
  char            hash[PATH_MAX];
  struct stat     st;   // use st.st_size for the length of the buffer pointed to by addr
} file_t;

file_t* map_file (const char* fqpn);
int unmap_file (file_t* file);

#endif // INC_FILE_H
