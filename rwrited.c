/*  -*- c -*-
 *
 * $RCSfile: rwrited.c,v $
 * ----------------------------------------------------------------------
 * Main file of rwrited remote message server.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:46 1994 tri
 * Last modified: Sun Apr 19 00:12:53 1998 tri
 * ----------------------------------------------------------------------
 * Copyright © 1994-1998
 * Timo J. Rinne <tri@iki.fi>
 * All rights reserved.  See file COPYRIGHT for details.
 *
 * Address: Cirion oy, PO-BOX 250, 00121 Helsinki, Finland
 * ----------------------------------------------------------------------
 * Any express or implied warranties are disclaimed.  In no event
 * shall the author be liable for any damages caused (directly or
 * otherwise) by the use of this software.
 *
 * Please, send your patches to <tri@iki.fi>.
 * ----------------------------------------------------------------------
 * $Revision: 1.45 $
 * $State: Exp $
 * $Date: 1998/04/18 21:13:02 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrited.c,v $
 * Revision 1.45  1998/04/18 21:13:02  tri
 * Promoted version number.
 *
 * Revision 1.44  1998/04/18 20:52:28  tri
 * New copyright in COPYRIGHT.
 *
 * Revision 1.43  1996/05/17 12:31:28  tri
 * SOCKS support from kivinen@iki.fi.
 *
 * Revision 1.42  1995/09/29 19:29:31  tri
 * Commented one hostile free() away.
 * There's still bug in search_utmp.
 *
 * Revision 1.41  1995/06/11  15:27:33  tri
 * Does not block in tty open if
 * utmp is broken and user has
 * logged out.
 *
 * Revision 1.40  1995/02/10  07:32:50  tri
 * Wrap it up and call it 1.1.
 *
 * Revision 1.39  1994/12/15  15:34:56  tri
 * Added one semicolon to make SQI compiler happy.
 *
 * Revision 1.38  1994/12/15  04:58:28  tri
 * Fixed udp-support.
 *
 * Revision 1.37  1994/12/14  22:22:38  tri
 * Removed a few warnings with better casting.
 *
 * Revision 1.36  1994/12/14  22:02:58  tri
 * Fixed the autoreply logic a bit.  Now one can get
 * autoreply from the remote user even if the delivery
 * of the original message is not possible.
 *
 * Revision 1.35  1994/12/14  19:26:16  tri
 * Minor fix.
 *
 * Revision 1.34  1994/12/14  19:12:36  tri
 * Hacked udp connection type a bit, but it
 * does not seem to work.
 *
 * Revision 1.33  1994/12/14  03:03:23  tri
 * Fixed a few annoying features and added
 * -version flag.
 *
 * Revision 1.32  1994/12/14  00:46:16  tri
 * Fixed for configure system.
 *
 * Revision 1.31  1994/12/13  20:28:57  tri
 * Preparation for autoconfig and tcp-port change.
 *
 * Revision 1.30  1994/12/13  16:20:22  tri
 * Direct rwp-dialogs should now work even with
 * over-intelligent telnet(1)s.
 *
 * Revision 1.29  1994/12/13  11:35:21  tri
 * Changed `n' in macro RWRITE_MSG to `code' to help
 * braindamaged c-preprosessors.
 *
 * Revision 1.28  1994/12/12  15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.27  1994/12/12  11:03:42  tri
 * Added compatibility fixes from toka.
 *
 * Revision 1.26  1994/12/11  22:04:16  tri
 * Added support for ancient System V style
 * utmp that holds all kind of bogus information.
 *
 * Revision 1.25  1994/12/11  21:25:30  tri
 * Cleaned up some warnings.  No functional changes.
 *
 * Revision 1.24  1994/12/11  18:16:28  tri
 * Some portability fixes and configuration stuff
 * moved to Makefile.
 *
 * Revision 1.23  1994/12/11  15:41:19  tri
 * #include <fcntl.h> for Solaris.  Should not make any harm
 * for other systems.
 *
 * Revision 1.22  1994/12/11  14:56:13  tri
 * Minor fix.
 *
 * Revision 1.21  1994/12/10  11:28:38  tri
 * Last known method to send terminal control codes
 * through correctly configured rwrite is now diabled.
 *
 * Revision 1.20  1994/12/09  23:57:49  tri
 * Added a outbond message logging.
 *
 * Revision 1.19  1994/12/09  10:28:56  tri
 * Fixed a return value of dequote_and_send().
 *
 * Revision 1.18  1994/12/09  10:17:26  tri
 * Fixed Camillo's violent debug output.
 *
 * Revision 1.17  1994/12/08  22:56:45  tri
 * Fixed the quotation system on message
 * delivery.  Same message can now be quoted
 * differently for the each receiver.
 * Also the autoreplies are now quoted right.
 *
 * Revision 1.16  1994/12/07  12:34:32  tri
 * Removed read_message() and dropped in Camillo's GetMsg()
 * instead.
 *
 * Revision 1.15  1994/11/22  20:49:13  tri
 * Added configurable parameter to limit the number
 * of lines in the incoming message.
 *
 * Revision 1.14  1994/11/20  11:45:01  tri
 * Added a few minor lines to complete rwp.
 *
 * Revision 1.13  1994/11/20  11:08:12  tri
 * Fixed minor quotation bug in backround mode.
 *
 * Revision 1.12  1994/11/20  00:47:18  tri
 * Completed autoreply and quotation stuff.
 * We are almost there now.
 *
 * Revision 1.11  1994/10/06  18:37:54  tri
 * Possible coredump in deliver() fixed.
 *
 * Revision 1.10  1994/10/06  18:32:37  tri
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
 */
