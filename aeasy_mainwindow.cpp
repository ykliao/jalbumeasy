/* --------------------------------------------------------------------------------------------
 *              aeasy_mainwindow.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Main application window implementation.
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
#include "aeasy_version.h"
#include "aeasy_parse.h"
#include "aeasy_album.h"
#include "aeasy_fonts.h"
#include "aeasy_flistwindow.h"
#include "aeasy_helpwindow.h"
#include "aeasy_mainwindow.h"
#include "aeasy_config.h"


/************************************************************************************************/
CMainWindow::CMainWindow(const QString &title,QWidget *parent)
             :QMainWindow(parent)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Constructor for the main application window.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  title: Title text
                parent: The parent widget
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{

  setWindowTitle(title);
                               //central text widget for displaying the HTML formatted status text
  m_textOut=new QTextEdit(this);
  m_textOut->setReadOnly(true);
  m_config=new CConfig;

                         //applying a css stylesheet ensures a more consistent cross platform look
  QString style="* {font-family:Arial,Helvetica,Sans-serif; font-size:11px;} "
                "b {font-size:12px; font-weight:bold;} ";
  m_textOut->document()->setDefaultStyleSheet(style);

  m_textOut->setHtml("");

  setCentralWidget(m_textOut );
                                                                      //create actions for buttons
                    //text action parameter is used for the menu as well as for the button tooltip
  m_actnOpen=new QAction(QIcon(":/resources/ButtonOpen.png"),tr("&Open File"),this);
              //add the accelerator using a shortcut as there is no menu to handle the accelerator
  m_actnOpen->setShortcut(QKeySequence("ALT+O"));
  m_actnOpen->setStatusTip(tr("Open the Album text file"));       //status bar text for the button
  connect(m_actnOpen,SIGNAL(triggered()),this,SLOT(openFile()));

  m_actnGenerate=new QAction(QIcon(":/resources/ButtonGenerate.png"),tr("&Generate Album"),this);
  m_actnGenerate->setShortcut(QKeySequence("ALT+G"));
  m_actnGenerate->setStatusTip(tr("Generate the PDF Album file"));
  connect(m_actnGenerate,SIGNAL(triggered()),this,SLOT(generate()));
  m_actnGenerate->setEnabled(false);

  m_actnFont=new QAction(QIcon(":/resources/ButtonFont.png"),tr("&Font"),this);
  m_actnFont->setShortcut(QKeySequence("ALT+F"));
  m_actnFont->setStatusTip(tr("Display the list of available fonts"));
  connect(m_actnFont,SIGNAL(triggered()),this,SLOT(displayFontSelector()));

  m_actnConfig=new QAction(QIcon(":/resources/ButtonConfigure.png"),tr("&Font"),this);
  m_actnConfig->setShortcut(QKeySequence("ALT+C"));
  m_actnConfig->setStatusTip(tr("Display the configuration dialogue box"));
  connect(m_actnConfig,SIGNAL(triggered()),this,SLOT(displayConfiguration()));

  m_actnDisplayHelp=new QAction(QIcon(":/resources/ButtonHelp.png"),tr("&Help"),this);
                                    //use the platform specific help key (F1 on Linux and Windows)
  m_actnDisplayHelp->setShortcut(QKeySequence::HelpContents);
  m_actnDisplayHelp->setStatusTip(tr("AlbumEasy Help and Documentation"));
  connect(m_actnDisplayHelp,SIGNAL(triggered()),this,SLOT(displayHelp()));

  m_actnExit=new QAction(QIcon(":/resources/ButtonExit.png"),tr("E&xit"),this);
  m_actnExit->setShortcut(QKeySequence("ALT+X"));
  m_actnExit->setStatusTip(tr("Exit the AlbumEasy application"));
  connect(m_actnExit,SIGNAL(triggered()),this,SLOT(close()));

                      //actions can only have one shortcut assigned to them, so connect additional
                      //shortcuts directly to the action
  QShortcut *helpshortcut=new QShortcut(QKeySequence("ALT+H"),this);
  connect(helpshortcut,SIGNAL(activated()),m_actnDisplayHelp,SLOT(trigger()));

  QToolBar *toolBar=addToolBar("ToolBarName");                    //add the buttons to the toolbar

  toolBar->addAction(m_actnOpen);
  toolBar->addAction(m_actnGenerate);
  toolBar->addSeparator();
  toolBar->addAction(m_actnFont);
  toolBar->addAction(m_actnConfig);
  toolBar->addSeparator();
  toolBar->addAction(m_actnDisplayHelp);

  QWidget* bumpRight=new QWidget();               //spacer widget to bump exit button to the right
  bumpRight->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  toolBar->addWidget(bumpRight);
  toolBar->addAction(m_actnExit);

  toolBar->setIconSize(QSize(56,42));
  toolBar->setMovable(false);
  toolBar->setFloatable (false);

  statusBar()->showMessage(tr("Ready"));                       //create the main window status bar

  m_parser=new CParser();
  m_albumData=new CAlbumData();
  m_fontFiles=new CFontFileList();

                             //connect signals from other classes to this window's logMessage slot
                             //so that they can use it to display status to the user
  connect(m_parser,SIGNAL(logMessage(QString,QString,bool)),
                  SLOT(logMessage(QString,QString,bool)));
  connect(m_albumData,SIGNAL(logMessage(QString,QString,bool)),
                  SLOT(logMessage(QString,QString,bool)));
  connect(m_fontFiles,SIGNAL(logMessage(QString,QString,bool)),
                  SLOT(logMessage(QString,QString,bool)));

                        //Load the size and location of the various windows from the configuration
  QRect wndRect;
  m_config->loadWindowLayouts(this,wndRect,m_helpWindowRect,m_fontListWindowRect);

      //position the main window. (it is recommended to use resize & move rather than setGeometry)
  resize(wndRect.size());
  move(wndRect.topLeft());


  logMessage("<br />If you are already familiar with AlbumEasy, press the <b><i>Open</i></b>"
             " button to select the album data file to load. Otherwise press the"
             " <b><i>Help</i></b> button for instructions.","black");
  show();
}


