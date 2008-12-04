/**
 * Extra logging support
 *
 * Copyright (C) 2000-2008 by
 * Liz Conlan 
 * This file is distributed as an addition to Siege 
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
#include <setup.h>
#include <fcntl.h>
#include <extralog.h>
#include <data.h>
#include <locale.h>

/**
 * writes the output from siege to a formatted
 * log file.  checks if the log exists, if not
 * it creates a new formatted file and appends
 * text to it.  If a file does exist,  then it
 * simply appends to it. 
 * 
 */

void
log_extra(char *message)
{
  int fd;
  char line[512];
	
  if(!exfile_exists(my.exfile)){
    if(!create_exlog(my.exfile)){
      joe_error("unable to create log file");
      return;
    }
  }

  /* create the log file entry */
  snprintf(line, sizeof line, "%s \n", message);

  /* open the log and write to file */
  if((fd = open( my.exfile, O_WRONLY | O_APPEND, 0644 )) < 0){
    joe_error("Unable to write to file");
    return;
  }

  write(fd, line, strlen(line));
  close(fd);
 
  return;
}  

/**
 * returns TRUE if the file exists,
 */
BOOLEAN
exfile_exists(char *file)
{
  int  fd;

  /* open the file read only  */
  if((fd = open(file, O_RDONLY)) < 0){
  /* the file does NOT exist  */
    close(fd);
    return FALSE;
  } else {
  /* party on Garth... */
    close(fd);
    return TRUE;
  }

  return FALSE;
}

/**
 * return TRUE upon the successful
 * creation of the file, FALSE if not.  The
 * function adds a header at the top of the
 * file, format is comma separated text for
 * spreadsheet import.
 *
 * Liz Conlan - hacked to include Failed 
 * Trans column (Failed column renamed to 
 * Failed Socket)
 */
BOOLEAN
create_exlog(const char *file)
{
  int fd;
	
  if((fd = open(file, O_CREAT | O_WRONLY, 0644)) < 0){
    return FALSE;
  }
  close(fd);
  
  return TRUE;
} 

