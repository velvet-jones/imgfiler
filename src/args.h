#ifndef INC_ARGS_H
#define INC_ARGS_H

#include <limits.h>

#define OPERATION_NOP  0
#define OPERATION_MOVE 1

typedef struct {
  char  src_dir[PATH_MAX];      // source dir
  char  dst_dir[PATH_MAX];      // dest dir
  char  dup_dir[PATH_MAX];      // holds duplicate photos
  char  dateless_dir[PATH_MAX]; // holds dateless photos (no exif date)
  int   verbose;                // extra logging
  int   operation;              // nop/copy/move
} args_t;

const args_t* get_args (int argc, char **argv);
void show_help();

#endif // INC_ARGS_H
