/*  -*- c -*-
 *
 * $RCSfile: rwrite.h,v $
 * ----------------------------------------------------------------------
 * Header file of the rwrite and rwrited programs that implement
 * the RWP protocol.
 * ----------------------------------------------------------------------
 * Created      : Tue Sep 13 15:27:58 1994 tri
 * Last modified: Tue Dec 13 18:24:52 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.32 $
 * $State: Exp $
 * $Date: 1994/12/13 16:25:06 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: rwrite.h,v $
 * Revision 1.32  1994/12/13 16:25:06  tri
 * Nothing.
 *
 * Revision 1.31  1994/12/12  22:11:48  tri
 * Version numbers.
 *
 * Revision 1.30  1994/12/12  21:17:55  tri
 * Closed files more pedantically.
 * Fix by toka & tri.
 *
 * Revision 1.29  1994/12/12  19:50:16  tri
 * Fixed a small but potentially harmful fclose(NULL) -bug.
 *
 * Revision 1.28  1994/12/12  15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.27  1994/12/12  14:53:51  tri
 * Version number.
 *
 * Revision 1.26  1994/12/12  11:08:14  tri
 * Moved the name of the file containing last
 * message into rwrite.h
 *
 * Revision 1.25  1994/12/11  21:25:30  tri
 * Cleaned up some warnings.  No functional changes.
 *
 * Revision 1.24  1994/12/11  18:45:50  tri
 * Minor fix.
 *
 * Revision 1.23  1994/12/11  18:16:28  tri
 * Some portability fixes and configuration stuff
 * moved to Makefile.
 *
 * Revision 1.22  1994/12/11  14:56:13  tri
 * Minor fix.
 *
 * Revision 1.21  1994/12/11  13:29:29  tri
 * Background message sending can be defaulted in
 * rwriterc.  Explicit -b or -B flag overrides the
 * default.
 *
 * Revision 1.20  1994/12/11  12:58:17  tri
 * Fixed the allow-deny -heuristics to be
 * more powerful.
 * Also added the cleardefs command to the rc-file syntax.
 *
 * Revision 1.19  1994/12/10  11:28:38  tri
 * Last known method to send terminal control codes
 * through correctly configured rwrite is now diabled.
 *
 * Revision 1.18  1994/12/09  23:57:49  tri
 * Added a outbond message logging.
 *
 * Revision 1.17  1994/12/09  21:08:12  tri
 * Added flush_stdin().
 *
 * Revision 1.16  1994/12/09  10:28:56  tri
 * Fixed a return value of dequote_and_send().
 *
 * Revision 1.15  1994/12/08  23:38:11  tri
 * Version strings update.
 *
 * Revision 1.14  1994/12/08  22:56:45  tri
 * Fixed the quotation system on message
 * delivery.  Same message can now be quoted
 * differently for the each receiver.
 * Also the autoreplies are now quoted right.
 *
 * Revision 1.13  1994/12/07  12:34:32  tri
 * Removed read_message() and dropped in Camillo's GetMsg()
 * instead.
 *
 * Revision 1.12  1994/11/22  20:49:13  tri
 * Added configurable parameter to limit the number
 * of lines in the incoming message.
 *
 * Revision 1.11  1994/11/20  11:08:12  tri
 * Fixed minor quotation bug in backround mode.
 *
 * Revision 1.10  1994/11/20  00:47:18  tri
 * Completed autoreply and quotation stuff.
 * We are almost there now.
 *
 * Revision 1.9  1994/10/06  18:32:37  tri
 * Hacked multitty option.
 *
 * Revision 1.8  1994/10/04  20:50:22  tri
 * Conforms now the current RWP protocol.
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
 * Revision 1.3  1994/09/14  16:04:50  tri
 * Nothing really.
 *
 * Revision 1.2  1994/09/14  14:59:48  tri
 * Added a few codes.
 *
 * Revision 1.1  1994/09/13  12:32:13  tri
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
/* Headers are included only once. */
#ifndef __RWRITE_H__
#define __RWRITE_H__ 1

#define RWP_VERSION_NUMBER	"1.0"		/* Protocol version */
#define RWRITED_VERSION_NUMBER	"1.1b21"	/* Server version   */
#define RWRITE_VERSION_NUMBER	"1.1b21"	/* Client version   */
/*
 * User definitions are in the following files.
 */
#define RWRITE_CONFIG_FILE	".rwriterc"
#define RWRITE_GLOBAL_CONFIG	"/etc/rwrite.conf"
#define RWRITE_AUTOREPLY_FILE	".rwrite-autoreply"
#define RWRITE_LAST_SENT_MSG    ".rwrite-last"

