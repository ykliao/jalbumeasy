/* --------------------------------------------------------------------------------------------
 *              aeasy_config.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Configuration settings and configuration dialogue box implementation.
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
#include "aeasy_mainwindow.h"
#include "aeasy_flistwindow.h"
#include "aeasy_helpwindow.h"
#include "aeasy_config.h"

/* ---------------------------------------------------------------------------------------------
/  Local Definitions
/  ---------------------------------------------------------------------------------------------*/
#define MAIN_WND_DFLT_XPOS   40                         //default position and size for mainwindow
#define MAIN_WND_DFLT_YPOS   40
#define MAIN_WND_DFLT_WIDTH  440
#define MAIN_WND_DFLT_HEIGHT 400

#define HELP_WND_DFLT_XPOS   120                        //default position and size for helpwindow
#define HELP_WND_DFLT_YPOS   140
#define HELP_WND_DFLT_WIDTH  640
#define HELP_WND_DFLT_HEIGHT 440

#define FLIST_WND_DFLT_XPOS   100                  //default position and size for fontlist window
#define FLIST_WND_DFLT_YPOS   120
#define FLIST_WND_DFLT_WIDTH  580
#define FLIST_WND_DFLT_HEIGHT 400


/************************************************************************************************/
CConfig::CConfig(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Constructor for the application configuration object loads the application
                 settings.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  none
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_settings=new QSettings(CONFIG_COMPANY,CONFIG_APPLICATION);

  QString defaultDir;
                              //get the document directory to use as the default working directory
  QStringList dirs=QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
  if(dirs.length()>0)                                            //if a directory was found use it
    defaultDir=dirs.at(0)+"/AlbumEasy";
  else
    defaultDir=QApplication::applicationDirPath();            //else use the application directory

  defaultDir=QDir::toNativeSeparators(defaultDir);
                                     //get the working directory, if not found use the the default
  m_workDir=m_settings->value("workdir",defaultDir).toString();

//DEBUGS(TR("Working Dir:=[%1] Default:=[%2]").arg(m_workDir).arg(defaultDir));

                                                                              //get other settings
  m_unicodeMode=m_settings->value("unicodeMode",false).toBool();
  m_includeSystemFonts=m_settings->value("includeSystemFonts",false).toBool();
  m_listBadFontFiles=m_settings->value("listBadFontFiles",false).toBool();
}


/************************************************************************************************/
CConfig::~CConfig(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Configuration object destructor.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  none
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  delete m_settings;
}


/************************************************************************************************/
void CConfig::loadWindowLayouts(QWidget *parent,QRect &mainWndRect,QRect &helpWindowRect,
                                QRect &fontListWindowRect)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Load the sizes and positions of the various application windows from the
                 configuration
   --------------------------------------------------------------------------------------------
    PARAMETERS:  parent:             Main application window
                 mainWndRect:        Variable for returning the main window size and position
                 helpWindowRect:     Variable for returning the help window size and position
                 fontListWindowRect: Variable for returning the font list window size and position
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{

  int minWidth=parent->minimumWidth();
  int minHeight=parent->minimumHeight();

  minWidth=(minWidth<64)?64:minWidth;
  minHeight=(minHeight<64)?64:minHeight;

  mainWndRect=QRect(MAIN_WND_DFLT_XPOS,MAIN_WND_DFLT_YPOS,              //set main window defaults
                    MAIN_WND_DFLT_WIDTH,MAIN_WND_DFLT_HEIGHT);
                                                      //load main window layout from configuration
  mainWndRect=m_settings->value("mainwindow",mainWndRect).toRect();
  constrainRectToScreen(mainWndRect,minWidth,minHeight);

  helpWindowRect=QRect(HELP_WND_DFLT_XPOS,HELP_WND_DFLT_YPOS,           //set help window defaults
                       HELP_WND_DFLT_WIDTH,HELP_WND_DFLT_HEIGHT);
                                                      //load help window layout from configuration
  helpWindowRect=m_settings->value("helpwindow",helpWindowRect).toRect();
  constrainRectToScreen(helpWindowRect,minWidth,minHeight);

  fontListWindowRect=QRect(FLIST_WND_DFLT_XPOS,FLIST_WND_DFLT_YPOS,       //set font-list defaults
                           FLIST_WND_DFLT_WIDTH,FLIST_WND_DFLT_HEIGHT);
                                                        //load font-list layout from configuration
  fontListWindowRect=m_settings->value("flistwindow",fontListWindowRect).toRect();
  constrainRectToScreen(fontListWindowRect,minWidth,minHeight);
}


