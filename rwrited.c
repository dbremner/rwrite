/*  -*- c -*-
 *
 * $RCSfile: rwrited.c,v $
 * ----------------------------------------------------------------------
 * Main file of rwrited remote message server.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:46 1994 tri
 * Last modified: Thu Oct  6 20:26:15 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.10 $
 * $State: Exp $
 * $Date: 1994/10/06 18:32:37 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrited.c,v $
 * Revision 1.10  1994/10/06 18:32:37  tri
 * Hacked multitty option.
 *
 * Revision 1.9  1994/10/04  20:50:22  tri
 * Conforms now the current RWP protocol.
 *
 * Revision 1.8  1994/09/26  23:14:14  tri
 * Fixed a minor feature that made helo resopnse
 * look a bit weird before fhst command.
 *
 * Revision 1.7  1994/09/20  19:08:57  tri
 * Added a configuration option for rwrited ran
 * without tty setgid.
 *
 * Revision 1.6  1994/09/20  08:24:13  tri
 * Support for .rwrite-allow and .rwrite-deny files.
 *
 * Revision 1.5  1994/09/19  22:40:37  tri
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
static char *RCS_id = "$Id: rwrited.c,v 1.10 1994/10/06 18:32:37 tri Exp $";
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
#include "match.h"

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
char from_host[128];        /* Host message originates to */
char remote_host[128];      /* Host that is connected to server */
char *from_path = NULL;     /* Path from original host to remote host */
char from_user[128];	    /* Sender's login uid */
char identd_from_user[128]; /* Sender's login uid from identd */
char to_user[128];          /* Recipient's login id */
char my_host[128];          /* My hostname */
char tty_hint[MAXPATHLEN];  /* Remote end has given tty as a hint */
char tty_force[MAXPATHLEN]; /* Remote end has given tty to deliver a message */
int fwd_count = 0;          /* Hop count. -1 if all forwarding is forbidden */
int fake_user = 0;	    /* 0=ok, 1=nonconfirmed, 2=fake */

#define HAS_TTY_HINT()  (tty_hint[0] != '\000')
#define HAS_TTY_FORCE() (tty_force[0] != '\000')

int server_euid = -1;
int server_egid = -1;
int cmd_line    = 0;        /* Is rwrited invoked from the command line */

int identify_remote_by_identd(char *buf, int bufsize)
{
    return 0;    /* We could dig remote user with identd here. XXX */
}

