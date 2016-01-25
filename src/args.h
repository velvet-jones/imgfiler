#ifndef INC_ARGS_H
#define INC_ARGS_H

#include <limits.h>

#define OPERATION_COPY 0
#define OPERATION_MOVE 1
#define OPERATION_NOP  2

typedef struct {
  char  src_dir[PATH_MAX]; // source dir
  char  dst_dir[PATH_MAX]; // dest dir
  char  dup_dir[PATH_MAX]; // holds duplicate photos
  int   verbose;           // extra logging
  int   operation;         // copy or move
  int   dry_run;           // don't change anything, just print
} args_t;

const args_t* get_args (int argc, char **argv);
void show_help();

#endif // INC_ARGS_H