#define __RWRITED_C__ 1
#ifndef lint
static char *RCS_id = "$Id: rwrited.c,v 1.45 1998/04/18 21:13:02 tri Exp $";
#endif /* not lint */

#define RWRITED_VERSION_NUMBER	"1.2"	/* Server version   */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <pwd.h>
#include <stdio.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <netdb.h>
#include <sys/types.h>
#include <sys/param.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <utmp.h>
#include <fcntl.h>

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

#ifndef MAXPATHLEN
#  define MAXPATHLEN PATH_MAX
#endif

#include "rwrite.h"

#define RWRITE_FATAL(msg) { fprintf(stdout,                        \
				     "%03d RWRITED FATAL: %s (%s:%d)\n",   \
				     RWRITE_ERR_FATAL,             \
				     msg, __FILE__, __LINE__);     \
			     exit(1); }

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
#define RWRITE_MSG(code, msg) { fprintf(stdout, "%03d %s\n", code, msg); \
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
char tty_hint[MAXPATHLEN + 1];
			    /* Remote end has given tty as a hint */
char tty_force[MAXPATHLEN + 1];
			    /* Remote end has given tty to deliver a message */
int fwd_count = 0;          /* Hop count. -1 if all forwarding is forbidden */
int fake_user = 0;	    /* 0=ok, 1=nonconfirmed, 2=fake */

int sent_autoreply = 0;     /* See if autoreply is sent during this conv. */

#define HAS_TTY_HINT()  (tty_hint[0] != '\000')
#define HAS_TTY_FORCE() (tty_force[0] != '\000')

int server_euid = -1;
int server_egid = -1;
int cmd_line    = 0;        /* Is rwrited invoked from the command line */

int identify_remote_by_identd(char *buf, int bufsize)
{
#ifdef NO_IDENTD
    return 0;
#else
    return 0;    /* We could dig remote user with identd here. XXX */
#endif
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

void set_hostnames(int get_remote, struct sockaddr_in *sa)
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
	if(!sa) {
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
	    if((!(remote_hostent = gethostbyaddr((char *)&sa->sin_addr,
						 sizeof(struct in_addr),
						 sa->sin_family))) ||
	       (!(remote_hostent->h_name))) {
		RWRITE_FATAL("Gethostbyaddr failed.");
	    }
	    strncpy(remote_host, remote_hostent->h_name, sizeof(remote_host));
	}
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
void rwrite_helo(int udp)
{ 
    if(!udp) {
	fprintf(stdout, "%03d Hello %s.  This is %s speaking.\n",
		RWRITE_HELO, 
		(remote_host[0] ? remote_host : "UNKNOWN"),
		my_host); 
	fflush(stdout);
    }
    return;
}

void rwrite_ver(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_VER, "Rwrited version " RWRITED_VERSION_NUMBER ".");
    }
    return;
}

void rwrite_prot(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_PROT, "RWP version " RWP_VERSION_NUMBER ".");
    }
    return;
}

void rwrite_ready(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_READY, "Ready.");
    }
    return;
}

void rwrite_help(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_HELP, "Valid commands are:");
	RWRITE_MSG(RWRITE_HELP, "    BYE,    DATA,   HELP,   HELO,");
	RWRITE_MSG(RWRITE_HELP, "    RSET,   SEND,   PROT,   QUIT,");
	RWRITE_MSG(RWRITE_HELP, "    VRFY,   VER");
	RWRITE_MSG(RWRITE_HELP, "    FROM senderlogin");
	RWRITE_MSG(RWRITE_HELP, "    FHST senderhost");
	RWRITE_MSG(RWRITE_HELP, "    TO   recipentlogin");
	RWRITE_MSG(RWRITE_HELP, "    FWDS current_hop_count");
    }
    return;
}

void rwrite_bye(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_BYE, "Goodbye.");
    }
    exit(0);
}

void rwrite_quit(int udp)
{
    if(!udp) {
	RWRITE_MSG(RWRITE_BYE, "Quit.");
    }
    exit(0);
}

int give_users_autoreply(char *home)
{
    FILE *f;
    char *line;
    char rcfilename[MAXPATHLEN + 1];

    if((!home) || (!(*home)))
	return 0;

    sprintf(rcfilename, "%s/%s", home, RWRITE_AUTOREPLY_FILE);
    if(f = fopen(rcfilename, "r")) {
	int n;
	
	n = 0;
	while((line = read_line(f)) && 
	      ((MAX_AUTOREPLY_LINES == -1) || (n < MAX_AUTOREPLY_LINES))) {
	    char *hlp;
	    
	    hlp = quote_str(line);
	    free(line);
	    line = hlp;
	    n++;
	    fprintf(stdout, "%03d |%s\n", RWRITE_AUTOREPLY, line);
	    free(line);
	}
	fclose(f);
	return(n ? 1 : 0);
    }
#ifdef DEBUG
    else {
	fprintf(stdout, "%03d Can't open \"%s\".\n", 
		RWRITE_DEBUG, rcfilename);
    }
#endif
    return 0;
}

