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

void process_dir (const char* dir);
void process_file (const char* dir, const char* name, const char* fqpn);
void perform_operation(const char* fqpn, const char* dst_dir, const char* dst_name);
bool format_dst (const char* base_dir, const date_t* date, const char* dst_name, char* dst_dir, char* dst_fqpn);
void set_sig_handlers ();

#endif // INC_MAIN_H
