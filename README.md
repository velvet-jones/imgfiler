Image Filer
===========
A utility for organizing digital photos.

*Copyright (c) 2016 Bud Millwood*

This program re-organizes your digital photos by moving them
into a directory based on the date the photo was taken.

Configuration
-------------
All setting are provided on the command line. There is no configuration file.

--source:: Required. The source directory containing photos you want to re-organize.

--destination:: Required. The destination directory that receives the re-organized photos.
                Can be the same as the source directory.

--dateless:: Optional. If supplied, the program moves photos that have no date
             into this directory. Without a 'dateless' directory the program will
             skip dateless photos.

--duplicates:: Optional. If supplied, the program moves photos that are duplicates
               into this directory. Without a 'duplicates' directory the program
               will delete duplicate photos.

--jobs:: Run multiple jobs in parallel. If your disk utilization is low, increase the number of jobs
         until you hit 100%.

--md5:: Use MD5 instead of SHA1 for hashing.

--verbose:: Print information about each operation. Verbose is enabled by default if
            '--move' is not supplied.

--help:: Print instructions for using the program.

--version:: Print the version number of the program.

--move:: Without this, the program simply suggests each operation it would like to perform.
         If this argument is supplied, the program will carry out all suggested actions.
         When invoked directly by a user, the '--verbose' option is useful to see the progress.

Operation
---------
IMPORTANT: The program will not make any changes to your system unless you
           provide the '--move' command line option. Without this option,
           the program simply suggests what it would do with each file.

- There are no issues with overlapping directories. You can specify the same
  directory for both source and destination.

- Photos are renamed according to their 'hash'. A hash of a digital photo is a
  long sequence of seemingly random characters that uniquely identify that photo.

- Hidden directories and files are skipped.

Example Usage
-------------
In the directory structure below we want to scan the 'photos/uploads/' directory for new photos
and file them under 'photos/' or 'photos/dateless/'. The command line would be:

----
$ imgfiler --move --source=/photos/uploads --destination=/photos --dateless=/photos/dateless
----

Directory structure

    /photos/
      |
      - dateless/
      |
      - uploads/
        |
        - stella/
            |
            - file.jpg
        |
        - august/
        - hugo/

Pruning empty directories
-------------------------
The program does not remove empty directories. You can do this with the following command:

----
$ find top_directory -type d -exec rmdir {} \; 2>/dev/null
----
