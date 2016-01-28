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
#include "date.h"
#include "config.h"
#include <string.h>

#ifdef HAVE_EXTRACTOR_H

#include <extractor.h>

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
      read_iso8601 (data,date);
      return 1;
    break;

    case EXTRACTOR_METATYPE_CREATION_DATE:
      read_yyyymmdd (data,date);
      return 1;
    break;

    case EXTRACTOR_METATYPE_MODIFICATION_DATE:  // hmmm.
    break;

    default:
    break;
  }

  return 0;  // continue
}
#else
bool init_extractor()
{
  return true;
}

void close_extractor()
{
}

bool extract_date (const char* fqpn, date_t* date)
{
  return false;
}
#endif // HAVE_EXTRACTOR_H
