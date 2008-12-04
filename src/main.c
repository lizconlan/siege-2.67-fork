/**
 * Siege, http regression tester / benchmark utility
 *
 * Copyright (C) 2000-2007 by  
 * Jeffrey Fulmer - <jeff@joedog.org>, etc al. 
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
 * 
 *
 */
#define  INTERN  1

#ifdef  HAVE_CONFIG_H
# include <config.h>
#endif/*HAVE_CONFIG_H*/

#ifdef  HAVE_PTHREAD_H
# include <pthread.h>
#endif/*HAVE_PTHREAD_H*/

/*LOCAL HEADERS*/
#include <setup.h>
#include <handler.h>
#include <timer.h>
#include <client.h>
#include <util.h>
#include <log.h>
#include <extralog.h>
#include <init.h>
#include <cfg.h>
#include <url.h>
#include <ssl.h>
#include <cookie.h>
#include <crew.h>
#include <data.h>
#include <version.h>
#include <sys/resource.h>
#ifdef __CYGWIN__
# include <getopt.h>
#else
# include <joedog/getopt.h>
#endif 

/**
 * long options, std options struct
 */
static struct option long_options[] =
{
  { "version",    no_argument,       NULL, 'V' },
  { "help",       no_argument,       NULL, 'h' },
  { "verbose",    no_argument,       NULL, 'v' },
  { "config",     no_argument,       NULL, 'C' },
  { "debug",      no_argument,       NULL, 'D' },
  { "get",        no_argument,       NULL, 'g' },
  { "concurrent", required_argument, NULL, 'c' },
  { "internet",   no_argument,       NULL, 'i' },
  { "benchmark",  no_argument,       NULL, 'b' },
  { "reps",       required_argument, NULL, 'r' },
  { "time",       required_argument, NULL, 't' },
  { "delay",      required_argument, NULL, 'd' },
  { "log",        no_argument,       NULL, 'l' },
  { "file",       required_argument, NULL, 'f' },
  { "rc",         required_argument, NULL, 'R' }, 
  { "url",        required_argument, NULL, 'u' },
  { "mark",       required_argument, NULL, 'm' },
  { "header",     required_argument, NULL, 'H' },
  { "user-agent", required_argument, NULL, 'A' },
  { "extra",      required_argument, NULL, 'E' }
};

/**
 * display_version   
 * displays the version number and exits on boolean false. 
 * continue running? TRUE=yes, FALSE=no
 * return void
 */

void 
display_version(BOOLEAN b)
{
  /**
   * version_string is defined in version.c 
   * adding it to a separate file allows us
   * to parse it in configure.  
   */
  char name[128]; 

  memset(name, 0, sizeof name);
  strncpy(name, program_name, strlen(program_name));

  if(my.debug){
    fprintf(stderr,"%s %s: debugging enabled\n\n%s\n", uppercase(name, strlen(name)), version_string, copyright);
  } else {
    if(b == TRUE){
      fprintf(stderr,"%s %s\n\n%s\n", uppercase(name, strlen(name)), version_string, copyright);
      exit(EXIT_SUCCESS);
    } else {
      fprintf(stderr,"%s %s\n", uppercase(name, strlen(name)), version_string);
    }
  }
}  /* end of display version */

/**
 * display_help 
 * displays the help section to STDOUT and exits
 */ 
void 
display_help()
{
  /**
   * call display_version, but do not exit 
   */
  display_version(FALSE); 
  printf("Usage: %s [options]\n", program_name);
  printf("       %s [options] URL\n", program_name);
  printf("       %s -g URL\n", program_name);
  printf("Options:\n"                    );
  puts("  -V, --version           VERSION, prints version number to screen.");
  puts("  -h, --help              HELP, prints this section.");
  puts("  -C, --config            CONFIGURATION, show the current configuration.");
  puts("  -v, --verbose           VERBOSE, prints notification to screen.");
  puts("  -g, --get               GET, pull down headers from the server and display HTTP");
  puts("                          transaction. Great for web application debugging.");
  puts("  -c, --concurrent=NUM    CONCURRENT users, default is 10");
  puts("  -u, --url=\"URL\"         Deprecated. Set URL as the last argument." );
  puts("  -i, --internet          INTERNET user simulation, hits the URLs randomly." );
  puts("  -b, --benchmark         BENCHMARK, signifies no delay for time testing." );
  puts("  -t, --time=NUMm         TIME based testing where \"m\" is the modifier S, M, or H" );
  puts("                          no space between NUM and \"m\", ex: --time=1H, one hour test." );
  puts("  -r, --reps=NUM          REPS, number of times to run the test, default is 25" );
  puts("  -f, --file=FILE         FILE, change the configuration file to file." );
  printf("  -R, --rc=FILE           RC, change the %src file to file.  Overrides\n",program_name);
  puts("                          the SIEGERC environmental variable.");
  printf("  -l, --log               LOG, logs the transaction to PREFIX/var/%s.log\n", program_name);
  puts("  -m, --mark=\"text\"       MARK, mark the log file with a string separator." );
  puts("  -d, --delay=NUM         Time DELAY, random delay between 1 and num designed" );
  puts("                          to simulate human activity. Default value is 3" );
  puts("  -H, --header=\"text\"     Add a header to request (can be many)" ); 
  puts("  -A, --user-agent=\"text\" Sets User-Agent in request" ); 
  puts("  -E, --extra=FILE        EXTRA, writes notification to the specified file");
  /**
   * our work is done, exit nicely
   */
  exit( EXIT_SUCCESS );
}