char *read_line(FILE *f)
{
    char *buf;
    int buflen;
    int c, i;

    if(!(buf = ((char *)malloc(BUF_ALLOC_STEP)))) {
	RWRITE_FATAL("Out of memory.");
    }
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
		RWRITE_FATAL("Out of memory.");
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

    identd_from_user[0] = '\000';
    from_user[0] = '\000';
    to_user[0] = '\000';
    tty_hint[0] = '\000';
    tty_force[0] = '\000';
    gethostname(my_host, sizeof(my_host));
    my_host[sizeof(my_host) - 1] = '\000';
    fake_user = 0;
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
	strncpy(remote_host, remote_hostent->h_name, sizeof(remote_host));
    } else {
	strncpy(remote_host, my_host, sizeof(remote_host));
    }
    remote_host[sizeof(remote_host) - 1] = '\000';
    strncpy(from_host, remote_host, sizeof(from_host));
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
	    (remote_host[0] ? remote_host : "UNKNOWN"),
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
    if(!(buf = ((char **)malloc(BUF_ALLOC_STEP * sizeof(char *))))) {
	RWRITE_FATAL("Out of memory.");
    }
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
		RWRITE_FATAL("Out of memory.");
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
int term_chk(char *tty, int uid, int *msgsokP, time_t *atimeP)
{
	struct stat s;

	if (stat(tty, &s) < 0)
		return 1;
	if(s.st_uid == server_euid) {
	    /* own write bit */
	    *msgsokP = !(!((((unsigned int)s.st_mode) &
			    ((unsigned int)(S_IWRITE)))));
	} else if(s.st_gid == server_egid) {
	    /* grp write bit */
	    *msgsokP = !(!((((unsigned int)s.st_mode) &
			    ((unsigned int)(S_IWRITE >> 3)))));
	} else {
	    /* all write bit */
	    *msgsokP = !(!((((unsigned int)s.st_mode) &
			    ((unsigned int)(S_IWRITE >> 6)))));
	}
	if(*msgsokP && (uid >= 0)) {
	    /* Check also the ownership */
	    *msgsokP = (s.st_uid == uid);
	}
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
int search_utmp(char *user,
		int uid, 
		char *tty, 
		char *userhome,
		char ***ttylist,
		int *multittyp)
{
	struct utmp u;
	time_t bestatime, atime;
	int ufd, nloggedttys, nttys, msgsok, no_timecomp;
	char atty[MAXPATHLEN];
	char **all_ttys;
	int all_ttys_size;
	int ttylistlen;

	if(ttylist) {
	    if(!(all_ttys = (char **)calloc(BUF_ALLOC_STEP, sizeof(char *)))) {
		RWRITE_FATAL("Out of memory.");
	    }
	    all_ttys_size = BUF_ALLOC_STEP;
	    ttylistlen = 0;
	}
	if(multittyp) {
	    *multittyp = 0;
	}
	if(userhome && (*userhome)) {
	    /* 
	     * We check first if user has RWRITE_FILE_TARGET file in 
	     * his home dir.
	     */
	    FILE *f;

	    strcpy(atty, userhome);
	    strcat(atty, "/");
	    strcat(atty, RWRITE_FILE_TARGET);
	    if(f = fopen(atty, "r")) {
		if(fgets(atty, sizeof(atty), f)) {
		    int len = strlen(atty);
		    fclose(f); /* We can do it here, so we can forget it. */
		    if(len) {
			if(atty[len - 1] == '\n') {
			    atty[--len] = '\000';
			}
			if(!(strcmp(atty, "all"))) {
			    if(multittyp) {
				*multittyp = 1;
			    }
			    goto continue_utmp_search;
			}
			if((!(term_chk(atty, uid, &msgsok, &atime))) && msgsok) {
			    if(HAS_TTY_FORCE() && (strcmp(atty, tty_force))) {
				/* Cannot force over the user force tty. */
				return DELIVER_PERMISSION_DENIED;
			    }
			    strcpy(tty, atty);
			    if(ttylist) {
				/*
				 * There's always room for one.
				 */
				if(!(all_ttys[0] = malloc(strlen(atty) + 1))) {
				    RWRITE_FATAL("Out of memory.");
				}
				strcpy(all_ttys[0], atty);
				*ttylist = all_ttys;
			    }
			    return DELIVER_OK;
			}
		    }
		}
		fclose(f); /* Read failed but file is still open. */
	    }
	    /* No success this far.  Slip through. */
	}
    continue_utmp_search:;
	if((ufd = open(_PATH_UTMP, O_RDONLY)) < 0) {
	    return DELIVER_USER_NOT_IN;
	}
	nloggedttys = nttys = 0;
	bestatime = 0;
	no_timecomp = 0;
	while (read(ufd, (char *) &u, sizeof(u)) == sizeof(u))
	    if (strncmp(user, u.ut_name, sizeof(u.ut_name)) == 0) {
		nloggedttys++;
		strcpy(atty, "/dev/");
		strncat(atty, u.ut_line, UT_LINESIZE);
		atty[strlen("/dev/") + UT_LINESIZE] = '\000';
		if(term_chk(atty, uid, &msgsok, &atime))
		    continue;	/* bad term? skip */
		if(!msgsok)
		    continue;	/* skip ttys with msgs off */
		if(ttylist) {
		    if((ttylistlen + 2) >= all_ttys_size) {
			char **new_all_ttys;
			if(!(new_all_ttys = 
			     (char **)calloc(BUF_ALLOC_STEP + all_ttys_size,
					     sizeof(char *)))) {
			    RWRITE_FATAL("Out of memory.");
			}
			memcpy(new_all_ttys, 
			       all_ttys, 
			       all_ttys_size * sizeof(char *));
			all_ttys_size += BUF_ALLOC_STEP;
			free(all_ttys);
			all_ttys = new_all_ttys;
		    }
		    if(!(all_ttys[ttylistlen] = malloc(strlen(atty) + 1))) {
			RWRITE_FATAL("Out of memory.");
		    }
		    strcpy(all_ttys[ttylistlen], atty);
		    ttylistlen++;
		    *ttylist = all_ttys; /* Not necessary every time but... */
		}
		if(HAS_TTY_FORCE()) {
		    if(!(strcmp(atty, tty_force))) {
			strcpy(tty, atty);
			nttys++;
		    }
		    no_timecomp = 1;
		    continue;
		}
		nttys++;
		if(HAS_TTY_HINT() && (!(strcmp(atty, tty_hint)))) {
		    strcpy(tty, atty);
		    no_timecomp = 1;
		    continue;
		}
		if(!(no_timecomp) && (atime > bestatime)) {
		    bestatime = atime;
		    strcpy(tty, atty);
		}
	    }
	close(ufd);
	if (nloggedttys == 0)
	    return DELIVER_USER_NOT_IN;
	if(nttys >= 1)
	    return DELIVER_OK;
	return DELIVER_PERMISSION_DENIED;
}
/********* END OF STUFF FROM Berkeley Unix's write(1) **********/

/*
 * Check if user has either denied of allowed access for
 * specified remote user.
 */
int is_allowed(char *homedir, char *fromuser, char *fromhost)
{
    FILE *f;
    char *line;
    char rcfile[MAXPATHLEN], sender[256];

    sprintf(sender, "%s@%s", fromuser, fromhost);
    sprintf(rcfile, "%s/%s", homedir, RWRITE_FILE_ALLOW);
    if(f = fopen(rcfile, "r")) {
	while(line = read_line(f)) {
	    if(!(StrMatch(sender, line))) {
		free(line);
		fclose(f);
		return 1;
	    }
	    free(line);
	}
	fclose(f);
	return 0;
    } else {
	sprintf(rcfile, "%s/%s", homedir, RWRITE_FILE_DENY);
	if(f = fopen(rcfile, "r")) {
	    while(line = read_line(f)) {
		if(!(StrMatch(sender, line))) {
		    free(line);
		    fclose(f);
		    return 0;
		}
		free(line);
	    }
	    fclose(f);
	}
    }
    return 1;
}

int writeto(char *tty,
	    char **msg,
	    char *from, 
	    char *fromhost,
	    char *via,
	    char *remotehost,
	    char *nowstr)
{
    int i;
    FILE *f;

    if(!(f = fopen(tty, "w")))
	return 0;
    fputc('\007', f);
    if(strcmp(remotehost, fromhost))
	if(via) {
	    fprintf(f, 
		    "\nMessage from %s@%s (via %s%c%s) at %s\n", 
		    from, 
		    fromhost, 
		    via,
		    PATH_SEPARATOR,
		    remotehost, 
		    (nowstr ? nowstr : "xxx"));
	} else {
	    fprintf(f, "\nMessage from %s@%s (via %s) at %s\n", from, fromhost, 
		    remotehost, (nowstr ? nowstr : "xxx"));
	}
    else
	fprintf(f, "\nMessage from %s@%s at %s\n", from, fromhost, 
		(nowstr ? nowstr : "xxx"));
    for(i = 0; msg[i]; i++)
	fprintf(f, "%s\n", msg[i]);
    fputc('\n', f);
    fclose(f);
    return 1;
}
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
	    char *via, 
	    char **msg)
{
    char tty[MAXPATHLEN];
    int d_status;
    time_t now;
    char *nowstr;
    struct passwd *pwd;
    int multittyp;
    char **all_ttys;
    int succeeded;

    now = time(NULL);
    nowstr = ctime(&now);
    succeeded = 0;

    if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	return DELIVER_NO_SUCH_USER;
    }
    if(!(is_allowed(pwd->pw_dir, from, fromhost))) {
	return DELIVER_PERMISSION_DENIED;
    }
    if((d_status = search_utmp(user, pwd->pw_uid, tty, 
			       pwd->pw_dir, &all_ttys, &multittyp)) != 
       DELIVER_OK)
	return(d_status);
    if(multittyp && all_ttys) {
	char **ttys;
	for(ttys = all_ttys; *ttys; ttys++) {
	    succeeded = (writeto(*ttys,
				 msg,
				 from, 
				 fromhost,
				 via,
				 remotehost,
				 nowstr) ||
			 succeeded);
	    free(*ttys);
	}
	free(all_ttys);
    } else {
	succeeded = writeto(tty,
			    msg,
			    from, 
			    fromhost,
			    via,
			    remotehost,
			    nowstr);
	if(all_ttys) { /* Free them anyway.  Not very important though. */
	    char **ttys;
	    for(ttys = all_ttys; *ttys; ttys++)
		free(*ttys);
	    free(all_ttys);
	}
    }
    return(succeeded ? DELIVER_OK : DELIVER_PERMISSION_DENIED);
}

