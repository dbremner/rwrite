/*  -*- c -*-
 *
 * $RCSfile: rwrite.c,v $
 * ----------------------------------------------------------------------
 * Client to RWP-protocol
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:28:07 1994 tri
 * Last modified: Sun Nov 20 02:24:07 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.8 $
 * $State: Exp $
 * $Date: 1994/11/20 00:47:18 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrite.c,v $
 * Revision 1.8  1994/11/20 00:47:18  tri
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
#define __RWRITE_C__ 1
#ifndef lint
static char *RCS_id = "$Id: rwrite.c,v 1.8 1994/11/20 00:47:18 tri Exp $";
#endif /* not lint */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pwd.h>

#include "rwrite.h"

int verbose = 0;
int quiet = 0;
int resend = 0;
int fwds = 0;
int backround = 0;
char **autoreply = NULL;
int autoreply_lines = 0;
int autoreply_sz = 0;

/*
 * There is a possible race condition here if many messages are written
 * simultaneously.
 */
FILE *open_history_write()
{
    char path[MAXPATHLEN];
    char *home;
    FILE *f;

    if(!(home = getenv("HOME"))) {
	return NULL;
    }
    sprintf(path, "%s/.lastrwrite#", home);
    f = fopen(path, "w");
    if(f)
	chmod(path, 0600);
    return(f);
} 

int close_history_write(FILE *f)
{
    char path1[MAXPATHLEN];
    char path2[MAXPATHLEN];
    char *home;
    int r;

    if(!(home = getenv("HOME"))) {
	return NULL;
    }
    sprintf(path1, "%s/.lastrwrite#", home);
    sprintf(path2, "%s/.lastrwrite", home);
    r = rename(path1, path2);
    chmod(path2, 0600);
    return(r ? 0 : 1);
} 

