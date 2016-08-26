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

unsigned char pearson_mixing_table[256] = {
251, 175, 119, 215, 81, 14, 79, 191, 103, 49, 181, 143, 186, 157,  0,
232, 31, 32, 55, 60, 152, 58, 17, 237, 174, 70, 160, 144, 220, 90, 57,
223, 59,  3, 18, 140, 111, 166, 203, 196, 134, 243, 124, 95, 222, 179,
197, 65, 180, 48, 36, 15, 107, 46, 233, 130, 165, 30, 123, 161, 209, 23,
97, 16, 40, 91, 219, 61, 100, 10, 210, 109, 250, 127, 22, 138, 29, 108,
244, 67, 207,  9, 178, 204, 74, 98, 126, 249, 167, 116, 34, 77, 193,
200, 121,  5, 20, 113, 71, 35, 128, 13, 182, 94, 25, 226, 227, 199, 75,

27, 41, 245, 230, 224, 43, 225, 177, 26, 155, 150, 212, 142, 218, 115,
241, 73, 88, 105, 39, 114, 62, 255, 192, 201, 145, 214, 168, 158, 221,
148, 154, 122, 12, 84, 82, 163, 44, 139, 228, 236, 205, 242, 217, 11,
187, 146, 159, 64, 86, 239, 195, 42, 106, 198, 118, 112, 184, 172, 87,
2, 173, 117, 176, 229, 247, 253, 137, 185, 99, 164, 102, 147, 45, 66,
231, 52, 141, 211, 194, 206, 246, 238, 56, 110, 78, 248, 63, 240, 189,
93, 92, 51, 53, 183, 19, 171, 72, 50, 33, 104, 101, 69, 8, 252, 83, 120,
76, 135, 85, 54, 202, 125, 188, 213, 96, 235, 136, 208, 162, 129, 190,
132, 156, 38, 47, 1, 7, 254, 24, 4, 216, 131, 89, 21, 28, 133, 37, 153,
149, 80, 170, 68, 6, 169, 234, 151
};

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

unsigned char compute_pearson_hash (const char* name, int combinations)
{
  if (combinations <= 0 || combinations > 256)
  {
    fprintf (stderr,"Failed to compute job hash.\n");
    exit(1);
  }

  unsigned char hash = (unsigned char) (strlen(name) % 256);

  while (*name != 0)
  {
    hash = pearson_mixing_table [ hash ^ *name ];
    name++;
  }

  return hash % combinations;
}
