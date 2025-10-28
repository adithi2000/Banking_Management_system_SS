#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>

char* getDateTime() {
    char *buff = malloc(100);  // Allocate 100 bytes
    if (!buff) return NULL;    // Always check malloc

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buff, 100, "%02d-%02d-%04d %02d:%02d:%02d",
             t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
             t->tm_hour, t->tm_min, t->tm_sec);

    return buff;
}

int generate_unique_no(char *filename,size_t record_size,int baseno){
        int fd=open(filename,O_RDONLY,0644);
        off_t size = lseek(fd,0,SEEK_END);
        int reccount= size/record_size;

        return baseno+reccount+1;
}

char* transactionFile(int acc_no) {
    char* trans = malloc(20);
    if (trans != NULL) {
        snprintf(trans, 20, "trans_his_%d", acc_no);
    }
    return trans;
}
void trim_whitespace(char *str) {
    if (str == NULL) { return; }
    char *start = str;
    while (isspace((unsigned char)*start)) { start++; }
    if (*start == '\0') { *str = '\0'; return; }
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) { end--; }
    *(end + 1) = '\0';
    if (start != str) { memmove(str, start, end - start + 2); }
}
