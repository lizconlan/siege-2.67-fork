/**
 * Error handling
 * Library: joedog
 *
 * Copyright (C) 2000-2007 by
 * Jeffrey Fulmer - <jeff@joedog.org>, et al.
 * This file is distributed as part of Siege 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * -- 
 */
#ifndef ERROR_H
#define ERROR_H 

void joe_openlog(void);
void joe_closelog(void);  

/**
 * write warning to stderr & continue; something
 * peculiar has occured...
 */
extern void joe_warning(const char *fmt, ...);

/**
 * log error to syslogd & continue
 */
extern void log_warning(const char *fmt, ...);

/**
 * write error to stderr & continue; something
 * failed but it is not necessarily fatal...
 */
extern void joe_error(const char *fmt, ...);

/**
 * log error to syslogd & die
 */
extern void log_error(const char *fmt, ...);

/**
 * write error to stderr & die; an event prevents
 * the program from continuing...
 */
extern void joe_fatal(const char *fmt, ...);

/**
 * log the error to syslogd & die
 */
extern void log_fatal(const char *fmt, ...);

#endif  /* ERROR_H */  
