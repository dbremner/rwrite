/*  -*- c -*-
 *
 * $RCSfile: rwriterc.c,v $
 * ----------------------------------------------------------------------
 * Resource file routines for rwrite.
 * ----------------------------------------------------------------------
 * Created      : Fri Oct 07 00:27:30 1994 tri
 * Last modified: Sun Dec 11 14:53:36 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.9 $
 * $State: Exp $
 * $Date: 1994/12/11 12:58:17 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwriterc.c,v $
 * Revision 1.9  1994/12/11 12:58:17  tri
 * Fixed the allow-deny -heuristics to be
 * more powerful.
 * Also added the cleardefs command to the rc-file syntax.
 *
 * Revision 1.8  1994/12/09  23:57:49  tri
 * Added a outbond message logging.
 *
 * Revision 1.7  1994/12/09  10:28:56  tri
 * Fixed a return value of dequote_and_send().
 *
 * Revision 1.6  1994/12/08  23:38:11  tri
 * Fixed a minor feature.
 *
 * Revision 1.5  1994/12/08  23:12:04  tri
 * Fixed a quite serious quotation bug that appeared
 * when only the global configuration was present.
 *
 * Revision 1.4  1994/12/08  22:56:45  tri
 * Fixed the quotation system on message
 * delivery.  Same message can now be quoted
 * differently for the each receiver.
 * Also the autoreplies are now quoted right.
 *
 * Revision 1.3  1994/11/22  20:49:13  tri
 * Added configurable parameter to limit the number
 * of lines in the incoming message.
 *
 * Revision 1.2  1994/11/20  13:17:39  tri
 * Included ctype.h.
 *
 * Revision 1.1  1994/11/20  00:47:18  tri
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
#define __RWRITERC_C__ 1
#ifndef lint
static char *RCS_id = "$Id: rwriterc.c,v 1.9 1994/12/11 12:58:17 tri Exp $";
#endif /* not lint */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rwrite.h"
#include "match.h"

char **deny_user = NULL;
char **allow_user = NULL;
char **rc_tty_list = NULL;
char **rc_outlog = NULL;

int rc_read = 0;
int all_ttys = 0;
int no_tty = 0;
int show_quoted = 0;

int deny_user_sz    = 0;
int allow_user_sz   = 0;
int rc_tty_list_sz  = 0;
int rc_outlog_sz    = 0;
int rc_incoming_max    = DEFAULT_MAX_LINES_IN;
int rc_ch_incoming_max = DEFAULT_MAX_CHARS_IN;
int no_bell = 0;

#define KILL_C_TABLE(c) { char **_x = c; if(c) { while(*_x) {   \
                                                     free(*_x); \
						     *_x++ = NULL; }}}

