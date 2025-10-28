#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>
char* getDateTime();
int generate_unique_no(char *filename,size_t record_size,int baseno);
char* transactionFile(int acc_no);
void trim_whitespace(char *str);
#endif