/*
 * get_msg() and gm_getline() are contributions from Mr. Camillo S{rs.
 * Some modifications by tri.
 *
 * Copyright 1994, Camillo S{rs <Camillo.Sars@hut.fi>.
 */

static char *gm_getline (FILE *pf, int *p_limit, int *p_EOF);

char **get_msg (FILE *pf, int line_limit, int char_limit, int udp)
{
    int pos = 0;
    int eof = 0;
    int c = 0;
    char** p_buffer;

    if(line_limit == INT_MAX)
	line_limit -= 1;

    p_buffer = (char **)malloc((line_limit+1) * sizeof (char*));

    if(p_buffer == NULL)
	return NULL;

    p_buffer[line_limit] = NULL;

    if(!udp)
	RWRITE_MSG(RWRITE_GETMSG, 
		   "Enter message.  Single dot '.' on line terminates.");

    while (!eof && char_limit) {
	if(pos == line_limit)
	    break;
	p_buffer[pos] = gm_getline(pf, &char_limit, &eof);
	if(p_buffer[pos] == NULL)
	    break;
#ifdef DEBUG
	printf("%03d <<<%s\n", RWRITE_DEBUG, p_buffer[pos]);
#endif
	if(((*p_buffer[pos] == '.') &&
	    (*(p_buffer[pos]+1) == '\0')) ||
	   ((*p_buffer[pos] == '.') &&
	    (*(p_buffer[pos]+1) == '\015') &&
	    (*(p_buffer[pos]+2) == '\0'))) {
	    eof = -1;
	    p_buffer[pos] = NULL;
	    break;
	}
	pos++;
    }

    if(!eof) {
#ifdef DEBUG
	printf ("%03d Skipping input up to '.'\n", RWRITE_DEBUG);
#endif
	while(1) {
	    c = fgetc(pf);
	    if(c == EOF)
		break;
	    if(c == '.') {
		c = fgetc(pf);
		if(c == EOF || c == '\012') {
		    break;
		} else if(c == '\015') {
		    c = fgetc(pf);
		    if(c == EOF || c == '\012')
			break;
		}
	    }
	    while((c = fgetc(pf)) != EOF && c != '\012')
		/*NOTHING*/;
	}
    }
    if(p_buffer[0] && (p_buffer[0][0] || p_buffer[1])) {
	return p_buffer;
    }
    free(p_buffer);
    return NULL;
}

#define GL_BUFFER_SIZE 256

static char *gm_getline(FILE *pf, int *p_limit, int *p_EOF)
{
    int c;
    int pos = 0;
    int size = GL_BUFFER_SIZE;
    char *p_tmp;

    char *p_buffer = (char*)malloc(GL_BUFFER_SIZE * sizeof(char));

    if(!p_buffer)
	return NULL;

    while((c = fgetc(pf)) != '\012') {
	if(c == EOF) {
	    *p_EOF = EOF;
	    break;
	}
	p_buffer[pos++] = (char)c;
	if(pos == *p_limit) {
	    while(((c = fgetc(pf)) != EOF) && (c != '\012'))
		/*NOTHING*/;
	    break;
	}
	if(pos == size) {
	    if(size == INT_MAX)
		break;
	    /* size += GL_BUFFER_SIZE; */
	    size = ((INT_MAX - GL_BUFFER_SIZE) < size) ? 
		INT_MAX : (size + GL_BUFFER_SIZE);
	    p_tmp = (char *)realloc (p_buffer, size * sizeof(char));
	    if(!(p_tmp)) { 
		free(p_buffer); 
		RWRITE_FATAL("Out of memory.");
	    }
	    p_buffer = p_tmp;
	}
    }
    p_buffer[pos] = '\0';
    *p_limit -= pos;
    return p_buffer;
}
/*************** End of contribution from Mr. Camillo S{rs. ***************/

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