static char hex_char[] = { '0', '1', '2', '3', '4', '5', '6', '7', 
			   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static char quote_list[256] = { '\000' };

int ring_bell()
{
    return(rc_read ? (!(no_bell)) : 1);
}

int max_lines_in()
{
    return(rc_read ? rc_incoming_max : DEFAULT_MAX_LINES_IN);
}

int max_chars_in()
{
    return(rc_read ? rc_ch_incoming_max : DEFAULT_MAX_CHARS_IN);
}

int deliver_all_ttys()
{
    return((rc_read && all_ttys) ? 1 : 0);
}

int no_tty_delivery()
{
    return((rc_read && no_tty) ? 1 : 0);
}

int rc_read_p()
{
    return(rc_read != 0);
}

int add_to_list(char ***list, int *list_sz, char *str)
{
    int i;

    if((!list) || (!list_sz) || (!str))
	return 0;
    
    if(!(*list_sz)) {
	if(!(*list = (char **)calloc(BUF_ALLOC_STEP, sizeof(char *))))
	    return 0;
	*list_sz = BUF_ALLOC_STEP;
    }
    for(i = 0; (*list)[i]; i++) {
	if(!(strcmp((*list)[i], str)))
	    return 1;
    }
    if((i + 2) > *list_sz) {
	char **newlist;
	if(!(newlist = (char **)calloc((BUF_ALLOC_STEP + *list_sz),
				       sizeof(char *))))
	    return 0;
	memcpy(newlist, *list, (*list_sz) * sizeof(char *));
	free(*list);
	(*list_sz) += BUF_ALLOC_STEP;
	*list = newlist;
    }
    (*list)[i] = str;
    return 1;
}

int add_list_to_list(char ***tgt, int *tgt_sz, char **list)
{
    if((!tgt) || (!tgt_sz))
	return 0;
    if(!list)
	return 1;
    for(/*NOTHING*/; *list; list++) {
	char *hlp;
	if(!(hlp = (char *)malloc(strlen(*list) + 1)))
	    return 0;
	strcpy(hlp, *list);
	if(!(add_to_list(tgt, tgt_sz, hlp)))
	    return 0;
    }
    return 1;
}

int is_in_list(char **list, char *str)
{
    if(list && str)
	for(/*NOTHING*/; *list; list++)
	    if(!(StrMatch(str, *list)))
		return 1;
    return 0;
}

void reset_rc()
{
    KILL_C_TABLE(deny_user);
    KILL_C_TABLE(allow_user);
    KILL_C_TABLE(rc_tty_list);
    KILL_C_TABLE(rc_outlog);

    no_bell = 0;
    rc_read = 0;
    all_ttys = 0;
    show_quoted = 0;
    rc_incoming_max = DEFAULT_MAX_LINES_IN;
    rc_ch_incoming_max = DEFAULT_MAX_CHARS_IN;
    memset(quote_list, 0, sizeof(quote_list));
    return;
}

static void split_line(char *line, char **s1, char **s2)
{
    char *hlp1, *hlp2;
    int out;

    *s1 = *s2 = NULL;
    out = 0;
    if(line) {
	hlp1 = line;
	/*
	 * Nuke the space from the end.
	 */
	for(hlp2 = hlp1; *hlp2; hlp2++)
	    /*NOTHING*/;
	while((hlp2 != hlp1) && (isspace(*hlp2)))
	    *hlp2-- = '\000';
	/*
	 * Get the first part.
	 */
	while((*hlp1) && (!(isspace(*hlp1))))
	    hlp1++;
	if(!(*hlp1))
	    out = 1;
	*hlp1 = '\000';
	if(*s1 = (char *)malloc(strlen(line) + 1))
	    strcpy(*s1, line);
	if(out)
	    return;
	/*
	 * And the second one.
	 */
	hlp1++;
	while((*hlp1) && (isspace(*hlp1)))
	    hlp1++;
	if((*hlp1) && (*s2 = (char *)malloc(strlen(hlp1) + 1)))
	    strcpy(*s2, hlp1);
    }
    return;
}

void read_rc(char *fn)
{
    FILE *f;
    char buf[0xff];
    int len;

    rc_read = 1;
    if(f = fopen(fn, "r")) {
#ifdef DEBUG
	fprintf(stdout, "%03d Opened \"%s\".\n", RWRITE_DEBUG, fn);
#endif
	while(fgets(buf, sizeof(buf), f)) {
	    buf[sizeof(buf) - 1] = '\000';
	    len = strlen(buf);
	    if(len && buf[len - 1] == '\n')
		buf[--len] = '\000';
	    if(len) {
		char *tag, *value;
		split_line(buf, &tag, &value);
		if(tag) {
		    if((!(strcmp(tag, "allow"))) && value) {
			add_to_list(&allow_user,
				    &allow_user_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "deny"))) && value) {
			add_to_list(&deny_user,
				    &deny_user_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "tty"))) && value) {
			add_to_list(&rc_tty_list,
				    &rc_tty_list_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "agent"))) && value) {
			add_to_list(&rc_tty_list,
				    &rc_tty_list_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "logfile"))) && value) {
			add_to_list(&rc_tty_list,
				    &rc_tty_list_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "outlog"))) && value) {
			add_to_list(&rc_outlog,
				    &rc_outlog_sz,
				    value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
		    } else if((!(strcmp(tag, "maxlinesin"))) && value) {
			int x;
			
			if((x = atoi(value)) > 0)
			    rc_incoming_max = x;
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %d\n", 
				RWRITE_DEBUG, tag, x);
#endif
		    } else if((!(strcmp(tag, "maxcharsin"))) && value) {
			int x;
			
			if((x = atoi(value)) > 0)
			    rc_ch_incoming_max = x;
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %d\n", 
				RWRITE_DEBUG, tag, x);
