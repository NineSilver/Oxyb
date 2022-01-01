#ifndef __OXYB__LIBK__LOG_H
#define __OXYB__LIBK__LOG_H

#include <misc.h>

typedef void (*putchar_fn_t)(char c);

enum loglevel
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DONE,
    LOG_NOPREFIX
};

void init_logging(putchar_fn_t log_fn);

void kputchar(const char c);
void kputstr(const char* s);

void klog(int loglevel, const char* message, ...);

#define panic(msg, ...)                 \
    klog(LOG_ERROR, msg, ##__VA_ARGS__);  \
    halt();

#endif /* !__OXYB__LIBK__LOG_H */
