/* --------------------------------------------------------------------------------------------
 *              aeasy_parse.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Class for parsing the album source file
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

#ifndef _AEASY_PARSE_H
#define _AEASY_PARSE_H

#include "AlbumEasy.h"

class CAlbumData;
class CConfig;
class CFontFileList;

/************************************************************************************************
CParser: class responsible for parsing the album source
************************************************************************************************/
class CParser:public QObject
{
  Q_OBJECT

public:
  bool parseFile(QFile *file,CAlbumData *album,QWidget *parent,CConfig *config,
                 CFontFileList *fontFiles);
signals:
  void logMessage(QString text,QString colour="",bool bold=false);
private:
  void cleanLine(QString &buf);
  bool splitLine(QString line,QString &cmnd,QString &parms);
  bool processCommand(QString cmnd,QString parms);
  bool parseBlankParameters(QString cmnd,QString parms);
  void extractParameterField(QString &parms,QString &field,bool toUpper=false);
  int parseDoubleParameters(QString cmnd,QString &parms,int count,double *vals,
                            bool displayErr=true);
  bool parseTextField(QString cmnd,QString &parms,QString &text);
  bool parseFontParameters(QString cmnd,QString &parms,int &fidex,double &size);
  bool pageRequired(QString cmnd);
  bool rowRequired(QString cmnd);
  void displayError(int line,QString msg);
public:
  bool processPageSizeCommand(QString cmnd,QString parms);
  bool processPageMarginsCommand(QString cmnd,QString parms);
  bool processPageBorderCommand(QString cmnd,QString parms);
  bool processPageSpacingCommand(QString cmnd,QString parms);
  bool processPageTitleCommand(QString cmnd,QString parms);
  bool processDefineFontCommand(QString cmnd,QString parms);
  bool processPageStartCommand(QString cmnd,QString parms);
  bool processPageTextCommand(QString cmnd,QString parms);
  bool processRowAlignCommand(QString cmnd,QString parms);
  bool processRowStartCommand(QString cmnd,QString parms);
  bool processStampAddCommand(QString cmnd,QString parms);
private:
  int m_currentLine;
  CAlbumData *m_albumData;
  CFontFileList *m_fontFiles;
  QWidget *m_parent;
  CConfig *m_config;
};


inline void CParser::displayError(int line,QString msg)
{
  emit(logMessage(tr("<b>Error in line %1: </b> %2").arg(line).arg(msg),"red",false));
}

#endif // _AEASY_PARSE_H



