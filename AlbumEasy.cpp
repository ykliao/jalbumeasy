/* ------------------------------------------------------------------------------------------
 *              AlbumEasy Version 3
 * ------------------------------------------------------------------------------------------
 * DESCRIPTION: Open source version of AlbumEasy based on the QT toolkit.
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

#include <QApplication>
#include "AlbumEasy.h"
#include "aeasy_version.h"
#include "aeasy_mainwindow.h"


int main(int argc,char *argv[])
{
  QApplication a(argc,argv);

  QIcon icon=QIcon(":/resources/AlbumEasyIcon32x32.png");
  icon.addFile(":/resources/AlbumEasyIcon16x16.png");

  a.setWindowIcon(icon);
  CMainWindow w(QString(APPLICATION_NAME)+" v"+QString::number(VER_MAJOR)+"."+QString::number(VER_MINOR)+VER_REV);
  w.show();
  return a.exec();
}
