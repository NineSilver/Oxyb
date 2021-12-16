#ifndef __OXYB__LIB__LOG_H
#define __OXYB__LIB__LOG_H

typedef void (*putchar_fn_t)(char c);

void init_logging(putchar_fn_t log_fn);

void putchar(const char c);
void putstr(const char* s);
void printf(const char* format, ...);

#endif /* !__OXYB__LIB__LOG_H */