/************************************************************************************************/
CMainWindow::~CMainWindow()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Destructor for the main application window.
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  delete m_albumData;
  delete m_fontFiles;
  delete m_parser;
  delete m_config;
}


/************************************************************************************************/
void CMainWindow::closeEvent(QCloseEvent *event)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Handler for the application closing event.
   --------------------------------------------------------------------------------------------
    PARAMETERS: event: The event
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  if(QMessageBox::warning(this,
                  tr("AlbumEasy"),
                  tr("Are you sure you want to exit?"),
                  QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
    {
    m_config->saveSettings(this);
    CHelpWindow::closeHelpWindow();
    CFontListWindow::closeFontListWindow();
    event->accept();
    }
  else
    event->ignore();
}



/************************************************************************************************/
void CMainWindow::logMessage(QString text,QString colour,bool bold)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Output a line of HTML formatted text to the status window
   --------------------------------------------------------------------------------------------
    PARAMETERS:   text: The text
                colour: HTML colour string, eg red, green, grey, etc
                  bold: true  => bold text
                        false => normal text
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  QString pre="";
  QString post="";

  if(colour!="")
    {
    pre=pre+"<font color=\""+colour+"\">";
    post="</font>"+post;
    }

  if(bold!=false)
    {
    pre=pre+"<b>";
    post="</b>"+post;
    }

  m_textOut->append(pre+text+post);
                                                         //ensure that newly added text is visible
  QScrollBar *sbar=m_textOut->verticalScrollBar();                   //get the vertical scroll bar
  sbar->setValue(sbar->maximum());                                          //scroll to the bottom

}


