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

#ifndef INC_ARGS_H
#define INC_ARGS_H

#include <limits.h>

#define OPERATION_NOP  0
#define OPERATION_MOVE 1

#define HASH_SHA  0
#define HASH_MD5  1

typedef struct {
  char  src_dir[PATH_MAX];      // source dir
  char  dst_dir[PATH_MAX];      // dest dir
  char  dup_dir[PATH_MAX];      // holds duplicate photos
  char  dateless_dir[PATH_MAX]; // holds dateless photos (no exif date)
  int   verbose;                // extra logging
  int   operation;              // nop/copy/move
  int   hash;                   // nop/copy/move
  int   src_is_file;            // src_dir refers to: 0 = dir, 1 = file
  int   jobs;
} args_t;

const args_t* get_args (int argc, char **argv);

#endif // INC_ARGS_H
