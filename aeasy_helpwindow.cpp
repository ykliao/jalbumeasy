/* --------------------------------------------------------------------------------------------
 *              aeasy_helpwindow.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Help window implementation.
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
#include "aeasy_helpwindow.h"

/* ---------------------------------------------------------------------------------------------
/  Static member variables definitions
/  ---------------------------------------------------------------------------------------------*/
CHelpWindow *CHelpWindow::m_helpWindow=NULL;                      //singleton help window object


/************************************************************************************************/
void CHelpWindow::showHelpWindow(const QString title,const QRect pos,QString page)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display the help window
   --------------------------------------------------------------------------------------------
    PARAMETERS:  title: String used in the window title
                   pos: window screen position
                  page:
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  QString homePage("index.html");

  if(m_helpWindow==NULL)                 //if the singleton help window not yet created, create it
    {
    m_helpWindow=new CHelpWindow("./help");
    m_helpWindow->setWindowTitle(tr("%1 - Help").arg(title));

          //position the window. (it is recommended to use resize & move rather than setGeometry)
    m_helpWindow->resize(pos.width(),pos.height());
    m_helpWindow->move(pos.left(),pos.top());
    m_helpWindow->m_txtBrowser->setSource(homePage);       //set the home page as the initial page
    }

  if(!page.isEmpty())                                                     //if a page was supplied
    m_helpWindow->m_txtBrowser->setSource(page);                                      //display it
  else
    m_helpWindow->m_txtBrowser->home();                                       //else the home page


  m_helpWindow->show();
                                       //make help window the active window if it is already shown
  m_helpWindow->raise();
  m_helpWindow->activateWindow();

}


/************************************************************************************************/
void CHelpWindow::closeHelpWindow(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Close the help window
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(m_helpWindow!=NULL)
    m_helpWindow->close();
  delete m_helpWindow;
  m_helpWindow=NULL;
}


/************************************************************************************************/
bool CHelpWindow::getLayout(QRect &wrect)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the layout of the help window
   --------------------------------------------------------------------------------------------
    PARAMETERS: wrect: rectangle to return the window layout in
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(m_helpWindow!=NULL)                                     //if the singleton help window exists
    {
    wrect=QRect(m_helpWindow->pos(),m_helpWindow->size());      //get the window position and size
    return true;
    }
  else
    return false;
}



/************************************************************************************************/
CHelpWindow::CHelpWindow(const QString path)
             :QWidget()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Help Window constructor
   --------------------------------------------------------------------------------------------
    PARAMETERS:   path: directory containing the help files
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_txtBrowser=new QTextBrowser;
  m_btnHome=new QPushButton(tr("&Home"));
  m_btnBack=new QPushButton(tr("&Back"));
  m_btnForward=new QPushButton(tr("&Forward"));

  m_btnHome->setEnabled(false);                                   //disable the navigation buttons
  m_btnBack->setEnabled(false);
  m_btnForward->setEnabled(false);

  m_btnClose=new QPushButton(tr("&Close"));

  QHBoxLayout *buttonLayout=new QHBoxLayout;         //horizontal layout that contains the buttons
  buttonLayout->addWidget(m_btnHome);
  buttonLayout->addWidget(m_btnBack);
  buttonLayout->addWidget(m_btnForward);
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_btnClose);

  QVBoxLayout *mainLayout=new QVBoxLayout;
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(m_txtBrowser);
  setLayout(mainLayout);

  connect(m_btnHome,SIGNAL(clicked()),m_txtBrowser,SLOT(home()));
  connect(m_btnBack,SIGNAL(clicked()),m_txtBrowser,SLOT(backward()));
  connect(m_btnForward,SIGNAL(clicked()),m_txtBrowser,SLOT(forward()));
  connect(m_btnClose,SIGNAL(clicked()),this,SLOT(close()));

  connect(m_txtBrowser,SIGNAL(sourceChanged(const QUrl &)),
          this,SLOT(updateButtonStates(const QUrl &)));
  connect(m_txtBrowser,SIGNAL(backwardAvailable(bool)),
          this,SLOT(backwardAvailable(bool)));
  connect(m_txtBrowser,SIGNAL(forwardAvailable(bool)),
          this,SLOT(forwardAvailable(bool)));

                   //buttons can only have one shortcut, and as the Close button has ALT+C already
                   //assigned to it, connected the Esc directly to the Close slot
  QShortcut *closeShortcut=new QShortcut(QKeySequence(tr("Esc")),this);
  connect(closeShortcut,SIGNAL(activated()),this,SLOT(close()));

  m_txtBrowser->setSearchPaths(QStringList() << path);
  m_txtBrowser->setOpenExternalLinks(true);
}



/************************************************************************************************/
void CHelpWindow::updateButtonStates(const QUrl &url)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Slot called when the URL of the displayed page has changed.
   --------------------------------------------------------------------------------------------
    PARAMETERS: url: The URL of the page being displayed
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  QString surl=url.toString();

  if(surl!="index.html")                                                    //if not the home page
    m_btnHome->setEnabled(true);                                          //enable the home button
  else
    m_btnHome->setEnabled(false);                                        //disable the home button
}

