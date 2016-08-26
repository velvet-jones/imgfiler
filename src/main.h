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

#ifndef INC_MAIN_H
#define INC_MAIN_H

#include "common.h"
#include "file.h"

void process_dir (const char* dir);
void map_and_process_file(const char* path);
void process_file (file_t*);
void perform_move_src(file_t* src_file, const char* dst_dir);
void perform_delete_src(file_t* src_file, const char* dst_fqpn);
bool format_dst (const char* base_dir, const date_t* date, const char* dst_name, char* dst_dir, char* dst_fqpn);
void set_sig_handlers ();
void wait_for_children();

#endif // INC_MAIN_H
