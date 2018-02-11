/*
 * Copyright (c) 1994 by Academic Press, Boston, Massachusetts.
 * Written by Douglas A. Gwyn.  Not derived from licensed software.
 * From the book "Software Solutions in C", edited by Dale Schumacher.
 *
 * Permission is granted to anyone to use this software for any
 * purpose on any computer system, and to redistribute it in any way,
 * subject to the following restrictions:
 *
 *   1. The author is not responsible for the consequences of use of
 *	this software, no matter how awful, even if they arise
 *	from defects in it.
 *
 *   2. The origin of this software must not be misrepresented, either
 *	by explicit claim or by omission.
 *
 *   3. Altered versions must be plainly marked as such, and must not
 *	be misrepresented (by explicit claim or omission) as being
 *	the original software.
 *
 *   4. This notice must not be removed or altered.
 */

/*
	opendir -- open a directory stream
		(PUBLIC-DOMAIN implementation for MS-DOS)

	last edit:	19-Jan-1994	Gwyn@ARL.Army.Mil
*/

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <dos.h>

#define	ATTRIBUTES	(_A_NORMAL|_A_RDONLY|_A_HIDDEN|_A_SYSTEM \
			|_A_VOLID|_A_SUBDIR|_A_ARCH)

#ifndef	ENOTDIR
#define	ENOTDIR	EDOM
#endif

#ifndef	ENOMEM
#define	ENOMEM	ERANGE
#endif

static char *
extend(const char *dir_name)		/* validate and append *.* */
{
	register size_t	n;		/* length of dir_name */
	register char	*buffer;	/* -> allocated new string */
	register char	*suffix;	/* -> wildcard suffix */

	/* Validate the directory name. */

	if (dir_name == NULL
	||  strchr(dir_name, '?') != NULL
	||  strchr(dir_name, '*') != NULL) {
		errno = ENOTDIR;
		return NULL;
	}

	/* Determine what form of *.* should be appended. */

	if ((n = strlen(dir_name)) == 0
	||  dir_name[n-1] == ':'	/* just a drive name */
	||  dir_name[n-1] == '/'
	||  dir_name[n-1] == '\\') {
		n += 4;
		suffix = "*.*";
	}
	else {
		n += 5;
		suffix = "\\*.*";
	}

	/* Construct the new wildcarded name. */

	if ((buffer = (char *)malloc(n)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	return strcat(strcpy(buffer, dir_name), suffix);
}

static int
append(struct find_t *fp, DIR *dirp, long *np)	/* add an entry */
{
	register struct dirent	*dp;	/* -> allocated entry */

	/* MS-DOS 6.2 (or Turbo C++ 3.1) now invents "." and ".." entries! */
	if (strcmp(fp->name, ".") == 0
	||  strcmp(fp->name, "..") == 0) {
		return 1;		/* successful no-op */
	}

	if ((dp = (struct dirent *)malloc(sizeof(struct dirent)))
		== NULL) {
		return 0;		/* failure */
	}

	/* link onto chain */

	if (dirp->d_first == NULL) {
		dirp->d_first = dp;
	}
	else {	/* dirp->d_next currently points to previous entry */
		dirp->d_next->d_next = dp;	/* append to tail */
	}

	dirp->d_next = dp;
	dp->d_next = NULL;
	dp->d_ino = (*np)++;		/* index number */
	strncpy(dp->d_name, fp->name, d_MAXNAMLEN);
	dp->d_name[d_MAXNAMLEN] = '\0';
	return 1;			/* success */
}

DIR *
opendir(const char *dir_name)
{
	struct find_t	f_data;		/* used by _dos_find* */
	register char	*wild_name;	/* -> dir_name\*.* */
	register DIR	*dirp;		/* -> malloc()ed storage */
	long		number;		/* entry number */

	if ((wild_name = extend(dir_name)) == NULL) {
		return NULL;		/* errno already set */
	}

	if (_dos_findfirst(wild_name, ATTRIBUTES, &f_data) != 0) {
		free(wild_name);
		errno = ENOTDIR;	/* not a directory? */
		return NULL;
	}

	if ((dirp = (DIR *)malloc(sizeof(DIR))) == NULL) {
		free(wild_name);
		errno = ENOMEM;
		return NULL;
	}

	dirp->d_first = NULL;
	number = 0L;

	while (append(&f_data, dirp, &number)) {
		if (_dos_findnext(&f_data) != 0) {	/* EOF */
			errno = 0;	/* clear ENOENT */
			dirp->d_next = dirp->d_first;	/* BOF */
			return dirp;	/* success */
		}
	}

	/* oops -- couldn't allocate the whole chain */

	closedir(dirp);			/* easy way to discard */
	free(wild_name);
	errno = ENOMEM;
	return NULL;
}
