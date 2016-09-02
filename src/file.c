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

#include "file.h"
#include "common.h"
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// returns 0 if error
file_t* map_file (const char* fqpn)
{
  FILE* f = fopen (fqpn,"rb");
  if (!f)
    return 0;

  file_t* file = malloc (sizeof(file_t));
  if (!file)
    return 0;
  memset (file,0,sizeof(file_t));

  int ret = stat(fqpn,&file->st);
  if (ret != 0)
    return 0;

  file->addr = mmap (0, file->st.st_size, PROT_READ, MAP_PRIVATE, fileno(f), 0);
  int err = errno;
  fclose (f);

  if (file->addr == MAP_FAILED)
  {
    free (file);
    errno = err;  // we saved it because fclose may have change it, and we want to report the original problem
    return 0;
  }

  strcpy (file->fqpn,fqpn);
  return file;
}

// returns 0 on success
int unmap_file (file_t* file)
{
  int ret = munmap (file->addr,file->st.st_size);
  free (file);
  return ret; // assumes 'free' doesn't set errno
}
