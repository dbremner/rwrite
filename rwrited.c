/*  -*- c -*-
 *
 * $RCSfile: rwrited.c,v $
 * ----------------------------------------------------------------------
 * Main file of rwrited remote message server.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:46 1994 tri
 * Last modified: Tue Sep 20 01:37:53 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.5 $
 * $State: Exp $
 * $Date: 1994/09/19 22:40:37 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrited.c,v $
 * Revision 1.5  1994/09/19 22:40:37  tri
 * TOOK replaced by VRFY and made some considerable
 * cleanup.
 *
 * Revision 1.4  1994/09/15  20:14:42  tri
 * Completed the support of RWP version 1.0.
 *
 * Revision 1.3  1994/09/14  15:10:18  tri
 * Reports now also the protocol version on startup.
 *
 * Revision 1.2  1994/09/14  14:58:53  tri
 * Fixed a few bugs.
 *
 * Revision 1.1  1994/09/13  12:32:08  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Timo Rinne <tri@cirion.fi> and Cirion oy.
 * 
 * Address: Cirion oy, PO-BOX 250, 00121 HELSINKI, Finland
 * 
 * Even though this code is copyrighted property of the author, it can
 * still be used for any purpose under following conditions:
 * 
 *     1) This copyright notice is not removed.
 *     2) Source code follows any distribution of the software
 *        if possible.
 *     3) Copyright notice above is found in the documentation
 *        of the distributed software.
 * 
 * Any express or implied warranties are disclaimed.  In no event
 * shall the author be liable for any damages caused (directly or
 * otherwise) by the use of this software.
 * ----------------------------------------------------------------------
 */
#define __RWRITED_C__ 1
#ifndef lint
static char *RCS_id = "$Id: rwrited.c,v 1.5 1994/09/19 22:40:37 tri Exp $";
#endif /* not lint */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <utmp.h>

#ifndef UT_LINESIZE
#  define UT_LINESIZE 32
#endif

#ifndef _PATH_UTMP
#  ifdef PATH_UTMP
#    define _PATH_UTMP PATH_UTMP
#  else
#    ifdef UTMP_FILE
#      define _PATH_UTMP UTMP_FILE
#    else
#      define _PATH_UTMP "/etc/utmp"
#    endif
#  endif
#endif

#include "rwrite.h"

#define RWRITE_FATAL(msg) { fprintf(stdout,                        \
				     "%03d RWRITED FATAL: %s\n",   \
				     RWRITE_ERR_FATAL,             \
				     msg);                         \
			     exit(1); }

/*
 * Allocation step in line buffer allocation. 
 * Has to be at least 2.  No need to modify this anyway.
 */
#define BUF_ALLOC_STEP	128
/*
 * Return status for message delivery functions should be
 * one of the following.
 */
#define DELIVER_OK			0
#define DELIVER_USER_NOT_IN		1
#define DELIVER_PERMISSION_DENIED	2
#define DELIVER_NO_SUCH_USER		3

/*
 * Print standard format message.
 */
#define RWRITE_MSG(n, msg) { fprintf(stdout, "%03d %s\n", n, msg); \
			     fflush(stdout); }
/*
 * Globals.  Eh... Urp... Well... Who cares...
 */
char from_host[128];
char remote_host[128];
char remote_user[64];
char to_user[64];
char my_host[128];
int fwd_count = 0;

