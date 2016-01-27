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
#include <openssl/md5.h>
#include "hash.h"
#include "common.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

unsigned char io_buf[_READ_BUF_SIZE];

bool compute_hash (const args_t* args,const char* fqpn, char* dst_name, int name_len)
{
  switch (args->hash)
  {
    case HASH_SHA:
      return compute_sha1(fqpn, dst_name, name_len);
    break;

    case HASH_MD5:
      return compute_md5(fqpn, dst_name, name_len);
    break;
  }
  return false;
}

// outputs dst_name, the string representation of the file's sha1
bool compute_sha1(const char* fqpn, char* dst_name, int name_len)
{
  SHA_CTX context;
  if(!SHA1_Init(&context))
    return false;

  FILE* f = fopen (fqpn,"rb");
  if (!f)
    return false;

  size_t len = 0;
  do
  {
    len = fread(io_buf, sizeof(char), _READ_BUF_SIZE, f);
    if (ferror(f) || !SHA1_Update(&context, io_buf, len))
    {
      fclose (f);
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
    return false;

  int i = 0;
  for (; i < SHA_DIGEST_LENGTH; i++)
    sprintf(&dst_name[i*2], "%02x", (unsigned int)md[i]);

  strcpy (&dst_name[i*2],get_extension(fqpn));
  return true;
}

// outputs dst_name, the string representation of the file's md5
bool compute_md5(const char* fqpn, char* dst_name, int name_len)
{
  MD5_CTX context;
  if(!MD5_Init(&context))
    return false;

  FILE* f = fopen (fqpn,"rb");
  if (!f)
    return false;

  size_t len = 0;
  do
  {
    len = fread(io_buf, sizeof(char), _READ_BUF_SIZE, f);
    if (ferror(f) || !MD5_Update(&context, io_buf, len))
    {
      fclose (f);
      return false;
    }

    if (feof (f))
    {
      fclose(f);
      break;
    }
  }
  while (1);

  unsigned char md[MD5_DIGEST_LENGTH];
  if(!MD5_Final(md, &context))
    return false;

  int i = 0;
  for (; i < MD5_DIGEST_LENGTH; i++)
    sprintf(&dst_name[i*2], "%02x", (unsigned int)md[i]);

  strcpy (&dst_name[i*2],get_extension(fqpn));
  return true;
}