/************************************************************************************************/
void CMainWindow::openFile(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Slot called by the open file button to get the name of the album text file
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_actnGenerate->setEnabled(false);                                 //disable the generate button

                                                              //display the open file dialogue box
  m_albumSourceFile=QFileDialog::getOpenFileName(this,tr("Open the album text file"),
                                                //m_workDir,
                                                m_config->workDir(),
                                                tr("Text files (*.txt);;All Files (*.*)"));

  if(!m_albumSourceFile.isEmpty())                                   //if the user selected a file
    {
    QFile file(m_albumSourceFile);
    if(file.exists()==false)
      {
      m_albumSourceFile="";
      }
    else
      {
      m_actnGenerate->setEnabled(true);                               //enable the generate button

      QFileInfo finfo(m_albumSourceFile);                                     //save the directory
      m_config->setWorkDir(finfo.canonicalPath());

      logMessage("<br /><b>Loaded File: </b>"+m_albumSourceFile,"black");
      logMessage("<br />Press the <b><i>Generate</i></b> button to create the album.","black");
      }
    }
}


/************************************************************************************************/
void CMainWindow::displayHelp(QString page)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Slot called by the help button to display the help and documentation
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  CHelpWindow::showHelpWindow(windowTitle(),m_helpWindowRect,page);
}


/************************************************************************************************/
void CMainWindow::displayFontSelector(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display the available fonts dialog box
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_fontFiles->populate(this,m_config->includeSystemFonts());

  CFontListWindow::showFontListWindow(this,windowTitle(),m_fontListWindowRect,m_fontFiles,
                                      m_config->listBadFontFiles());
}


/************************************************************************************************/
void CMainWindow::displayConfiguration(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display the configuration dialog box
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  bool oldIncludeSystemFonts=m_config->includeSystemFonts();

  CConfigDlg dlgconfig(m_config,this);

  if(dlgconfig.exec()!= QDialog::Rejected)                             //display the config dialog
    {
    if(dlgconfig.changed()==true)
      {
                                             //if the list of fonts has already been populated and
                                             //include system fonts configuration has changed
      if(m_fontFiles->populated()==true &&  oldIncludeSystemFonts!=m_config->includeSystemFonts())
        {
        QString toggle(tr(" "));
        if(m_config->includeSystemFonts()==false)
          toggle=tr(" not ");
        QMessageBox::information(this,
                           tr("AlbumEasy"),
                           tr("You have chosen%1to include system fonts when searching for fonts."
                             "\nThis change will take place after AlbumEasy is restarted.")
                             .arg(toggle));
        }
      m_config->saveSettings(this);
      }
    }
}



/************************************************************************************************/
void CMainWindow::generate(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Slot called by the generate button to process the source file and generate the
                album.
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  logMessage("");                                                                   //a blank line
  if(m_albumSourceFile.isEmpty())
    logMessage(tr("<b>Error:</b> Please open an Album file before selecting Generate."),"red");
  else
    {
    QFile file(m_albumSourceFile);

    if(!file.exists() || !file.open(QFile::ReadOnly|QFile::Text))
      {
      logMessage("<b>Error:</b> While attempting to read the Album file - "+m_albumSourceFile,
                 "red");
      }
    else
      {
      logMessage(tr("Generating the Album ..."),"LimeGreen",true);
                                                                           //parse the source file
      bool error=m_parser->parseFile(&file,m_albumData,this,m_config,m_fontFiles);
      file.close();

      if(error==false)
        {
                 //generated pdf file has same base name as source file but with a "pdf" extension
        QString pdfFile=QFileInfo(m_albumSourceFile).absolutePath();            //source file path
        pdfFile.append("/");
        pdfFile.append(QFileInfo(m_albumSourceFile).completeBaseName());   //source file base name
        pdfFile.append(".pdf");                                                    //pdf extension

        if(m_albumData->generatePdf(pdfFile)==false)                          //generate the album
          {
          logMessage(tr("Successfully created %1 <br />").arg(pdfFile),"LimeGreen",true);
          }
        }

      }

    }
}



