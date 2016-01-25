#include "common.h"

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

bool same_file (struct stat* st1, struct stat* st2)
{
  return ( (memcmp (st1->st_dev,st2->st_dev,sizeof(st1->st_dev)) == 0) &&
           (memcmp (st1->st_ino,st2->st_ino,sizeof(st1->st_ino)) == 0) );
}
