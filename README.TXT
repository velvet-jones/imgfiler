This utility scans a directory for image files, renames each file
according to its SHA-1 checksum, then moves the file to a
user-defined destination directory based on EXIF information. This
is typically used for sorting photographs based on the date they
were taken.

To compile this program:

1. Install CMake
2. mkdir build
3. cd build
4. cmake ..
5. make
6. make install