FILE *open_history_read()
{
    char path[MAXPATHLEN];
    char *home;

    if(!(home = getenv("HOME"))) {
	return NULL;
    }
    sprintf(path, "%s/.lastrwrite", home);
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

/*
 * Read message data until EOF from console.
 */
char **read_user_message()
{
    int buflen, i;
    char **buf;
    char *line;

    if(!(buf = ((char **)malloc(BUF_ALLOC_STEP * sizeof(char *)))))
	return NULL;
    buflen = BUF_ALLOC_STEP;
    for(i = 0; /*NOTHING*/; i++) {
	if(!(line = read_line(stdin))) {
	    char *hlp;
	    
	    hlp = quote_str(line);
	    free(line);
	    line = hlp;
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

#define LEGAL_CODE(c) (((c)>=100)&&((c)<=999))
#define IGNORABLE_CODE(c) ((((c)>=500)&&((c)<=599))&&((c) != RWRITE_AUTOREPLY)&&((c) != RWRITE_AUTOREPLY_AS_COMMENT))

#define WRITE_STRING(f, str) {                                            \
       if(!(write_string(s, str))) {                                      \
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

    autoreply_lines = 0;
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
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
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
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		mode = DIALOG_TO;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
#ifndef DO_NOT_TELL_USERS
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
#endif
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    default:
		fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		return 0;
	    }
	case DIALOG_TO:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		mode = DIALOG_FROM;
		modeattr = 0;
		goto redo_dialog_loop;
	    /* here maybe some error handling XXX */
	    default:
		fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		return 0;
	    }
	case DIALOG_FROM:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>VRFY\n");
		WRITE_STRING(s, "VRFY\012");
		goto redo_dialog_loop;
	    case RWRITE_RCPT_OK_TO_SEND:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		mode = DIALOG_VRFY;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
	    default:
		fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
			fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
			fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
			return 0;
		    }
		    mode = DIALOG_FWDS;
		    modeattr = 0;
		    goto redo_dialog_loop;
		case RWRITE_ERR_FWD_LIMIT_EXCEEDED:
		    if(modeattr != 1) {
			fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
			return 0;
		    }
		    fprintf(stderr, "rwrite: Forward limit exceeded.\n");
		    return 0;
		default:
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
	    }
	case DIALOG_FWDS:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>DATA\n");
		WRITE_STRING(s, "DATA\012");
		goto redo_dialog_loop;
	    case RWRITE_GETMSG:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		if(resend) {
		    if(!(hist_file = open_history_read())) {
			fprintf(stderr, "rwrite: Can't open history file.\n");
			return 0;
		    }
		    while(line = read_line(hist_file)) {
			if(verbose > 1)
			    fprintf(stderr, ">>>>%s\n", line);
			WRITE_STRING(s, line);
			WRITE_STRING(s, "\012");
		    }
		} else {
		    if(msg) {
			int i;
			/*
			 * Message is in msg array.
			 */
			if(writehist) {
			    if(!(hist_file = open_history_write()))
				if(!quiet)
				    fprintf(stderr, "rwrite: Warning, can't open history file.\n");
			} else {
			    hist_file = NULL;
			}
			for((i = 0, line = msg[i]); line; line = msg[++i]) {
			    if(hist_file)
				fprintf(hist_file, "%s\n", line);
			    if(verbose > 1)
				fprintf(stderr, ">>>>%s\n", line);
			    WRITE_STRING(s, line);
			    WRITE_STRING(s, "\012");
			}
			if(hist_file) {
			    if(!(close_history_write(hist_file)))
				if(!quiet)
				    fprintf(stderr, "rwrite: Warning, can't close history file.\n");
			}
		    } else {
			/*
			 * Read message from user input.
			 */
			if(writehist) {
			    if(!(hist_file = open_history_write()))
				if(!quiet)
				    fprintf(stderr, "rwrite: Warning, can't open history file.\n");
			} else {
			    hist_file = NULL;
			}
			    
			while(line = read_line(stdin)) {
			    char *hlp;
			    
			    hlp = quote_str(line);
			    free(line);
			    line = hlp;
			    if(hist_file)
				fprintf(hist_file, "%s\n", line);
			    if(verbose > 1)
				fprintf(stderr, ">>>>%s\n", line);
			    WRITE_STRING(s, line);
			    WRITE_STRING(s, "\012");
			}
			if(hist_file) {
			    if(!(close_history_write(hist_file)))
				if(!quiet)
				    fprintf(stderr, "rwrite: Warning, can't close history file.\n");
			}
		    }
		}
		if(verbose > 1)
		    fprintf(stderr, ">>>>.\n");
		WRITE_STRING(s, ".\012");
		modeattr = 2;
		goto redo_dialog_loop;
	    case RWRITE_ERR_NO_MESSAGE:
		if(modeattr != 2) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Empty message.\n");
		return 0;
	    case RWRITE_MSG_OK:
		if(modeattr != 2) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		mode = DIALOG_DATA;
		modeattr = 0;
		goto redo_dialog_loop;
	    default:
		fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		return 0;
	    }
	case DIALOG_DATA:
	    switch(code) {
	    case RWRITE_READY:
		if(modeattr != 0) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		mode = DIALOG_SEND;
		modeattr = 0;
		goto redo_dialog_loop;
	    case RWRITE_AUTOREPLY:
	    case RWRITE_AUTOREPLY_AS_COMMENT:
		{
		    char *hlp;
		    if(modeattr != 1) {
			fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
			    if(!(newauto = (char **)calloc(BUF_ALLOC_STEP + autoreply_sz, sizeof(char *)))) {
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
			if(!(autoreply[autoreply_lines] = malloc(strlen(hlp) + 1))) {
			    fprintf(stderr, "rwrite: Out of memory.\n");
			    return 0;
			}
			strcpy(autoreply[autoreply_lines++], hlp);
		    }
		}
		goto redo_dialog_loop;
	    case RWRITE_ERR_PERMISSION_DENIED:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: Permission denied.\n");
		return 0;
	    case RWRITE_ERR_USER_NOT_IN:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: User not in.\n");
		return 0;
	    case RWRITE_ERR_NO_SUCH_USER:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 0;
		}
		fprintf(stderr, "rwrite: No such user.\n");
		return 0;
	    default:
		fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
		    return 1;
		}
		modeattr = 1;
		if(verbose > 1)
		    fprintf(stderr, ">>>>BYE\n");
		WRITE_STRING(s, "BYE\012");
		goto redo_dialog_loop;
	    case RWRITE_BYE:
		if(modeattr != 1) {
		    fprintf(stderr, "rwrite: Unexpected RWP response code (%03d).\n", code);
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

/**************** From Berkeley Unix's finger(1) *****************/
/********************* Modifications by tri *********************/
int open_to(char *name)
{
    int defport;
    struct in_addr defaddr;
    struct hostent *hp, def;
    struct servent *sp;
    struct sockaddr_in sin;
    int s;
    char *alist[1], *host, *rindex();
    u_long inet_addr();

    if(!(host = rindex(name, '@'))) {
	host = "localhost";
    } else {
	*host++ = '\000';
    }
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
    if(!(sp = getservbyname("rwrite", "tcp"))) {
	if(RWRITE_DEFAULT_PORT > 0) {
	    if(verbose)
		fprintf(stderr, "rwrite: Warning, tcp port defaulted to %d.  Update /etc/services.\n", RWRITE_DEFAULT_PORT);
	    defport = RWRITE_DEFAULT_PORT;
	} else {
	    fprintf(stderr, "rwrite: rwrite/tcp unknown service.  Update /etc/services.\n");
	    return 0;
	}
    } else {
	defport = 0;
    }
    sin.sin_family = hp->h_addrtype;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = (sp ? (sp->s_port) : htons(defport));
    if((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
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
	*hlp++;
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


int spit_autoreply(char *user)
{
    int j;
    char *hlp;
	
    if(autoreply_lines) {
	fprintf(stdout, "Automatic reply from %s\n\n", user);
	for(j = 0; j < autoreply_lines; j++) {
	    
	    hlp = dequote_str(autoreply[j]);
	    fprintf(stderr, "%s\n", hlp);
	    free(hlp);
	}
	fputc('\n', stdout);
	return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int ch, s, ret;
    char *to, *from, *tty;
    char **msg;
    extern char *optarg;
    extern int optind, optopt;
    
    while ((ch = getopt(argc, argv, ":vrf:bq")) != -1) {
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
		exit(1);
	    }
	    break;
	case 'b':
	    backround = 1;
	    break;
	case 'q':
	    quiet = 1;
	    break;
	case ':':
	    fprintf(stderr, 
		    "rwrite: Option -%c needs an option-argument\n.", optopt);
	    exit(1);
	case '?':
	    fprintf (stderr, "rwrite: Unrecognized option: -%c\n.", optopt);
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
	    char rcfilename[MAXPATHLEN];
	    
	    sprintf(rcfilename, "%s/%s", pwd->pw_dir, RWRITE_CONFIG_FILE);
	    read_rc(rcfilename);
	}
	if(!(from = (char *)malloc(strlen(tmp) + 1))) {
	    exit(2);
	}
	strcpy(from, tmp);
    }	
    if(((argc - optind) == 1) && (!resend) && (!backround)) {
	if(!(to = (char *)malloc(strlen(argv[optind]) + 1))) {
	    exit(2);
	}
	strcpy(to, argv[optind]);
	blow_target_addr(to, &to, &tty);
	fix_tty_quote(tty);
	if(0 > (s = open_to(to))) {
	    exit(3);
	}
	ret = rwp_dialog(s, to, tty, from, NULL, 1);
	close(s);
	spit_autoreply(argv[optind]);
	exit(ret ? 0 : (4));
    } else if((argc - optind) >= 1) {
	int i;
	if(resend) {
	    msg = NULL;
	} else {
	    FILE *f;
		
	    if(!(msg = read_user_message())) {
		fprintf(stderr, "rwrite: Empty message.\n");
		exit(4);
	    }
	    /* Writes history file. */
	    if(!(f = open_history_write())) {
		if(!quiet)
		    fprintf(stderr, "rwrite: Warning, can't open history file.\n");
	    } else {
		int i;
		char *line;
		
		for((i = 0, line = msg[i]); line; line = msg[++i])
		    fprintf(f, "%s\n", line);
		if((!(close_history_write(f))))
		    if(!quiet)
			fprintf(stderr, "rwrite: Warning, can't close history file.\n");
	    }
	}
	if(backround) {
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
	    if(0 > (s = open_to(to))) {
		if(!quiet)
		    fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
	    } else {
		ret = rwp_dialog(s, to, tty, from, msg, 0);
		close(s);
		if(!ret) {
		    if(!quiet)
			fprintf(stderr, "rwrite: Skipped %s.\n", argv[i]);
		}
		spit_autoreply(argv[i]);
	    }
	    free(to);
	}
	/* Message array msg could be freed here but... XXX */
    } else {
	fprintf(stderr, "USAGE: rwrite [-f #] [-r] [-v] [-b] user[@host] ...\n");
	exit(1);
    }
    /*NOTREACHED*/
    return 0;
}

/* EOF (rwrite.c) */
