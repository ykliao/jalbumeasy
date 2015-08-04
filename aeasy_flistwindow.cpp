/* --------------------------------------------------------------------------------------------
 *              aeasy_flistwindow.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Font list window implementation - Displays a list of available fonts
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

#include "AlbumEasy.h"
#include "aeasy_mainwindow.h"
#include "aeasy_flistwindow.h"

/* ---------------------------------------------------------------------------------------------
/  Static member variables definitions
/  ---------------------------------------------------------------------------------------------*/
CFontListWindow *CFontListWindow::m_fontListWindow=NULL;       //singleton font list window object


/************************************************************************************************/
void CFontListWindow::showFontListWindow(CMainWindow *parent,const QString title,const QRect pos,
                                         CFontFileList *fontFileList,bool showBadFontFiles)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display a window containing the list of available fonts
   --------------------------------------------------------------------------------------------
    PARAMETERS:           parent: Parent window
                           title: The window title
                             pos: Position on screen
                    fontFileList: The list of fonts
                showBadFontFiles: true  => display the list of incompatible fonts
                                  false => hide the list
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(m_fontListWindow==NULL)         //if the singleton window has not yet been created, create it
    {
    m_fontListWindow=new CFontListWindow(parent);
    m_fontListWindow->setWindowTitle(tr("%1 - Available fonts").arg(title));

           //position the window. (it is recommended to use resize & move rather than setGeometry)
    m_fontListWindow->resize(pos.width(),pos.height());
    m_fontListWindow->move(pos.left(),pos.top());
    }

  m_fontListWindow->show();
                                            //make window the active window if it is already shown
  m_fontListWindow->raise();
  m_fontListWindow->activateWindow();

  m_fontListWindow->m_tableGoodFonts->clearContents();
  m_fontListWindow->m_tableGoodFonts->setRowCount(0);

  m_fontListWindow->m_tableBadFonts->clearContents();
  m_fontListWindow->m_tableBadFonts->setRowCount(0);



  QTableWidgetItem *tableItem;

  for(int i=0; i< fontFileList->count(); i++)    //fill the table with the list of available fonts
    {
    CFontFileInfo *finfo=fontFileList->at(i);
    if(finfo!=0)
      {
      int row=m_fontListWindow->m_tableGoodFonts->rowCount();  //add a row to the end of the table
      m_fontListWindow->m_tableGoodFonts->insertRow(row);

                             //add an item containing the font name to the first column in the row
      tableItem=new QTableWidgetItem(finfo->fontName());
      tableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      m_fontListWindow->m_tableGoodFonts->setItem(row,0,tableItem);

                                  //add an item containing available code pages to the next column
      QString codePages;
      finfo->availableCodePages(codePages);
      tableItem=new QTableWidgetItem(codePages);
      tableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      m_fontListWindow->m_tableGoodFonts->setItem(row,1,tableItem);

                                         //add an item containing the file name to the next column
      tableItem=new QTableWidgetItem(finfo->fileName());
      tableItem->setFlags(Qt::NoItemFlags);
      m_fontListWindow->m_tableGoodFonts->setItem(row, 2, tableItem);

      m_fontListWindow->m_tableGoodFonts->resizeRowToContents(row);
      }
    }

  QStringList *badfiles=fontFileList->badFileList();          //get the list of incompatible fonts

  for(int i=0;i<badfiles->count();i++)                          //and fill the table with the list
    {
    QString fileName=badfiles->at(i);
    if(fileName.size()>0)
      {
      int row=m_fontListWindow->m_tableBadFonts->rowCount();
      m_fontListWindow->m_tableBadFonts->insertRow(row);

      tableItem=new QTableWidgetItem(fileName);
      tableItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      m_fontListWindow->m_tableBadFonts->setItem(row, 0, tableItem);

      }
    }

  if(showBadFontFiles==true)
    {
    m_fontListWindow->m_tableBadFonts->show();
    m_fontListWindow->m_lblBadFonts->show();
    }
  else
    {
    m_fontListWindow->m_tableBadFonts->hide();
    m_fontListWindow->m_lblBadFonts->hide();
    }

}

