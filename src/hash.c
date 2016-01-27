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
//#include <errno.h>
#include <string.h>

bool compute_hash (const args_t* args, file_t* file)
{
  switch (args->hash)
  {
    case HASH_SHA:
      return compute_sha1(file);
    break;

    case HASH_MD5:
      return compute_md5(file);
    break;
  }
  return false;
}

// outputs dst_name, the string representation of the file's sha1
bool compute_sha1(file_t* file)
{
  SHA_CTX context;
  if(!SHA1_Init(&context))
    return false;

  if (!SHA1_Update(&context, file->addr, file->st.st_size))
    return false;

  unsigned char md[SHA_DIGEST_LENGTH];
  if(!SHA1_Final(md, &context))
    return false;

  int i = 0;
  for (; i < SHA_DIGEST_LENGTH; i++)
    sprintf(&file->hash[i*2], "%02x", (unsigned int)md[i]);

  strcpy (&file->hash[i*2],get_extension(file->fqpn));
  return true;
}

// outputs dst_name, the string representation of the file's md5
bool compute_md5(file_t* file)
{
  MD5_CTX context;
  if(!MD5_Init(&context))
    return false;

  if (!MD5_Update(&context, file->addr, file->st.st_size))
    return false;

  unsigned char md[MD5_DIGEST_LENGTH];
  if(!MD5_Final(md, &context))
    return false;

  int i = 0;
  for (; i < MD5_DIGEST_LENGTH; i++)
    sprintf(&file->hash[i*2], "%02x", (unsigned int)md[i]);

  strcpy (&file->hash[i*2],get_extension(file->fqpn));
  return true;
}
