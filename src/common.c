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

#include "common.h"
#include <errno.h>
#include <dirent.h>
#include <string.h>

// trim whitespace on right
void right_trim(char* buf)
{
  char* s = buf-1;
  for (; *buf; ++buf) {
    if (*buf != ' ')
      s = buf;
  }
  *++s = 0; /* nul terminate the string on the first of the final spaces */
}

// check if two files are actually the same file on disk (not content - the *same file*)
bool same_file (struct stat* st1, struct stat* st2)
{
  return ( (memcmp (&st1->st_dev,&st2->st_dev,sizeof(st1->st_dev)) == 0) &&
           (memcmp (&st1->st_ino,&st2->st_ino,sizeof(st1->st_ino)) == 0) );
}

// convert a string to a long integer
bool to_long (const char* s, long* l)
{
  char dummy = '\0';
  char* lastValid = &dummy;
  errno = 0;  // we must use errno, since we have to set it to 0 before calling strtoul
  *l = strtoul (s,&lastValid,10);

  if (!lastValid || *lastValid != 0 || errno != 0)
    return false;
  else
    return true;
}

const char* get_extension (const char* fqpn)
{
  // find last slash character; if it exists, start after that
  const char* begin = fqpn;
  const char* slash = strrchr (fqpn,'/');
  if (slash)
    begin = slash+1;

  // find the last dot character
  const char* dot = strrchr (begin,'.');

  // if the file name started with a dot, there is no extension
  if (!dot || dot == begin)
    return &fqpn[strlen(fqpn)];  // returns the null terminator only
  else
    return dot;  // returns extension preceded by dot
}

// check if a directory exists
bool validate_dir (const char* dir)
{
  DIR* d;
  d = opendir (dir);

  if (!d)
    return false;

  closedir (d);
  return true;
}

// create a directory structure, like 'mkdir -p' does
bool create_directory_if (const char* dir)
{
  char tmp[PATH_MAX];
  memset (tmp,0,PATH_MAX);

  const char* component = strchr (dir,'/');
  while (component)
  {
    strncpy (tmp,dir,component-dir);
    int ret = mkdir (tmp,0700);
    if (ret != 0 && errno != EEXIST)
      return false;
    component = strchr (component+1,'/');
  }

  if (mkdir (dir,0700) != 0 && errno != EEXIST)
    return false;

  return true;
}
