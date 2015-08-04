/* --------------------------------------------------------------------------------------------
 *              aeasy_config.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Class declarations for configuration settings and configuration dialogue box.
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

#ifndef _AEASY_CONFIG_H_
#define _AEASY_CONFIG_H_

#include "AlbumEasy.h"

class CMainWindow;

/************************************************************************************************
 Application Configuration and Settings
 ************************************************************************************************/
class CConfig
{
public:
  CConfig(void);
  ~CConfig();
  void loadWindowLayouts(QWidget *parent,
                         QRect &mainWndRect,QRect &helpWindowRect,QRect &fontListWindowRect);
  void saveSettings(QWidget *parent);
  bool unicodeMode(void);
  void setUnicodeMode(bool unicodeMode);
  bool includeSystemFonts(void);
  void setIncludeSystemFonts(bool includeSystemFonts);
  bool listBadFontFiles(void);
  void setListBadFontFiles(bool listBadFontFiles);
  QString workDir(void);
  void setWorkDir(QString workDir);
private:
  void constrainRectToScreen(QRect &rect,int minWidth,int minHeight);

  QSettings *m_settings;
  QString m_workDir;
  bool  m_unicodeMode;
  bool  m_includeSystemFonts;
  bool  m_listBadFontFiles;
};

inline bool CConfig::unicodeMode(void)
{
  return m_unicodeMode;
}

inline void CConfig::setUnicodeMode(bool unicodeMode)
{
  m_unicodeMode=unicodeMode;
}


inline bool CConfig::includeSystemFonts(void)
{
  return m_includeSystemFonts;
}

inline void CConfig::setIncludeSystemFonts(bool includeSystemFonts)
{
  m_includeSystemFonts=includeSystemFonts;
}

inline bool CConfig::listBadFontFiles(void)
{
  return m_listBadFontFiles;
}

inline void CConfig::setListBadFontFiles(bool listBadFontFiles)
{
  m_listBadFontFiles=listBadFontFiles;
}

inline QString CConfig::workDir(void)
{
  return m_workDir;
}
inline void CConfig::setWorkDir(QString workDir)
{
  m_workDir=workDir;
}


/************************************************************************************************
Configuration dialogue box
************************************************************************************************/
class CConfigDlg : public QDialog
{
  Q_OBJECT

public:
  CConfigDlg(CConfig *config,CMainWindow *parent);
  bool changed(void);
private slots:
  void displayHelp(void);
  void accept(void);
signals:
  void displayHelpSignal(QString page);
private:
  CConfig *m_config;
  bool m_changed;
  QRadioButton  *m_radioLatin1;
  QRadioButton  *m_radioUnicode;
  QCheckBox     *m_chkSysFonts;
  QCheckBox     *m_chkBadFontList;
  QPushButton   *m_btnHelp;
  QPushButton   *m_btnOk;
  QPushButton   *m_btnCancel;
};


inline bool CConfigDlg::changed(void)
{
  return m_changed;
}

#endif // _AEASY_CONFIG_H_