int can_deliver(char *user, char *from, char *fromhost)
{
    char tty[MAXPATHLEN];
    struct passwd *pwd;
    
    if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	return DELIVER_NO_SUCH_USER;
    }
    /*
     * Check user's allow and deny file only if remote user has
     * already told who he is.
     */
    if(from && fromhost && (!(is_allowed(pwd->pw_dir, from, fromhost))))
	return DELIVER_PERMISSION_DENIED;
    return(search_utmp(user, pwd->pw_uid, tty, pwd->pw_dir, NULL, NULL));
}

int main(int argc, char **argv)
{
    char *cmd;
    char **message;

#ifdef NO_GETEUID
    server_euid = getuid();
#else
    server_euid = geteuid();
#endif
#ifdef NO_GETEGID
    server_egid = getgid();
#else
    server_egid = getegid();
#endif
    if((argc == 2) && (argv[1][0] == '-') && (argv[1][1] == '\000')) {
	/*
	 * We can run this on cmd-line using flag -.
	 */
	cmd_line = 1;
	set_hostnames(0);
    } else {
	/*
	 * Otherwise we presume we are running through a socket.
	 */
	cmd_line = 0;
	set_hostnames(1);
    }
    rwrite_helo();
    rwrite_ver();
    rwrite_prot();
    if(!(identify_remote_by_identd(identd_from_user, sizeof(identd_from_user))))
	identd_from_user[0] = '\000';
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
		if(from_path) {
		    free(from_path);
		    from_path = NULL;
		}
		if(message) {
		    int i;
		    for(i = 0; message[i]; i++)
			free(message[i]);
		    free(message);
		    message = NULL;
		}
		set_hostnames((!cmd_line) ? 1 : 0);
		RWRITE_MSG(RWRITE_RSET_OK, "RSET ok.");
	    } else if((!(strcmp(cmd, "from"))) || 
		      (!(strcmp(cmd, "FROM"))) ||
		      (!(strncmp(cmd, "from ", 5))) || 
		      (!(strncmp(cmd, "FROM ", 5)))) {
		char *user_from = get_user_name(cmd);
		
		from_user[0] = '\000';
		if((!user_from) || 
		   (!(strlen(user_from))) || 
		   ((strlen(user_from) + 2) >= sizeof(from_user))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FROM userid");
		    goto out_of_parse;
		}
		strcpy(from_user, user_from);
#ifdef NO_IDENTD
		RWRITE_MSG(RWRITE_SENDER_OK, "Sender ok.");
#else
		if(!(identd_from_user[0])) {
		    /* Identd failed so no-one knows. */
		    fake_user = 1;
		    RWRITE_MSG(RWRITE_SENDER_OK, 
			       "Sender ok but nonconfirmed.");
		} else if(strcmp(from_user, identd_from_user)) {
		    /* It's a fake. */
		    fake_user = 2;
		    RWRITE_MSG(RWRITE_SENDER_OK, 
			       "Sender ok but doesn't match with identd.");
		} else {
		    /* It's confirmed by identd. */
		    fake_user = 0;
		    RWRITE_MSG(RWRITE_SENDER_OK, "Sender ok.");
		}
#endif /* NO_IDENTD */
	    } else if((!(strcmp(cmd, "to"))) ||
		      (!(strcmp(cmd, "TO"))) ||
		      (!(strncmp(cmd, "to ", 3))) || 
		      (!(strncmp(cmd, "TO ", 3)))) {
		char *user_to = get_user_name(cmd);
		char *tty_to;
		int len;

		to_user[0] = '\000';
		tty_hint[0] = '\000';
		tty_force[0] = '\000';
		if((!user_to) || 
		   (!(len = strlen(user_to))) || 
		   (len >= sizeof(to_user))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: TO userid [tty]");
		    goto out_of_parse;
		}
		strcpy(to_user, user_to);
		tty_to = get_user_name(user_to); /* Possible tty */
		user_to = to_user;
		while(*user_to) {
		    if(isspace(*user_to)) {
			*user_to = '\000';
			break;
		    }
		    user_to++;
		}
		if(tty_to && (len = strlen(tty_to))) {
		    if((len + 6) >= sizeof(tty_hint)) {
			RWRITE_MSG(RWRITE_ERR_SYNTAX, 
				   "Syntax: TO userid [tty]");
			goto out_of_parse;
		    }
		    if((tty_to[0] == '[') && (tty_to[len - 1] == ']')) {
			/* It's a hint */
			tty_to[len - 1] = '\000';
			tty_to++;
			if(!(strlen(tty_to))) {
			    RWRITE_MSG(RWRITE_ERR_SYNTAX, 
				       "Syntax: TO userid [tty]");
			    goto out_of_parse;
			}
			strcpy(tty_hint, "/dev/");
			strcat(tty_hint, tty_to);
		    } else {
			/* It's a demand */
			strcpy(tty_force, "/dev/");
			strcat(tty_force, tty_to);
		    }
		}
		RWRITE_MSG(RWRITE_RCPT_OK, "Recipient ok.");
	    } else if((!(strcmp(cmd, "fhst"))) ||
		      (!(strcmp(cmd, "FHST"))) ||
		      (!(strncmp(cmd, "fhst ", 5))) || 
		      (!(strncmp(cmd, "fhst ", 5)))) {
		char *hlp1;
		char *frm = get_user_name(cmd);
		if((!frm) ||
		   (!(strlen(frm)))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, 
			       "Syntax: FHST remote.host ...");
		    goto out_of_parse;
		}
		if(!(strcmp(frm, remote_host))) {
		    RWRITE_MSG(RWRITE_FHST_OK, "Original sender host ok.");
		    goto out_of_parse;
		}
		hlp1 = frm;
		while((*hlp1) && (!(isspace(*hlp1))))
		    hlp1++;
		if(from_path) {
		    free(from_path);
		    from_path = NULL;
		}
		if(*hlp1) {
		    int len;
		    char *tail, *hlp2;
		    /*
		     * The tail will be the path.
		     */
		    tail = hlp2 = hlp1;
		    while(*hlp1) {
			while((*hlp1) && (isspace(*hlp1)))
			    hlp1++;
			if(*hlp1) {
			    while((*hlp1) && (!(isspace(*hlp1)))) {
				*hlp2 = *hlp1;
				hlp1++;
				hlp2++;
			    }
			    if(*hlp1) {
				*hlp2 = PATH_SEPARATOR;
				hlp2++;
			    }
			}
		    }
		    *hlp2 = '\000';
		    if(len = strlen(tail)) {
			if(!(from_path = ((char *)malloc(len + 1)))) {
			    RWRITE_FATAL("Out of memory.");
			}
			strcpy(from_path, tail);
		    }
		    *tail = '\000';
		}
		strncpy(from_host, frm, sizeof(from_host));
		from_host[sizeof(from_host) - 1] = '\000';
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
		hlp = n_str;
		if(*hlp == '-')
		    hlp++;
		for(/*NOTHING*/; *hlp; hlp++) {
		    if(!(isdigit(*hlp))) {
			RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FWDS number");
			goto out_of_parse;
		    }
		}
		if((n = atoi(n_str)) < -1) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, 
			       "Number of forwards less than -1.");
		    goto out_of_parse;
		}
		if((n != -1) && (n <= RWRITE_FWD_LIMIT)) {
		    RWRITE_MSG(RWRITE_RCPT_OK_TO_FWD, "Ok to forward.");
		} else {
		    RWRITE_MSG(RWRITE_ERR_FWD_LIMIT_EXCEEDED,
			       "Forward limit exceeded.");
		}
		fwd_count = n;
	    } else if((!(strcmp(cmd, "vrfy"))) || (!(strcmp(cmd, "VRFY")))) {
		int d_status;

		if(!(to_user[0])) {
		    RWRITE_MSG(RWRITE_ERR_NO_ADDRESS, 
			       "Use TO before VRFY.");
		    goto out_of_parse;
		}
		if((d_status = can_deliver(to_user, 
					   (from_user[0] ? from_user : NULL),
					   from_host)) != DELIVER_OK) {
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

		if(!(from_user[0])) {
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
				       from_user, 
				       from_host,
				       remote_host,
				       from_path,
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
		char *qcmd =  get_user_name(cmd);
		if((!qcmd) && (!(*qcmd))) {
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: QUOTE cmd ...");
		    goto out_of_parse;
		}
		/* Here we should deal with commands like CHARSET. XXX */
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
