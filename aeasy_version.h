#ifndef _AEASY_VERSION_H_
#define _AEASY_VERSION_H_

/* --------------------------------------------------------------------------------------------
 *              aeasy_version.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: This file contains the AlbumEasy Version Number. It is also used by the
 *              build script.
 *
 *              The version number consists of three parts, VER_MAJOR, VER_MINOR and VER_REV
 *              VER_REV is incremented for bug fixes
 *
 *              VER_REV should be set to a letter for beta builds e.g
 *                #define VER_REV "a"
 *              or period and digit in quote for a release version
 *                #define VER_REV ".0"
 *
 *              #define VER_ definitions MUST be at the start of the line so as to not confuse
 *              the very simplistic build script.
 * --------------------------------------------------------------------------------------------
 * AUTHORS:     Clive Levinson
 * --------------------------------------------------------------------------------------------
 * REVISIONS:   Date          Version   Who    Comment
 *
 *              2011/04/26    3.0       cl     First QT Version
 * -------------------------------------------------------------------------------------------- */


#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_REV   ".0"


#define APPLICATION_NAME   "AlbumEasy"

#define CONFIG_COMPANY     "AlbumEasy"
#define CONFIG_APPLICATION "AlbumEasy3"

#endif // _AEASY_VERSION_H_