/* Check the command line for the presence of the -R or --RC switch.  We
 * need to do this seperately from the other command line switches because
 * the options are initialized from the .siegerc file before the command line
 * switches are parsed. The argument index is reset before leaving the
 * function. */
void 
parse_rc_cmdline(int argc, char *argv[])
{
  int a = 0;
  strcpy(my.rc, "");
  
  while( a > -1 ){
    a = getopt_long(argc, argv, "VhvCDglibr:t:f:d:c:u:m:H:R:A:E:", long_options, (int*)0);
    if(a == 'R'){
      strcpy(my.rc, optarg);
      a = -1;
    }
  }
  optind = 0;
} 

/**
 * parses command line arguments and assigns
 * values to run time variables. relies on GNU
 * getopts included with this distribution.  
 */ 
void 
parse_cmdline(int argc, char *argv[])
{
  int c = 0;
  int nargs;
  while((c = getopt_long( argc, argv, "VhvCDglibr:t:f:d:c:u:m:H:R:A:E:", 
          long_options, (int *)0)) != EOF){
  switch(c){
      case 'V':
        display_version(TRUE);
        break;
      case 'h':
        display_help();
        exit(EXIT_SUCCESS);
      case 'D':
        my.debug = TRUE;
        break;
      case 'C':
        my.config = TRUE;
        break;
      case 'c':
        my.cusers  = atoi(optarg);
        break;
      case 'i':
        my.internet = TRUE;
        break;
      case 'b':
        my.bench    = TRUE;
        break;
      case 'd':
	/* XXX range checking? use strtol? */
        my.delay   = atoi(optarg);
	if(my.delay < 0){
	  my.delay = 0; 
	}
        break;
      case 'g':
        my.get = TRUE;
        break;
      case 'l':
        my.logging = TRUE;
        break;
      case 'm':
        my.mark    = TRUE;
        my.markstr = optarg;
        my.logging = TRUE; 
        break;
      case 'v':
        my.verbose = TRUE;
        break;
      case 'r':
        my.reps = atoi(optarg);
        break;
      case 't':
        parse_time(optarg);
        break;
      case 'f':
        memset(my.file, 0, sizeof(my.file));
        if(optarg == NULL) break; /*paranoia*/
        strncpy(my.file, optarg, strlen(optarg));
        break;
      case 'u':
        printf("-u has been deprecated.\n");
        display_help(); 
        exit(1);
        break;
      case 'A':
        strncpy(my.uagent, optarg, 255);
        break;
      case 'R':  
        /**
         * processed above 
         */
        break; 
      case 'H':
        {
          if(!strchr(optarg,':')) joe_fatal("no ':' in http-header");
          if((strlen(optarg) + strlen(my.extra) + 3) > 512)
              joe_fatal("too many headers");
          strcat(my.extra,optarg);
          strcat(my.extra,"\015\012");
        }
        break;
      case 'E':
	    my.extralog = TRUE;
		memset(my.exfile, 0, sizeof(my.exfile));
        if(optarg == NULL) break; /*paranoia*/
        strncpy(my.exfile, optarg, strlen(optarg));
		break;
    } /* end of switch( c )           */
  }   /* end of while c = getopt_long */
  nargs = argc - optind;
  if(nargs)
    my.url = xstrdup(argv[argc-1]); 
  if(my.get && my.url==NULL){
    puts("ERROR: -g/--get requires a commandline URL");
    exit(1);
  }
  return;
} /* end of parse_cmdline */