/************************************************************************************************/
void CConfig::saveSettings(QWidget *parent)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Save the application settings
   --------------------------------------------------------------------------------------------
    PARAMETERS:  parent: Main application window
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  QRect wndRect(parent->pos(),parent->size());                             //main window rectangle
  QRect helpWindowRect;
  QRect fontListWindowRect;

  if(CHelpWindow::getLayout(helpWindowRect)==false)                        //help window rectangle
    {
    helpWindowRect=QRect(HELP_WND_DFLT_XPOS,HELP_WND_DFLT_YPOS,
                         HELP_WND_DFLT_WIDTH,HELP_WND_DFLT_HEIGHT);
    }

  if(CFontListWindow::getLayout(fontListWindowRect)==false)           //font-list window rectangle
    {
    fontListWindowRect=QRect(FLIST_WND_DFLT_XPOS,FLIST_WND_DFLT_YPOS,
                            FLIST_WND_DFLT_WIDTH,FLIST_WND_DFLT_HEIGHT);
    }
                                                                         //save the window layouts
  m_settings->setValue("mainwindow",wndRect);                                 //main window layout
  m_settings->setValue("helpwindow",helpWindowRect);                          //help window layout
  m_settings->setValue("flistwindow",fontListWindowRect);                //font-list window layout

  m_settings->setValue("workdir",workDir());                              //save working directory

                                                                   //save other configuration data
  m_settings->setValue("unicodeMode",unicodeMode());
  m_settings->setValue("includeSystemFonts",includeSystemFonts());
  m_settings->setValue("listBadFontFiles",listBadFontFiles());
}



/************************************************************************************************/
void CConfig::constrainRectToScreen(QRect &rect,int minWidth,int minHeight)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: ensure that the supplied rectangle is completely on the screen
   --------------------------------------------------------------------------------------------
    PARAMETERS:      rect: Rectangle to constrain
                 minWidth: minimum width of the rectangle
                minHeight: minimum height of the rectangle
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  QRect screen(QApplication::desktop()->availableGeometry());         //available screen rectangle

  if(rect.width()<minWidth)
    rect.setWidth(minWidth);

  if(rect.height()<minHeight)
    rect.setHeight(minHeight);


  if(screen.contains(rect)==false)       //if not completely inside the available screen rectangle
    {
    if(rect.left()< screen.left())
      {
      rect.moveLeft(screen.left());
      rect.setWidth(qMin(screen.width(),rect.width()));
      }
    else if(rect.right()> screen.right())
      {
      rect.moveRight(screen.right());
      rect.setWidth(qMin(screen.width(),rect.width()));
      }

    if(rect.top()< screen.top())
      {
      rect.moveTop(screen.top());
      rect.setHeight(qMin(screen.height(),rect.height()));
      }
    else if(rect.bottom()> screen.bottom())
      {
      rect.moveBottom(screen.bottom());
      rect.setHeight(qMin(screen.height(),rect.height()));
      }

    }
}


