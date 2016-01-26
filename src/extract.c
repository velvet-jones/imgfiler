#include "extract.h"
#include <extractor.h>
#include <string.h>

int process_data(void *cls,const char *plugin_name, enum EXTRACTOR_MetaType type, enum EXTRACTOR_MetaFormat format,
                 const char *data_mime_type, const char *data,size_t data_len);

struct EXTRACTOR_PluginList* plugins = 0;

bool extract_date (const char* fqpn, date_t* date)
{
  if (!plugins)
    return false;

  memset (date,0,sizeof(date_t));
  date->valid = false;

  EXTRACTOR_extract (plugins, fqpn, NULL, 0, &process_data, date);

  return date->valid;
}

bool init_extractor()
{
  if (plugins != 0)
    return false;

  plugins = EXTRACTOR_plugin_add_defaults (EXTRACTOR_OPTION_DEFAULT_POLICY);
  return plugins != 0;
}

void close_extractor()
{
  if (plugins)
    EXTRACTOR_plugin_remove_all (plugins);
  plugins = 0;
}

int process_data(void *cls,const char *plugin_name, enum EXTRACTOR_MetaType type, enum EXTRACTOR_MetaFormat format,
                 const char *data_mime_type, const char *data,size_t data_len)
{
  date_t* date = (date_t*) cls;

  switch (type)
  {
    case EXTRACTOR_METATYPE_CREATION_TIME:
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
      date->tm.tm_isdst = -1;  // no dst data available
      date->tm.tm_mon -= 1;    // zero-based months
      date->tm.tm_year -= 1900;  // years are offset from 1900
      return 1;
    break;

    default:
    break;
  }

  return 0;  // continue
}