int 
main(int argc, char *argv[])
{
  int            x; 
  int            result;
  DATA           D = new_data();
  CREW           crew;           /* thread pool - crew.h */
  LINES          *lines;         /* defined in setup.h   */
  CLIENT         *client;        /* defined in setup.h   */
  URL            **urls;         /* urls struct array    */
  pthread_t      cease;          /* the shutdown thread  */
  pthread_t      timer;          /* the timer thread     */
  pthread_attr_t scope_attr;     /* set to PTHREAD_SCOPE_SYSTEM */
  unsigned int   randrseed;      /* seed pthread_rand_np */
  char           exlogmsg[512];
  void *statusp;
  sigset_t sigs;

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGHUP);
  sigaddset(&sigs, SIGINT);
  sigaddset(&sigs, SIGALRM);
  sigaddset(&sigs, SIGTERM);
  sigprocmask(SIG_BLOCK, &sigs, NULL);

  lines = xcalloc(sizeof(LINES),1);
  lines->index   = 0;
  lines->line    = NULL;

  memset(&my, 0, sizeof(struct CONFIG));

  parse_rc_cmdline(argc, argv); 
  if(init_config() < 0){        /* defined in init.h   */
    exit( EXIT_FAILURE );       /* polly was a girl... */
  } 
  parse_cmdline(argc, argv);    /* defined above       */
  ds_module_check();            /* check config integ  */

  if(my.get){
    my.cusers  = 1;
    my.reps    = 1;
    my.logging = FALSE;
    my.bench   = TRUE;
  } 

  if(my.config){
    show_config(TRUE);    
  }

  if(my.url != NULL){
    my.length = 1; 
  } else { 
    my.length = read_cfg_file( lines, my.file ); 
  }

  if(my.length == 0){ 
    display_help();
  }

  /* cookie is an EXTERN, defined in setup */ 
  cookie = xcalloc(sizeof(COOKIE), 1); 
  cookie->first = NULL;
  if((result = pthread_mutex_init( &(cookie->mutex), NULL)) !=0){
    joe_fatal( "pthread_mutex_init" );
  } 

  /* memory allocation for threads and clients */
  urls   = xmalloc(my.length * sizeof(URL));
  client = xmalloc(sizeof(CLIENT) * my.cusers);
  if((crew = new_crew(my.cusers, my.cusers, FALSE)) == NULL){
    joe_fatal("unable to allocate memory for %d simulated browser", my.cusers);  
  }

  /** 
   * determine the source of the url(s),
   * command line or file, and add them
   * to the urls struct.
   */
  if(my.url != NULL){
    urls[0]   =  add_url(my.url, 1);         /* from command line  */
  } else { 
    for(x = 0; x < my.length; x ++){
      urls[x] =  add_url(lines->line[x], x); /* from urls.txt file */
    }
  } 

  /**
   * display information about the siege
   * to the user and prepare for verbose 
   * output if necessary.
   */
  if(!my.get){
    fprintf(stderr, "** "); 
    display_version(FALSE);
    fprintf(stderr, "** Preparing %d concurrent users for battle.\n", my.cusers);
    fprintf(stderr, "The server is now under siege...");
    if(my.verbose){ fprintf(stderr, "\n"); }
	if(my.extralog) { 
		snprintf(exlogmsg, sizeof exlogmsg, "** Preparing %d concurrent users for battle.", my.cusers);
		log_extra(exlogmsg);
		log_extra("The server is now under siege...");
	}
  }

  /**
   * record start time before spawning threads
   * as the threads begin hitting the server as
   * soon as they are created.
   */
  data_set_start(D);

  /**
   * for each concurrent user, spawn a thread and
   * loop until condition or pthread_cancel from the
   * handler thread.
   */
  pthread_attr_init(&scope_attr);
  pthread_attr_setscope(&scope_attr, PTHREAD_SCOPE_SYSTEM);
#if defined(_AIX)
  /* AIX, for whatever reason, defies the pthreads standard and  *
   * creates threads detached by default. (see pthread.h on AIX) */
  pthread_attr_setdetachstate(&scope_attr, PTHREAD_CREATE_JOINABLE);
#endif

  /** 
   * invoke OpenSSL's thread safety
   */
#ifdef HAVE_SSL
  SSL_thread_setup();
