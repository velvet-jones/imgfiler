#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "args.h"
#include "common.h"

static args_t args;

const args_t* get_args (int argc, char **argv)
{
  memset (&args,0,sizeof(args_t));

  int c;

  while (1) {
    static struct option long_options[] = {
        {"verbose",     no_argument,       &args.verbose, 1},
        {"move",        no_argument,       &args.operation, 1},
        {"dry-run",     no_argument,       &args.dry_run, 1},
        {"version",     no_argument,       0, 'v'},
        {"help",        no_argument,       0, 'h'},
        {"source",      required_argument, 0, 's'},
        {"destination", required_argument, 0, 'd'},
        {"duplicates",  required_argument, 0, 'u'},
        {0,             0,                 0, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "vhs:d:u:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
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

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
  {
    printf ("non-option ARGV-elements: ");
    while (optind < argc)
      printf ("%s ", argv[optind++]);
    putchar ('\n');
  }
  return &args;
}

void show_help(const char* app)
{
  printf("Usage: %s [OPTIONS]\n", app);
  printf("  -s, --source              The source directory containing photos\n");
  printf("  -d, --destination         The destination directory for photos\n");
  printf("  -u, --duplicates          The directory for holding duplicate photos\n");
  printf("  -v, --version             Show the version number\n");
  printf("  -h, --help                Show this help\n");
}
