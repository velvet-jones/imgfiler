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

#ifndef INC_HASH_H
#define INC_HASH_H

#include <stdbool.h>
#include "args.h"
#include "file.h"

bool compute_hash (const args_t* args, file_t* file);
bool compute_sha1(file_t* file);
bool compute_md5(file_t* file);
unsigned char compute_pearson_hash (const char* name, int combinations);

#endif // INC_HASH_H
