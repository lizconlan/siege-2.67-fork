/**
 * Memory handling
 * Library: joedog
 * 
 * Copyright (C) 2000-2007 by
 * Jeffrey Fulmer - <jeff@joedog.org>
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
 *
 */
#include <config.h>
#include <memory.h>
#include <error.h>
#include <string.h>

char *
xstrdup(const char *str)
{
  char *dupe;
#ifndef HAVE_STRDUP
  int  i;
#endif/*HAVE_STRDUP*/

  if(!str)
    joe_fatal("string has no value!");
#ifdef HAVE_STRDUP
  dupe = strdup(str);
  if(dupe==NULL)
    joe_fatal("xstrdup"); 
#else
  i    = strlen(str);
  dupe = malloc(i + 1);
  if(dupe==NULL)
    joe_fatal("xstrdup");
  memcpy(dupe, str, i + 1);
#endif

  return dupe; 
}

/**
 * xrealloc    replaces realloc
 */
void * 
xrealloc(void *object, size_t size)
{
  void *tmp;
  if(object)
    tmp = realloc(object, size);
  else
    tmp = malloc(size);
  if(tmp==NULL)
    joe_fatal("Memory exhausted");
  return tmp;
} /** end xrealloc **/

/**
 * xmalloc     replaces malloc
 */
void *
xmalloc(size_t size)
{
  void *tmp  =  malloc(size);

  if(tmp==NULL)
    joe_fatal("Memory exhausted");
  
  return tmp;
} /** end of xmalloc **/

/**
 * xcalloc     replaces calloc
 */
void *
xcalloc(size_t num, size_t size)
{
  void *tmp  =  xmalloc(num * size);

  memset(tmp, 0, (num * size));

  return tmp;
} /** end of xcalloc **/

void
xfree(void *ptr)
{
  if(ptr!=NULL){
    free(ptr);
  }
  return;
} 

