/*
 * x11-debug.h
 *
 * Copyright(C) 2018   MEJT
 * 
 * Define debug macros.
 *
 * This  program is free software: you can redistribute it and/or modify  it
 * under  the  terms of the GNU General Public License as published  by  the
 * Free  Software  Foundation, either version 3 of the License, or (at  your
 * option) any later version.
 *
 * This  program  is  distributed in the hope that it will  be  useful,  but
 * WITHOUT   ANY   WARRANTY;   without  even   the   implied   warranty   of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You  should have received a copy of the GNU General Public License  along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 17 Aug 13         - Initial version - MEJT
 * 16 Dec 18         - Re-wrote debug macros to allow debugging to be turned
 *                     on and off as needed by modifying DEBUG - MEJT
 * 26 Dec 18         - Removed non ANSI compliant macro definations - MEJT
 *
 */
 
#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <stdarg.h>
 
/* Execute code if DEBUG is True */
#ifndef debug /* Don't redefine macro if already defined. */
#define debug(code) do {if(DEBUG){code;}} while(0)
 
void print(const char *_fmt, ...) { /* Print formatted error message */
   va_list _args;
   va_start(_args, _fmt);
   fprintf(stderr, "Debug: ");
   vfprintf(stderr, _fmt, _args);
   va_end(_args);
}

#endif

void error(const char *_fmt, ...) { /* Print formatted error message */
   va_list _args;
   va_start(_args, _fmt);
   fprintf(stderr, "%s : ", NAME);
   vfprintf(stderr, _fmt, _args);
   va_end(_args);
}
