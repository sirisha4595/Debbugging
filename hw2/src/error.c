/*
 * Error handling routines
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

int errors;
int warnings;
int dbflag = 1;
void fatal(char* fmt, ...)
{
        va_list ap;
        fprintf(stderr, "\nFatal error: ");
        va_start(ap,fmt);
        vfprintf(stderr,fmt,ap);
        va_end(ap);
        fprintf(stderr, "\n");
        exit(1);
}

void error(char* fmt, ...)
{
        va_list ap;
        fprintf(stderr, "\nError: ");
        va_start(ap,fmt);
        vfprintf(stderr,fmt,ap);
        va_end(ap);
        fprintf(stderr, "\n");
        errors++;
}

void warning(char* fmt, ...)
{
        va_list ap;
        fprintf(stderr, "\nWarning: ");
        va_start(ap,fmt);
        vfprintf(stderr,fmt,ap);
        va_end(ap);
        fprintf(stderr, "\n");
        warnings++;
}

void debug(char* fmt, ...)
{
        if(!dbflag) return;
        va_list ap;
        fprintf(stderr, "\nDebug: ");
        va_start(ap,fmt);
        vfprintf(stderr,fmt,ap);
        va_end(ap);
        fprintf(stderr, "\n");
}
