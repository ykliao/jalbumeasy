/* --------------------------------------------------------------------------------------------
 *              aeasy_mainwindow.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Main application window class declaration.
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

#ifndef _AEASY_MAIN_WINDOW_H_
#define _AEASY_MAIN_WINDOW_H_

#include "AlbumEasy.h"

class CParser;
class CAlbumData;
class CFontFileList;
class CConfig;


/************************************************************************************************
 Main window class
 ************************************************************************************************/
class CMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  CMainWindow(const QString &title,QWidget *parent = 0);
  ~CMainWindow();
public slots:
  void logMessage(QString text,QString colour="",bool bold=false);
protected:
  void closeEvent(QCloseEvent *event);
public slots:
    void displayHelp(QString page="");
private slots:
    void openFile(void);
    void generate(void);
    void displayFontSelector(void);
    void displayConfiguration(void);
private:
  CParser *m_parser;
  CAlbumData *m_albumData;
  CFontFileList *m_fontFiles;
  QTextEdit *m_textOut;
  QAction *m_actnOpen;
  QAction *m_actnGenerate;
  QAction *m_actnFont;
  QAction *m_actnConfig;
  QAction *m_actnDisplayHelp;
  QAction *m_actnExit;
  QString m_albumSourceFile;
  QRect m_helpWindowRect;
  QRect m_fontListWindowRect;
  CConfig *m_config;
};


#endif // _AEASY_MAIN_WINDOW_H_
