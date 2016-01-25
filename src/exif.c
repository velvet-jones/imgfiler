#include "exif.h"

void get_tag(ExifData* d, ExifIfd ifd, ExifTag tag, char* buf, int len);

bool exif_date (const char* fqpn, char* date, int len)
{
  ExifData *ed;
  ExifEntry *entry;

  /* Load an ExifData object from an EXIF file */
  ed = exif_data_new_from_file(fqpn);
  if (!ed)
  {
    return false;  // no exif data in this file
  }

  get_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL,date,len);

  if (!*date)
    get_tag(ed, EXIF_IFD_0, EXIF_TAG_DATE_TIME,date,len);

  exif_data_unref(ed);

  return (*date != 0);
}

// get the tag
void get_tag(ExifData* d, ExifIfd ifd, ExifTag tag, char* buf, int len)
{
  ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
  if (entry) {
    exif_entry_get_value(entry, buf, len);
    right_trim(buf);
  }
}