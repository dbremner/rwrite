/*  -*- c -*-
 *
 * $RCSfile: mkrwritebox.c,v $
 * ----------------------------------------------------------------------
 * Make rwrite inboxes writable by tty group .
 * ----------------------------------------------------------------------
 * Created      : Sat Dec 10 17:27:21 1994 toka
 * Last modified: Sat Dec 10 17:33:46 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.1 $
 * $State: Exp $
 * $Date: 1994/12/10 15:38:22 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: mkrwritebox.c,v $
 * Revision 1.1  1994/12/10 15:38:22  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Tomi Kause <toka@cirion.fi> and Cirion oy.
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
#define __MKRWRITEBOX_C__ 1
#ifndef lint
static char *RCS_id = "$Id: mkrwritebox.c,v 1.1 1994/12/10 15:38:22 tri Exp $";
#endif /* not lint */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    char path[PATH_MAX + 1];
    char file[PATH_MAX + 1];
    char *t1 = NULL, *t2 = NULL;
    int f = -1;
    uid_t uid = -1;
    gid_t ttygid = -1;

    if(2 != argc) {
	fprintf(stderr, "Usage: mkrwritebox filename\n");
	return(1);
    }

#define BADFNAMEXIT()                                       \
    fputs("mkrwritebox: Bad filename. Exiting.\n", stderr); \
    return(7);

    if(!(*(argv[1])) || !(strcmp("..", argv[1])) || !(strcmp(".", argv[1]))) {
	BADFNAMEXIT();
    }

    for(t1 = argv[1], t2 = file; t1 && *t1 ; t1++) {
	if(*t1 == '/') {
	    BADFNAMEXIT();
	}
	*(t2++) = *t1;
    }

    pwd = getpwuid(uid = getuid());
    grp = getgrnam(TTY_GROUP_NAME);
    if(!pwd || !grp) {
	fputs("mkrwritebox: Cannot get user or group.\n", stderr);
	return(2);
    }
    ttygid = grp->gr_gid;
    strcpy(path, pwd->pw_dir);
    strcat(path, "/");
    strcat(path, file);
    puts(path);
    (void)umask(0);
    if(0 > (f = open(path, O_CREAT | O_EXCL, 0620))) {
	perror("mkrwritebox");
	return(3);
    }
    if(0 > (chown(path, uid, ttygid))) {
	perror("mkrwritebox");
	(void)unlink(path);
	return(4);
    }
    (void)close(f);
    return(0);
}

/* EOF (mkrwritebox.c) */
