#ifndef INC_EXIF_H
#define INC_EXIF_H

#include "common.h"
#include <libexif/exif-data.h>

bool exif_date (const char* fqpn, char* date);

void get_tag(ExifData* d, ExifIfd ifd, ExifTag tag, char* buf);

#endif // INC_EXIF_H
