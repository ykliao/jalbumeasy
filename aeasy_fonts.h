/* --------------------------------------------------------------------------------------------
 *              aeasy_fonts.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Class declarations for the font handling objects:
 * --------------------------------------------------------------------------------------------
 * COPYRIGHT:   Copyright (c) 2005-2013
 *              Clive Levinson <clivel@bundu.com>
 *              Bundu Technology Ltd.
 * --------------------------------------------------------------------------------------------
 * LICENCE:     AlbumEasy is free software: you can redistribute it and/or modify it under
 *              the terms of the GNU General Public License as published by the
 *              Free Software Foundation, either version 3 of the License, or (at your option)
 *              any later version.
 * --------------------------------------------------------------------------------------------
 * AUTHORS:     Clive Levinson
 * --------------------------------------------------------------------------------------------
 * REVISIONS:   Date          Version   Who    Comment
 *
 *              2011/04/26    3.0       cl     First QT Version
 * -------------------------------------------------------------------------------------------- */

#ifndef _AEASY_FONTS_H
#define _AEASY_FONTS_H


#include "AlbumEasy.h"
#include <hpdf.h>

class CFontFileList;
class CFontFileInfo;

/************************************************************************************************
 CFontManager: maintains the relationship between fonts defined in the album and the actual
               font file and code page used by libharu
 ************************************************************************************************/
class CFontManager:public QObject
{
public:
  static void initialise(void);
  static bool addUserDefinedFont(CFontFileList *fontFiles,QString fontId,QString fontName,
                                 QString encoding);
  static int getFontIndex(QString fontId);
  static HPDF_Font getFont(HPDF_Doc pdfDoc,int index);
  static QTextCodec *getCodec(int index);
  static QString getError(void) {return m_error;};
private:
  static QString m_error;
};

/************************************************************************************************
 CFontFileList: list of CFontFileInfo records, one for each of the available TTF files
 ************************************************************************************************/
class CFontFileList:public QObject
{
  Q_OBJECT
public:
  CFontFileList(void){m_populated=false; m_badFontFiles=new QStringList;};
  void populate(QWidget *parent,bool includeSystemFonts);
  bool populated(void) {return m_populated;};
  bool find(QString fontName,QString &fileName,QString &filePath);
  int count(void) {return m_fontFileInfoRecs.count() ;};
  CFontFileInfo *at(int idx);
  QStringList *badFileList(void) {return m_badFontFiles; };
  ~CFontFileList();
signals:
  void logMessage(QString text,QString colour="",bool bold=false);
private:
  static bool sortByName(CFontFileInfo *f1, CFontFileInfo *f2);
  bool contains(const QString fileName);
  void parseFontFile(const QString fileName,const QString filePath);

  QList<CFontFileInfo *> m_fontFileInfoRecs;
  QStringList *m_badFontFiles;
  bool m_populated;
};


/************************************************************************************************
 CFontPathList: list of paths to search for TTF files
 ************************************************************************************************/
class CFontPathList:public QStringList
{

public:
  void populate(bool includeSystemFonts);
private:
  void append(QString path);
  void parseLinuxFontConfigFile(QString file);
};


/************************************************************************************************
CFontFileInfo: contains the font name, file name, and code pages
************************************************************************************************/
class CFontFileInfo
{
public:
  void setFileDetails(QString fontName,QString fileName,QString filePath,quint32 codePageRange1,
                      quint32 codePageRange2);
  QString fileName(void) {return m_fileName;};
  QString filePath(void) {return m_filePath;};
  QString fontName(void) {return m_fontName;};
  void availableCodePages(QString &codePages);

private:
  QString m_fontName;                                                //name to display to the user
  QString m_fileName;                                                //name of the .ttf file
  QString m_filePath;                                                //path to the .ttf file
  quint32 m_codePageRange1;
  quint32 m_codePageRange2;
};


#endif // _AEASY_FONTS_H

