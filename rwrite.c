/*  -*- c -*-
 *
 * $RCSfile: rwrite.c,v $
 * ----------------------------------------------------------------------
 * Client to RWP-protocol
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:28:07 1994 tri
 * Last modified: Tue Oct 24 23:53:56 1995 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.40 $
 * $State: Exp $
 * $Date: 1995/10/24 21:54:56 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrite.c,v $
 * Revision 1.40  1995/10/24 21:54:56  tri
 * Added support for gnu libreadline.
 *
 * Revision 1.39  1995/02/10  07:32:50  tri
 * Wrap it up and call it 1.1.
 *
 * Revision 1.38  1994/12/15  04:57:33  tri
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
 * Revision 1.30  1994/12/12  22:09:03  tri
 * Fixed the annoying quotation bug.
 *
 * Revision 1.29  1994/12/12  21:17:55  tri
 * Closed files more pedantically.
 * Fix by toka & tri.
 *
 * Revision 1.28  1994/12/12  19:50:16  tri
 * Fixed a small but potentially harmful fclose(NULL) -bug.
 *
 * Revision 1.27  1994/12/12  15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.26  1994/12/12  11:30:54  tri
 * Another one.
 *
 * Revision 1.25  1994/12/12  11:25:57  tri
 * Glitch.
 *
 * Revision 1.24  1994/12/12  11:08:14  tri
 * Moved the name of the file containing last
 * message into rwrite.h
 *
 * Revision 1.23  1994/12/12  11:03:42  tri
 * Added compatibility fixes from toka.
 *
 * Revision 1.22  1994/12/11  21:25:30  tri
 * Cleaned up some warnings.  No functional changes.
 *
 * Revision 1.21  1994/12/11  18:16:28  tri
 * Some portability fixes and configuration stuff
 * moved to Makefile.
 *
 * Revision 1.20  1994/12/11  14:56:13  tri
 * Minor fix.
 *
 * Revision 1.19  1994/12/11  13:29:29  tri
 * Background message sending can be defaulted in
 * rwriterc.  Explicit -b or -B flag overrides the
 * default.
 *
 * Revision 1.18  1994/12/10  23:35:52  tri
 * Variable resend was not properly initialized.
 *
 * Revision 1.17  1994/12/10  11:28:38  tri
 * Last known method to send terminal control codes
 * through correctly configured rwrite is now diabled.
 *
 * Revision 1.16  1994/12/09  23:57:49  tri
 * Added a outbond message logging.
 *
 * Revision 1.15  1994/12/09  21:08:12  tri
 * Added flush_stdin().
 *
 * Revision 1.14  1994/12/09  20:44:05  tri
 * Nuked the global resend variable.
 *
 * Revision 1.13  1994/12/08  22:56:45  tri
 * Fixed the quotation system on message
 * delivery.  Same message can now be quoted
 * differently for the each receiver.
 * Also the autoreplies are now quoted right.
 *
 * Revision 1.12  1994/11/20  11:51:17  tri
 * sys/time.h is included.
 *
 * Revision 1.11  1994/11/20  11:45:01  tri
 * Added a few minor lines to complete rwp.
 *
 * Revision 1.10  1994/11/20  11:16:28  tri
 * Autoreply header now contains a time stamp
 *
 * Revision 1.9  1994/11/20  11:08:12  tri
 * Fixed minor quotation bug in background mode.
 *
 * Revision 1.8  1994/11/20  00:47:18  tri
 * Completed autoreply and quotation stuff.
 * We are almost there now.
 *
 * Revision 1.7  1994/10/04  20:50:22  tri
 * Conforms now the current RWP protocol.
 *
 * Revision 1.6  1994/09/20  18:52:51  tri
 * Fixed few minor warnings.
 *
 * Revision 1.5  1994/09/19  22:43:38  tri
 * Warning about defaulted tcp-port now comes only
 * in verbose mode.
 *
 * Revision 1.4  1994/09/19  22:40:37  tri
 * TOOK replaced by VRFY and made some considerable
 * cleanup.
 *
 * Revision 1.3  1994/09/15  20:14:42  tri
 * Completed the support of RWP version 1.0.
 *
 * Revision 1.2  1994/09/14  16:04:50  tri
 * Added -r option.
 *
 * Revision 1.1  1994/09/14  14:58:53  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Timo J. Rinne <tri@cirion.fi> and Cirion oy.
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
#define __RWRITE_C__ 1
#ifndef lint
static char *RCS_id = "$Id: rwrite.c,v 1.40 1995/10/24 21:54:56 tri Exp $";
#endif /* not lint */

#define RWRITE_VERSION_NUMBER	"1.1"	/* Client version   */

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

#ifndef DONT_FLUSH_INPUT_IN_FAILURE
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif
#endif

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifndef MAXPATHLEN
#  define MAXPATHLEN PATH_MAX
#endif

