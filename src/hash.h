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

#define _READ_BUF_SIZE 4194304  // 4MB; photos are not small

bool compute_hash (const args_t* args,const char* fqpn, char* dst_name, int name_len);
bool compute_sha1(const char* fqpn, char* dst_name, int name_len);
bool compute_md5(const char* fqpn, char* dst_name, int name_len);

#endif // INC_HASH_H
