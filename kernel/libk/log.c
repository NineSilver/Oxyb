#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <log.h>
#include <string.h>

static putchar_fn_t __internal_putchar_fn = 0;

void init_logging(putchar_fn_t log_fn)
{
    __internal_putchar_fn = log_fn;
}

void kputchar(const char c)
{
    __internal_putchar_fn(c);
}

void kputstr(const char* s)
{
    size_t i;
    for(i = 0; s[i]; i++)
        kputchar(s[i]);
}

static void kprint(const char* format, va_list args)
{
    const char* i = format;

    while(*i)
    {
        if(*i == '%')
        {
            i++;

            switch(*i)
            {
                case 'c':
                    kputchar((char)va_arg(args, int));
                    break;

                case 'd':
                case 'i':
                {
                    int num = va_arg(args, int);
                    if(num < 0)
                    {
                        num = -num;
                        kputchar('-');
                    }
                    kputstr(itoa(num, 10));
                    break;
                }

                case 'l':
                {
                    long num = va_arg(args, long);
                    if(num < 0)
                    {
                        num = -num;
                        kputchar('-');
                    }
                    kputstr(itoa(num, 10));
                    break;
                }

                case 's':
                    kputstr(va_arg(args, const char*));
                    break;

                case 'x':
                    kputstr("0x");
                    kputstr(itoa(va_arg(args, unsigned int), 16));
                    break;

                case '%':
                    kputchar('%');
                    break;

                default:
                    kputchar('?');
                    break;
            }
        }
        else
        {
            kputchar(*i);
        }

        i++;
    }
}

void klog(int loglevel, const char* message, ...)
{
    switch(loglevel)
    {
        case LOG_INFO:
            kputstr("[" "\033[94m" "INFO" "\033[97m" "] ");
            break;

        case LOG_WARN:
            kputstr("[" "\033[93m" "WARN" "\033[97m" "] ");
            break;

        case LOG_ERROR:
            kputstr("[" "\033[31m" "ERROR" "\033[97m" "] ");
            break;

        case LOG_DONE:
            kputstr("[" "\033[92m" "DONE" "\033[97m" "] ");
            break;
        
        case LOG_NOPREFIX:
            break;
    }

    va_list args;
    va_start(args, message);

    kprint(message, args);
    
    va_end(args);
}
