/*
 * ----------------------------------------------------------------------
 * Created      : Sat Dec 10 17:27:21 1994 toka
 * Last modified: Tue Dec 13 21:55:05 1994 tri
 * ----------------------------------------------------------------------
 * $Revision: 1.7 $
 * $State: Exp $
 * $Date: 1994/12/13 20:28:57 $
 * $Author: tri $
 * ----------------------------------------------------------------------
 * $Log: mkrwritebox.c,v $
 * Revision 1.7  1994/12/13 20:28:57  tri
 * Preparation for autoconfig and tcp-port change.
 *
 * Revision 1.6  1994/12/12  15:58:41  tri
 * Copyright fixed a bit.
 *
 * Revision 1.5  1994/12/12  11:03:42  tri
 * Added compatibility fixes from toka.
 *
 * Revision 1.4  1994/12/11  18:48:49  cirion
 * Fixed potential security bug. Minor cleanup.
 *
 * Revision 1.3  1994/12/11  18:40:28  tri
 * Now makerules work like the lavatory in the train
 * and portability is as good as I can make it.
 *
 * Revision 1.2  1994/12/11  14:56:13  tri
 * Minor fix.
 *
 * Revision 1.1  1994/12/10  15:38:22  tri
 * Initial revision
 *
 * ----------------------------------------------------------------------
 * Copyright 1994, Tomi Kause <toka@cirion.fi> and Cirion oy.
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
#define __MKRWRITEBOX_C__ 1
#ifndef lint
static char *RCS_id = "$Id: mkrwritebox.c,v 1.7 1994/12/13 20:28:57 tri Exp $";
#endif /* not lint */

#include <stdio.h>
#include <fcntl.h>

#ifndef NO_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/param.h>

#ifndef MAXPATHLEN
#  define MAXPATHLEN PATH_MAX
#endif

int main(int argc, char **argv) {
    struct passwd *pwd = NULL;
    struct group *grp = NULL;
    char path[MAXPATHLEN + 1] = {NULL};
    char *file = NULL;
    char *t = NULL;
    int f = -1;
    int uid = -1;
    int ttygid = -1;

    if(2 != argc) {
	fprintf(stderr, "Usage: mkrwritebox filename\n");
	return(1);
    }
    file = argv[1];

#define BADFNAMEXIT()                                       \
    fputs("mkrwritebox: Bad filename. Exiting.\n", stderr); \
    return(7);

    if(!(*file) || !(strcmp("..", file)) || !(strcmp(".", file))) {
	BADFNAMEXIT();
    }

    for(t = file; t && *t ; t++) {
	if(*t == '/') {
	    BADFNAMEXIT();
	}
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
#ifndef NEITHER_FCHOWN_NOR_FCHMOD
    if(0 > (fchown(f, uid, ttygid)))
#else
    /*
     * Chown supposedly clears sgids, in case some Fast Eddie
     * managed to set it after open(). 
     */
    if(0 > (chown(path, uid, ttygid)))
#endif
	{
	    perror("mkrwritebox");
	    (void)unlink(path);
	    return(4);
	}
    (void)close(f);
    return(0);
}

/* EOF (mkrwritebox.c) */