#include "rwrite.h"

int verbose = 0;
int quiet = 0;
int fwds = 0;
char **autoreply = NULL;
int autoreply_lines = 0;
int autoreply_sz = 0;
char **last_msg = NULL;

int unquote_and_raw_write_str(FILE *f, char *str)
{
    unsigned char *s = (unsigned char *)str;
    int c;

    if(!s)
	return 0;
    while(*s) {
        if(*s == '=') {
            s++;
            if(((*s >= '0') && (*s <= '9')) ||
               ((*s >= 'a') && (*s <= 'f')) ||
               ((*s >= 'A') && (*s <= 'F'))) {
                c = ((*s >= '0') && (*s <= '9')) ? (*s - '0') :
                    (((*s >= 'a') && (*s <= 'f')) ? (*s - 'a' + 10) :
                     (((*s >= 'A') && (*s <= 'F')) ? (*s - 'A' + 10) : 0));
                s++;
		if(((*s >= '0') && (*s <= '9')) ||
		   ((*s >= 'a') && (*s <= 'f')) ||
		   ((*s >= 'A') && (*s <= 'F'))) {
		    c = (c << 4) | 
			(((*s >= '0') && (*s <= '9')) ? (*s - '0') :
			 (((*s >= 'a') && (*s <= 'f')) ? (*s - 'a' + 10) :
			  (((*s >= 'A') && (*s <= 'F')) ?
			   (*s - 'A' + 10) : 0)));
		} else {
		    c = '=';
		    s--;
		    s--;
		}
            } else {
		c = '=';
		s--;
	    }
        } else {
            c = (int)(*s);
        }
	fputc(c, f);
	s++;
    }
    fputc('\n', f);
    return 1;
}

/*
 * There is a possible race condition here if many messages are written
 * simultaneously.
 */
FILE *open_history_write()
{
    char path[MAXPATHLEN + 1];
    char *home;
    FILE *f;

    if(!(home = getenv("HOME"))) {
	return NULL;
    }
    sprintf(path, "%s/%s#", home, RWRITE_LAST_SENT_MSG);
    f = fopen(path, "w");
    if(f)
#ifdef HAVE_FCHMOD
	fchmod(fileno(f), 0600);
#else
	chmod(path, 0600);
#endif
    return(f);
} 

int close_history_write(FILE *f)
{
    char path1[MAXPATHLEN + 1];
    char path2[MAXPATHLEN + 1];
    char *home;
    int r;

    if((!(home = getenv("HOME"))) || !f) {
	return 0;
    }
    fclose(f);
    sprintf(path1, "%s/%s#", home, RWRITE_LAST_SENT_MSG);
    sprintf(path2, "%s/%s", home, RWRITE_LAST_SENT_MSG);
    r = rename(path1, path2);
    chmod(path2, 0600);
    return(r ? 0 : 1);
} 

FILE *open_history_read()
{
    char path[MAXPATHLEN + 1];
    char *home;

    if(!(home = getenv("HOME"))) {
	return NULL;
    }
    sprintf(path, "%s/%s", home, RWRITE_LAST_SENT_MSG);
    return(fopen(path, "r"));
}

int read_char(int fd)
{
    unsigned char buf[4];
    int ret;

    ret = read(fd, buf, 1);

    if(ret != 1)
	return -1;

    return((int)buf[0]);
}

