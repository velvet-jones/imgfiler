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

#include <openssl/sha.h>
#include "sha.h"
#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

unsigned char io_buf[_READ_BUF_SIZE];

// outputs dst_name, the string representation of the file's sha1
bool compute_sha1(const char* fqpn, char* dst_name, int name_len)
{
  SHA_CTX context;
  if(!SHA1_Init(&context))
  {
    fprintf (stderr,"Failed to create SHA1 context for %s: %s.\n",fqpn,strerror (errno));
    return false;
  }

  FILE* f = fopen (fqpn,"rb");
  if (!f)
  {
    fprintf (stderr,"Failed to open file %s: %s.\n",fqpn,strerror (errno));
    return false;
  }

  size_t len = 0;
  do
  {
    len = fread(io_buf, sizeof(char), _READ_BUF_SIZE, f);
    if (ferror(f))
    {
      fclose (f);
      fprintf (stderr,"Failed to read file %s: %s.\n",fqpn,strerror (errno));
      return false;
    }

    if(!SHA1_Update(&context, io_buf, len))
    {
      fclose (f);
      fprintf (stderr,"Failed to update SHA1 for %s: %s.\n",fqpn,strerror (errno));
      return false;
    }

    if (feof (f))
    {
      fclose(f);
      break;
    }
  }
  while (1);

  unsigned char md[SHA_DIGEST_LENGTH];
  if(!SHA1_Final(md, &context))
  {
    fprintf (stderr,"Failed to finalize SHA1 for %s: %s.\n",fqpn,strerror (errno));
    return false;
  }

  int i = 0;
  for (; i < SHA_DIGEST_LENGTH; i++)
    sprintf(&dst_name[i*2], "%02x", (unsigned int)md[i]);

  strcpy (&dst_name[i*2],get_extension(fqpn));
  return true;
}