#endif
		    } else if((!(strcmp(tag, "quote"))) && value) {
			int b, e;
			char *hlp;
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
			b = atoi(value);
			hlp = value;
			while((*hlp) && (*hlp != '-'))
			    hlp++;
			if(*hlp == '-') {
			    hlp++;
			    e = atoi(hlp);
			} else {
			    e = b;
			}
			if((b >= 0) && (b <= 255) && (e >= 0) && (e <= 255) &&
			   (e >= b)) {
			    while(b <= e) {
				quote_list[b] = 1;
				b++;
			    }
			}
		    } else if((!(strcmp(tag, "dontquote"))) && value) {
			int b, e;
			char *hlp;
#ifdef DEBUG
			fprintf(stdout, "%03d ok > %s %s\n", 
				RWRITE_DEBUG, tag, value);
#endif
			b = atoi(value);
			hlp = value;
			while((*hlp) && (*hlp != '-'))
			    hlp++;
			if(*hlp == '-') {
			    hlp++;
			    e = atoi(hlp);
			} else {
			    e = b;
			}
			if((b >= 0) && (b <= 255) && (e >= 0) && (e <= 255) &&
			   (e >= b)) {
			    while(b <= e) {
				quote_list[b] = 0;
				b++;
			    }
			}
		    } else if(!(strcmp(tag, "multitty"))) {
			all_ttys = 1;
			no_tty = 0;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok multitty\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "singletty"))) {
			all_ttys = 0;
			no_tty = 0;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok singletty\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "notty"))) {
			all_ttys = 0;
			no_tty = 1;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok notty\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "bell"))) {
			no_bell = 0;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok bell\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "nobell"))) {
			no_bell = 1;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok nobell\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "showquoted"))) {
			show_quoted = 1;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok showquoted\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "hidequoted"))) {
			show_quoted = 0;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok hidequoted\n", RWRITE_DEBUG);
#endif
		    } else if(!(strcmp(tag, "cleardefs"))) {
			reset_rc();
			rc_read = 1;
			if(value)
			    free(value);
#ifdef DEBUG
			fprintf(stdout, "%03d ok cleardefs\n", RWRITE_DEBUG);
#endif
		    } else {
#ifdef DEBUG
			fprintf(stdout, 
				"%03d ??? \"%s\"\n",
				RWRITE_DEBUG, 
				tag);
#endif
			if(value)
			    free(value);
		    }
		    /*
		     * We can always free tag.
		     */
		    if(tag)
			free(tag);
		}
 	    }
	}
	fclose(f);
    } else {
#ifdef DEBUG
	fprintf(stdout, "%03d Failed to open \"%s\".\n", RWRITE_DEBUG, fn);
#endif
    }
    return;
}

int is_allowed(char *name, char *host)
{
    char buf[256];

    if(rc_read &&
       name && 
       host && 
       ((strlen(name) + strlen(host) + 2) < sizeof(buf))) {
	sprintf(buf, "%s@%s", name, host);
	/* If one explicitely is allowed it's ok. */
	if(is_in_list(allow_user, buf))
	    return 1;
	/* If one explicitely is denied it's too bad. */
	if(is_in_list(deny_user, buf))
	    return 0;
	/* If someone is allowed and no-one denied then default is denied. */
	if(allow_user && !deny_user)
	    return 0;
	/* Otherwise it's ok. */
	return 1;
    }
    /* No configuration.  No permission. */
    return 0;
}

#ifdef DEBUG
void print_configuration()
{
    if(rc_read) {
	if(deny_user) {
	    char **hlp;
	    fprintf(stdout, "%03d permission denied:\n", RWRITE_DEBUG);
	    for(hlp = deny_user; *hlp; hlp++) {
		fprintf(stdout, "%03d \"%s\":\n", RWRITE_DEBUG, *hlp);
	    }
	} else {
	    fprintf(stdout, "%03d permission denied = NULL\n", RWRITE_DEBUG);
	}
	if(allow_user) {
	    char **hlp;
	    fprintf(stdout, "%03d permission allowed:\n", RWRITE_DEBUG);
	    for(hlp = allow_user; *hlp; hlp++) {
		fprintf(stdout, "%03d \"%s\":\n", RWRITE_DEBUG, *hlp);
	    }
	} else {
	    fprintf(stdout, "%03d permission allowed = NULL\n", RWRITE_DEBUG);
	}
	if(rc_tty_list) {
	    char **hlp;
	    fprintf(stdout, "%03d tty_list:\n", RWRITE_DEBUG);
	    for(hlp = rc_tty_list; *hlp; hlp++) {
		fprintf(stdout, "%03d \"%s\":\n", RWRITE_DEBUG, *hlp);
	    }
	} else {
	    fprintf(stdout, "%03d tty list = NULL\n", RWRITE_DEBUG);
	}
	fprintf(stdout, "%03d deliver to all ttys = %s\n", 
		RWRITE_DEBUG, all_ttys ? "TRUE" : "FALSE");
	fprintf(stdout, "%03d don't deliver tty = %s\n", 
		RWRITE_DEBUG, no_tty ? "TRUE" : "FALSE");
    } else {
	fprintf(stdout, "%03d No configuration present.\n", RWRITE_DEBUG);
    }
}
#else
void print_configuration()
{
    return;
}
#endif

