#ifndef INC_SHA_H
#define INC_SHA_H

#include <stdbool.h>
#include <openssl/sha.h>

// the buffer length required to transform a sha1 into a string
#define SHA1_SBUF_LEN SHA_DIGEST_LENGTH*2+1

#define _READ_BUF_SIZE 4194304  // 4MB; photos are not small

bool compute_sha1(const char* fqpn, char* mds);

#endif // INC_SHA_H