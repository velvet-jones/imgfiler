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
#include <stdlib.h>

// trim whitespace on right
void right_trim(char* buf, char c)
{
  char* s = buf-1;
  for (; *buf; ++buf) {
    if (*buf != c)
      s = buf;
  }
  *++s = 0; /* nul terminate the string on the first of the final spaces */
}

bool is7BitAscii (const char* s)
{
  const char* p = s;
  while (*p != 0)
  {
    if (*p & 0x80)
      return false;
    p++;
  }
  return true;
}

const char* contains (const char* s, char c)
{
  const char* p = s;
  while (*p != 0)
  {
    if (*p == c)
      return p;
    p++;
  }
  return 0;
}

/* Read 'max_chars' into 'buf', then convert that to an integer. Return null if
   the number fails to convert. Always stop reading at any character found in
   'delimiters'. If no delimiter is found within max_chars, check the next character.
   If it's in 'delimiters', skip it, and return that pointer.
*/
const char* read_integer (const char* s,const char* delimiters,int max_chars,int* result)
{
  long l = 0;
  const char* ret = read_long (s,delimiters,max_chars,&l);
  if (ret != 0)
    *result = (int)l;
  return ret;
}

const char* read_long (const char* s,const char* delimiters,int max_chars,long* result)
{
  char tmp[100];
  memset (tmp,0,sizeof(tmp));

  if (max_chars > sizeof(tmp)-1)
    return 0;

  const char* p = s;
  const char* delim = 0;
  while (*p != 0)
  {
    max_chars--;
    if (max_chars < 0)
      break;

    delim = contains (delimiters,*p);
    if (delim)
      break;
    p++;
  }

  // if max_chars < 0, we stopped because we ran out of characters to check
  // if delim != 0, we stopped because we found a delimiter
  // otherwise, we stopped because we hit the end of the string

  int length = p-s;

  if (length > 0)
  {
    strncpy (tmp,s,length);
    if (!to_long (tmp,result))
      return 0;

    if (*p != 0 && (delim || max_chars < 0))
      p++;  // skip either the delimiter or just move to the next char

    return p;
  }
  else
    return 0;  // we failed to read a number
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
  long tmp = strtoul (s,&lastValid,10);

  if (!lastValid || *lastValid != 0 || errno != 0)
    return false;

  *l = tmp;
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

// check if a name refers to a file, directory, etc.
bool validate_name (const char* name, mode_t mode)
{
  struct stat st;
  if (stat(name,&st) != 0)
    return false;
  return (st.st_mode & mode);
}

// create a directory structure, like 'mkdir -p' does
bool create_directory_if (const char* dir)
{
  char tmp[PATH_MAX];
  memset (tmp,0,PATH_MAX);

  const char* component = strchr (dir,'/');
  while (component)
  {
    if (strlen(component) != strlen(dir))
    {
      strncpy (tmp,dir,component-dir);
      int ret = mkdir (tmp,0700);
      if (ret != 0 && errno != EEXIST)
        return false;
    }
    component = strchr (component+1,'/');
  }

  if (mkdir (dir,0700) != 0 && errno != EEXIST)
    return false;

  return true;
}