/************************************************************************************************/
CConfigDlg::CConfigDlg(CConfig *config,CMainWindow *parent)
           :QDialog(parent,Qt::Dialog|Qt::WindowTitleHint)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display the configuration dialogue box
   --------------------------------------------------------------------------------------------
    PARAMETERS: config: Configuration data
                parent: Parent window
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  setWindowTitle(tr("Configuration"));
  m_config=config;

      //only modal to parent window, thus allowing interaction with the help window if it is shown
  setWindowModality(Qt::WindowModal);


  QLabel *prompt  =new QLabel (tr("Character Encoding"));
  m_radioLatin1   =new QRadioButton(tr("&Latin 1"),this);
  m_radioUnicode  =new QRadioButton(tr("&Unicode (UTF-8)"),this);
  m_chkSysFonts   =new QCheckBox(tr("&Include system fonts when searching for fonts"),this);
  m_chkBadFontList=new QCheckBox(tr("&Display the list of incompatible fonts in the font list"
                                    " dialogue box"),this);
  m_btnHelp       =new QPushButton(tr("&Help"));
  m_btnOk         =new QPushButton(tr("&OK"));
  m_btnCancel     =new QPushButton(tr("&Cancel"));

  QVBoxLayout *radioButtonsVbox=new QVBoxLayout;
  QHBoxLayout *radioButtonsHbox=new QHBoxLayout;
  radioButtonsVbox->addWidget(m_radioLatin1);
  radioButtonsVbox->addWidget(m_radioUnicode);
  radioButtonsHbox->addSpacing(16);
  radioButtonsHbox->addLayout(radioButtonsVbox);

  QHBoxLayout *buttonsHbox=new QHBoxLayout;
  buttonsHbox->addWidget(m_btnHelp);
  buttonsHbox->addStretch();
  buttonsHbox->addWidget(m_btnOk);
  buttonsHbox->addWidget(m_btnCancel);

  QVBoxLayout *mainVLayout=new QVBoxLayout;
  mainVLayout->addWidget(prompt);
  mainVLayout->addLayout(radioButtonsHbox);

  mainVLayout->addSpacing(12);

  mainVLayout->addWidget(m_chkSysFonts);
  mainVLayout->addWidget(m_chkBadFontList);
  mainVLayout->addSpacing(12);

  mainVLayout->addStretch();
  QGroupBox *separator=new QGroupBox;
  separator->setFlat(true);
  mainVLayout->addWidget(separator);

  mainVLayout->addLayout(buttonsHbox);

  setLayout(mainVLayout);

  int mleft;          //increase the size of the margins between the layout and the dialog borders
  int mtop;
  int mright;
  int mbottom;

  mainVLayout->getContentsMargins(&mleft,&mtop,&mright,&mbottom);
  mleft=(3*mleft)/2;
  mtop=(3*mtop)/2;
  mright=(3*mright)/2;
  mbottom=(3*mbottom)/2;
  mainVLayout->setContentsMargins(mleft,mtop,mright,mbottom);

                                                                          //connect button signals
  connect(m_btnHelp,  SIGNAL(clicked()),this,SLOT(displayHelp()));
  connect(m_btnOk,    SIGNAL(clicked()),this,SLOT(accept()));
  connect(m_btnCancel,SIGNAL(clicked()),this,SLOT(reject()));

        //buttons can only have one shortcut, and as the Help button has ALT+H already assigned to
        //it, instead connect the platform specific help key (F1) directly to the displayHelp slot
  QShortcut *helpshortcut=new QShortcut(QKeySequence::HelpContents,this);
  connect(helpshortcut,SIGNAL(activated()),this,SLOT(displayHelp()));

            //connect the displayHelpSignal to the parent window slot to actually display the help
  connect(this,SIGNAL(displayHelpSignal(QString)),parent,SLOT(displayHelp(QString)));

  if(m_config->unicodeMode()==true)                                //set controls from config data
    m_radioUnicode->setChecked(true);
  else
    m_radioLatin1->setChecked(true);
  m_chkSysFonts->setChecked(m_config->includeSystemFonts());
  m_chkBadFontList->setChecked(m_config->listBadFontFiles());

  m_changed=false;
}


/************************************************************************************************/
void CConfigDlg::accept(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: When the OK button is selected, this slot is activated.
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{

  m_changed=false;
                                                       //if "Unicode (UTF-8)" checkbox was changed
  if(m_config->unicodeMode()!=m_radioUnicode->isChecked())
//  if(m_config->unicodeMode()!=m_chkUnicode->isChecked())
    {
    m_changed=true;
//    m_config->setUnicodeMode(m_chkUnicode->isChecked());
    m_config->setUnicodeMode(m_radioUnicode->isChecked());
    }
                                                  //if "include system fonts" checkbox was changed
  if(m_config->includeSystemFonts()!=m_chkSysFonts->isChecked())
    {
    m_changed=true;
    m_config->setIncludeSystemFonts(m_chkSysFonts->isChecked());
    }
                                    //if "display the list of unusable fonts" checkbox was changed
  if(m_config->listBadFontFiles()!= m_chkBadFontList->isChecked())
    {
    m_changed=true;
    m_config->setListBadFontFiles(m_chkBadFontList->isChecked());
    }

  QDialog::accept();                                                             //exit the dialog
}

/************************************************************************************************/
void CConfigDlg::displayHelp(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Display the help topic for this dialogue box
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
                                                   //signal main window to display this help topic
  emit(displayHelpSignal("config.html#configDialogue"));
}