/********** Originally from Berkeley Unix's write(1) *****************/
/************** Almost totally rewritten by tri **********************/
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
		char *userhome,
		char ***ttylist)
{
	struct utmp u;
	time_t bestatime, atime;
	int ufd, nloggedttys, nttys, msgsok, no_timecomp;
	char atty[MAXPATHLEN + 1];
	char **all_ttys;
	int all_ttys_size;
	int ttylistlen;

	all_ttys = NULL;
	all_ttys_size = 0;
	ttylistlen = 0;
	nloggedttys = nttys = 0;
	if((rc_read_p()) && rc_tty_list) {
	    int i;
	    char *hlp;

	    for(i = 0; rc_tty_list[i]; i++) {
		char tty[MAXPATHLEN + 1];

		if(((rc_tty_list[i][0]) == '~') && 
		   ((rc_tty_list[i][1]) == '/') &&
		   ((strlen(rc_tty_list[i]) + strlen(userhome)) < 
		    MAXPATHLEN)) {
		    sprintf(tty, "%s/%s", userhome, &(rc_tty_list[i][2]));
		} else {
		    strcpy(tty, rc_tty_list[i]);
		}
		if(term_chk(tty, uid, &msgsok, &atime))
		    continue;	/* bad term? skip */
		if(!msgsok)
		    continue;	/* skip ttys with msgs off */
		if(!(hlp = (char *)malloc(strlen(tty) + 1))) {
		    RWRITE_FATAL("Out of memory.");
		}
		strcpy(hlp, tty);
		if(!(add_to_list(&all_ttys,
				 &all_ttys_size,
				 hlp))) {
		    RWRITE_FATAL("Out of memory.");
		}
		ttylistlen++;
	    }
	}
	if((rc_read_p()) && (no_tty_delivery()) && (ttylistlen > 0)) {
	    if(ttylist) {
		*ttylist = all_ttys;
	    } else {
		int i;
		for(i = 0; i < ttylistlen; i++) {
		    if(all_ttys[i])
			free(all_ttys[i]);
		    /* free(all_ttys); XXX XXX XXX XXX */
		}
	    }
	    return DELIVER_OK;
	}
	if((ufd = open(_PATH_UTMP, O_RDONLY)) < 0) {
	    if(ttylistlen < 1)
		return DELIVER_USER_NOT_IN;
	    else
		return DELIVER_OK;
	}
	if(deliver_all_ttys()) {
	    while(read(ufd, (char *) &u, sizeof(u)) == sizeof(u)) {
#ifndef NO_UT_TYPE_IN_STRUCT_UTMP
#ifdef USER_PROCESS
		/*
		 * System V dumps all kind of shit into the utmp.
		 * USER_PROCESS should not be defined in non SYSV-
		 * systems, but...
		 */
 		if(u.ut_type != USER_PROCESS)
 		    continue;
#endif
#endif
		if(strncmp(user, u.ut_name, sizeof(u.ut_name)) == 0) {
		    char *hlp;

		    nloggedttys++;
		    strcpy(atty, "/dev/");
		    strncat(atty, u.ut_line, UT_LINESIZE);
		    atty[strlen("/dev/") + UT_LINESIZE] = '\000';
		    if(term_chk(atty, uid, &msgsok, &atime))
			continue; /* bad term? skip */
		    if(!msgsok)
			continue; /* skip ttys with msgs off */
		    nttys++;
		    if(!(hlp = (char *)malloc(strlen(atty) + 1))) {
			RWRITE_FATAL("Out of memory.");
		    }
		    strcpy(hlp, atty);
		    if(!(add_to_list(&all_ttys,
				     &all_ttys_size,
				     hlp))) {
			RWRITE_FATAL("Out of memory.");
		    }
		    ttylistlen++;
		}
	    }
        } else {
	    char best_tty[MAXPATHLEN + 1];

	    best_tty[0] = 0;
	    bestatime = 0;
	    no_timecomp = 0;
	    while(read(ufd, (char *) &u, sizeof(u)) == sizeof(u)) {
#ifndef NO_UT_TYPE_IN_STRUCT_UTMP
#ifdef USER_PROCESS
		/*
		 * System V dumps all kind of shit into the utmp.
		 * USER_PROCESS should not be defined in non SYSV-
		 * systems, but...
		 */
 		if(u.ut_type != USER_PROCESS)
 		    continue;
#endif
#endif
		if(strncmp(user, u.ut_name, sizeof(u.ut_name)) == 0) {
		    nloggedttys++;
		    strcpy(atty, "/dev/");
		    strncat(atty, u.ut_line, UT_LINESIZE);
		    atty[strlen("/dev/") + UT_LINESIZE] = '\000';
		    if(term_chk(atty, uid, &msgsok, &atime))
			continue;	/* bad term? skip */
		    if(!msgsok)
			continue;	/* skip ttys with msgs off */
		    if(HAS_TTY_FORCE()) {
			if(!(strcmp(atty, tty_force))) {
			    strcpy(best_tty, atty);
			    no_timecomp = 1;
			    nttys++;
			}
			continue;
		    }
		    nttys++;
		    if(HAS_TTY_HINT() && (!(strcmp(atty, tty_hint)))) {
			strcpy(best_tty, atty);
			no_timecomp = 1;
			continue;
		    }
		    if(!(no_timecomp) && (atime > bestatime)) {
			bestatime = atime;
			strcpy(best_tty, atty);
		    }
		}
	    }
	    if(best_tty[0]) {
		char *hlp;

		if(!(hlp = (char *)malloc(strlen(best_tty) + 1))) {
		    RWRITE_FATAL("Out of memory.");
		}
		strcpy(hlp, best_tty);
		if(!(add_to_list(&all_ttys,
				 &all_ttys_size,
				 hlp))) {
		    RWRITE_FATAL("Out of memory.");
		}
	    }
	}
	close(ufd);
	if (nloggedttys == 0)
	    return DELIVER_USER_NOT_IN;
	if(nttys >= 1) {
	    if(ttylist) {
		*ttylist = all_ttys;
	    } else {
		int i;
		for(i = 0; i < ttylistlen; i++) {
		    if(all_ttys[i])
			free(all_ttys[i]);
		    /* free(all_ttys); XXX XXX XXX XXX */
		}
	    }
	    return DELIVER_OK;
	}
	return DELIVER_PERMISSION_DENIED;
}
/********* END OF STUFF FROM Berkeley Unix's write(1) **********/

