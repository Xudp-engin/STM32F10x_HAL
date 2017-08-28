#include <stdio.h>
#include <rt_misc.h>
#include "serial.h"

#pragma import(__use_no_semihosting_swi)


struct __FILE 
{ 
    int handle; /* Add whatever you need here */ 
};

FILE __stdout;
FILE __stdin;


int fputc(int c, FILE *f) 
{
    return (sendchar(c));
}


int fgetc(FILE *f) 
{
    return (getkey());
}


int ferror(FILE *f) 
{
    /* Your implementation of ferror */
    return EOF;
}


void _ttywrch(int c) 
{
    sendchar(c);
}

void _sys_exit(int return_code) 
{
    while(1);
}
