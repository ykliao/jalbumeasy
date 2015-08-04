/* ------------------------------------------------------------------------------------------
 *              AlbumEasy.h
 * ------------------------------------------------------------------------------------------
 * DESCRIPTION: Application wide include file
 * ------------------------------------------------------------------------------------------
 * COPYRIGHT:   Copyright (c) 2005-2013
 *              Clive Levinson <clivel@bundu.com>
 *              Bundu Technology Ltd.
 * ------------------------------------------------------------------------------------------
 * LICENCE:     AlbumEasy is free software: you can redistribute it and/or modify it under
 *              the terms of the GNU General Public License as published by the
 *              Free Software Foundation, either version 3 of the License, or (at your option)
 *              any later version.
 * ------------------------------------------------------------------------------------------
 * AUTHORS:     Clive Levinson
 * ------------------------------------------------------------------------------------------
 * REVISIONS:   Date          Version   Who    Comment
 *
 *              2011/04/26    3.0       cl     First QT Version
 * ------------------------------------------------------------------------------------------ */

#ifndef _ALBUM_EASY_H
#define _ALBUM_EASY_H

#include <QtWidgets>

#define TR(s) QObject::tr(s)

#ifndef QT_NO_DEBUG
/* ------------------------------------------------------------------------------------------
   Convenience function for outputting debug messages to the console
   ------------------------------------------------------------------------------------------
   Example Usage: DEBUGS(tr("Count:%1 Index:%2 Val:%3").arg(total).arg(idx).arg(val));
   ------------------------------------------------------------------------------------------ */
   #define DEBUGS(s) DEBUGSX(s,__FILE__,__LINE__)

   inline void DEBUGSX(QString s,const char *f, int l)
     {
     QString dbgs=QString("%1[%2]: %3").arg(f).arg(l).arg(s);
     qDebug("%s",dbgs.toLatin1().data());
     }
#else
   #define DEBUGS(s)
#endif //QT_NO_DEBUG

#endif //_ALBUM_EASY_H



