#include <openssl/sha.h>
#include "sha.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

unsigned char io_buf[_READ_BUF_SIZE];

// mds is an output; the string form of the sha1
bool compute_sha1(const char* fqpn, char* mds)
{
  SHA_CTX context;
  if(!SHA1_Init(&context))
  {
    fprintf (stderr,"Failed to create SHA1 context for '%s': %s.\n",fqpn,strerror (errno));
    return false;
  }

  FILE* f = fopen (fqpn,"rb");
  if (!f)
  {
    fprintf (stderr,"Failed to open file '%s': %s.\n",fqpn,strerror (errno));
    return false;
  }

  size_t len = 0;
  do
  {
    len = fread(io_buf, sizeof(char), _READ_BUF_SIZE, f);
    if (ferror(f))
    {
      fclose (f);
      fprintf (stderr,"Failed to read file '%s': %s.\n",fqpn,strerror (errno));
      return false;
    }

    if(!SHA1_Update(&context, io_buf, len))
    {
      fclose (f);
      fprintf (stderr,"Failed to update SHA1 for '%s': %s.\n",fqpn,strerror (errno));
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
    fprintf (stderr,"Failed to finalize SHA1 for '%s': %s.\n",fqpn,strerror (errno));
    return false;
  }

  int i = 0;
  for (; i < SHA_DIGEST_LENGTH; i++)
    sprintf(&mds[i*2], "%02x", (unsigned int)md[i]);

  mds[++i*2] = 0;  // add a null terminator
  return true;
}