#endif

  /**
   * create the signal handler and timer;  the
   * signal handler thread (cease) responds to
   * ctrl-C (sigterm) and the timer thread sends
   * sigterm to cease on time out.
   */
  if((result = pthread_create(&cease, NULL, (void*)sig_handler, (void*)crew)) < 0){
    joe_fatal("failed to create handler: %d\n", result);
  }
  if(my.secs > 0){
    if((result = pthread_create(&timer, NULL, (void*)siege_timer, (void*)cease)) < 0){
      joe_fatal("failed to create handler: %d\n", result);
    } 
  }
  
  randrseed = time(0);

  /**
   * loop until my.cusers and create a corresponding thread...
   */  
  for(x = 0; x < my.cusers && crew_get_shutdown(crew) != TRUE; x++){
    client[x].id              = x; 
    client[x].bytes           = 0;
    client[x].time            = 0.0;
    client[x].hits            = 0;
    client[x].code            = 0;
    client[x].ok200           = 0;   
    client[x].fail            = 0; 
    client[x].U               = urls;
    client[x].auth.www        = 0;
    client[x].auth.proxy      = 0;
    client[x].auth.type.www   = BASIC;
    client[x].auth.type.proxy = BASIC;
    client[x].rand_r_SEED     = pthread_rand_np(&randrseed);
    result = crew_add(crew, (void*)start_routine, &(client[x]));
    if(result == FALSE){ 
      my.verbose = FALSE;
      fprintf(stderr, "Unable to spawn additional threads; you may need to\n");
      fprintf(stderr, "upgrade your libraries or tune your system in order\n"); 
      fprintf(stderr, "to exceed %d users.\n", my.cusers);
      joe_fatal("system resources exhausted"); 
    }
  } /* end of for pthread_create */

  crew_join(crew, TRUE, &statusp);

#ifdef HAVE_SSL
  SSL_thread_cleanup();
#endif

  /**
   * collect all the data from all the threads that
   * were spawned by the run.
   */
  for(x = 0; x < ((crew_get_total(crew) > my.cusers || 
                   crew_get_total(crew)==0 ) ? my.cusers : crew_get_total(crew)); x++){
    data_increment_count(D, client[x].hits);
    data_increment_bytes(D, client[x].bytes);
    data_increment_total(D, client[x].time);
    data_increment_code (D, client[x].code);
    data_increment_ok200(D, client[x].ok200);
    data_increment_fail (D, client[x].fail);
    data_set_highest    (D, client[x].bigtime);
    data_set_lowest     (D, client[x].smalltime);
    client[x].rand_r_SEED = pthread_rand_np(&randrseed);
  } /* end of stats accumulation */
  
  /**
   * record stop time
   */
  data_set_stop(D);

  /**
   * cleanup crew
   */ 
  crew_destroy(crew);

  if(my.get){
    if(data_get_count(D) > 0){
      exit(EXIT_SUCCESS);
    } else {
      printf("[done]\n");
      exit(EXIT_FAILURE);
    }
  }

  /**
   * take a short nap  for  cosmetic  effect
   * this does NOT affect performance stats.
   */
  pthread_usleep_np(10000);
  if(my.verbose)
    fprintf(stderr, "done.\n");
  else
    fprintf(stderr, "\b      done.\n");
  if(my.extralog)
	log_extra("done.");

  /**
   * prepare and print statistics.
   */
  if(my.failures > 0 && my.failed >= my.failures){
    fprintf(stderr, "%s aborted due to excessive socket failure; you\n", program_name);
    fprintf(stderr, "can change the failure threshold in $HOME/.%src\n", program_name);
  }
  fprintf(stderr, "\bTransactions:\t\t%12u hits\n",        data_get_count(D));
  fprintf(stderr, "Availability:\t\t%12.2f %%\n",          data_get_count(D)==0 ? 0 :
                                                           (double)data_get_count(D) /
                                                           (data_get_count(D)+my.failed)
                                                           *100
  );
  fprintf(stderr, "Elapsed time:\t\t%12.2f secs\n",        data_get_elapsed(D));
  fprintf(stderr, "Data transferred:\t%12.2f MB\n",        data_get_megabytes(D)); /*%12llu*/
  fprintf(stderr, "Response time:\t\t%12.2f secs\n",       data_get_response_time(D));
  fprintf(stderr, "Transaction rate:\t%12.2f trans/sec\n", data_get_transaction_rate(D));
  fprintf(stderr, "Throughput:\t\t%12.2f MB/sec\n",        data_get_throughput(D));
  fprintf(stderr, "Concurrency:\t\t%12.2f\n",              data_get_concurrency(D));
  fprintf(stderr, "Successful transactions:%12u\n",        data_get_code(D)); 
  if(my.debug){
    fprintf(stderr, "HTTP OK received:\t%12u\n",             data_get_ok200(D));
  }
  fprintf(stderr, "Failed transactions:\t%12u\n",          my.failed);
  fprintf(stderr, "Longest transaction:\t%12.2f\n",        data_get_highest(D));
  fprintf(stderr, "Shortest transaction:\t%12.2f\n",       data_get_lowest(D));
  fprintf(stderr, " \n");
  if(my.mark)    mark_log_file(my.markstr);
  if(my.logging) log_transaction(D);

  data_destroy(D);
  if(my.url == NULL){
    for(x = 0; x < my.length; x++)
       xfree(lines->line[x]);
    xfree(lines->line);
    xfree(lines);
  }  

  exit(EXIT_SUCCESS);	
} /* end of int main **/



