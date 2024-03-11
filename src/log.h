//
// Created by qiushao on 3/11/24.
//

#ifndef CPPSOCKETLIBRARY_LOG_H
#define CPPSOCKETLIBRARY_LOG_H

#include <cstdint>

#ifndef LOGD
#define LOGD(...) myLog(stdout, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define LOGE(...) myLog(stderr, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#endif

void myLog(FILE *fp, const char *fname, const char *func, uint32_t line, const char *fmt, ...);
#endif //CPPSOCKETLIBRARY_LOG_H
