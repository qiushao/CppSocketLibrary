//
// Created by qiushao on 3/11/24.
//
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdarg>

const int BUFFER_SIZE = 4096;
const int FMT_BUFFER_SIZE = 256;

void myLog(FILE *fp, const char *fname, const char *func, uint32_t line, const char *fmt, ...) {
    char buffer[BUFFER_SIZE];
    char fmtBuffer[FMT_BUFFER_SIZE];
    snprintf(fmtBuffer, sizeof(fmtBuffer), "[%s@%s:%d]:%s", basename(fname), func, line, fmt);

    va_list arglist;
    va_start(arglist, fmt);
    vsnprintf(buffer, BUFFER_SIZE, fmtBuffer, arglist);
    va_end(arglist);

    fprintf(fp,"%s\n", buffer);
}