char *read_line_fd(int f)
{
    char *buf;
    int buflen;
    int c, i;

    if(!(buf = ((char *)malloc(BUF_ALLOC_STEP))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    for(i = 0; (-1 != (c = (read_char(f)))); i++) {
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
    if((c == -1) && (i == 0)) {
	free(buf);
	return NULL;
    }
    buf[i] = '\000';
    return(buf);
}

char *read_line(char *prompt, FILE *f)
{
    char *buf;
    int buflen;
    int c, i;

    if(!(buf = ((char *)malloc(BUF_ALLOC_STEP))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    if(prompt)
	fputs(prompt, stderr);
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

/*
 * Read message data until EOF from console.
 */
char **read_user_message(FILE *f)
{
    int buflen, i;
    char **buf;
    char *line;

    last_msg = NULL;
    if(!(buf = ((char **)malloc(BUF_ALLOC_STEP * sizeof(char *)))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    for(i = 0; /*NOTHING*/; i++) {
	char *hlp;

	if(f == stdin) {
#ifdef HAVE_LIBREADLINE
	    line = readline(RWRITE_PROMPT);
	    if(line) {
		add_history(line);
	    } else {
		putchar('\n');
	    }
#else
	    line = read_line(RWRITE_PROMPT, f);
#endif
	} else {
	    line = read_line(NULL, f);
	}
	if(!line) {
	    if(!i) {
		free(buf);
		return NULL;
	    }
	    buf[i] = NULL;
	    last_msg = buf;
	    return buf;
	}
	hlp = quote_str(line);
	free(line);
	line = hlp;
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

int dump_msg_to_outlogs(char **msg, 
			char *addr, 
			int failed, 
			char *userhome, 
			int udp_p)
{
    int i, n;
    FILE *f;
    time_t now;
    char *nowstr;
    int len;

    if((!(rc_read_p())) || 
       (!rc_outlog) || 
       (!(*rc_outlog)) ||
       (!msg) || 
       (!(*msg)) ||
       (!userhome))
	return 0;

    now = time(NULL);
    nowstr = ctime(&now);
    if((!nowstr) || (!(*nowstr))) {
	nowstr = "xxx";
    } else if('\n' == (nowstr[len = (strlen(nowstr) - 1)])) {
	nowstr[len] = '\000';
    }

    if((!addr) || (!(*addr)))
	addr = "xxx";
    for((i = 0, n = 0); rc_outlog[i]; i++) {
	char logfile[MAXPATHLEN + 1];

	if(((rc_outlog[i][0]) == '~') && 
	   ((rc_outlog[i][1]) == '/') &&
	   ((strlen(rc_outlog[i]) + strlen(userhome)) <  MAXPATHLEN)) {
	    sprintf(logfile, "%s/%s", userhome, &(rc_outlog[i][2]));
	} else {
	    strcpy(logfile, rc_outlog[i]);
	}
	if(f = fopen(logfile, "a")) {
	    fprintf(f, "\n%s%s%cessage to %s at %s:\n", 
		    (failed ? "Failed " : ""),
		    (udp_p ? "UDP " : ""),
		    ((failed || udp_p) ? 'm' : 'M'),
		    addr, 
		    nowstr);
	    dequote_and_write(f,
			      msg, 
			      max_lines_in(), 
			      max_chars_in(),
			      0);
	    fclose(f);
	    n++;
	} else {
	    fprintf(stderr,
		    "rwrite: Warning, can't open outlog file \"%s\".\n", 
		    logfile);
	}
    }
    return(n);
}

char *read_rwp_resp(int s, int *code)
{
    char *ret;
    int c;

    if(!(ret = read_line_fd(s))) {
	return NULL;
    }
    c = atoi(ret);
    if(code)
	*code = c;
    return ret;
}

int write_string(int fd, char *s)
{
    int n, len, r;

    len = strlen(s);
    n = 0;
    while(len) {
	r = write(fd, &(s[n]), len);
	if(r < 0)
	    return 0;
	n += r;
	len -= r;
    }
    return 1;
}

/*
 * Is string NULL or empty or full of whitespace?
 */
int is_str_whitespace(char *str)
{
    if((!str) || (!(*str))) {
	return 1;
    } else {
	char *hlp;
	for(hlp = str; ((*hlp) && ((*hlp == ' ') || (*hlp == '\011'))); hlp++)
	    /*NOTHING*/;
	if(!(*hlp))
	    return 1;
    }
    return 0;
}





#define LEGAL_CODE(c) (((c)>=100)&&((c)<=999))
#define IGNORABLE_CODE(c) ((((c)>=500)&&((c)<=599))  && \
                           ((c) != RWRITE_AUTOREPLY) && \
                           ((c) != RWRITE_AUTOREPLY_AS_COMMENT))

#define WRITE_STRING(sock, str) {                                         \
       if(!(write_string(sock, str))) {                                   \
	   fprintf(stderr, "rwrite: Remote server closed connection.\n"); \
	   return 0;                                                      \
       } }

#define DIALOG_BEGIN 1
#define DIALOG_TO    2
#define DIALOG_FROM  3
#define DIALOG_VRFY  4
#define DIALOG_FWDS  5
#define DIALOG_DATA  6
#define DIALOG_SEND  7

int rwp_dialog(int s, 
	       char *to,
	       char *tty,
	       char *from,
	       char **msg,
	       int writehist)
{
    int code;
    char *resp, *line;
    int mode, modeattr;
    FILE *hist_file;
    int we_have_autoreply = 0;

    if(is_str_whitespace(to)) {
	fprintf(stderr, "rwrite: Empty address.\n");
	return 0;
    }
    if(is_str_whitespace(from)) {
	fprintf(stderr, "rwrite: Empty from address.\n");
	return 0;
    }

    if(!autoreply_sz) {
	if(!(autoreply = (char **)calloc(BUF_ALLOC_STEP, sizeof(char *)))) {
	    fprintf(stderr, "rwrite: Out of memory.\n");
	    return 0;
	}
	autoreply_sz = BUF_ALLOC_STEP;
    } else {
	if(autoreply_lines) {
	    int i;
	    for(i = 0; i < autoreply_lines; i++)
		free(autoreply[i]);
	}
	memset(autoreply, 0, sizeof(char *) * autoreply_sz);
    }
    autoreply_lines = 0;
    mode = DIALOG_BEGIN;
    modeattr = 0;
    while(1) {
    redo_dialog_loop:;
	if(!(resp = read_rwp_resp(s, &code))) {
	    fprintf(stderr, "rwrite: Remote server closed connection.\n");
	    return 0;
	}
	if(verbose > 1)
	    fprintf(stderr, "<<<<%s\n", resp); 
	if(!(LEGAL_CODE(code))) {
	    fprintf(stderr, "rwrite: Illegal RWP response code (%03d).\n", code);
	    return 0;
	}
	if(IGNORABLE_CODE(code)) {
	    goto redo_dialog_loop;
	}
	switch(mode) {
	case DIALOG_BEGIN:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    if(tty)
			fprintf(stderr, ">>>>TO %s %s\n", to, tty);
		    else
			fprintf(stderr, ">>>>TO %s\n", to);
		WRITE_STRING(s, "TO ");
		WRITE_STRING(s, to);
		if(tty) {
		    WRITE_STRING(s, " ");
		    WRITE_STRING(s, tty);
		}
		WRITE_STRING(s, "\012");
		goto redo_dialog_loop;
	    case RWRITE_RCPT_OK:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n",
			    code);
		    return 0;
		}
		mode = DIALOG_TO;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
#ifndef DO_NOT_TELL_USERS
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n",
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
#endif
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    default:
		fprintf(stderr, 
			"rwrite: Unexpected RWP response code (%03d).\n", 
			code);
		return 0;
	    }
	case DIALOG_TO:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>FROM %s\n", from);
		WRITE_STRING(s, "FROM ");
		WRITE_STRING(s, from);
		WRITE_STRING(s, "\012");
		goto redo_dialog_loop;
	    case RWRITE_SENDER_OK:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		mode = DIALOG_FROM;
		modeattr = 0;
		goto redo_dialog_loop;
	    /* here maybe some error handling XXX */
	    default:
		fprintf(stderr, 
			"rwrite: Unexpected RWP response code (%03d).\n", 
			code);
		return 0;
	    }
	case DIALOG_FROM:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>VRFY\n");
		WRITE_STRING(s, "VRFY\012");
		goto redo_dialog_loop;
	    case RWRITE_AUTOREPLY:
	    case RWRITE_AUTOREPLY_AS_COMMENT:
		{
		    char *hlp;
		    if(modeattr != 1) {
			fprintf(stderr, 
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    for(hlp = resp; (*hlp && (*hlp != '|')); hlp++)
			/*NOTHING*/;
		    /*
		     * We got autoreply line.
		     */
		    if(*hlp == '|') {
			hlp++;
			if((autoreply_lines + 2) >= autoreply_sz) {
			    char **newauto;
			    if(!(newauto = 
				 (char **)calloc(BUF_ALLOC_STEP + autoreply_sz,
						 sizeof(char *)))) {
				fprintf(stderr, "rwrite: Out of memory.\n");
				return 0;
			    }
			    memcpy(newauto, 
				   autoreply, 
				   autoreply_sz * sizeof(char *));
			    free(autoreply);
			    autoreply = newauto;
			    autoreply_sz += BUF_ALLOC_STEP;
			}
			if(!(autoreply[autoreply_lines] = 
			     (char *)malloc(strlen(hlp) + 1))) {
			    fprintf(stderr, "rwrite: Out of memory.\n");
			    return 0;
			}
			strcpy(autoreply[autoreply_lines++], hlp);
		    }
		}
		goto redo_dialog_loop;
	    case RWRITE_RCPT_OK_TO_SEND:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		if(autoreply_lines)
		    we_have_autoreply = 1;
		else
		    we_have_autoreply = 0;
		mode = DIALOG_VRFY;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
	    default:
		fprintf(stderr, 
			"rwrite: Unexpected RWP response code (%03d).\n", 
			code);
		return 0;
	    }
	case DIALOG_VRFY:
	    if(!fwds) {
		mode = DIALOG_FWDS;
		modeattr = 0;
		/* Fallthrough to case DIALOG_FWDS */
	    } else {
		switch(code) {
		case RWRITE_READY:
		    if(modeattr != 0) {
			fprintf(stderr, 
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    modeattr = 1;
		    {
			char tmp[32];
			sprintf(tmp, "FWDS %d", fwds);
			if(verbose > 1)
			    fprintf(stderr, ">>>>%s\n", tmp);
			WRITE_STRING(s, tmp);
			WRITE_STRING(s, "\012");
		    }
		    goto redo_dialog_loop;
		case RWRITE_RCPT_OK_TO_FWD:
		    if(modeattr != 1) {
			fprintf(stderr, 
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    mode = DIALOG_FWDS;
		    modeattr = 0;
		    goto redo_dialog_loop;
		case RWRITE_ERR_FWD_LIMIT_EXCEEDED:
		    if(modeattr != 1) {
			fprintf(stderr,
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    fprintf(stderr, "rwrite: Forward limit exceeded.\n");
		    return 0;
		default:
		    fprintf(stderr,
			    "rwrite: Unexpected RWP response code (%03d).\n",
			    code);
		    return 0;
		}
	    }
	case DIALOG_FWDS:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr,
			    "rwrite: Unexpected RWP response code (%03d).\n",
			    code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>DATA\n");
		WRITE_STRING(s, "DATA\012");
		goto redo_dialog_loop;
	    case RWRITE_GETMSG:
		{
		    int i;
		    if(modeattr != 1) {
			fprintf(stderr,
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    if(!msg) {
			if(!(msg = read_user_message(stdin))) 
			    return 0;
			else
			    writehist = 1;
		    }
		    /*
		     * Message is in msg array.
		     */
		    if(writehist) {
			if(!(hist_file = open_history_write()))
			    if(!quiet)
				fprintf(stderr,
					"rwrite: Warning, can't open history file.\n");
		    } else {
			hist_file = NULL;
		    }
		    for((i = 0, line = msg[i]); line; line = msg[++i]) {
			if(hist_file)
			    unquote_and_raw_write_str(hist_file, line);
			if(verbose > 1)
			    fprintf(stderr, ">>>>%s\n", line);
			WRITE_STRING(s, line);
			WRITE_STRING(s, "\012");
		    }
		    if(hist_file) {
			if(!(close_history_write(hist_file)))
			    if(!quiet)
				fprintf(stderr, 
					"rwrite: Warning, can't close history file.\n");
		    }
		}
		if(verbose > 1)
		    fprintf(stderr, ">>>>.\n");
		WRITE_STRING(s, ".\012");
		modeattr = 2;
		goto redo_dialog_loop;
	    case RWRITE_ERR_NO_MESSAGE:
		if(modeattr != 2) {
		    fprintf(stderr,
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Empty message.\n");
		return 0;
	    case RWRITE_MSG_OK:
		if(modeattr != 2) {
		    fprintf(stderr,
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		mode = DIALOG_DATA;
		modeattr = 0;
		goto redo_dialog_loop;
	    default:
		fprintf(stderr,
			"rwrite: Unexpected RWP response code (%03d).\n", 
			code);
		return 0;
	    }
	case DIALOG_DATA:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr,
			    "rwrite: Unexpected RWP response code (%03d).\n",
			    code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>SEND\n");
		WRITE_STRING(s, "SEND\012");
		goto redo_dialog_loop;
	    case RWRITE_DELIVERY_OK:
	    case RWRITE_DELIVERY_FORWARDED:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		mode = DIALOG_SEND;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_AUTOREPLY:
	    case RWRITE_AUTOREPLY_AS_COMMENT:
		if(we_have_autoreply) {
		    /*
		     * We have autoreply from VRFY command, but remote
		     * wants to give it to us again.  We nuke the old one
		     * and be very happy with the new version of the
		     * autoreply.
		     */
		    if(autoreply_lines) {
			int i;
			for(i = 0; i < autoreply_lines; i++)
			    free(autoreply[i]);
		    }
		    memset(autoreply, 0, sizeof(char *) * autoreply_sz);
		    we_have_autoreply = 0;
		    autoreply_lines = 0;
		}
		{
		    char *hlp;
		    if(modeattr != 1) {
			fprintf(stderr, 
				"rwrite: Unexpected RWP response code (%03d).\n",
				code);
			return 0;
		    }
		    for(hlp = resp; (*hlp && (*hlp != '|')); hlp++)
			/*NOTHING*/;
		    /*
		     * We got autoreply line.
		     */
		    if(*hlp == '|') {
			hlp++;
			if((autoreply_lines + 2) >= autoreply_sz) {
			    char **newauto;
			    if(!(newauto = 
				 (char **)calloc(BUF_ALLOC_STEP + autoreply_sz,
						 sizeof(char *)))) {
				fprintf(stderr, "rwrite: Out of memory.\n");
				return 0;
			    }
			    memcpy(newauto, 
				   autoreply, 
				   autoreply_sz * sizeof(char *));
			    free(autoreply);
			    autoreply = newauto;
			    autoreply_sz += BUF_ALLOC_STEP;
			}
			if(!(autoreply[autoreply_lines] = 
			     (char *)malloc(strlen(hlp) + 1))) {
			    fprintf(stderr, "rwrite: Out of memory.\n");
			    return 0;
			}
			strcpy(autoreply[autoreply_lines++], hlp);
		    }
		}
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
	    default:
		fprintf(stderr,
			"rwrite: Unexpected RWP response code (%03d).\n", 
			code);
		return 0;
	    }
	    /*
	     * We already have sent ok ack, so we can return 1
	     * even if something woes for now on.
	     */
	case DIALOG_SEND:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 1;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>BYE\n");
		WRITE_STRING(s, "BYE\012");
		goto redo_dialog_loop;
	    case RWRITE_BYE:
		if(modeattr != 1) {
		    fprintf(stderr, 
			    "rwrite: Unexpected RWP response code (%03d).\n", 
			    code);
		    return 1;
		}
		return 1;
	    default:
		fprintf(stderr, "rwrite: Internal error.\n");
		return 1;
	    }
	}
    }
    return 1;
}

int get_port_no(char *port)
{
    int p = 0;
    char *hlp;
    
    if(port && *port && (strlen(port) < 7)) {
	for(hlp = port; *hlp; hlp++)
	    if((*hlp < '0') || (*hlp > '9'))
		return -1;
	    else
		p = (p * 10) + (*hlp - '0');
	if(p > 0) 
	    return p;
    }
    return -1;
}

/**************** From Berkeley Unix's finger(1) *****************/
/********************* Modifications by tri *********************/
int open_to(char *name, int udp_p)
{
    int defport;
    struct in_addr defaddr;
    struct hostent *hp, def;
    struct servent *sp;
    struct sockaddr_in sin;
    int s;
    char *port = NULL;
    int portno = 0;
    char *alist[1], *host;
    u_long inet_addr();

    if(!(host = strrchr(name, '@'))) {
	host = "localhost";
	if(!(port = strrchr(name, '#'))) {
	    port = "rwrite";
	} else {
	    *port++ = '\000';
	}
    } else {
	*host++ = '\000';
	if(!(port = strrchr(host, '#'))) {
	    port = "rwrite";
	} else {
	    *port++ = '\000';
	}
    }
    portno = get_port_no(port);
    if(!(hp = gethostbyname(host))) {
	defaddr.s_addr = inet_addr(host);
	if(defaddr.s_addr == -1) {
	    fprintf(stderr, "rwrite: Unknown host: %s\n", host);
	    return -1;
	}
	def.h_name = host;
	def.h_addr_list = alist;
	def.h_addr = (char *)&defaddr;
	def.h_length = sizeof(struct in_addr);
	def.h_addrtype = AF_INET;
	def.h_aliases = 0;
	hp = &def;
    }
    if(!(sp = getservbyname(port, (udp_p ? "udp" : "tcp")))) {
	if(portno > 0) {
	    defport = portno;
	} else if((RWRITE_DEFAULT_PORT > 0) && (!(strcmp(port, "rwrite")))) {
	    if(verbose)
		fprintf(stderr, 
			"rwrite: Warning, port defaulted to %d.  Update /etc/services.\n", 
			RWRITE_DEFAULT_PORT);
	    defport = RWRITE_DEFAULT_PORT;
	} else {
	    fprintf(stderr, 
		    "rwrite: %s unknown service.  Update /etc/services.\n", 
		    port);
	    return -1;
	}
    } else {
	defport = 0;
    }
    sin.sin_family = hp->h_addrtype;
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = (sp ? (sp->s_port) : htons(defport));
    if((s = socket(hp->h_addrtype,
		   (udp_p ? SOCK_DGRAM : SOCK_STREAM),
		   0)) < 0) {
	perror("rwrite: socket");
	return -1;
    }
    /* have network connection; identify the host connected with */
    if(verbose)
	printf("[%s]\n", hp->h_name);
    if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	perror("rwrite: connect");
	close(s);
	return -1;
    }
    return s;
}

/********* END OF STUFF FROM Berkeley Unix's finger(1) **********/

int blow_target_addr(char *str, char **to, char **tty)
{
    char *hlp;

    if((!str))
	return 0;
    *to = hlp = str;
    while((*hlp) && (*hlp != ADDRESS_TTY_SEPARATOR))
	hlp++;
    if(*hlp) {
	*hlp = '\000';
	hlp++;
    }
    if(*hlp) {
	*tty = hlp;
	return 2;
    } else {
	*tty = NULL;
    }
    return 1;
}

int fix_tty_quote(char *str)
{
    if(str && *str) {
	int len = strlen(str);
	if((str[0] == '/') &&
	   (str[len - 1] == '/')) {
	    str[0] = '[';
	    str[len - 1] = ']';
	}
	return 1;
    }
    return 0;
}


int spit_autoreply(char *user, int not_delivered)
{
    time_t now;
    char *nowstr;
    int len;

    now = time(NULL);
    nowstr = ctime(&now);
    if((!nowstr) || (!(*nowstr))) {
	nowstr = "xxx";
    } else if('\n' == (nowstr[len = (strlen(nowstr) - 1)])) {
	nowstr[len] = '\000';
    }

    if(autoreply_lines) {
	if(not_delivered)
	    fprintf(stdout, 
		    "Your message could not be delivered, but remote host sent\n");
	fprintf(stdout,
		"%sutomatic reply from %s at %s:\n", 
		(not_delivered ? "an a" : "A"),
		user,
		nowstr);
	return(dequote_and_write(stdout,
				 autoreply, 
				 max_lines_in(), 
				 max_chars_in(),
				 0));
	return 1;
    }
    return 0;
}

/*
 * We try to flush input in the failure.
 */
void flush_stdin()
{
#ifndef DONT_FLUSH_INPUT_IN_FAILURE
#ifdef FIONREAD
    int a, r;
    char buf[256];

    while(1) {
	ioctl(0, FIONREAD, &a);
	if(!a)
	    break;
	while (a > 0) {
	    r = read(0, buf, sizeof(buf));
	    if(r < 0)
		return;
	    if(r == 0)
		break;
	    a -= r;
	}
    }
#endif
#endif
    return;
}

char *generate_udp_dialog(char *to, 
			  char *tty, 
			  char *from, 
			  char **msg, 
			  int *len)
{
    int dialog_len;
    char *dialog;
    int ttyp;
    int i;

    if((!msg) || (!(msg[0]))) {
	fprintf(stderr, "rwrite: Empty message.\n");
	return NULL;
    }
    if(is_str_whitespace(to)) {
	fprintf(stderr, "rwrite: Empty address.\n");
	return NULL;
    }
    if(is_str_whitespace(from)) {
	fprintf(stderr, "rwrite: Empty from address.\n");
	return NULL;
    }
    ttyp = !(is_str_whitespace(tty));

    dialog_len = 64 + strlen(to) + strlen(from);
    if(ttyp)
	dialog_len += strlen(tty) + 1;
    for(i = 0; msg[i]; i++)
	dialog_len += strlen(msg[i]) + 1;
    if(dialog_len > UDP_DIALOG_LEN_MAX) {
	fprintf(stderr, "rwrite: Too long UDP message..\n");
	return NULL;
    }
    if(!(dialog = (char *)malloc(dialog_len * sizeof(char)))) {
	fprintf(stderr, "rwrite: Out of memory.\n");
	return NULL;
    }
    strcpy(dialog, "FROM ");
    strcat(dialog, from);
    strcat(dialog, "\012TO ");
    strcat(dialog, to);
    if(ttyp) {
	strcat(dialog, " ");
	strcat(dialog, tty);
    }
    strcat(dialog, "\012DATA\012");
    for(i = 0; msg[i]; i++) {
	strcat(dialog, msg[i]);
	strcat(dialog, "\012");
    }
    strcat(dialog, ".\012SEND\012QUIT\012");
    if(len)
	*len = strlen(dialog);
    return(dialog);
}

#define USAGE()           \
     { fprintf(stderr,    \
       "Usage: rwrite [-r] [-b|B] [-t|u] [-v[v]] user[@host][#port][:tty] ...\n"); }

int main(int argc, char **argv)
{
    int ch, s, ret;
    char *to, *from, *tty;
    char **msg;
    extern char *optarg;
    extern int optind, optopt;
    int resend = 0, explicit_bg = 0, background = 0, udp = 0;
    char *userhome;

    if((argc == 2) && (!(strcmp("-version", argv[1])))) {
	fprintf(stderr, "Rwrite version %s.\n", RWRITE_VERSION_NUMBER);
	exit(0);
    }
    while ((ch = getopt(argc, argv, ":vrf:bBqut")) != -1) {
	switch(ch) {
	case 'v':	
	    verbose++;
	    break;
	case 'r':
	    resend = 1;
	    break;
	case 'f':
	    fwds = atoi(optarg);
	    if((fwds < 1) && (fwds != (-1))) {
		fprintf(stderr, "rwrite: -f needs an argument > 0.\n");
		USAGE();
		exit(1);
	    }
	    break;
	case 'b':
	    background = 1;
	    explicit_bg = 1;
	    break;
	case 'B':
	    background = 0;
	    explicit_bg = 1;
	    break;
	case 'q':
	    quiet = 1;
	    break;
	case 't':
	    udp = 0;
	    break;
	case 'u':
	    udp = 1;
	    break;
	case ':':
	    fprintf(stderr, 
		    "rwrite: Option -%c needs an option-argument.\n", optopt);
	    USAGE();
	    exit(1);
	case '?':
	    fprintf (stderr, "rwrite: Unrecognized option: -%c.\n", optopt);
	    USAGE();
	    exit(1);
	default:
	    fprintf(stderr, "rwrite: Internal error.\n");
	    exit(1);
	}
    }
    /*
     * Dig the sender information.
     */
    {
	struct passwd *pwd;
	char *tmp;

	if(!(tmp = getlogin())) {
	    pwd = getpwuid(getuid());
	    tmp = pwd ? (pwd->pw_name) : "UNKNOWN";
	} else {
	    pwd = getpwnam(tmp);
	}
	read_rc(RWRITE_GLOBAL_CONFIG);
	if(pwd && (pwd->pw_dir)) {
	    char rcfilename[MAXPATHLEN + 1];
	    
	    sprintf(rcfilename, "%s/%s", pwd->pw_dir, RWRITE_CONFIG_FILE);
	    read_rc(rcfilename);
	    userhome = pwd->pw_dir;
	} else {
	    userhome = "/nonexistent";
	}
	if(!(from = (char *)malloc(strlen(tmp) + 1))) {
	    exit(2);
	}
	strcpy(from, tmp);
    }	
    if(!explicit_bg)
	background = default_bg();
    if(((argc - optind) == 1) && (!resend) && (!background) && (!udp)) {
	if(!(to = (char *)malloc(strlen(argv[optind]) + 1))) {
	    exit(2);
	}
	strcpy(to, argv[optind]);
	blow_target_addr(to, &to, &tty);
	fix_tty_quote(tty);
	if(0 > (s = open_to(to, 0))) {
	    flush_stdin();
	    exit(3);
	}
	ret = rwp_dialog(s, to, tty, from, NULL, 1);
	close(s);
	spit_autoreply(argv[optind], (!ret));
	dump_msg_to_outlogs(last_msg, argv[optind], (!ret), userhome, 0);
	if(!ret)
	    flush_stdin();
	exit(ret ? 0 : (4));
    } else if((argc - optind) >= 1) {
	int i = 0;
	FILE *f = NULL;

	if(resend) {
	    if(!(f = open_history_read())) {
		fprintf(stderr, "rwrite: Can't open history file.\n");
		exit(7);
	    }
	    if(!(msg = read_user_message(f))) {
		fprintf(stderr, "rwrite: Empty message.\n");
		fclose(f);
		exit(4);
	    }
	    fclose(f);
	} else {
	    if(!(msg = read_user_message(stdin))) {
		fprintf(stderr, "rwrite: Empty message.\n");
		exit(4);
	    }
	    /* Writes history file. */
	    if(!(f = open_history_write())) {
		if(!quiet)
		    fprintf(stderr, 
			    "rwrite: Warning, can't open history file.\n");
	    } else {
		char *line;
		
		for((i = 0, line = msg[i]); line; line = msg[++i])
		    unquote_and_raw_write_str(f, line);
		if((!(close_history_write(f))))
		    if(!quiet)
			fprintf(stderr, 
				"rwrite: Warning, can't close history file.\n");
	    }
	}
	if(background) {
	    switch(fork()) {
	    case 0:
		break; /* Child continues */
	    case -1:
		fprintf(stderr, "rwrite: Unable to fork.\n");
		exit(5);
	    default:
		exit(0);
	    }
	}
	for(i = optind; i < argc; i++) {
	    if(!(to = (char *)malloc(strlen(argv[i]) + 1))) {
		exit(2);
	    }
	    strcpy(to, argv[i]);
	    blow_target_addr(to, &to, &tty);
	    fix_tty_quote(tty);
	    if(udp) {
		if(0 > (s = open_to(to, 1))) {
		    ret = 0;
		    if(!quiet)
			fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		} else {
		    int len;
		    char *dialog;

		    if(dialog = generate_udp_dialog(to, tty, from, msg, &len)) {
			/* Dump dialog to the socket. XXX */
			ret = 0;
			ret = (len == send(s, dialog, len, 0));
			free(dialog);
		    } else {
			ret = 0;
			if(!quiet)
			    fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		    }
		    close(s);
		    if(!ret) {
			if(!quiet)
			    fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		    }
		}
	    } else {
		if(0 > (s = open_to(to, 0))) {
		    ret = 0;
		    if(!quiet)
			fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		} else {
		    ret = rwp_dialog(s, to, tty, from, msg, 0);
		    close(s);
		    spit_autoreply(argv[i], (!ret));
		    if(!ret && !autoreply_lines) {
			if(!quiet)
			    fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		    }
		}
	    }
	    dump_msg_to_outlogs(last_msg, argv[i], (!ret), userhome, udp);
	    free(to);
	}
	/* Message array msg could be freed here but... XXX */
    } else {
	USAGE();
	exit(1);
    }
    /*NOTREACHED*/
    return 0;
}

/* EOF (rwrite.c) */
