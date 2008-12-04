/**
 * Load Post Data
 *
 * Copyright (C) 2002-2007 by
 * Jeffrey Fulmer - <jeff@joedog.org>, et al. 
 * This file is distributed as part of Siege
 *
 * Copyright (c) Ian F. Darwin 1986-1995.
 * Software written by Ian F. Darwin and others;
 * maintained 1995-present by Christos Zoulas and others.
 * (see: file-4.03 from ftp.astron.com for details)
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
#include <stdio.h> 
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <load.h>
#include <errno.h>
#include <util.h>
#include <joedog/joedog.h>

typedef unsigned long unichar;

#define F 0   /* character never appears in text */
#define T 1   /* character appears in plain ASCII text */
#define I 2   /* character appears in ISO-8859 text */
#define X 3   /* character appears in non-ISO extended ASCII (Mac, IBM PC) */

static char text_chars[256] = 
{
  F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  /* 0x0X */
  F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  /* 0x1X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x2X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x3X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x4X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x5X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  /* 0x6X */
  T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  /* 0x7X */
  X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  /* 0x8X */
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  /* 0x9X */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xaX */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xbX */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xcX */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xdX */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  /* 0xeX */
  I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   /* 0xfX */
};

struct ContentType {
  char *ext;
  char *type;
};

