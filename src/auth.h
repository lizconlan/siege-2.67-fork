#ifndef AUTH_H
#define AUTH_H

/**
 * authorization service
 */
typedef enum { WWW,   PROXY  } SERVICE; 

struct LOGIN
{
  const char *realm; 
  const char *username;
  const char *password;
  struct LOGIN *next;
};  


/**
 * authorization type
 */
typedef enum { BASIC, DIGEST } TYPE;

void add_authorization(SERVICE S, char *username, char *password, char *realm); 
int  display_authorization(SERVICE S); 
int  set_authorization(SERVICE S, char *realm); 

#endif/*AUTH_H*/