int writeto(char *tty,
	    char **msg,
	    char *from, 
	    char *fromhost,
	    char *via,
	    char *remotehost,
	    char *nowstr, 
	    int replied)
{
    int ttyp;
    FILE *f;
#ifdef HAVE_FDOPEN
    int tmpfd;
#endif

#ifdef HAVE_FDOPEN
    if(0 > (tmpfd = open(tty, O_NONBLOCK |  O_WRONLY | O_APPEND, 0)))
	return 0;
    if(!(f = fdopen(tmpfd, "a")))
	return 0;
#else
    if(!(f = fopen(tty, "a")))
	return 0;
#endif /* HAVE_FDOPEN */
    if((ttyp = isatty(fileno(f))) && ring_bell())
	fputc('\007', f);
    fputc('\n', f);
    if(ttyp)
	fputc('\r', f);

    if(!(from = dequote_str(from, 1024, NULL))) {
	RWRITE_FATAL("Out of memory.");
    }
    if(!(remotehost = dequote_str(remotehost, 1024, NULL))) {
	RWRITE_FATAL("Out of memory.");
    }
    if(!(fromhost = dequote_str(fromhost, 1024, NULL))) {
	RWRITE_FATAL("Out of memory.");
    }
    if(via)
	if(!(via = dequote_str(via, 1024, NULL))) {
	    RWRITE_FATAL("Out of memory.");
	}
    if((!nowstr) || (!(*nowstr))) {
	nowstr = "xxx";
    }
    if(strcmp(remotehost, fromhost))
	if(via) {
	    fprintf(f, 
		    "Message from %s@%s (via %s%c%s) at %s%s:\n", 
		    from, 
		    fromhost, 
		    via,
		    PATH_SEPARATOR,
		    remotehost, 
		    nowstr,
		    (replied ? " (autoreplied)" : ""));
	} else {
	    fprintf(f, 
		    "Message from %s@%s (via %s) at %s%s:\n",
		    from, 
		    fromhost, 
		    remotehost, 
		    nowstr,
		    (replied ? " (autoreplied)" : ""));
	}
    else
	fprintf(f, 
		"Message from %s@%s at %s%s:\n", 
		from, 
		fromhost, 
		nowstr,
		(replied ? " (autoreplied)" : ""));
    free(from);
    free(fromhost);
    free(remotehost);
    if(via)
	free(via);
#ifdef HAVE_FDOPEN
    {
	int ret = 0;
	ret = dequote_and_write(f, msg, max_lines_in(), max_chars_in(), ttyp); 
	fclose(f);
	close(tmpfd);
	return(ret);
    }
#else
    return(dequote_and_write(f, msg, max_lines_in(), max_chars_in(), ttyp));
#endif /* NO_FDOPEN */
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
    char rcfilename[MAXPATHLEN + 1];
    int d_status;
    time_t now;
    char *nowstr;
    struct passwd *pwd;
    char **all_ttys;
    int succeeded;
    int len;
    
    now = time(NULL);
    nowstr = ctime(&now);
    if((!nowstr) || (!(*nowstr))) {
	nowstr = "xxx";
    } else if('\n' == (nowstr[len = (strlen(nowstr) - 1)])) {
	nowstr[len] = '\000';
    }

    succeeded = 0;

    if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	return DELIVER_NO_SUCH_USER;
    }
    if(!(rc_read_p())) {
	sprintf(rcfilename, "%s/%s", pwd->pw_dir, RWRITE_CONFIG_FILE);
	read_rc(RWRITE_GLOBAL_CONFIG);
	read_rc(rcfilename);
    }
#ifdef DEBUG
    print_configuration();
#endif
    if(!(is_allowed(from, fromhost))) {
	return DELIVER_PERMISSION_DENIED;
    }
    if(!sent_autoreply)
	sent_autoreply = give_users_autoreply(pwd->pw_dir);
    if((d_status = search_utmp(user, pwd->pw_uid,
			       pwd->pw_dir, &all_ttys)) != 
       DELIVER_OK)
	return(d_status);
    if(all_ttys) {
	int i;
	for(i = 0; all_ttys[i]; i++) {
#ifdef DEBUG
	    fprintf(stdout, "%03d Writeto \"%s\".\n", RWRITE_DEBUG, all_ttys[i]);
#endif
	    succeeded = (writeto(all_ttys[i],
				 msg,
				 from, 
				 fromhost,
				 via,
				 remotehost,
				 nowstr,
				 sent_autoreply) ||
			 succeeded);
	    free(all_ttys[i]);
	}
	free(all_ttys);
    }
    return(succeeded ? DELIVER_OK : DELIVER_PERMISSION_DENIED);
}

