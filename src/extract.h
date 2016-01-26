#ifndef INC_EXTRACT_H
#define INC_EXTRACT_H

#include "common.h"

bool extract_date (const char* fqpn, date_t* date);

bool init_extractor();
void close_extractor();

#endif // INC_EXTRACT_H
