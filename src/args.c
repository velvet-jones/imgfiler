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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "args.h"
#include "common.h"

static args_t args;

void validate_args(const char* app, const args_t* args);
void show_help();

const args_t* get_args (int argc, char **argv)
{
  memset (&args,0,sizeof(args_t));

  int c;

  while (1)
  {
    static struct option long_options[] = {
        {"verbose",     no_argument,       &args.verbose, 1},
        {"move",        no_argument,       &args.operation, 1},
        {"md5",         no_argument,       &args.hash, 1},
        {"version",     no_argument,       0, 'v'},
        {"help",        no_argument,       0, 'h'},
        {"source",      required_argument, 0, 's'},
        {"destination", required_argument, 0, 'd'},
        {"duplicates",  required_argument, 0, 'u'},
        {"dateless",    required_argument, 0, 'l'},
        {0,             0,                 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "vhs:d:u:l:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
    {
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;
        printf ("option %s", long_options[option_index].name);
        if (optarg)
          printf (" with arg %s", optarg);
        printf ("\n");
      break;

      case 's':
        strcpy (args.src_dir,optarg);
      break;

      case 'd':
        strcpy (args.dst_dir,optarg);
      break;

      case 'u':
        strcpy (args.dup_dir,optarg);
      break;

      case 'l':
        strcpy (args.dateless_dir,optarg);
      break;

      case 'v':
        printf ("%s\n",APP_VERSION);
      break;

      case 'h':
        show_help(argv[0]);
        exit(0);
      break;

      case '?':
        /* getopt_long already printed an error message. */
      break;

      default:
        abort();
    }
  }

  // if performing a dry run (default), be verbose
  if (args.operation == OPERATION_NOP)
    args.verbose = 1;

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
  {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    putchar ('\n');
  }

  right_trim (args.src_dir,'/');
  right_trim (args.dst_dir,'/');
  right_trim (args.dup_dir,'/');
  right_trim (args.dateless_dir,'/');

  validate_args (argv[0],&args);
  return &args;
}

void show_help(const char* app)
{
  printf("Usage: %s [OPTIONS]\n", app);
  printf("  -s, --source              The source directory containing photos\n");
  printf("  -d, --destination         The destination directory for photos\n");
  printf("  -u, --duplicates          If provided, holds duplicates; otherwise duplicate files are deleted\n");
  printf("  -l, --dateless            If provided, holds dateless; otherwise dateless files are skipped\n");
  printf("  --move                    Default behavior suggests only; use this to perform the operations\n");
  printf("  --md5                     Use md5 instead of the default sha1\n");
  printf("  -v, --version             Show the version number\n");
  printf("  -h, --help                Show this help\n");
}

void validate_args(const char* app, const args_t* args)
{
  /* If no dateless_dir, we just leave the original file as is.
     If no duplicates dir, we delete the source file. (Otherwise we move it to the dup dir)
  */
  if (!*args->src_dir && !*args->dst_dir)
  {
    show_help(app);
    exit(1);
  }

  if (!validate_dir(args->src_dir))
  {
    fprintf (stderr,"Source directory %s does not exist.\n",args->src_dir);
    exit (1);
  }

  if (!validate_dir(args->dst_dir))
  {
    fprintf (stderr,"Destination directory %s does not exist.\n",args->dst_dir);
    exit (1);
  }

  if (*args->dup_dir != 0 && !validate_dir(args->dup_dir))
  {
    fprintf (stderr,"Duplicates directory %s does not exist.\n",args->dup_dir);
    exit (1);
  }

  if (*args->dateless_dir != 0 && !validate_dir(args->dateless_dir))
  {
    fprintf (stderr,"Dateless directory %s does not exist.\n",args->dateless_dir);
    exit (1);
  }
}
