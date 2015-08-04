/* --------------------------------------------------------------------------------------------
 *              aeasy_helpwindow.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Help window class declaration
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

#ifndef _AEASY_HELP_WINDOW_H_
#define _AEASY_HELP_WINDOW_H_

#include "AlbumEasy.h"

class CHelpWindow : public QWidget
{
Q_OBJECT

public:
  static void showHelpWindow(const QString title,const QRect pos,QString page);
  static void closeHelpWindow(void);
  static bool getLayout(QRect &wrect);
private slots:
  void updateButtonStates(const QUrl &);
  void backwardAvailable(bool available);
  void forwardAvailable(bool available);
private:                                                         //singleton, therefore:
  CHelpWindow(const QString path);                               //  private constructor
  ~CHelpWindow(){;};                                             //  private destructor
  CHelpWindow(const CHelpWindow &);                              //  undefined copy constructor
  CHelpWindow& operator=(const CHelpWindow &);                   //  undefined assignment operator

private:
  static CHelpWindow *m_helpWindow;
  QTextBrowser *m_txtBrowser;
  QPushButton *m_btnHome;
  QPushButton *m_btnBack;
  QPushButton *m_btnForward;
  QPushButton *m_btnClose;
};

inline void CHelpWindow::backwardAvailable(bool available)
{
  m_btnBack->setEnabled(available);
}

inline void CHelpWindow::forwardAvailable(bool available)
{
  m_btnForward->setEnabled(available);
}


#endif // _AEASY_HELP_WINDOW_H_