static const struct ContentType tmap[] = {
  {"default", "application/x-www-form-urlencoded"},
  {".ai",     "application/postscript"},
  {".aif",    "audio/x-aiff"},
  {"aifc",    "audio/x-aiff"},
  {"aiff",    "audio/x-aiff"},
  {"asc",     "text/plain"},
  {"au",      "audio/basic"},
  {"avi",     "video/x-msvideo"},
  {"bcpio",   "application/x-bcpio"},
  {"bin",     "application/octet-stream"},
  {"c",       "text/plain"},
  {"cc",      "text/plain"},
  {"ccad",    "application/clariscad"},
  {"cdf",     "application/x-netcdf"},
  {"class",   "application/octet-stream"},
  {"cpio",    "application/x-cpio"},
  {"cpt",     "application/mac-compactpro"},
  {"csh",     "application/x-csh"},
  {"css",     "text/css"},
  {"dcr",     "application/x-director"},
  {"dir",     "application/x-director"},
  {"dms",     "application/octet-stream"},
  {"doc",     "application/msword"},
  {"drw",     "application/drafting"},
  {"dvi",     "application/x-dvi"},
  {"dwg",     "application/acad"},
  {"dxf",     "application/dxf"},
  {"dxr",     "application/x-director"},
  {"eps",     "application/postscript"},
  {"etx",     "text/x-setext"},
  {"exe",     "application/octet-stream"},
  {"ez",      "application/andrew-inset"},
  {"f",       "text/plain"},
  {"f90",     "text/plain"},
  {"fli",     "video/x-fli"},
  {"gif",     "image/gif"},
  {"gtar",    "application/x-gtar"},
  {"gz",      "application/x-gzip"},
  {"h",       "text/plain"},
  {"hdf",     "application/x-hdf"},
  {"hh",      "text/plain"},
  {"hqx",     "application/mac-binhex40"},
  {"htm",     "text/html"},
  {"html",    "text/html"},
  {"ice",     "x-conference/x-cooltalk"},
  {"ief",     "image/ief"},
  {"iges",    "model/iges"},
  {"igs",     "model/iges"},
  {"ips",     "application/x-ipscript"},
  {"ipx",     "application/x-ipix"},
  {"jpe",     "image/jpeg"},
  {"jpeg",    "image/jpeg"},
  {"jpg",     "image/jpeg"},
  {"js",      "application/x-javascript"},
  {"kar",     "audio/midi"},
  {"latex",   "application/x-latex"},
  {"lha",     "application/octet-stream"},
  {"lsp",     "application/x-lisp"},
  {"lzh",     "application/octet-stream"},
  {"m",       "text/plain"},
  {"man",     "application/x-troff-man"},
  {"me",      "application/x-troff-me"},
  {"mesh",    "model/mesh"},
  {"mid",     "audio/midi"},
  {"midi",    "audio/midi"},
  {"mif",     "application/vnd.mif"},
  {"mime",    "www/mime"},
  {"mov",     "video/quicktime"},
  {"movie",   "video/x-sgi-movie"},
  {"mp2",     "audio/mpeg"},
  {"mp3",     "audio/mpeg"},
  {"mpe",     "video/mpeg"},
  {"mpeg",    "video/mpeg"},
  {"mpg",     "video/mpeg"},
  {"mpga",    "audio/mpeg"},
  {"ms",      "application/x-troff-ms"},
  {"msh",     "model/mesh"},
  {"nc",      "application/x-netcdf"},
  {"oda",     "application/oda"},
  {"pbm",     "image/x-portable-bitmap"},
  {"pdb",     "chemical/x-pdb"},
  {"pdf",     "application/pdf"},
  {"pgm",     "image/x-portable-graymap"},
  {"pgn",     "application/x-chess-pgn"},
  {"png",     "image/png"},
  {"pnm",     "image/x-portable-anymap"},
  {"pot",     "application/mspowerpoint"},
  {"ppm",     "image/x-portable-pixmap"},
  {"pps",     "application/mspowerpoint"},
  {"ppt",     "application/mspowerpoint"},
  {"ppz",     "application/mspowerpoint"},
  {"pre",     "application/x-freelance"},
  {"prt",     "application/pro_eng"},
  {"ps",      "application/postscript"},
  {"qt",      "video/quicktime"},
  {"ra",      "audio/x-realaudio"},
  {"ram",     "audio/x-pn-realaudio"},
  {"ras",     "image/cmu-raster"},
  {"rgb",     "image/x-rgb"},
  {"rm",      "audio/x-pn-realaudio"},
  {"roff",    "application/x-troff"},
  {"rpm",     "audio/x-pn-realaudio-plugin"},
  {"rtf",     "text/rtf"},
  {"rtx",     "text/richtext"},
  {"scm",     "application/x-lotusscreencam"},
  {"set",     "application/set"},
  {"sgm",     "text/sgml"},
  {"sgml",    "text/sgml"},
  {"sh",      "application/x-sh"},
  {"shar",    "application/x-shar"},
  {"silo",    "model/mesh"},
  {"sit",     "application/x-stuffit"},
  {"skd",     "application/x-koan"},
  {"skm",     "application/x-koan"},
  {"skp",     "application/x-koan"},
  {"skt",     "application/x-koan"},
  {"smi",     "application/smil"},
  {"smil",    "application/smil"},
  {"snd",     "audio/basic"},
  {"sol",     "application/solids"},
  {"spl",     "application/x-futuresplash"},
  {"src",     "application/x-wais-source"},
  {"step",    "application/STEP"},
  {"stl",     "application/SLA"},
  {"stp",     "application/STEP"},
  {"sv4cpio", "application/x-sv4cpio"},
  {"sv4crc",  "application/x-sv4crc"},
  {"swf",     "application/x-shockwave-flash"},
  {"t",       "application/x-troff"},
  {"tar",     "application/x-tar"},
  {"tcl",     "application/x-tcl"},
  {"tex",     "application/x-tex"},
  {"texi",    "application/x-texinfo"},
  {"texinfo", "application/x-texinfo"},
  {"tif",     "image/tiff"},
  {"tiff",    "image/tiff"},
  {"tr",      "application/x-troff"},
  {"tsi",     "audio/TSP-audio"},
  {"tsp",     "application/dsptype"},
  {"tsv",     "text/tab-separated-values"},
  {"txt",     "text/plain"},
  {"unv",     "application/i-deas"},
  {"ustar",   "application/x-ustar"},
  {"vcd",     "application/x-cdlink"},
  {"vda",     "application/vda"},
  {"viv",     "video/vnd.vivo"},
  {"vivo",    "video/vnd.vivo"},
  {"vrml",    "model/vrml"},
  {"wav",     "audio/x-wav"},
  {"wrl",     "model/vrml"},
  {"xbm",     "image/x-xbitmap"},
  {"xlc",     "application/vnd.ms-excel"},
  {"xll",     "application/vnd.ms-excel"},
  {"xlm",     "application/vnd.ms-excel"},
  {"xls",     "application/vnd.ms-excel"},
  {"xlw",     "application/vnd.ms-excel"},
  {"xml",     "text/xml"},
  {"xpm",     "image/x-xpixmap"},
  {"xwd",     "image/x-xwindowdump"},
  {"xyz",     "chemical/x-pdb"},
  {"zip",     "application/zip"}
};

char * 
get_content_type(char *filename)
{
  int i;

  for(i=0; i < (int)sizeof(tmap) / (int)sizeof(tmap[0]); i++) {
    if(strlen(filename) >= strlen(tmap[i].ext)) {
      if(strmatch(((char*)filename + strlen(filename) - strlen(tmap[i].ext)), (char*)tmap[i].ext)) {
        return tmap[i].type;
      }
    }
  }
  return tmap[0].type;
}

int
looks_ascii(const unsigned char *buf, size_t nbytes, unichar *ubuf, size_t *ulen)
{
  size_t  i;
  *ulen = 0;

  for (i = 0; i < nbytes; i++) {
    int t = text_chars[buf[i]];
    if (t != T)
      return 0;
    ubuf[(*ulen)++] = buf[i];
  }
  return 1;
}


