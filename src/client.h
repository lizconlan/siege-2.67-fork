/**
 * Client Header
 *
 * Copyright (C) 2000, 2001, 2002 by
 * Jeffrey Fulmer - <jdfulmer@armstrong.com>
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
#ifndef CLIENT_H
#define CLIENT_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */
#include <setup.h>
#include <joedog/joedog.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_SSL
# include <openssl/ssl.h>
# include <openssl/err.h> 
#endif /* HAVE_SSL */

#include <auth.h>


struct trans
{
  int   bytes;
  int   code;
  float time;
};

/**
 * client data
 */
typedef struct
{
  int      id;
  unsigned long  hits;
  unsigned long  bytes;
  unsigned int   code;
  unsigned int   fail;
  unsigned int   ok200;
  URL    **U;
  struct {
    int  www;
    int  proxy;
    struct {
      int  www;
      int  proxy;
    } bids;
    struct {
      TYPE www;
      TYPE proxy;
    } type;
  } auth;
  int      status;
  float    time;
  unsigned int rand_r_SEED;
  float bigtime;
  float smalltime;
} CLIENT;

void * start_routine(CLIENT *client);

#endif/*CLIENT_H */
