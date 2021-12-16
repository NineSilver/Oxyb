#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <log.h>
#include <string.h>

static  putchar_fn_t __internal_putchar_fn = 0;

void init_logging(putchar_fn_t log_fn)
{
    __internal_putchar_fn = log_fn;
}

void putchar(const char c)
{
    __internal_putchar_fn(c);
}

void putstr(const char* s)
{
    size_t i;
    for(i = 0; s[i]; i++)
    {
        putchar(s[i]);
    }
}

void printf(const char* format,...)
{
    va_list args;
    va_start(args, format);

    const char* i = format;

    while(*i)
    {
        if(*i == '%')
        {
            i++;

            switch(*i)
            {
                case 'c':
                    putchar((char)va_arg(args, int));
                    break;

                case 'd':
                case 'i':
                {
                    int num = va_arg(args, int);
                    if(num < 0)
                    {
                        num = -num;
                        putchar('-');
                    }
                    putstr(itoa(num, 10));
                    break;
                }

                case 'l':
                {
                    long num = va_arg(args, long);
                    if(num < 0)
                    {
                        num = -num;
                        putchar('-');
                    }
                    putstr(itoa(num, 10));
                    break;
                }

                case 's':
                    putstr(va_arg(args, const char*));
                    break;

                case 'x':
                    putstr("0x");
                    putstr(itoa(va_arg(args, unsigned int), 16));
                    break;

                case '%':
                    putchar('%');
                    break;

                default:
                    putchar('?');
                    break;
            }
        }
        else
        {
            putchar(*i);
        }

        i++;
    }

    va_end(args);
}
