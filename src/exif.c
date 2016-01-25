#include "exif.h"

bool exif_date (const char* fqpn, char* date)
{
  ExifData *ed;
  ExifEntry *entry;

  /* Load an ExifData object from an EXIF file */
  ed = exif_data_new_from_file(fqpn);
  if (!ed)
  {
    counters.missing_exif++;
    return false;  // no exif data in this file
  }

  get_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL,date);

  if (!*date)
    get_tag(ed, EXIF_IFD_0, EXIF_TAG_DATE_TIME,date);

  exif_data_unref(ed);

  return (*date != 0);
}

// get the tag
void get_tag(ExifData* d, ExifIfd ifd, ExifTag tag, char* buf)
{
  ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
  if (entry) {
    exif_entry_get_value(entry, buf, sizeof(buf));
    right_trim(buf);
  }
}