/* --------------------------------------------------------------------------------------------
 *              aeasy_flistwindow.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Font list window class declaration
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

#ifndef _AEASY_FLIST_WINDOW_H_
#define _AEASY_FLIST_WINDOW_H_


#include "AlbumEasy.h"
#include "aeasy_fonts.h"

class CMainWindow;


class CFontListWindow : public QWidget
{
Q_OBJECT

public:
  static void showFontListWindow(CMainWindow *parent,const QString title,const QRect pos,
                                 CFontFileList *fontFileList, bool showBadFontFiles);
  static void closeFontListWindow(void);
  static bool getLayout(QRect &wrect);
private slots:
  void displayHelp(void);
signals:
  void displayHelpSignal(QString page);
private:                                                         //singleton, therefore:
  CFontListWindow(CMainWindow *parent);                          //  private constructor
  ~CFontListWindow(){;};                                         //  private destructor
  CFontListWindow(const CFontListWindow &);                      //  undefined copy constructor
  CFontListWindow& operator=(const CFontListWindow &);           //  undefined assignment operator

private:
  static CFontListWindow *m_fontListWindow;
  QTableWidget *m_tableGoodFonts;
  QTableWidget *m_tableBadFonts;
  QPushButton *m_btnHelp;
  QPushButton *m_btnClose;
  QLabel *m_lblBadFonts;
  QLabel *m_lblGoodFonts;
};

inline void CFontListWindow::displayHelp(void)
{
  emit(displayHelpSignal("fonts.html#fontsDialogue"));
}


#endif // _AEASY_FLIST_WINDOW_H_



