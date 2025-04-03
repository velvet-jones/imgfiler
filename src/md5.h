#ifndef __md5_h__
#define __md5_h__

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

#define MD5_DIGEST_LENGTH 16

typedef struct {
  uint32 total[2];
  uint32 state[4];
  uint8 buffer[64];
} md5_context;

void md5_starts(md5_context *ctx);
void md5_update(md5_context *ctx, uint8 *input, uint32 length);
void md5_finish(md5_context *ctx, uint8 digest[16]);

#endif // __md5_h__