#define PATH_SEPARATOR          ((int)'!')  /* Separator char in delivery path */
#define ADDRESS_TTY_SEPARATOR	((int)':')

/*
 * Allocation step in line buffer allocation. 
 * Has to be at least 4.  No need to modify this anyway.
 */
#define BUF_ALLOC_STEP	128

/*
 * Daemon sends only first MAX_AUTOREPLY_LINES lines of autoreply.
 * -1 means unlimited.
 */
#define MAX_AUTOREPLY_LINES 64

/*
 * Maximum number of lines in the incoming message.  
 * This can be overridden in user configuration file.
 */
#define DEFAULT_MAX_LINES_IN	1024
#define DEFAULT_MAX_CHARS_IN	(DEFAULT_MAX_LINES_IN * 64)

/*
 * Maximum number of lines that DATA command can get.
 */
#define DATA_MAXLINES	DEFAULT_MAX_LINES_IN
#define DATA_MAXCHARS	(DATA_MAXLINES * 64)

/*************************************************/
/*************************************************/
/********* END OF THE USER CONFIGURATION *********/
/*************************************************/
/*************************************************/

/*
 * Prototypes of the resource functions
 */
int rc_read_p(void);
int ring_bell(void);
int max_lines_in(void);
int max_chars_in(void);
int default_bg(void);
int add_to_list(char ***list, int *list_sz, char *str);
int add_list_to_list(char ***tgt, int *tgt_sz, char **list);
int is_in_list(char **list, char *str);
void reset_rc(void);
void read_rc(char *fn);
int is_allowed(char *name, char *host);
int deliver_all_ttys(void);
int no_tty_delivery(void);
char *quote_str(char *s);
char *dequote_str(char *s, int maxlen, int *len);
int dequote_and_write(FILE *f, char **msg, int maxlines, int maxchars, 
		      int is_f_tty);
#ifndef __RWRITERC_C__
extern char **rc_tty_list;
extern char **rc_outlog;
#endif
/*
 * #
 * # Entry to enable rwrite service in /etc/services.
 * # The port number is about to change in near future.
 * #
 * rwrite		2801/tcp			# rwrite
 */
#define RWRITE_DEFAULT_PORT	2801

#define RWRITE_FWD_LIMIT	32
/*
 * These response codes follow the RWP version 1.0 RFC.
 */
/*
 * Success codes.
 */
#define RWRITE_READY		100
#define RWRITE_BYE		101
#define RWRITE_DELIVERY_OK	103
#define RWRITE_DELIVERY_FORWARDED	104
#define RWRITE_SENDER_OK	105
#define RWRITE_RCPT_OK		106
#define RWRITE_MSG_OK		107
#define RWRITE_RCPT_OK_TO_SEND	108
#define RWRITE_RSET_OK		109
#define RWRITE_RCPT_OK_TO_FWD	110
#define RWRITE_FHST_OK		111
#define RWRITE_QUOTE_OK		112
/*
 * Server readu to receive message body.
 */
#define RWRITE_GETMSG		200
/*
 * Autoreply
 */
#define RWRITE_AUTOREPLY	555 /* To be 300 */
/* Older RWP clients barf with 300 but ignore 555 so let it be 555 for now. */
#define RWRITE_AUTOREPLY_AS_COMMENT	556
/*
 * Informational responses.
 */
#define RWRITE_HELO		500
#define RWRITE_VER		501
#define RWRITE_PROT		502
#define RWRITE_HELP		510
#define RWRITE_INFO		511 /* Stuff for client to ignore. */
#define RWRITE_DEBUG		512 /* Stuff for client to ignore. */
/*
 * Error codes.
 */
#define RWRITE_ERR_FATAL	666
#define RWRITE_ERR_SYNTAX	668
#define RWRITE_ERR_PERMISSION_DENIED	669
#define RWRITE_ERR_USER_NOT_IN	670
#define RWRITE_ERR_NO_SUCH_USER	671
#define RWRITE_ERR_NO_MESSAGE	672
#define RWRITE_ERR_NO_SENDER	673
#define RWRITE_ERR_NO_ADDRESS	674
#define RWRITE_ERR_NO_DATA	675
#define RWRITE_ERR_FWD_LIMIT_EXCEEDED	676
#define RWRITE_ERR_FWD_FAILED	677
#define RWRITE_ERR_QUOTE_CMD_FAILED	678
#define RWRITE_ERR_QUOTE_CMD_UNKNOWN	679
#define RWRITE_ERR_INTERNAL	698
#define RWRITE_ERR_UNKNOWN	699

#endif /* not __RWRITE_H__ */
/* EOF (rwrite.h) */
