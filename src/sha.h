#ifndef INC_SHA_H
#define INC_SHA_H

#include <stdbool.h>
#include <openssl/sha.h>

#define _READ_BUF_SIZE 4194304  // 4MB; photos are not small

bool compute_sha1(const char* fqpn, char* mds);

#endif // INC_SHA_H