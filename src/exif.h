#ifndef INC_EXIF_H
#define INC_EXIF_H

#include "common.h"
#include <libexif/exif-data.h>

bool exif_date (const char* fqpn, char* date, int len);

#endif // INC_EXIF_H
