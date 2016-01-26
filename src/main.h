#ifndef INC_MAIN_H
#define INC_MAIN_H

#include "common.h"

void process_dir (const char* dir);
void process_file (const char* dir, const char* name, const char* fqpn);
void process_operation(const char* fqpn, const char* dst_dir, const char* dst_name);
bool format_dst (const char* base_dir, const date_t* date, const char* dst_name, char* dst_dir, char* dst_fqpn);

#endif // INC_MAIN_H