int can_deliver(char *user, char *from, char *fromhost, int autoreply)
{
    char tty[MAXPATHLEN + 1];
    struct passwd *pwd;
    int ret;

    if((!(pwd = getpwnam(user))) || (!(pwd->pw_dir))) {
	return DELIVER_NO_SUCH_USER;
    }
    if(!(rc_read_p())) {
	sprintf(tty, "%s/%s", pwd->pw_dir, RWRITE_CONFIG_FILE);
	read_rc(RWRITE_GLOBAL_CONFIG);
	read_rc(tty);
    }
    /*
     * Check user's allow and deny file only if remote user has
     * already told who he is.
     */
    if(from && fromhost && (!(is_allowed(from, fromhost))))
	return DELIVER_PERMISSION_DENIED;
    ret = search_utmp(user, pwd->pw_uid, pwd->pw_dir, NULL);
    if(autoreply &&
       from &&
       (*from) &&
       (!sent_autoreply) &&
       ((ret == DELIVER_USER_NOT_IN) ||
	(ret == DELIVER_OK))) {
	sent_autoreply = give_users_autoreply(pwd->pw_dir);
    }
    return(ret);
}

int main(int argc, char **argv)
{
    char *cmd;
    char **message;
    int cmdmaxlen;
    int cmdeofp;
    int udp = 0;
    char *udpbuf;
    char *udpbufsav = NULL;
    int udpbuflen;

    if((argc > 1) && (!(strcmp("-version", argv[1])))) {
	fprintf(stderr, "Rwrited version %s.\n", RWRITED_VERSION_NUMBER);
	exit(0);
    }
#ifdef HAVE_GETEUID
    server_euid = geteuid();
#else
    server_euid = getuid();
#endif
#ifdef HAVE_GETEGID
    server_egid = getegid();
#else
    server_egid = getgid();
#endif
    if((argc > 1) && (argv[1][0] == '-') && (argv[1][1] == '\000')) {
	/*
	 * We can run this on cmd-line using flag -.
	 */
	cmd_line = 1;
	set_hostnames(0, NULL);
	udp = 0; /* commad line is reliable :) */
    } else {
	/*
	 * Otherwise we presume we are running through a socket.
	 */
	cmd_line = 0;
	if(!(udp = ((argc > 1) && (!(strcmp(argv[1], "udp"))))))
	    set_hostnames(1, NULL);
    }
 read_another_udp:;
    if(udp) {
	struct sockaddr_in sin;
	int sin_len = sizeof(sin);

	if(!udpbufsav) {
	    if(!(udpbufsav = 
		 (udpbuf = 
		  (char *)malloc((UDP_DIALOG_LEN_MAX + 1) * sizeof(char))))) {
		/* Out of memory */
		exit(1);
	    }
	} else {
	    udpbuf = udpbufsav;
	}
	if(0 >= 
	   (udpbuflen = recvfrom(0, 
				 udpbuf, 
				 UDP_DIALOG_LEN_MAX, 
				 0,
				 (struct sockaddr *)(&sin),
				 &sin_len))) {
	    /* Failed */
	    exit(1);
	}
	set_hostnames(1, &sin);
	udpbuf[udpbuflen] = '\000';
    }
    rwrite_helo(udp);
    rwrite_ver(udp);
    rwrite_prot(udp);
    if(!udp) {
	if(!(identify_remote_by_identd(identd_from_user, 
				       sizeof(identd_from_user))))
	    identd_from_user[0] = '\000';
    }
    message = NULL;
    do {
	rwrite_ready(udp);
	cmdmaxlen = 1024;
	cmdeofp = 0;
	if(udp) {
	    if(*udpbuf) {
		cmd = udpbuf;
		while((*udpbuf) && (*udpbuf != '\012'))
		    udpbuf++;
		if(*udpbuf) {
		    *udpbuf = '\000';
		    udpbuf++;
		} else {
		    cmdeofp = 1;
		}
	    } else {
		cmd = NULL;
		cmdeofp = 1;
	    }
	} else {
	    int len;
	    cmd = gm_getline(stdin, &cmdmaxlen, &cmdeofp);
	    if(cmd && (len = strlen(cmd)) && (cmd[len - 1] == '\015'))
		cmd[len - 1] = '\000';
	}
	if(!cmd)
	    break;
	if(strlen(cmd)) {
	    if((!(strcmp(cmd, "bye")) || (!(strcmp(cmd, "BYE"))))) {
		rwrite_bye(udp);
	    } else if((!(strcmp(cmd, "quit"))) || (!(strcmp(cmd, "QUIT")))) {
		if(udp)
		    goto leave_parse_loop;
		rwrite_quit(udp);
	    } else if((!(strcmp(cmd, "help"))) || (!(strcmp(cmd, "HELP")))) {
		rwrite_help(udp);
	    } else if((!(strcmp(cmd, "helo"))) || (!(strcmp(cmd, "HELO")))) {
		rwrite_helo(udp);
	    } else if((!(strcmp(cmd, "ver"))) || (!(strcmp(cmd, "VER")))) {
		rwrite_ver(udp);
	    } else if((!(strcmp(cmd, "prot"))) || (!(strcmp(cmd, "PROT")))) {
		rwrite_prot(udp);
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
		if(!udp) {
		    set_hostnames(((!cmd_line) ? 1 : 0), NULL);
		    RWRITE_MSG(RWRITE_RSET_OK, "RSET ok.");
		}
	    } else if((!(strcmp(cmd, "from"))) || 
		      (!(strcmp(cmd, "FROM"))) ||
		      (!(strncmp(cmd, "from ", 5))) || 
		      (!(strncmp(cmd, "FROM ", 5)))) {
		char *user_from = get_user_name(cmd);
		
		from_user[0] = '\000';
		if((!user_from) || 
		   (!(strlen(user_from))) || 
		   ((strlen(user_from) + 2) >= sizeof(from_user))) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FROM userid");
		    goto out_of_parse;
		}
		strcpy(from_user, user_from);
#ifdef NO_IDENTD
		if(!udp)
		    RWRITE_MSG(RWRITE_SENDER_OK, "Sender ok.");
#else
		if(!(identd_from_user[0])) {
		    /* Identd failed so no-one knows. */
		    fake_user = 1;
		    if(!udp)
			RWRITE_MSG(RWRITE_SENDER_OK, 
				   "Sender ok but nonconfirmed.");
		} else if(strcmp(from_user, identd_from_user)) {
		    /* It's a fake. */
		    fake_user = 2;
		    if(!udp)
			RWRITE_MSG(RWRITE_SENDER_OK, 
				   "Sender ok but doesn't match with identd.");
		} else {
		    /* It's confirmed by identd. */
		    fake_user = 0;
		    if(!udp)
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

		reset_rc();
		to_user[0] = '\000';
		tty_hint[0] = '\000';
		tty_force[0] = '\000';
		sent_autoreply = 0;
		if((!user_to) || 
		   (!(len = strlen(user_to))) || 
		   (len >= sizeof(to_user))) {
		    if(!udp)
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
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_SYNTAX, 
				       "Syntax: TO userid [tty]");
			goto out_of_parse;
		    }
		    if((tty_to[0] == '[') && (tty_to[len - 1] == ']')) {
			/* It's a hint */
			tty_to[len - 1] = '\000';
			tty_to++;
			if(!(strlen(tty_to))) {
			    if(!udp)
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
		if(!udp)
		    RWRITE_MSG(RWRITE_RCPT_OK, "Recipient ok.");
	    } else if((!(strcmp(cmd, "fhst"))) ||
		      (!(strcmp(cmd, "FHST"))) ||
		      (!(strncmp(cmd, "fhst ", 5))) || 
		      (!(strncmp(cmd, "FHST ", 5)))) {
		char *hlp1;
		char *frm = get_user_name(cmd);
		if((!frm) ||
		   (!(strlen(frm)))) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_SYNTAX, 
				   "Syntax: FHST remote.host ...");
		    goto out_of_parse;
		}
		if(!(strcmp(frm, remote_host))) {
		    if(!udp)
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
		if(!udp)
		    RWRITE_MSG(RWRITE_FHST_OK, "Original sender host ok.");
	    } else if((!(strcmp(cmd, "fwds"))) ||
		      (!(strcmp(cmd, "FWDS"))) ||
		      (!(strncmp(cmd, "fwds ", 5))) || 
		      (!(strncmp(cmd, "FWDS ", 5)))) {
		char *n_str = get_user_name(cmd);
		char *hlp;
		int n;

		if((!n_str) ||
		   (!(strlen(n_str)))) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FWDS number");
		    goto out_of_parse;
		}
		hlp = n_str;
		if(*hlp == '-')
		    hlp++;
		for(/*NOTHING*/; *hlp; hlp++) {
		    if(!(isdigit(*hlp))) {
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: FWDS number");
			goto out_of_parse;
		    }
		}
		if((n = atoi(n_str)) < -1) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_SYNTAX, 
				   "Number of forwards less than -1.");
		    goto out_of_parse;
		}
		if((n != -1) && (n <= RWRITE_FWD_LIMIT)) {
		    if(!udp)
			RWRITE_MSG(RWRITE_RCPT_OK_TO_FWD, "Ok to forward.");
		} else {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_FWD_LIMIT_EXCEEDED,
				   "Forward limit exceeded.");
		}
		fwd_count = n;
	    } else if((!(strcmp(cmd, "vrfy"))) || (!(strcmp(cmd, "VRFY")))) {
		int d_status;
		if(!(to_user[0])) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_NO_ADDRESS, 
				   "Use TO before VRFY.");
		    goto out_of_parse;
		}
		if((d_status = can_deliver(to_user, 
					   (from_user[0] ? from_user : NULL),
					   from_host,
					   1)) != DELIVER_OK) {
		    switch(d_status) {
		    case DELIVER_NO_SUCH_USER:
#ifndef DO_NOT_TELL_USERS
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_NO_SUCH_USER, "No such user.");
			break;
#endif
		    case DELIVER_USER_NOT_IN:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_USER_NOT_IN, "User not in.");
			break;
		    case DELIVER_PERMISSION_DENIED:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_PERMISSION_DENIED, 
				       "Permission denied.");
			break;
		    default:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_UNKNOWN, "Unknown error.");
			break;
		    }
		    goto out_of_parse;
		}
		if(!udp)
		    RWRITE_MSG(RWRITE_RCPT_OK_TO_SEND, "Recipient ok to send.");
	    } else if((!(strcmp(cmd, "data"))) || (!(strcmp(cmd, "DATA")))) {
		if(message) {
		    int i;
		    for(i = 0; message[i]; i++)
			free(message[i]);
		    free(message);
		}
		if(udp) {
		    int msg_size;
		    int msg_line;

		    if(!(message = (char **)calloc(BUF_ALLOC_STEP,
						   sizeof(char *)))) {
			/* Out of memory. */
			exit(1);
		    }
		    msg_size = BUF_ALLOC_STEP;
		    msg_line = 0;
		    do {
			if((msg_line + 2) >= msg_size) {
			    char **newmsg;

			    if(!(newmsg = 
				 (char **)calloc(BUF_ALLOC_STEP + msg_size, 
						 sizeof(char *)))) {
				/* Out of memory. */
				exit(1);
			    }
			    memcpy(newmsg, message, msg_size * sizeof(char *));
			    free(message);
			    message = newmsg;
			    msg_size += BUF_ALLOC_STEP;
			}
			if(*udpbuf) {
			    cmd = udpbuf;
			    while((*udpbuf) && (*udpbuf != '\012'))
				udpbuf++;
			    if(*udpbuf) {
				*udpbuf = '\000';
				udpbuf++;
			    } else {
				cmdeofp = 1;
			    }
			} else {
			    cmd = NULL;
			    cmdeofp = 1;
			}
			if((!cmd) || ((cmd[0] == '.') && (cmd[1] == '\000')))
			    break;
			if(!(message[msg_line] = 
			     (char *)malloc(strlen(cmd) + 1))) {
			    /* Out of memory. */
			    exit(1);
			}
			strcpy(message[msg_line++], cmd);
		    } while(!cmdeofp);
		} else {
		    if(!(message =
			 get_msg(stdin, DATA_MAXLINES, DATA_MAXCHARS, udp))) {
			RWRITE_MSG(RWRITE_ERR_NO_MESSAGE, "No message.");
			goto out_of_parse;
		    }
		    RWRITE_MSG(RWRITE_MSG_OK, "Message ok.");
		}
	    } else if((!(strcmp(cmd, "send"))) || (!(strcmp(cmd, "SEND")))) {
		int d_status;

		if(!(from_user[0])) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_NO_SENDER, 
				   "Use FROM before SEND.");
		    goto out_of_parse;
		}
		if(!(to_user[0])) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_NO_ADDRESS, 
				   "Use TO before SEND.");
		    goto out_of_parse;
		}
		if(!message) {
		    if(!udp)
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
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_NO_SUCH_USER, "No such user.");
			break;
		    case DELIVER_USER_NOT_IN:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_USER_NOT_IN, "User not in.");
			break;
		    case DELIVER_PERMISSION_DENIED:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_PERMISSION_DENIED, 
				       "Permission denied.");
			break;
		    default:
			if(!udp)
			    RWRITE_MSG(RWRITE_ERR_UNKNOWN,"Unknown error.");
			break;
		    }
		    goto out_of_parse;
		}
		if(!udp)
		    RWRITE_MSG(RWRITE_DELIVERY_OK, "Message delivered.");
	    } else if((!(strcmp(cmd, "quote"))) ||
		      (!(strcmp(cmd, "QUOTE"))) ||
		      (!(strncmp(cmd, "quote ", 6))) || 
		      (!(strncmp(cmd, "QUOTE ", 6)))) {
		char *qcmd =  get_user_name(cmd);
		if((!qcmd) && (!(*qcmd))) {
		    if(!udp)
			RWRITE_MSG(RWRITE_ERR_SYNTAX, "Syntax: QUOTE cmd ...");
		    goto out_of_parse;
		}
		/* Here we should deal with commands like CHARSET. XXX */
		if(!udp)
		    RWRITE_MSG(RWRITE_ERR_QUOTE_CMD_UNKNOWN, 
			       "Unknown QUOTE command.");
	    } else {
		if(!udp)
		    RWRITE_MSG(RWRITE_ERR_SYNTAX, "Does not compute.");
            }
   	}
	if(!udp)
	    free(cmd);
    out_of_parse:;
    } while(!cmdeofp);
 leave_parse_loop:
    if(udp) {
	identd_from_user[0] = '\000';
	from_user[0] = '\000';
	to_user[0] = '\000';
	tty_hint[0] = '\000';
	tty_force[0] = '\000';
	my_host[sizeof(my_host) - 1] = '\000';
	fake_user = 0;
	goto read_another_udp;
    }
    rwrite_bye(udp);
    /*NOTREACHED*/
    return 0;
}

/* EOF (rwrited.c) */
