/* --------------------------------------------------------------------------------------------
 *              aeasy_album.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Classes for encapsulating the album data and generate the PDF document
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

#ifndef _AEASY_ALBUM_H_
#define _AEASY_ALBUM_H_

#include "AlbumEasy.h"
#include <hpdf.h>


#define DOTS_PER_MM (72.0/25.4)                                                            //72dpi

enum ROW_STYLE
  {
  ROW_STYLE_EQUAL,
  ROW_STYLE_JUSTIFY,
  ROW_STYLE_FIXED
  };


enum ROW_ALIGN
  {
  ROW_ALIGN_TOP,
  ROW_ALIGN_MIDDLE,
  ROW_ALIGN_BOTTOM
  };

enum STAMP_STYLE
  {
  STAMP_STYLE_BLANK,
  STAMP_STYLE_BLOCK,
  STAMP_STYLE_BLOCKX,
  STAMP_STYLE_TRIANGLE,
  STAMP_STYLE_TRIANGLE_INV,
  STAMP_STYLE_DIAMOND
  };



class CAlbumPage;
class CPageItem;
class CPageStampRow;
class CStamp;
class CFormattedText;


/************************************************************************************************
CAlbumData: class that generates the album PDF from the parsed album data
************************************************************************************************/

class CAlbumData: public QObject
{
  Q_OBJECT
public:
  CAlbumData(void);
  ~CAlbumData();
  void reset(void);
  bool generatePdf(QString file);
  void setPageSize(double w,double h);
  void setMargins(double l,double r,double t,double b,bool odd);
  void setBorder(double outer,double inner,double spacing);
  void setSpacing(double hspace,double vspace);
  void setTitle(int findex,double fsize,QString title);
  void setRowAlignment(ROW_ALIGN rowAlign);
  bool hasPage(void);
  bool hasRow(void);
  void startNewPage(double hspace=-1.0,double vspace=-1.0);
  void addPageTextToPage(int findex,double fsize,QString text,bool centre);
  void addStampRowToPage(int findex,double fsize,double lineWidth,ROW_STYLE style,double spacing);
  void addStampToRow(STAMP_STYLE style,double width,double height,QString stampText[]);
  static void pdfErrorHandler(HPDF_STATUS error,HPDF_STATUS detail,void *user);
signals:
  void logMessage(QString text,QString colour="",bool bold=false);
private:
  void displayError(QString msg);
  void pageBorderRect(QRectF &borders, bool odd, bool inner);
  double pageHorizontalDrawArea(double &drawWidth,double hspace,bool odd);
  double drawPageToPdf(CAlbumPage *page,int pageno,double ypos,HPDF_Page pdfPage,bool &error);
  double pageHorizontalCentre(bool odd);
private:
  bool m_sizeSet;
  double m_width;
  double m_height;
  bool m_mrgSet;
  double m_mrgLeft;
  double m_mrgRight;
  double m_mrgTop;
  double m_mrgBottom;
  double m_mrgLeftE;
  double m_mrgRightE;
  double m_mrgTopE;
  double m_mrgBottomE;
  bool m_hasBorders;
  double m_outerBorder;
  double m_innerBorder;
  double m_borderSpacing;
  bool m_spacingSet;
  double m_hspace;
  double m_vspace;
  ROW_ALIGN m_rowAlign;

  CFormattedText *m_title;
  HPDF_Doc m_pdfDoc;
  QList<CAlbumPage *> m_pages;
  CAlbumPage *m_activeDrawingPage;

};

inline CAlbumData::CAlbumData(void)
{
  m_title=0; reset();
}

inline CAlbumData::~CAlbumData()
{
  reset();
}

inline bool CAlbumData::hasPage(void)
{
  return (m_activeDrawingPage==0)?false:true;
}

inline void CAlbumData::displayError(QString msg)
{
  emit(logMessage(tr("<b>PDF generation error: </b> %1").arg(msg),"red",false));
}

