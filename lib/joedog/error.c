/**
 * Error handling
 * Library: joedog
 *
 * Copyright (C) 2000-2007 by
 * Jeffrey Fulmer - <jeff@joedog.org>, et. al.
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
 *  
 */

#ifdef  HAVE_CONFIG_H
# include <config.h>
#endif/*HAVE_CONFIG_H*/

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 4096

static void   log_message(int status, const char *mode, const char *fmt, va_list ap);
static void error_message(int status, const char *mode, const char *fmt, va_list ap);

void
joe_openlog(char *program)
{
  openlog(program, LOG_PID, LOG_DAEMON); 
  return;
}

void 
joe_closelog(void)
{
  closelog();
  return;
}

/**
 *  error_message    program centric error message
 */
void 
log_message(int status, const char *mode, const char *fmt, va_list ap)
{
  char buf[BUFSIZE];


  vsprintf(buf, fmt, ap);
  if(errno == 0){
    sprintf(buf + strlen(buf), "\n");
  } else {
    sprintf(buf + strlen(buf), ": %s\n", strerror(errno)); 
  }

  syslog(LOG_ERR, "%s: %s", mode, buf);

  if(status >= 0){ exit(status); }
  return;
}

/**
 *  error_message    program centric error message
 */
void
error_message(int status, const char *mode, const char *fmt, va_list ap)
{
  char buf[BUFSIZE/2];
  char msg[BUFSIZE];

  vsprintf(buf, fmt, ap);
  if(errno == 0 || errno == ENOSYS){
    snprintf(msg, sizeof msg, "%s\n", buf);
  } else {
    snprintf(msg, sizeof msg, "%s: %s\n", buf, strerror(errno));
  }

  fflush(stdout);
  fprintf(stderr, "%s: %s", mode, msg);

  if(status >= 0){ exit(status); }
  return;
}


/**
 *  log_warning but continue...
 */
void 
log_warning(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  
  /** 
   * -1, no exit 
   */
  log_message(-1, "warning", fmt, ap);
  va_end(ap);
  
  return;
}

/**
 *  joe_warning warn the user but continue to run
 */
void
joe_warning(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  /** 
   * -1, no exit 
   */
  error_message(-1, "warning", fmt, ap);
  va_end(ap);

  return;
}

/** 
 * log_error    signal error but continue to run
 */
void 
log_error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  /** 
   * -1, no exit 
   */
  log_message(-1, "Error", fmt, ap);
  va_end(ap);

  return;
}

/** 
 * joe_error    signal error but continue to run
 */
void
joe_error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  /** 
   * -1, no exit 
   */
  error_message(-1, "Error", fmt, ap);
  va_end(ap);

  return;
}


/**
 *  log_fatal   EXIT on fatal condition
 */
void 
log_fatal(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  /** 
   * 1, print message and exit 
   */
  log_message(1, "FATAL", fmt, ap);
  va_end(ap);

  return;
}


/**
 *  joe_fatal   EXIT on fatal condition
 */
void
joe_fatal(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  /** 
   * 1, print message and exit 
   */
  error_message(1, "FATAL", fmt, ap);
  va_end(ap);

  return;
}