char *read_line(FILE *f)
{
    char *buf;
    int buflen;
    int c, i;

    if(!(buf = ((char *)malloc(BUF_ALLOC_STEP))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    for(i = 0; (EOF != (c = (fgetc(f)))); i++) {
	if((c == '\000') ||
	   (c == '\015')) {
	    i--;
	    continue;
	}
	if(c == '\012')
	    break;
	if((i + 1) >= buflen) {
	    char *newbuf;
	    buflen += BUF_ALLOC_STEP;
	    if(!(newbuf = (char *)malloc(buflen))) {
		free(buf);
		return NULL;
	    }
	    memcpy(newbuf, buf, i);
	    free(buf);
	    buf = newbuf;
	}
	buf[i] = c;
    }
    buf[i] = '\000';
    if((c == EOF) && (i == 0)) {
	free(buf);
	return NULL;
    }
    buf[i] = '\000';
    return(buf);
}

void set_hostnames(int get_remote)
{
    struct sockaddr_in remote_sock;
    int remote_sock_len = sizeof(remote_sock);
    struct hostent *remote_hostent;

    remote_user[0] = '\000';
    remote_host[0] = '\000';
    from_host[0] = '\000';
    to_user[0] = '\000';
    gethostname(my_host, sizeof(my_host));
    my_host[sizeof(my_host) - 1] = '\000';
    if(get_remote) {
	if(-1 == getpeername(0, 
			     (struct sockaddr *)(&remote_sock),
			     &remote_sock_len)) {
	    RWRITE_FATAL("Getpeername failed.");
	}
	if((!(remote_hostent = gethostbyaddr((char *)&remote_sock.sin_addr, 
					     sizeof(struct in_addr),
					     remote_sock.sin_family))) ||
	   (!(remote_hostent->h_name))) {
	    RWRITE_FATAL("Gethostbyaddr failed.");
	}
	strncpy(from_host, remote_hostent->h_name, sizeof(from_host));
    } else {
	strncpy(from_host, my_host, sizeof(from_host));
    }
    from_host[sizeof(from_host) - 1] = '\000';

    return; 
}

/*
 * Simple commands
 */
void rwrite_helo()
{ 
    fprintf(stdout, "%03d Hello %s.  This is %s speaking.\n",
	    RWRITE_HELO, 
	    remote_host, 
	    my_host); 
    fflush(stdout);
    return;
}

void rwrite_ver()
{
    RWRITE_MSG(RWRITE_VER, "Rwrited version " RWRITED_VERSION_NUMBER ".");
    return;
}

void rwrite_prot()
{
    RWRITE_MSG(RWRITE_PROT, "RWP version " RWP_VERSION_NUMBER ".");
    return;
}

void rwrite_ready()
{
    RWRITE_MSG(RWRITE_READY, "Ready.");
    return;
}

void rwrite_help()
{
    RWRITE_MSG(RWRITE_HELP, "Valid commands are:");
    RWRITE_MSG(RWRITE_HELP, "    BYE,    DATA,   HELP,   HELO,");
    RWRITE_MSG(RWRITE_HELP, "    RSET,   SEND,   PROT,   QUIT,");
    RWRITE_MSG(RWRITE_HELP, "    VRFY,   VER");
    RWRITE_MSG(RWRITE_HELP, "    FROM senderlogin");
    RWRITE_MSG(RWRITE_HELP, "    FHST senderhost");
    RWRITE_MSG(RWRITE_HELP, "    TO   recipentlogin");
    RWRITE_MSG(RWRITE_HELP, "    FWDS current_hop_count");
    return;
}

void rwrite_bye()
{
    RWRITE_MSG(RWRITE_BYE, "Goodbye.");
    exit(0);
}

void rwrite_quit()
{
    RWRITE_MSG(RWRITE_BYE, "Quit.");
    exit(0);
}

/*
 * Read message data.
 */
char **read_message()
{
    int buflen, i;
    char **buf;
    char *line;

    RWRITE_MSG(RWRITE_GETMSG, 
	       "Enter message.  Single dot '.' on line terminates.");
    if(!(buf = ((char **)malloc(BUF_ALLOC_STEP * sizeof(char *)))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    for(i = 0; /*NOTHING*/; i++) {
	if(!(line = read_line(stdin))) {
	    RWRITE_MSG(RWRITE_ERR_NO_MESSAGE, "No message.");
	    rwrite_bye();
	}
	if(!(strcmp(".", line))) {
	    if(!i) {
		free(buf);
		return NULL;
	    }
	    buf[i] = NULL;
	    return buf;
	}
	if((i + 1) >= buflen) {
	    char **newbuf;
	    buflen += BUF_ALLOC_STEP;
	    if(!(newbuf = (char **)malloc(buflen * sizeof(char *)))) {
		free(buf);
		return NULL;
	    }
	    memcpy(newbuf, buf, i * sizeof(char *));
	    free(buf);
	    buf = newbuf;
	}
	buf[i] = line;
    }
    /*NOTREACHED*/
}

/* 
 *  Skip first token in line.
 *  If there are white space in the tail of the command,
 *  null them.
 *  Return pointer to the first non-whitespace char after the
 *  first token.
 *  If there are none, return NULL.
 */
char *get_user_name(char *cmd)
{
    char *ret, *hlp;

    for(ret = cmd; (isalnum(*ret)); ret++)
	/*NOTHING*/;
    for(/*NOTHING*/; (isspace(*ret)); ret++)
	/*NOTHING*/;
    if(!(*ret))
	return NULL;
    for(hlp = ret; *hlp; hlp++)
	/*NOTHING*/;
    hlp--;
    while(isspace(*hlp))
	*hlp-- = '\000';
    return(ret);
}

/**************** From Berkeley Unix's write(1) *****************/
/********************* Modifications by tri *********************/
/*
 * term_chk - check that a terminal exists, and get the message bit
 *     and the access time
 */
int term_chk(char *tty, int *msgsokP, time_t *atimeP)
{
	struct stat s;
	char path[MAXPATHLEN];

	(void)sprintf(path, "/dev/%s", tty);
	if (stat(path, &s) < 0)
		return 1;
	*msgsokP = (s.st_mode & (S_IWRITE >> 3)) != 0;	/* group write bit */
	*atimeP = s.st_atime;
	return 0;
}
/*
 * search_utmp - search utmp for the "best" terminal to write to
 *
 * Ignores terminals with messages disabled, and of the rest, returns
 * the one with the most recent access time.  Returns as value the number
 * of the user's terminals with messages enabled, or -1 if the user is
 * not logged in at all.
 *
 * Special case for writing to yourself - ignore the terminal you're
 * writing from, unless that's the only terminal with messages enabled.
 */
int search_utmp(char *user, char *tty)
{
	struct utmp u;
	time_t bestatime, atime;
	int ufd, nloggedttys, nttys, msgsok, user_is_me;
	char atty[UT_LINESIZE + 1];

	if((ufd = open(_PATH_UTMP, O_RDONLY)) < 0) {
	    return DELIVER_USER_NOT_IN;
	}
	nloggedttys = nttys = 0;
	bestatime = 0;
	user_is_me = 0;
	while (read(ufd, (char *) &u, sizeof(u)) == sizeof(u))
	    if (strncmp(user, u.ut_name, sizeof(u.ut_name)) == 0) {
		++nloggedttys;
		(void)strncpy(atty, u.ut_line, UT_LINESIZE);
		atty[UT_LINESIZE] = '\0';
		if (term_chk(atty, &msgsok, &atime))
		    continue;	/* bad term? skip */
		if (!msgsok)
		    continue;	/* skip ttys with msgs off */
		++nttys;
		if (atime > bestatime) {
		    bestatime = atime;
		    sprintf(tty, "/dev/%s", atty);
		}
	    }

	(void)close(ufd);
	if (nloggedttys == 0) {
	    return DELIVER_USER_NOT_IN;
	}
	if(nttys >= 1) {
	    return DELIVER_OK;
	}
	return DELIVER_PERMISSION_DENIED;
}
/********* END OF STUFF FROM Berkeley Unix's write(1) **********/

/*
 * This is a function that should be developed radically.
 * Users should be able to have resource file to modify
 * the delivery methods and to allow forwarding etc.
 * Also support for some kind of support for kinda 
 * message agent would be nice.
 */
int deliver(char *user, 
	    char *from, 
	    char *fromhost, 
	    char *remotehost, 
	    char **msg)
{
    FILE *f;
    int i;
    char tty[MAXPATHLEN];
    char userhome[MAXPATHLEN];
    int d_status;
    time_t now;
    char *nowstr;

    now = time(NULL);
    nowstr = ctime(&now);
    {
	struct passwd *pwd;

	if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	    return DELIVER_NO_SUCH_USER;
	}
	strcpy(userhome, pwd->pw_dir);
    }
    
    if((d_status = search_utmp(user, tty)) != DELIVER_OK)
	return(d_status);
    if(!(f = fopen(tty, "w")))
	return DELIVER_PERMISSION_DENIED;
    fputc('\007', f);
    if(remotehost && (*remotehost))
	fprintf(f, "\nMessage from %s@%s (via %s) at %s\n", from, fromhost, 
		remotehost, (nowstr ? nowstr : "xxx"));
    else
	fprintf(f, "\nMessage from %s@%s at %s\n", from, fromhost, 
		(nowstr ? nowstr : "xxx"));
    for(i = 0; msg[i]; i++)
	fprintf(f, "%s\n", msg[i]);
    fputc('\n', f);
    fclose(f);
    return DELIVER_OK;
}

int can_deliver(char *user)
{
    char tty[MAXPATHLEN];

    {
	struct passwd *pwd;

	if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	    return DELIVER_NO_SUCH_USER;
	}
    }
    return(search_utmp(user, tty));
}

int main(int argc, char **argv)
{
    char *cmd;
    char **message;

    if((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == '\000')) {
	/*
	 * We can run this on cmd-line using flag -.
	 */
	set_hostnames(0);
    } else {
	/*
	 * Otherwise we presume we are running through a socket.
	 */
	set_hostnames(1);
    }
    rwrite_helo();
    rwrite_ver();
    rwrite_prot();
    for((message = NULL), (rwrite_ready(), (cmd = read_line(stdin)));
	cmd;
	(rwrite_ready(), (cmd = read_line(stdin)))) {
	if(strlen(cmd)) {
	    if((!(strcmp(cmd, "bye")) || (!(strcmp(cmd, "BYE"))))) {
		rwrite_bye();
	    } else if((!(strcmp(cmd, "quit"))) || (!(strcmp(cmd, "QUIT")))) {
		rwrite_quit();
	    } else if((!(strcmp(cmd, "help"))) || (!(strcmp(cmd, "HELP")))) {
		rwrite_help();
	    } else if((!(strcmp(cmd, "helo"))) || (!(strcmp(cmd, "HELO")))) {
		rwrite_helo();
	    } else if((!(strcmp(cmd, "ver"))) || (!(strcmp(cmd, "VER")))) {
		rwrite_ver();
	    } else if((!(strcmp(cmd, "prot"))) || (!(strcmp(cmd, "PROT")))) {
		rwrite_prot();
	    } else if((!(strcmp(cmd, "rset"))) || (!(strcmp(cmd, "RSET")))) {
		remote_user[0] = '\000';
		to_user[0] = '\000';
		fwd_count = 0;
		if(remote_host[0]) {
		    strncpy(from_host, remote_host, sizeof(from_host));
		    from_host[sizeof(from_host) - 1] = '\000';
		    remote_host[0] = '\000';
		}
		if(message) {
		    int i;
		    for(i = 0; message[i]; i++)
			free(message[i]);
		    free(message);
		    message = NULL;
		}
		RWRITE_MSG(RWRITE_RSET_OK, "RSET ok.");
	    } else if((!(strcmp(cmd, "from"))) || 
		      (!(strcmp(cmd, "FROM"))) ||
		      (!(strncmp(cmd, "from ", 5))) || 
		      (!(strncmp(cmd, "FROM ", 5)))) {
		char *user_from = get_user_name(cmd);
		
		remote_user[0] = '\000';
		if((!user_from) || 
		   (!(strlen(user_from))) || 
		   (strlen(user_from) >= sizeof(remote_user))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FROM userid");
		    goto out_of_parse;
		}
		strcpy(remote_user, user_from);
		RWRITE_MSG(RWRITE_SENDER_OK, "Sender ok.");
	    } else if((!(strcmp(cmd, "to"))) ||
		      (!(strcmp(cmd, "TO"))) ||
		      (!(strncmp(cmd, "to ", 3))) || 
		      (!(strncmp(cmd, "TO ", 3)))) {
		char *user_to = get_user_name(cmd);
		
		to_user[0] = '\000';
		if((!user_to) || 
		   (!(strlen(user_to))) || 
		   (strlen(user_to) >= sizeof(to_user))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: TO userid");
		    goto out_of_parse;
		}
		strcpy(to_user, user_to);
		RWRITE_MSG(RWRITE_RCPT_OK, "Recipient ok.");
	    } else if((!(strcmp(cmd, "fhst"))) ||
		      (!(strcmp(cmd, "FHST"))) ||
		      (!(strncmp(cmd, "fhst ", 5))) || 
		      (!(strncmp(cmd, "fhst ", 5)))) {
		char *frm = get_user_name(cmd);

		if((!frm) ||
		   (!(strlen(frm)))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FHST remote.host");
		    goto out_of_parse;
		}
		if(strcmp(frm, from_host)) {
		    if(!(remote_host[0])) {
			strncpy(remote_host, from_host, sizeof(remote_host));
			remote_host[sizeof(remote_host) - 1] = '\000';
		    }
		    strncpy(from_host, frm, sizeof(from_host));
		    from_host[sizeof(from_host) - 1] = '\000';
		}
		RWRITE_MSG(RWRITE_FHST_OK, "Original sender host ok.");
	    } else if((!(strcmp(cmd, "fwds"))) ||
		      (!(strcmp(cmd, "FWDS"))) ||
		      (!(strncmp(cmd, "fwds ", 5))) || 
		      (!(strncmp(cmd, "FWDS ", 5)))) {
		char *n_str = get_user_name(cmd);
		char *hlp;
		int n, i;

		if((!n_str) ||
		   (!(strlen(n_str)))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FWDS number");
		    goto out_of_parse;
		}
		for(hlp = n_str; *hlp; hlp++) {
		    if(!(isdigit(*hlp))) {
			RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FWDS number");
			goto out_of_parse;
		    }
		}
		if((n = atoi(n_str)) < 0) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, 
			       "Number of forwards less than 0.");
		    goto out_of_parse;
		}
		if(n <= RWRITE_FWD_LIMIT) {
		    RWRITE_MSG(RWRITE_RCPT_OK_TO_FWD, "Ok to forward.");
		} else {
		    RWRITE_MSG(RWRITE_ERR_FWD_LIMIT_EXCEEDED,
			       "Forward limit exceeded.");
		}
	    } else if((!(strcmp(cmd, "vrfy"))) || (!(strcmp(cmd, "VRFY")))) {
		int d_status;

		if(!(to_user[0])) {
		    RWRITE_MSG(RWRITE_ERR_NO_ADDRESS, 
			       "Use TO before VRFY.");
		    goto out_of_parse;
		}
		if((d_status = can_deliver(to_user)) != DELIVER_OK) {
		    switch(d_status) {
		    case DELIVER_NO_SUCH_USER:
#ifndef DO_NOT_TELL_USERS
			RWRITE_MSG(RWRITE_ERR_NO_SUCH_USER, "No such user.");
			break;
#endif
		    case DELIVER_USER_NOT_IN:
			RWRITE_MSG(RWRITE_ERR_USER_NOT_IN, "User not in.");
			break;
		    case DELIVER_PERMISSION_DENIED:
			RWRITE_MSG(RWRITE_ERR_PERMISSION_DENIED, 
				   "Permission denied.");
			break;
		    default:
			RWRITE_MSG(RWRITE_ERR_UNKNOWN, "Unknown error.");
			break;
		    }
		    goto out_of_parse;
		}
		RWRITE_MSG(RWRITE_RCPT_OK_TO_SEND, "Recipient ok to send.");
	    } else if((!(strcmp(cmd, "data"))) || (!(strcmp(cmd, "DATA")))) {
		if(message) {
		    int i;
		    for(i = 0; message[i]; i++)
			free(message[i]);
		    free(message);
		}
		if(!(message = read_message())) {
		    RWRITE_MSG(RWRITE_ERR_NO_MESSAGE, "No message.");
		    goto out_of_parse;
		}
		RWRITE_MSG(RWRITE_MSG_OK, "Message ok.");
	    } else if((!(strcmp(cmd, "send"))) || (!(strcmp(cmd, "SEND")))) {
		int d_status;

		if(!(remote_user[0])) {
		    RWRITE_MSG(RWRITE_ERR_NO_SENDER, 
			       "Use FROM before SEND.");
		    goto out_of_parse;
		}
		if(!(to_user[0])) {
		    RWRITE_MSG(RWRITE_ERR_NO_ADDRESS, 
			       "Use TO before SEND.");
		    goto out_of_parse;
		}
		if(!message) {
		    RWRITE_MSG(RWRITE_ERR_NO_DATA, 
			       "Use DATA before SEND.");
		    goto out_of_parse;
		}
		if((d_status = deliver(to_user, 
				       remote_user, 
				       from_host,
				       remote_host,
				       message)) != DELIVER_OK) {
		    switch(d_status) {
		    case DELIVER_NO_SUCH_USER:
			RWRITE_MSG(RWRITE_ERR_NO_SUCH_USER, "No such user.");
			break;
		    case DELIVER_USER_NOT_IN:
			RWRITE_MSG(RWRITE_ERR_USER_NOT_IN, "User not in.");
			break;
		    case DELIVER_PERMISSION_DENIED:
			RWRITE_MSG(RWRITE_ERR_PERMISSION_DENIED, 
				   "Permission denied.");
			break;
		    default:
			RWRITE_MSG(RWRITE_ERR_UNKNOWN,"Unknown error.");
			break;
		    }
		    goto out_of_parse;
		}
		RWRITE_MSG(RWRITE_DELIVERY_OK, "Message delivered.");
	    } else if((!(strcmp(cmd, "quote"))) ||
		      (!(strcmp(cmd, "QUOTE"))) ||
		      (!(strncmp(cmd, "quote ", 6))) || 
		      (!(strncmp(cmd, "QUOTE ", 6)))) {
                RWRITE_MSG(RWRITE_ERR_QUOTE_CMD_UNKNOWN, 
			   "Unknown QUOTE command.");
	    } else {
                RWRITE_MSG(RWRITE_ERR_SYNTAX, "Does not compute.");
            }
   	}
	free(cmd);
    out_of_parse:;
    }
    rwrite_bye();
    /*NOTREACHED*/
    return 0;
}

/* EOF (rwrited.c) */
