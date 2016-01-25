#ifndef INC_MAIN_H
#define INC_MAIN_H

#include <stdbool.h>

void process_dir (const char* dir);
void process_file (const char* dir, const char* name, const char* fqpn);
void process_operation(const char* fqpn, const char* dst_dir, const char* dst_name);
bool format_dst (const char* base_dir, const char* date, const char* dst_name, char* dst_dir, char* dst_fqpn);
bool to_long (const char* s, long* l);

#endif // INC_MAIN_H
