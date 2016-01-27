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

#include "date.h"
#include <string.h>
#include <stdio.h>

void fix_iso8601 (date_t* date);

bool read_iso8601 (const char* data, date_t* date)
{
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

  date->valid = false;

  // read an integer starting from data, max 4 chars, delimiter after may be nothing/colon/slash; return next position
  const char* p = read_integer (data,":/-",4,&date->tm.tm_year);
  if (!p)
    return false;

  // this will be either the month or the ordinal day; we'll know in the next read
  p = read_integer (p,":/-",3,&date->tm.tm_mon);
  if (!p)
    return false;

  // the day
  p = read_integer (p,":/-T",2,&date->tm.tm_mday);
  if (!p)
  {
    // we got the first format shown above: 2016-022
    date->tm.tm_yday = date->tm.tm_mday;
    date->tm.tm_mday = 0;
  }
  else
  {
    // if there's an hour, there must be a minute and second
    p = read_integer (p,":/-",2,&date->tm.tm_hour);
    if (p)
    {
      p = read_integer (p,":/-",2,&date->tm.tm_min);
      if (!p)
        return false;
      p = read_integer (p,":/-",2,&date->tm.tm_sec);
      if (!p)
        return false;
    }
  }

  fix_iso8601 (date);

  if (date->tm.tm_year == 0 || date->tm.tm_mon < 1 || date->tm.tm_mon > 12 || date->tm.tm_mday < 1 || date->tm.tm_mday > 31)
      return false;

  date->tm.tm_isdst = -1;    // no dst data available
  date->tm.tm_mon -= 1;      // zero-based months
  date->tm.tm_year -= 1900;  // years are offset from 1900

  // if the time is 1970-01-01 00:00:00, it was probablyt set to "0000:00:00 00:00:00"
  time_t t = mktime (&date->tm);
  if (t == 0)
    return false;

  date->valid = true;
  return true;
}

/* The extractor is calculating ISO 8601 dates from a 1904 epoch. If the year is before 1970,
   it means that the date was encoded from the 1970 epoch, so we add 66 years to correct it.

    https://gnunet.org/bugs/view.php?id=4157
*/
void fix_iso8601 (date_t* date)
{
  if (date->tm.tm_year < 1970)
    date->tm.tm_year += 66;
}

bool read_yyyymmdd (const char* data, date_t* date)
{
  date->valid = false;

  // read an integer starting from data, max 4 chars, delimiter after may be nothing/colon/slash; return next position
  const char* p = read_integer (data,":/",4,&date->tm.tm_year);
  if (!p)
    return false;

  p = read_integer (p,":/",2,&date->tm.tm_mon);
  if (!p)
    return false;

  p = read_integer (p," ",2,&date->tm.tm_mday);
  if (!p)
    return false;

  p = read_integer (p,":",2,&date->tm.tm_hour);
  if (!p)
    return false;

  p = read_integer (p,":",2,&date->tm.tm_min);
  if (!p)
    return false;

  p = read_integer (p,"",2,&date->tm.tm_sec);
  if (!p)
    return false;

  if (date->tm.tm_year == 0 || date->tm.tm_mon < 1 || date->tm.tm_mon > 12 || date->tm.tm_mday < 1 || date->tm.tm_mday > 31)
      return false;

  date->tm.tm_isdst = -1;    // no dst data available
  date->tm.tm_mon -= 1;      // zero-based months
  date->tm.tm_year -= 1900;  // years are offset from 1900

  // if the time is 1970-01-01 00:00:00, it was probablyt set to "0000:00:00 00:00:00"
  time_t t = mktime (&date->tm);
  if (t == 0)
    return false;

  date->valid = true;
  return true;
}