/************************************************************************************************/
void CFontListWindow::closeFontListWindow(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Close the fonts list window
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(m_fontListWindow!=NULL)
    m_fontListWindow->close();
  delete m_fontListWindow;
  m_fontListWindow=NULL;
}

/************************************************************************************************/
bool CFontListWindow::getLayout(QRect &wrect)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the layout of the fonts list window
   --------------------------------------------------------------------------------------------
    PARAMETERS: wrect: rectangle to return the window layout in
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(m_fontListWindow!=NULL)                                      //if the singleton window exists
    {
    wrect=QRect(m_fontListWindow->pos(),m_fontListWindow->size());  //get window position and size
    return true;
    }
  else
    return false;
}



/************************************************************************************************/
CFontListWindow::CFontListWindow(CMainWindow *parent)
                :QWidget()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Font list window constructor
   --------------------------------------------------------------------------------------------
    PARAMETERS: parent: The parent widget
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
                    //create table widgets for the lists of available-fonts and incompatible-fonts
  m_tableGoodFonts=new QTableWidget(0,3,this);
  m_tableBadFonts=new QTableWidget(0,1,this);

  QStringList headers;

  headers= (QStringList() << "Name" << "Code Pages"  << "File");
  m_tableGoodFonts->setHorizontalHeaderLabels(headers);

  headers.clear();
  headers= (QStringList() << " ");
  m_tableBadFonts->setHorizontalHeaderLabels(headers);

       //column width is controlled by horizontal headers - set to stretch filling available space
  m_tableGoodFonts->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
  m_tableGoodFonts->horizontalHeader()->setStretchLastSection(true);

  m_tableBadFonts->horizontalHeader()->setStretchLastSection(true);

  m_tableGoodFonts->verticalHeader()->hide();
  m_tableBadFonts->verticalHeader()->hide();
  m_tableBadFonts->horizontalHeader()->hide();

  m_tableGoodFonts->setColumnWidth(0,width()/3);
  m_tableGoodFonts->setColumnWidth(1,width()/3);

  m_btnClose=new QPushButton(tr("&Close"));
  connect(m_btnClose,SIGNAL(clicked()),this,SLOT(close()));

  m_btnHelp=  new  QPushButton(tr("&Help"));
  connect(m_btnHelp,SIGNAL(clicked()),this,SLOT(displayHelp()));


        //buttons can only have one shortcut, and as the Help and Close buttons have the ALT+H and
        //ALT+C already assigned to them, instead connect the platform specific help key (F1) and
        //the the Esc key directly to the displayHelp and Close slots
  QShortcut *helpShortcut=new QShortcut(QKeySequence::HelpContents,this);
  connect(helpShortcut,SIGNAL(activated()),this,SLOT(displayHelp()));

  QShortcut *closeShortcut=new QShortcut(QKeySequence(tr("Esc")),this);
  connect(closeShortcut,SIGNAL(activated()),this,SLOT(close()));

                                         //parent window is signalled to actually display the help
  connect(this,SIGNAL(displayHelpSignal(QString)),parent,SLOT(displayHelp(QString)));


  m_lblGoodFonts=new QLabel("<b>The list of fonts that are compatible with AlbumEasy:</b>",this);
  m_lblBadFonts=new QLabel("<b>These fonts are incompatible with AlbumEasy:</b>",this);

  QHBoxLayout *buttonLayout=new QHBoxLayout;    //horizontal layout that contains the close button
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_btnHelp);
  buttonLayout->addWidget(m_btnClose);


  QFontMetrics fmetrics(m_tableBadFonts->font());     //restrict the height of the  bad font table
  int h=fmetrics.height();
  m_tableBadFonts->setMaximumHeight(6*h);


  QVBoxLayout *mainLayout=new QVBoxLayout;
  mainLayout->addWidget(m_lblGoodFonts);
  mainLayout->addWidget(m_tableGoodFonts,1);    //the good font table is set to stretch vertically
  mainLayout->addWidget(m_lblBadFonts);
  mainLayout->addWidget(m_tableBadFonts);
  mainLayout->addLayout(buttonLayout);
  setLayout(mainLayout);
}