char *quote_str(char *str)
{
    unsigned char *r, *hlp, *s;
    int quoted;

    s = (unsigned char *)str;
    if(!s)
	s = "";
    for((quoted = 0, hlp = s); *hlp; hlp++)
	if((*hlp < 32) || (*hlp > 126) || (*hlp == '=') || (*hlp == '.'))
	    quoted++;
    if(!(r = malloc(strlen(s) + (2 * quoted) + 1)))
	return NULL;
    for(hlp = r; *s; s++) {
	if((*s >= 32) && (*s < 127) && (*s != '=') && (*s != '.')) {
	    *hlp = *s;
	    hlp++;
	} else {
	    *hlp = '=';
	    hlp++;
	    *hlp = hex_char[(((int)(*s)) >> 4) & 15];
	    hlp++;
	    *hlp = hex_char[((int)(*s)) & 15];
	    hlp++;
	}
    }
    *hlp = '\000';
    return((char *)r);
}

#define QUOTE_ME(c) ((((c) >= 0) && ((c) < 256) && (!(quote_list[c]))) ? 0 : 1)

char *dequote_str(char *str, int maxlen, int *len)
{
    unsigned char *r, *hlp, *s;
    int c, dummy, l;

    if(!len)
	len = &dummy;
    *len = 0;
    s = (unsigned char *)str;
    if(!s)
	s = "";
    if(maxlen < 1)
	return NULL;
    l = strlen(s) * 3;
    l = ((l > maxlen) ? maxlen : l) + 1;
    if(!(r = malloc((strlen(s) * 3) + 1)))
	return NULL;
    for((*len = 0, hlp = r); ((*s) && (*len < maxlen)); s++) {
	if(*s == '=') {
	    s++;
	    if(((*s >= '0') && (*s <= '9')) ||
	       ((*s >= 'a') && (*s <= 'f')) ||
	       ((*s >= 'A') && (*s <= 'F'))) {
		c = ((*s >= '0') && (*s <= '9')) ? (*s - '0') :
		    (((*s >= 'a') && (*s <= 'f')) ? (*s - 'a' + 10) :
		     (((*s >= 'A') && (*s <= 'F')) ? (*s - 'A' + 10) : 0));
		s++;
	    }
	    if(((*s >= '0') && (*s <= '9')) ||
	       ((*s >= 'a') && (*s <= 'f')) ||
	       ((*s >= 'A') && (*s <= 'F'))) {
		c = (c << 4) | 
		    (((*s >= '0') && (*s <= '9')) ? (*s - '0') :
		     (((*s >= 'a') && (*s <= 'f')) ? (*s - 'a' + 10) :
		      (((*s >= 'A') && (*s <= 'F')) ? (*s - 'A' + 10) : 0)));
	    }
	} else {
	    c = (int)(*s);
	}
	if(QUOTE_ME(c)) {
	    if(show_quoted) {
		*hlp = '=';
		hlp++;
		*hlp = hex_char[(((int)(c)) >> 4) & 15];
		hlp++;
		*hlp = hex_char[((int)(c)) & 15];
		hlp++;
		(*len) += 3;
	    }
	} else {
	    *hlp = (unsigned char)c;
	    hlp++;
	    (*len)++;
	}
    }
    *hlp = '\000';
    return((char *)r);
}

int dequote_and_write(FILE *f, 
		      char **msg, 
		      int maxlines, 
		      int maxchars, 
		      int is_f_tty)
{
    int i, linelen, x;
    char *out;
    
#ifdef DEBUG
    fprintf(stdout, "%03d d_&_w(f, msg, %d, %d)\n", RWRITE_DEBUG, maxlines, 
	    maxchars);
#endif

    if((!f) || (!msg) || (!(*msg)) || (maxlines < 1) || (maxchars < 1))
	return 0;
    fputc('\n', f);
    if(is_f_tty) {
	/* It may be in the raw mode and... */
	fputc('\r', f);
    }
    for(i = 0;  ((msg[i]) && (maxlines > 0) && (maxchars > 0)); i++) {
	if(!(out = dequote_str(msg[i], maxchars, &linelen)))
	    return 0;
	if((fwrite(out, sizeof(char), linelen, f)) != linelen) {
	    free(out);
	    return 0;
	}
	fputc('\n', f);
	if(is_f_tty) {
	    /* It may be in the raw mode and... */
	    fputc('\r', f);
	}
	free(out);
	maxchars -= (linelen + 1);
	maxlines--;
    }
    return 1;
}

/* EOF (rwriterc.c) */
