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

#include "extract.h"
#include <extractor.h>
#include <string.h>

int process_data(void *cls,const char *plugin_name, enum EXTRACTOR_MetaType type, enum EXTRACTOR_MetaFormat format,
                 const char *data_mime_type, const char *data,size_t data_len);
void fix_iso8601 (date_t* date);

struct EXTRACTOR_PluginList* plugins = 0;

bool extract_date (const char* fqpn, date_t* date)
{
  // FLAG: ideally the next line would be removed and we'd uncomment the init_extractor function
  plugins = EXTRACTOR_plugin_add_defaults (EXTRACTOR_OPTION_DEFAULT_POLICY);

  if (!plugins)
    return false;

  memset (date,0,sizeof(date_t));
  date->valid = false;

  EXTRACTOR_extract (plugins, fqpn, NULL, 0, &process_data, date);

  // FLAG: ideally the next line would be removed and we'd uncomment the close_extractor function
  EXTRACTOR_plugin_remove_all (plugins);

  return date->valid;
}

/* FLAG: there appears to be a bug in libextractor that does not let us perform one-time initialization
    https://gnunet.org/bugs/view.php?id=4158
*/
bool init_extractor()
{
/*  if (plugins != 0)
    return false;

  plugins = EXTRACTOR_plugin_add_defaults (EXTRACTOR_OPTION_DEFAULT_POLICY);
  return plugins != 0;
*/
  return true;
}

/* FLAG: there appears to be a bug in libextractor that does not let us perform one-time initialization
    https://gnunet.org/bugs/view.php?id=4158
*/
void close_extractor()
{
/*  if (plugins)
    EXTRACTOR_plugin_remove_all (plugins);
  plugins = 0;
*/
}

int process_data(void *cls,const char *plugin_name, enum EXTRACTOR_MetaType type, enum EXTRACTOR_MetaFormat format,
                 const char *data_mime_type, const char *data,size_t data_len)
{
  date_t* date = (date_t*) cls;

  switch (type)
  {
    // an ISO 8601 date
    case EXTRACTOR_METATYPE_CREATION_TIME:
      /* Possible date formats:
          2016-022                    "%d-%d"                     // 022 is ordinal day of year
          2016-01-22                  "%d-%d-%d"                  // no timestamp
          2016-01-22T17:32:47         "%d-%d-%dT%d:%d:%d"         // assume UTC
          2016-01-22T17:32:47Z        "%d-%d-%dT%d:%d:%dZ"        // UTC
          2016-01-22T17:32:47+00:00   "%d-%d-%dT%d:%d:%d+%d:%d"   // positive UTC offset
          2016-01-22T17:32:47+0000    "%d-%d-%dT%d:%d:%d+%d"      // positive UTC offset
          2016-01-22T17:32:47-00:00   "%d-%d-%dT%d:%d:%d-%d:%d"   // negative UTC offset
          2016-01-22T17:32:47-0000    "%d-%d-%dT%d:%d:%d-%d"      // negative UTC offset

          Formats that we hopefully never see:

          20160122T173247Z      // Digits: 4,2,2,T,2,2,2,Z
          2016-W03              // Year - week number
          2016-W03-5            // Seriously, wtf is this? The fifth ordinal day of week 3??
      */
      // 2016-01-22T17:32:47-01:00
      if (sscanf(data, "%d-%d-%dT%d:%d:%d",&date->tm.tm_year,&date->tm.tm_mon,&date->tm.tm_mday,
            &date->tm.tm_hour,&date->tm.tm_min,&date->tm.tm_sec) == 6)
      {
        fix_iso8601 (date);
        date->valid = true;
      }
      // 2016-01-22
      else if (sscanf(data, "%d-%d-%d",&date->tm.tm_year,&date->tm.tm_mon,&date->tm.tm_mday) == 3)
      {
        fix_iso8601 (date);
        date->valid = true;
      }
      // 2016-022
      else if (sscanf(data, "%d-%d",&date->tm.tm_year,&date->tm.tm_yday) == 2)
      {
        fix_iso8601 (date);
        date->valid = true;
      }
      else
        date->valid = false;

      // a few sanity checks
      if (date->tm.tm_year == 0 || date->tm.tm_mon < 1 || date->tm.tm_mon > 12 ||
          date->tm.tm_mday < 1 || date->tm.tm_mday > 31)
      {
        date->valid = false;
      }
      else
      {
        date->tm.tm_isdst = -1;    // no dst data available
        date->tm.tm_mon -= 1;      // zero-based months
        date->tm.tm_year -= 1900;  // years are offset from 1900
      }

      return 1;
    break;

    case EXTRACTOR_METATYPE_CREATION_DATE:
      // date format is 'YYYY:MM:DD HH:MM:SS'
      if (sscanf(data, "%d:%d:%d %d:%d:%d",&date->tm.tm_year,&date->tm.tm_mon,&date->tm.tm_mday,
            &date->tm.tm_hour,&date->tm.tm_min,&date->tm.tm_sec) != 6 ||
          date->tm.tm_year == 0 || date->tm.tm_mon < 1 || date->tm.tm_mon > 12 ||
          date->tm.tm_mday < 1 || date->tm.tm_mday > 31)
      {
        date->valid = false;
        return 1;
      }

      date->valid = true;
      date->tm.tm_isdst = -1;    // no dst data available
      date->tm.tm_mon -= 1;      // zero-based months
      date->tm.tm_year -= 1900;  // years are offset from 1900
      return 1;
    break;

    default:
    break;
  }

  return 0;  // continue
}

/* The extractor is calculating ISO 8601 dates from a 1904 epoch. If the year is before 1970,
   it means that the date was encoded from the 1970 epoch, so we add 66 years to correct it.

    https://gnunet.org/bugs/view.php?id=4157
*/
void fix_iso8601 (date_t* date)
{
  /* We don't use hour_offset or minute_offset because the photo was taken, according to the
     person that took it, on the date specified. Not the date that it was wherever in the
     world you ran this software to file the photo.
  */
  if (date->tm.tm_year < 1970)
    date->tm.tm_year += 66;
}