/************************************************************************************************
CAlbumPage: class containing the data for generating an album page
************************************************************************************************/

 class CAlbumPage
{
public:
  CAlbumPage(void);
  ~CAlbumPage();
  void setPageSpacingOverride(double hspace,double vspace);
  bool pageSpacingOverride(double &hspace,double &vspace);
  void addPageText(int findex,double fsize,QString text,bool centre);
  void addStampRow(int findex,double fsize,double lineWidth,ROW_STYLE style,
                  double spacing,ROW_ALIGN rowAlign);
  QList<CPageItem *> items(void);
  CPageStampRow *activeRow(void);
private:
  double m_tmpHspace;
  double m_tmpVspace;

  QList<CPageItem *> m_items;

  CPageStampRow *m_activeDrawingRow;

};

inline QList<CPageItem *> CAlbumPage::items(void)
{
  return m_items;
}

inline CPageStampRow *CAlbumPage::activeRow(void)
{
  return m_activeDrawingRow;
}

/************************************************************************************************
Classes containing the data for generating the items on an album page

CPageItem:     virtual base class for items on the page
CPageText:     text item
CPageStampRow: item for generating a row of stamps
************************************************************************************************/

class CPageItem
{
public:
  CPageItem(){;};
  virtual ~CPageItem(){;};
  virtual double drawToPdf(HPDF_Doc pdfDoc,HPDF_Page pdfPage,bool &error,double xpos,double ypos,
                           double drawWidth,double pageWidth,double hspacing,double vspacing)=0;
};


class CPageText: public CPageItem
{
public:
  CPageText(int findex,double fsize,QString text,bool centre);
  virtual ~CPageText();
  virtual double drawToPdf(HPDF_Doc pdfDoc,HPDF_Page pdfPage,bool &error,double xpos,double ypos,
                           double drawWidth,double pageWidth,double hspacing,double vspacing);
private:
  CFormattedText *m_ftext;
};


class CPageStampRow: public CPageItem
{
public:
  CPageStampRow(int findex,double fsize,double lineWidth,ROW_STYLE style,double spacing,
                ROW_ALIGN rowAlign);
  virtual ~CPageStampRow();
  void addStamp(STAMP_STYLE style,double width,double height,QString stampText[]);
  virtual double drawToPdf(HPDF_Doc pdfDoc,HPDF_Page pdfPage,bool &error,double xpos,double ypos,
                           double drawWidth,double pageWidth,double hspacing,double vspacing);
  double drawStamp(CStamp *stamp,HPDF_Page pdfPage,QTextCodec *qtCodec,double xpos,double ypos);
private:
  int m_findex;
  double m_fsize;
  double m_lineWidth;
  ROW_STYLE m_style;
  double m_spacing;
  ROW_ALIGN m_rowAlign;
  double m_maxStampHeight;
  QList<CStamp *> m_stamps;
};


/************************************************************************************************
CStamp: class containing the data for generating an individual stamp
************************************************************************************************/

class CStamp
{
public:
  CStamp(STAMP_STYLE style,double width,double height,QString stampText[]);
  double width(void);
  double height(void);
  STAMP_STYLE style(void);
  QString text(int index);
public:
  STAMP_STYLE m_style;
  double m_width;
  double m_height;
  QString m_text[9];
};

inline double CStamp::width(void)
{
  return m_width;
}

inline double CStamp::height(void)
{
  return m_height;
}

inline STAMP_STYLE CStamp::style(void)
{
  return m_style;
}

inline QString CStamp::text(int index)
{
  return (index>=0 && index<9) ?m_text[index] : QString("");
}


/************************************************************************************************
CFormattedText: Class containing multiline text that has had escape characters substitued.
                It also contains the font information used for drawing the text.
************************************************************************************************/

class CFormattedText
{
public:
  CFormattedText(int findex,double fsize,QString text,bool centre=false,bool parseNewLines=true);
  ~CFormattedText();
  bool centred(void);
  int findex(void);
  const QList<QString> strings(void);
  double fontSize(void);
private:
  int m_findex;
  double m_fsize;
  bool m_centred;
  QList<QString> m_text;
};


inline CFormattedText::~CFormattedText()
{
  m_text.clear();
}

inline bool CFormattedText::centred(void)
{
  return m_centred;
}

inline int CFormattedText::findex(void)
{
  return m_findex;
}

inline const QList<QString> CFormattedText::strings(void)
{
  return m_text;
}

inline double CFormattedText::fontSize(void)
{
  return m_fsize;
}


#endif // _AEASY_ALBUM_H_