int
looks_latin1(const unsigned char *buf, size_t nbytes, unichar *ubuf, size_t *ulen)
{
  size_t i;
  *ulen = 0;

  for(i = 0; i < nbytes; i++){
    int t = text_chars[buf[i]];
    if(t != T && t != I)
      return 0;

    ubuf[(*ulen)++] = buf[i];
  }
  return 1;
}

int
looks_utf8(const unsigned char *buf, size_t nbytes, unichar *ubuf, size_t *ulen)
{
  size_t i;
  int    n;
  unichar c;
  int gotone = 0;
  *ulen = 0;

  for(i = 0; i < nbytes; i++){
    if((buf[i] & 0x80) == 0){                 /* 0xxxxxxx is plain ASCII */
      if (text_chars[buf[i]] != T)
        return 0;
        ubuf[(*ulen)++] = buf[i];
      } else if ((buf[i] & 0x40) == 0) {      /* 10xxxxxx never 1st byte */
        return 0;
      } else {                                /* 11xxxxxx begins UTF-8 */
        int following;
        if((buf[i] & 0x20) == 0){             /* 110xxxxx */
          c = buf[i] & 0x1f;
          following = 1;
        } else if ((buf[i] & 0x10) == 0) {    /* 1110xxxx */
          c = buf[i] & 0x0f;
          following = 2;
       } else if ((buf[i] & 0x08) == 0) {      /* 11110xxx */
          c = buf[i] & 0x07;
          following = 3;
       } else if ((buf[i] & 0x04) == 0) {      /* 111110xx */
          c = buf[i] & 0x03;
          following = 4;
       } else if ((buf[i] & 0x02) == 0) {      /* 1111110x */
          c = buf[i] & 0x01;
          following = 5;
       } else
          return 0;

       for(n = 0; n < following; n++){
         i++;
         if(i >= nbytes)
           goto done;
         if((buf[i] & 0x80) == 0 || (buf[i] & 0x40))
           return 0;
         c = (c << 6) + (buf[i] & 0x3f);
       }
     ubuf[(*ulen)++] = c;
     gotone = 1;
   }
 }
done:
 return gotone;   /* don't claim it's UTF-8 if it's all 7-bit */
}


int
looks_extended(const unsigned char *buf, size_t nbytes, unichar *ubuf, size_t *ulen)
{
  size_t  i;
  *ulen = 0;

  for(i = 0; i < nbytes; i++){
    int t = text_chars[buf[i]];
    if(t != T && t != I && t != X)
      return 0;
    ubuf[(*ulen)++] = buf[i];
  }
  return 1;
}

int
looks_unicode(const unsigned char *buf, size_t nbytes, unichar *ubuf, size_t *ulen)
{
  int    bigend;
  size_t i;

  if(nbytes < 2)
    return 0;

  if(buf[0] == 0xff && buf[1] == 0xfe)
    bigend = 0;
  else if (buf[0] == 0xfe && buf[1] == 0xff)
    bigend = 1;
  else
    return 0;

  *ulen = 0;

  for(i = 2; i + 1 < nbytes; i += 2){
    if(bigend)
      ubuf[(*ulen)++] = buf[i + 1] + 256 * buf[i];
    else
      ubuf[(*ulen)++] = buf[i] + 256 * buf[i + 1];
    if(ubuf[*ulen - 1] == 0xfffe)
      return 0;
    if(ubuf[*ulen - 1] < 128 && text_chars[(size_t)ubuf[*ulen - 1]] != T)
      return 0;
  }
  return 1 + bigend;
}

/**
 * maps a file to our address space 
 * and returns it the calling function.
 */
void 
load_file(URL *U, char *file)
{
  FILE     *fp;
  size_t   len = 0;
  struct   stat st; 
  char     *filename;
  char     postdata[POST_BUF]; 
  unichar  ubuf[POST_BUF+1];   
  size_t   ulen;

  filename = trim(file);
  memset(postdata, 0, POST_BUF);

  if((lstat(filename, &st) == 0) || (errno != ENOENT)){ 
    len = st.st_size;  
    if((fp = fopen(filename, "r")) == NULL){
      joe_error("could not open file: %s", filename);
      return;
    }
    if((fread(postdata, 1, len, fp )) == len){
      if(looks_ascii((unsigned char*)postdata,len,ubuf,&ulen))
        trim(postdata);
      else if(looks_utf8((unsigned char*)postdata,len,ubuf,&ulen))
        trim(postdata);
    } else {
      joe_error( "unable to read file: %s", filename );
    }
    fclose(fp);
  }

  if(strlen(postdata) > 0){
    U->conttype = xstrdup(get_content_type(filename));
    U->postlen  = strlen(postdata);
    U->postdata = malloc(U->postlen);
    memcpy(U->postdata, postdata, U->postlen);
    U->postdata[U->postlen] = 0;
  } 
  return;
}

#undef F
#undef T
#undef I
#undef X

