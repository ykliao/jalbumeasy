/* --------------------------------------------------------------------------------------------
 *              aeasy_album.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Classes that encapsulate the parsed data and generate the PDF album
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
#include "aeasy_fonts.h"
#include "aeasy_album.h"


/************************************************************************************************/
void CAlbumData::reset(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  clear album data in prepartion for a new album.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  none
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_sizeSet=false;
  m_mrgSet=false;
  m_hasBorders=false;
  m_spacingSet=false;
  m_rowAlign=ROW_ALIGN_TOP;

  if(m_title!=0)
    {
    delete m_title;
    m_title=0;
    }

  m_activeDrawingPage=0;

  while(m_pages.isEmpty()==false)                                         //delete all album pages
    {
    delete m_pages.takeFirst();
    }
  CFontManager::initialise();                     //initialise the font manager for a new document
}


/************************************************************************************************/
void CAlbumData::setPageSize(double w,double h)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Set the album page size
   --------------------------------------------------------------------------------------------
    PARAMETERS:  w: width in mm
                 h: height in mm
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_width=w*DOTS_PER_MM;
  m_height=h*DOTS_PER_MM;
  m_sizeSet=true;
}


/************************************************************************************************/
void CAlbumData::setMargins(double l,double r,double t,double b,bool odd)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Set the page margins.
                Odd numbered page margins are always required and used for odd and even pages
                unless the even page margins are overriden by being set separately
   --------------------------------------------------------------------------------------------
    PARAMETERS:   l: left margin in mm
                  r: right margin in mm
                  t: top margin in mm
                  b: bottom margin in mm
                odd: true  => setting margins for odd numbered pages as well as even
                     false => setting margins for even numbered pages if it is required that
                              they be different
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  if(odd==true)                              //odd margins are the default, and must always be set
    {
    m_mrgLeft  =l*DOTS_PER_MM;
    m_mrgRight =r*DOTS_PER_MM;
    m_mrgTop   =t*DOTS_PER_MM;
    m_mrgBottom=b*DOTS_PER_MM;
    m_mrgSet   =true;

    m_mrgLeftE    = m_mrgLeft;                              //set the even margins same as the odd
    m_mrgRightE   = m_mrgRight;
    m_mrgTopE     = m_mrgTop;
    m_mrgBottomE  = m_mrgBottom;
    }
  else                                                         //override the even margin settings
    {
    m_mrgLeftE    = l*DOTS_PER_MM;
    m_mrgRightE   = r*DOTS_PER_MM;
    m_mrgTopE     = t*DOTS_PER_MM;
    m_mrgBottomE  = b*DOTS_PER_MM;
    }
}

/************************************************************************************************/
void CAlbumData::setBorder(double outer,double inner,double spacing)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Set the page border (frame)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  outer:   width of outer frame line in mm
                 inner:   width of inner frame line in mm
                 spacing: distance between the frame lines in mm
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_outerBorder=outer*DOTS_PER_MM;
  m_innerBorder=inner*DOTS_PER_MM;
  m_borderSpacing=spacing*DOTS_PER_MM;
  m_hasBorders=true;
}

/************************************************************************************************/
void CAlbumData::setSpacing(double hspace,double vspace)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: set horizontal and vertical spacing for all elements on the page
   --------------------------------------------------------------------------------------------
    PARAMETERS:  hspace: horizontal spacing
                 vspace: vertical spacing
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_hspace=hspace*DOTS_PER_MM;
  m_vspace=vspace*DOTS_PER_MM;
  m_spacingSet=true;
}

/************************************************************************************************/
void CAlbumData::setTitle(int findex,double fsize,QString title)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: set the page title for all pages in the album
   --------------------------------------------------------------------------------------------
    PARAMETERS:  findex: font index
                  fsize: font size
                  title: text to use for the title
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  if(m_title!=0)
    delete m_title;
  m_title=new CFormattedText(findex,fsize,title,true);
}

/************************************************************************************************/
void CAlbumData::setRowAlignment(ROW_ALIGN rowAlign)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: set the alignment of stamps in a row
   --------------------------------------------------------------------------------------------
    PARAMETERS:  rowAlign: top, middle or bottom row alignment
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_rowAlign=rowAlign;
}


/************************************************************************************************/
bool CAlbumData::hasRow(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: check if a row has been defined for this album
   --------------------------------------------------------------------------------------------
    PARAMETERS:  none
   --------------------------------------------------------------------------------------------
       RETURNS:  true  => album contains at least one row
                 false => album has no rows
   -------------------------------------------------------------------------------------------- */
{
  bool row=false;

  if(m_activeDrawingPage!=0)
    {
    if(m_activeDrawingPage->activeRow()!=0)
      row=true;
    }
  return row;
}


/************************************************************************************************/
void CAlbumData::startNewPage(double hspace,double vspace)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: begin a new page, optionally overriding the default page spacing
   --------------------------------------------------------------------------------------------
    PARAMETERS:  hspace: >=0 : overridden  horizontal spacing value
                          <0 : don't override horizontal spacing
                 vspace: >=0 : overridden  vertical spacing value
                          <0 : don't override vertical spacing
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_activeDrawingPage=new CAlbumPage;
  m_activeDrawingPage->setPageSpacingOverride(hspace,vspace);
  m_pages.append(m_activeDrawingPage);
}


/************************************************************************************************/
void CAlbumData::addPageTextToPage(int findex,double fsize,QString text,bool centre)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add text to the  active page
   --------------------------------------------------------------------------------------------
    PARAMETERS:  findex:  font index
                  fsize:  font size
                   text:  the text
                  centre: true  => centre the text
                          false => don't centre the text
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  if(m_activeDrawingPage!=0)
    m_activeDrawingPage->addPageText(findex,fsize,text,centre);
}

/************************************************************************************************/
void CAlbumData::addStampRowToPage(int findex,double fsize,double lineWidth,ROW_STYLE style,
                                   double spacing)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add a row containing stamps to the active page
   --------------------------------------------------------------------------------------------
    PARAMETERS:    findex: font index for text in/under the stamps
                    fsize: font size
                lineWidth: width of the frame line around the stamp (mm)
                    style: row style (type of spacing of stamps in the row)
                  spacing: space (mm) between stamps for fixed style rows
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{

  if(m_activeDrawingPage!=0)
    m_activeDrawingPage->addStampRow(findex,fsize,lineWidth,style,spacing,m_rowAlign);
}


/************************************************************************************************/
void CAlbumData::addStampToRow(STAMP_STYLE style,double width,double height,QString stampText[])
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add a stamp to the active row
   --------------------------------------------------------------------------------------------
    PARAMETERS:     style: style of stamp (shape)
                    width: width of stamp rectangle
                   height: height of stamp rectangle
                stampText: array of up to 9 strings for the text inside and under the stamp
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  if(m_activeDrawingPage!=0)
    {
    CPageStampRow *row=m_activeDrawingPage->activeRow();
    if(row!=0)
      row->addStamp(style,width,height,stampText);
    }
}


/************************************************************************************************/
bool CAlbumData::generatePdf(QString file)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Generate the pdf file from the parsed album data.
   --------------------------------------------------------------------------------------------
    PARAMETERS: file: Name of the pdf file to generate
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  int pageno=0;
                                 //ensure that the mandatory page formatting options have been set
  if(m_sizeSet==false || m_mrgSet==false || m_spacingSet==false)
    {
    error=true;
    if(m_sizeSet==false)
      displayError(tr("Page size not defined."));
    if(m_mrgSet==false)
      displayError(tr("Page margins not set."));
    if(m_spacingSet==false)
      displayError(tr("Page spacing not set."));
    }
  else if((m_pdfDoc=HPDF_New(pdfErrorHandler,NULL))==NULL)
    {
    error=true;
    displayError(tr("Unable to create the PDF object."));
    }

  if(error==false)
    {
                                  //set AlbumEasy as the application that created the PDF document
    HPDF_SetInfoAttr(m_pdfDoc,HPDF_INFO_CREATOR ,
                     (QString("AlbumEasy v")+QString::number(VER_MAJOR)+"."+
                     QString::number(VER_MINOR)+VER_REV).toLatin1()
                     );
                                           //iterate through the list of pages generating each one
    for(int i=0;i<m_pages.size() && error==false;i++)
      {
      HPDF_Page pdfPage=HPDF_AddPage(m_pdfDoc);                               //add a new pdf page
      pageno++;
      if( pdfPage==NULL)
        {
        error=true;
        displayError(tr("Memory allocation failure while adding a page."));
        }
      else
        {
        CAlbumPage *page=m_pages.at(i);                                //get the data for the page

        HPDF_Page_SetWidth(pdfPage,m_width);
        HPDF_Page_SetHeight(pdfPage,m_height);

        bool odd=((pageno%2)!=0)?true:false;

        double hspacing;
        double vspacing;
                                                            //if no page specific spacing override
        if(page->pageSpacingOverride(hspacing,vspacing)==false)
          {
          hspacing=m_hspace;                                           //use album default spacing
          vspacing=m_vspace;
          }

        double ypos=m_height;                        //initial drawing position at the top of page
                                                         //draw page background - border and title
        ypos=drawPageToPdf(page,pageno,ypos,pdfPage,error);

        double drawWidth;
        double xpos=pageHorizontalDrawArea(drawWidth,hspacing,odd);

        QList<CPageItem *> items=page->items();      //iterate through the list of items on a page
        for(int j=0;j<items.size() && error==false;j++)                        //drawing each item
          {
          CPageItem *item=items.at(j);

          if(ypos>0.0)                                //if not below bottom of page, draw the item
            ypos=item->drawToPdf(m_pdfDoc,pdfPage,error,
                                 xpos,ypos,drawWidth,m_width,hspacing,vspacing);
          if(error==true)                                   //if an error while generating the pdf
            displayError(CFontManager::getError());                            //display the error
          }
        }
      }

    if(error==false)
      {
      if(HPDF_SaveToFile(m_pdfDoc, file.toLatin1())!=HPDF_OK)                  //save the pdf file
        {
        error=true;       //if an error while saving the pdf, display an appropriate error message

        HPDF_STATUS ec=HPDF_GetError(m_pdfDoc);

        if(ec==HPDF_FILE_OPEN_ERROR || ec==HPDF_FILE_IO_ERROR)
          {
          displayError(tr("Unable to write to file %1.<br />"
                          "If you have the file open for viewing, please close the viewer.")
                          .arg(file));
          }
        else if(ec==HPDF_TTF_CANNOT_EMBEDDING_FONT || ec==HPDF_TTF_INVALID_CMAP ||
                ec==HPDF_TTF_INVALID_FOMAT || ec==HPDF_TTF_MISSING_TABLE)
          {
          displayError(tr("Error generating file %1.<br />"
                          "One of the specified TrueType fonts is not valid.")
                          .arg(file));
          }
        else
          {
          displayError(tr("Unknown error generating file %1.").arg(file));
          }

        }
      }
    HPDF_Free (m_pdfDoc);
    }
  return error;
}


/************************************************************************************************/
double CAlbumData::pageHorizontalDrawArea(double &drawWidth,double hspacing,bool odd)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: return the horizontal offset and width of the available drawing area
                on a page.
   --------------------------------------------------------------------------------------------
    PARAMETERS: drawWidth: Return the drawing area width
                 hspacing: Horizontal element spacing for this page
                      odd: true  => odd numbered page
                           false => even numbered page
   --------------------------------------------------------------------------------------------
       RETURNS:  double: xpos of the drawing area
   -------------------------------------------------------------------------------------------- */
{
  double left;
  double right;

  if(odd==true)
    {
    left=m_mrgLeft+m_borderSpacing+m_innerBorder+hspacing;
    right=m_width-m_mrgRight-m_borderSpacing-m_innerBorder-hspacing;
    }
  else
    {
    left=m_mrgLeftE+m_borderSpacing+m_innerBorder+hspacing;
    right=m_width-m_mrgRightE-m_borderSpacing-m_innerBorder-hspacing;
    }

  drawWidth=(right-left);
  return left;
}


/************************************************************************************************/
void CAlbumData::pageBorderRect(QRectF &borders,bool odd, bool inner)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the rectangle containing the page border.
   --------------------------------------------------------------------------------------------
    PARAMETERS: borders: returns the border rectangle
                    odd: true  => get odd numbered page border rectangle
                         false => get even numbered page border rectangle
                  inner: true  => rectangle of inner border
                         false => rectangle of outer border
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  if(inner==true)                                                     //get inner border rectangle
    {
    if(odd==true)
      {
      borders=QRectF(m_mrgLeft+m_borderSpacing,
                     m_mrgBottom+m_borderSpacing,
                     m_width - m_mrgLeft -m_mrgRight-2*m_borderSpacing-m_innerBorder/2,
                     m_height - m_mrgBottom -m_mrgTop-2*m_borderSpacing-m_innerBorder/2);
      }
    else
      {
      borders=QRectF(m_mrgLeftE+m_borderSpacing,
                     m_mrgBottomE+m_borderSpacing,
                     m_width - m_mrgLeftE -m_mrgRightE-2*m_borderSpacing-m_innerBorder/2,
                     m_height - m_mrgBottomE -m_mrgTopE-2*m_borderSpacing-m_innerBorder/2);
      }
    }
  else                                                                //get outer border rectangle
    {
    if(odd==true)
      {
      borders=QRectF(m_mrgLeft,
                     m_mrgBottom,
                     m_width - m_mrgLeft -m_mrgRight,
                     m_height - m_mrgBottom -m_mrgTop);
      }
    else
      {
      borders=QRectF(m_mrgLeftE,m_mrgBottomE,
                      m_width - m_mrgLeftE -m_mrgRightE,
                      m_height - m_mrgBottomE -m_mrgTopE);
      }
    }
}

/************************************************************************************************/
double CAlbumData::pageHorizontalCentre(bool odd)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the horizontal centre point of the page between the margins
   --------------------------------------------------------------------------------------------
    PARAMETERS:    odd: true  => get centre for odd numbered page
                        false => get centre for even numbered page
   --------------------------------------------------------------------------------------------
       RETURNS: double: page centre
   -------------------------------------------------------------------------------------------- */
{
  if(odd==true)
    return (m_width-m_mrgLeft-m_mrgRight)/2+m_mrgLeft;
  else
    return  (m_width-m_mrgLeftE-m_mrgRightE)/2+m_mrgLeftE;
}


/************************************************************************************************/
double CAlbumData::drawPageToPdf(CAlbumPage *page,int pageno,double ypos,HPDF_Page pdfPage,
                                 bool &error)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Draw page common items i.e border and title to the pdf page
   --------------------------------------------------------------------------------------------
    PARAMETERS:    page: page date
                 pageno: number of the page
                   ypos: vertical position to start drawing from
                pdfPage: pdf being output
                  error: flag set to true if there are any errors while generating the page
   --------------------------------------------------------------------------------------------
       RETURNS:  double: vertical position to start drawing the items on the page
   -------------------------------------------------------------------------------------------- */
{
  bool odd=((pageno%2)!=0) ? true : false;

  double hspacing;
  double vspacing;
  if(page->pageSpacingOverride(hspacing,vspacing)==false)   //if no page specific spacing override
    {
    hspacing=m_hspace;                                             //use the album default spacing
    vspacing=m_vspace;
    }

  if(m_hasBorders==true)            //if borders have been defined for the album, draw the borders
    {
    QRectF rect;
    if(m_outerBorder>0.0)                   //if the outer border line thickness is greater than 0
      {
      bool inner=false;
      pageBorderRect(rect,odd,inner);                                 //the outer border rectangle
                                                                                         //draw it
      HPDF_Page_SetLineWidth(pdfPage,m_outerBorder);
      HPDF_Page_Rectangle(pdfPage,rect.x(),rect.y(),rect.width(),rect.height());
      HPDF_Page_Stroke (pdfPage);
      }
    if(m_innerBorder>0.0)                   //if the inner border line thickness is greater than 0
      {
      bool inner=true;
      pageBorderRect(rect,odd,inner);                                 //the inner border rectangle
                                                                                         //draw it
      HPDF_Page_SetLineWidth(pdfPage,m_innerBorder);
      HPDF_Page_Rectangle(pdfPage,rect.x(),rect.y(),rect.width(),rect.height());
      HPDF_Page_Stroke (pdfPage);
      }
    }

    //to be strictly correct, pages with a double border should have the border spacing subtracted
    //from the ypos, but to keep compatible with earlier versions of AE this is not done so
  ypos=ypos-((odd==true)? m_mrgTop:m_mrgTopE);

  if(m_title!=0)                                                         //if a title has been set
    {
    HPDF_Font font=CFontManager::getFont(m_pdfDoc,m_title->findex());
    QTextCodec *qtCodec=CFontManager::getCodec(m_title->findex());

    if(font==NULL || qtCodec==NULL)
      {
      error=true;
      displayError(CFontManager::getError());
      }
    else
      {
      HPDF_Page_SetFontAndSize(pdfPage,font,m_title->fontSize());

      double pageCentre=pageHorizontalCentre(odd);

      const QList<QString> strings=m_title->strings();

                 //do not use page specific spacing override for title positioning, to ensure that
                 //the title will always be in the same position on all pages in an album
      ypos=ypos-m_vspace;

      for(int i=0;i<strings.size();i++)
        {
        ypos=ypos-m_title->fontSize();

        QString str=strings.at(i);

        double strWidth=HPDF_Page_TextWidth(pdfPage,qtCodec->fromUnicode(str));
        HPDF_Page_BeginText(pdfPage);
        HPDF_Page_TextOut(pdfPage, pageCentre-strWidth/2.0,ypos,qtCodec->fromUnicode(str));
        HPDF_Page_EndText(pdfPage);
        }
      ypos=ypos-vspacing;
      }
    }

  return ypos;
}


/************************************************************************************************/
#ifndef QT_NO_DEBUG
void CAlbumData::pdfErrorHandler(HPDF_STATUS error,HPDF_STATUS detail,void *)
#else
void CAlbumData::pdfErrorHandler(HPDF_STATUS,HPDF_STATUS,void *)
#endif
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Error handler for catching and displaying errors generated by
                the Haru library. Only functional in debug mode
   -------------------------------------------------------------------------------------------- */
{
  DEBUGS(QString("pdfErrorHandler Error:[%1] Detail:[%2]")
         .arg((HPDF_UINT)error,0,16).arg((HPDF_UINT)detail,0,16) );
}


/************************************************************************************************/
CAlbumPage::CAlbumPage(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: constructor for a new page with no items on it yet
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  setPageSpacingOverride(-1.0,-1.0);                          //default - no page spacing override
  m_activeDrawingRow=0;                                          //no stamp row has been added yet
}


/************************************************************************************************/
CAlbumPage::~CAlbumPage()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: destructor deletes all items on the page
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  while(m_items.isEmpty()==false)          //delete all items on page
    {
    delete m_items.takeFirst();
    }
}


/************************************************************************************************/
void CAlbumPage::setPageSpacingOverride(double hspace,double vspace)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Override the default album page spacing for this page
   --------------------------------------------------------------------------------------------
    PARAMETERS: hspace: horizontal spacing in mm for this page
                vspace: vertical spacing in mm for this page
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_tmpHspace=(hspace<0)?hspace:hspace*DOTS_PER_MM;
  m_tmpVspace=(vspace<0)?vspace:vspace*DOTS_PER_MM;
}


/************************************************************************************************/
bool CAlbumPage::pageSpacingOverride(double &hspace,double &vspace)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: check if the page spacing is overridden for this page
   --------------------------------------------------------------------------------------------
    PARAMETERS: hspace: returns the horizontal spacing in mm for this page
                vspace: returns the vertical spacing in mm for this page
   --------------------------------------------------------------------------------------------
       RETURNS: true  => page spacing is overridden
                false => page spacing not overridden
   -------------------------------------------------------------------------------------------- */
{
  if(m_tmpHspace>=0.0 && m_tmpVspace>=0.0)                         //if page spacing  is overriden
    {
    hspace=m_tmpHspace;
    vspace=m_tmpVspace;
    return true;
    }
  else
    return false;
}


/************************************************************************************************/
void CAlbumPage::addPageText(int findex,double fsize,QString text,bool centre)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add text to this page
   --------------------------------------------------------------------------------------------
    PARAMETERS:  findex:  font index
                  fsize:  font size
                   text:  the text
                  centre: true  => centre the text
                          false => don't centre the text
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  CPageItem *item=new CPageText(findex,fsize,text,centre);
  m_items.append(item);
}


/************************************************************************************************/
void CAlbumPage::addStampRow(int findex,double fsize,double lineWidth,ROW_STYLE style,
                             double spacing,ROW_ALIGN rowAlign)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add a row that will contain stamps to this page
   --------------------------------------------------------------------------------------------
    PARAMETERS:    findex: font index for text in/under the stamps
                    fsize: font size
                lineWidth: width of the frame line around the stamp (mm)
                    style: row style (type of spacing of stamps in the row)
                  spacing: space (mm) between stamps for fixed style rows
                  rowAlignTop: true  => align the top edges of stamps in the row
                               false => align the bottom edges stamps in the row
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  m_activeDrawingRow=new CPageStampRow(findex,fsize,lineWidth,style,spacing,rowAlign);
  m_items.append(m_activeDrawingRow);
}


/************************************************************************************************/
CPageText::CPageText(int findex,double fsize,QString text,bool centre)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: constructor for a text item on a page
   --------------------------------------------------------------------------------------------
    PARAMETERS: findex: font index
                 fsize: font size
                  text: the text
                centre: true  => centre the text
                        false => don't centre the text
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_ftext=new CFormattedText(findex,fsize,text,centre);
}



/************************************************************************************************/
CPageText::~CPageText()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: destructor for the text item on a page
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  delete m_ftext;
}


/************************************************************************************************/
double CPageText::drawToPdf(HPDF_Doc pdfDoc,HPDF_Page pdfPage,bool &error,double xpos,double ypos,
                            double drawWidth,double,double,double vspacing)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Draw the text on the PDF page
   --------------------------------------------------------------------------------------------
    PARAMETERS:    pdfDoc: PDF document being generated
                  pdfPage: PDF page in the document
                    error: error flag return
                     xpos: the horizontal position of the text
                     ypos: the vertical position  of the text
                drawWidth: the width of the drawing area
                 vspacing: the vertical spacing between items on the page
   --------------------------------------------------------------------------------------------
       RETURNS:    double: vertical position to start drawing the next items on the page
   -------------------------------------------------------------------------------------------- */
{
  HPDF_Font font=CFontManager::getFont(pdfDoc,m_ftext->findex());
  QTextCodec *qtCodec=CFontManager::getCodec(m_ftext->findex());

  if(font==NULL || qtCodec==NULL)
    {
    error=true;
    }
  else
    {
    HPDF_Page_SetFontAndSize(pdfPage,font,m_ftext->fontSize());

    const QList<QString> strings=m_ftext->strings();

    for(int i=0;i<strings.size();i++)
      {
      ypos=ypos-m_ftext->fontSize();

      QString str=strings.at(i);

      if(m_ftext->centred()==true && ypos>0.0)   //draw centred text if not below bottom of page
        {
        double strWidth=HPDF_Page_TextWidth(pdfPage,qtCodec->fromUnicode(str));

        while(strWidth>drawWidth && str.length()>0)  //reduce string width to fit the draw width
          {
          str=str.left(str.length()-1);
          strWidth=HPDF_Page_TextWidth(pdfPage,qtCodec->fromUnicode(str));
          }
        if(strWidth>0 &&  str.length()>0)
          {
          HPDF_Page_BeginText(pdfPage);
          HPDF_Page_TextOut(pdfPage,xpos+(drawWidth-strWidth)/2,ypos,qtCodec->fromUnicode(str));
          HPDF_Page_EndText(pdfPage);
          }
        }
      else if(ypos>0.0)              //draw text that is not centred if not below bottom of page
        {
        while(ypos>0.0 && str.length()>0)
          {
          QString tmpStr=str;

          double strWidth=HPDF_Page_TextWidth(pdfPage,qtCodec->fromUnicode(tmpStr));
          bool shortened=false;
                                               //reduce string width until it fits in draw width
          while(strWidth>drawWidth && tmpStr.length()>0)
            {
            tmpStr=tmpStr.left(tmpStr.length()-1);
            strWidth=HPDF_Page_TextWidth(pdfPage, qtCodec->fromUnicode(tmpStr));
            shortened=true;
            }

          if(shortened==true)
            {
                                                     //step back to the end of the previous word
            while(tmpStr.length()>0 && tmpStr.at(tmpStr.length()-1).isSpace()==false)
              {
              tmpStr.chop(1);                                        //remove the last character
              }
            }

          HPDF_Page_BeginText (pdfPage);
          HPDF_Page_TextOut (pdfPage, xpos, ypos,qtCodec->fromUnicode(tmpStr));
          HPDF_Page_EndText (pdfPage);

          if(tmpStr.length()<str.length())     //if the whole text string has not yet been drawn
            {
            str=str.right(str.length()-tmpStr.length());       //get the remainder of the string
            ypos=ypos-m_ftext->fontSize();
            }
          else
            str="";                                                //finished drawing the string

          while(str.length()>0 && str.at(0).isSpace()==true)
            {
            str=str.remove(0,1);            //remove leading spaces after the first line of text
            }

          }
        }
      }
    ypos=ypos-vspacing;                                         //set yposition for next element
    }

  return ypos;
}


/************************************************************************************************/
CPageStampRow::CPageStampRow(int findex,double fsize,double lineWidth,ROW_STYLE style,
                             double spacing,ROW_ALIGN rowAlign)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Constructor for a new row of stamps
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_findex=findex;
  m_fsize=fsize;

  m_lineWidth=lineWidth*DOTS_PER_MM;
  m_style=style;

  m_spacing=spacing*DOTS_PER_MM;
  m_rowAlign=rowAlign;

  m_maxStampHeight=0.0;                           //maximum height of all stamps added to this row
}


/************************************************************************************************/
CPageStampRow::~CPageStampRow()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Stamp row destructor deletes all stamps in the row
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  while(m_stamps.isEmpty()==false)
    {
    delete m_stamps.takeFirst();
    }
}


/************************************************************************************************/
void CPageStampRow::addStamp(STAMP_STYLE style,double width,double height,QString stampText[])
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Add a stamp to the row of stamps
   --------------------------------------------------------------------------------------------
    PARAMETERS:    style: style stamp - rectangular, triangle, etc
                   width: width of stamp
                  height: width of stamp
                stampText: arrays of text strings associated with the stamp
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
  height=height*DOTS_PER_MM;
  width=width*DOTS_PER_MM;
                                                      //get height of the tallest stamp in the row
  m_maxStampHeight=((height)>m_maxStampHeight) ? (height):m_maxStampHeight;

  CStamp *stamp=new CStamp(style,width,height,stampText);
  m_stamps.append(stamp);
}


/************************************************************************************************/
double CPageStampRow::drawToPdf(HPDF_Doc pdfDoc,HPDF_Page pdfPage,bool &error,
                               double xpos,double ypos,double drawWidth,double pageWidth,
                               double hspacing,double vspacing)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Draw the row of stamps on the PDF page
   --------------------------------------------------------------------------------------------
    PARAMETERS:    pdfDoc: PDF document being generated
                  pdfPage: PDF page in the document
                    error: error flag return
                     xpos: the horizontal position
                     ypos: the vertical position
                drawWidth: the width of the drawing area
                pageWidth: the width of the page
                 hspacing: the horizontal spacing between items on the page
                 vspacing: the vertical spacing between items on the page
   --------------------------------------------------------------------------------------------
       RETURNS:    double: vertical position to start drawing the next items on the page
   -------------------------------------------------------------------------------------------- */
{
  double rowHeight=0;
  double rowWidth=0;
  double stampSpace=0;
  int countStamps=0;;
  double sxpos=0.0;


  for(int i=0;i<m_stamps.size() ;i++)                         //iterate through the list of stamps
    {
    CStamp *stamp=m_stamps.at(i);
    rowWidth=rowWidth+stamp->width();
    countStamps++;
    }

  if(countStamps<2)                                                              //if only 1 stamp
    {
    if(m_style==ROW_STYLE_JUSTIFY)
      sxpos=xpos;                                                              //draw at left edge
    else
      sxpos=xpos+drawWidth/2-rowWidth/2;                                               //centre it
    stampSpace=hspacing;
    }
  else
    {
    bool treatAsJustify=false;
    stampSpace=hspacing;
    sxpos=xpos;

    if(m_style==ROW_STYLE_EQUAL)           //equal spacing between stamps, margins same as spacing
      {
                                                //don't take horizontal space margins into account
      double l=xpos-hspacing;
      double r=xpos+drawWidth+hspacing;
      double pw=r-l;

      if(rowWidth<pw)
        {
        stampSpace=(pw-rowWidth);
          stampSpace=stampSpace/(countStamps+1);
        }
      else
        stampSpace=hspacing;                //default to horizontal spacing if the row doesn't fit

                                                                //check position of leftmost stamp
      if(stampSpace<hspacing)               //if it starts within the left horizontal space margin
        treatAsJustify=true;                                             //format row as justified
      else
        sxpos=l+stampSpace;
      }
                                              //left edge of left stamp at left margin, right edge
                                              //of right stamp at right margin, equispaced
    if(m_style==ROW_STYLE_JUSTIFY || treatAsJustify==true)
      {
      sxpos=xpos;
      if(rowWidth<drawWidth)
        {
        stampSpace=(drawWidth-rowWidth);
          stampSpace=stampSpace/(countStamps-1);
        }
      else
        stampSpace=hspacing;                //default to horizontal spacing if the row doesn't fit
      }

    if(m_style==ROW_STYLE_FIXED)           //fixed spacing between stamps, centred between margins
      {
      stampSpace=m_spacing;
      rowWidth=rowWidth+(countStamps-1)*stampSpace;
      if(rowWidth<drawWidth)
        {
        sxpos=xpos+(drawWidth-rowWidth)/2;
        }
      else
        sxpos=xpos;
      }
    }
                                  //get the font used for drawing text inside and under the stamps
  HPDF_Font font=CFontManager::getFont(pdfDoc,m_findex);
  QTextCodec *qtCodec=CFontManager::getCodec(m_findex);

  if(font==NULL || qtCodec==NULL)
    error=true;
  else
    {
    HPDF_Page_SetFontAndSize(pdfPage, font, m_fsize);

    for(int i=0;i<m_stamps.size() ;i++)                       //iterate through the list of stamps
      {
      CStamp *stamp=m_stamps.at(i);
      if(sxpos<(xpos+pageWidth) && ypos>0.0)            //only draw stamps that  start on the page
        {
        double h=drawStamp(stamp,pdfPage,qtCodec,sxpos,ypos);            //actually draw the stamp
        rowHeight=(rowHeight>h) ?rowHeight:h;
        sxpos=sxpos+stamp->width()+stampSpace;
        }
      }
    if(rowHeight>0)
      ypos=ypos-rowHeight-vspacing;
    }

  return ypos;
}


/************************************************************************************************/
double CPageStampRow::drawStamp(CStamp *stamp,HPDF_Page pdfPage,QTextCodec *qtCodec,
                                double xpos,double ypos)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Draw a stamp
   --------------------------------------------------------------------------------------------
    PARAMETERS:   stamp: The stamp to draw
                pdfPage: PDF page in the document
                qtCodec: Codec to use for the text in the stamp
                   xpos: the horizontal position of the stamp
                   ypos: the vertical position of the stamp
   --------------------------------------------------------------------------------------------
       RETURNS:  double: the height of the stamp including the text below it
   -------------------------------------------------------------------------------------------- */
{
  double voffset=0.0;

  if(m_rowAlign==ROW_ALIGN_BOTTOM)
    voffset=m_maxStampHeight-stamp->height();
  else if(m_rowAlign==ROW_ALIGN_MIDDLE)
    voffset=(m_maxStampHeight-stamp->height())/2.0;

  HPDF_Page_SetLineWidth(pdfPage,m_lineWidth);

  if(stamp->style()==STAMP_STYLE_BLOCK || stamp->style()==STAMP_STYLE_BLOCKX)
    {
    HPDF_Page_Rectangle(pdfPage,xpos,ypos-stamp->height()-voffset,stamp->width(),stamp->height());
    HPDF_Page_Stroke (pdfPage);
    }
  else if(stamp->style()==STAMP_STYLE_TRIANGLE)
    {
    HPDF_Page_MoveTo(pdfPage,xpos,ypos-stamp->height()-voffset);             //move to bottom left
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width()/2,ypos-voffset);             //line to centre top
    HPDF_Page_Stroke(pdfPage);

    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width()/2,ypos-voffset);             //move to centre top
                                                                            //line to bottom right
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width(),ypos-stamp->height()-voffset);
    HPDF_Page_Stroke(pdfPage);
                                                                           //move  to bottom right
    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width(),ypos-stamp->height()-voffset);
    HPDF_Page_LineTo(pdfPage,xpos,ypos-stamp->height()-voffset);             //line to bottom left
    HPDF_Page_Stroke(pdfPage);
    }
  else if(stamp->style()==STAMP_STYLE_TRIANGLE_INV)
    {
    HPDF_Page_MoveTo(pdfPage,xpos,ypos-voffset);                                //move to top left
                                                                           //line to centre bottom
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width()/2,ypos-stamp->height()-voffset);
    HPDF_Page_Stroke(pdfPage);
                                                                           //move to centre bottom
    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width()/2,ypos-stamp->height()-voffset);
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width(),ypos-voffset);                //line to top right
    HPDF_Page_Stroke(pdfPage);

    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width(),ypos-voffset);            //move to top right
    HPDF_Page_LineTo(pdfPage,xpos,ypos-voffset);                                 //line to top left
    HPDF_Page_Stroke(pdfPage);
    }
  else if(stamp->style()==STAMP_STYLE_DIAMOND)
    {
    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width()/2,ypos-voffset);             //move to centre top
    HPDF_Page_LineTo(pdfPage,xpos,ypos-stamp->height()/2-voffset);           //line to centre left
    HPDF_Page_Stroke(pdfPage);

    HPDF_Page_MoveTo(pdfPage,xpos,ypos-stamp->height()/2-voffset);           //move to centre left
                                                                           //line to centre bottom
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width()/2,ypos-stamp->height()-voffset);
    HPDF_Page_Stroke(pdfPage);
                                                                          //move to centre bottom
    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width()/2,ypos-stamp->height()-voffset);
                                                                            //line to centre right
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width(),ypos-stamp->height()/2-voffset);
    HPDF_Page_Stroke(pdfPage);
                                                                           //move  to centre right
    HPDF_Page_MoveTo(pdfPage,xpos+stamp->width(),ypos-stamp->height()/2-voffset);
    HPDF_Page_LineTo(pdfPage,xpos+stamp->width()/2,ypos-voffset);             //line to centre top
    HPDF_Page_Stroke(pdfPage);
    }

  double txtHeight=0;
  double stampHeight=stamp->height();

  if(stamp->style()!=STAMP_STYLE_BLANK)                      //if not a blank stamp, draw the text
    {
    int txtRows=0;
    for(int i=0;i<3;i++)                               //up to three rows of text inside the stamp
      {
      QString str=stamp->text(i);
      if(str.length()>0)
        txtRows++;
      }
                                                           //y base position for first row of text
    double txtYpos=ypos-(stamp->height()-(txtRows*m_fsize))/2-m_fsize;
    for(int i=0;i<3;i++)
      {
      QString str=stamp->text(i);
      if(str.length()>0)
        {
        double swidth=HPDF_Page_TextWidth(pdfPage, qtCodec->fromUnicode(str));
        double txtXpos=xpos+(stamp->width()-swidth)/2;

        HPDF_Page_BeginText(pdfPage);
        HPDF_Page_TextOut(pdfPage, txtXpos, txtYpos-voffset, qtCodec->fromUnicode(str));
        HPDF_Page_EndText(pdfPage);
        txtYpos=txtYpos-m_fsize;
        }
      }
    for(int i=3;i<6;i++)                       //draw the first row of three items under the stamp
      {
      QString str=stamp->text(i);
      if(str.length()>0)
        {
        txtHeight=m_fsize+2;                           //leave a 2mm space under stamp before text

        double swidth=HPDF_Page_TextWidth(pdfPage,qtCodec->fromUnicode(str));
        double txtXpos;

        if(i==3)                                                    //left text string under stamp
          txtXpos=xpos;
        else if(i==4)                                             //centre text string under stamp
          txtXpos=xpos+(stamp->width()-swidth)/2;
        else                                                       //right text string under stamp
          txtXpos=xpos+stamp->width()-swidth;

        HPDF_Page_BeginText(pdfPage);
        HPDF_Page_TextOut(pdfPage,txtXpos,
                          ypos-stampHeight-txtHeight-voffset,qtCodec->fromUnicode(str));
        HPDF_Page_EndText(pdfPage);
        }
      }

    stampHeight=stampHeight+txtHeight;
    txtHeight=0;

    for(int i=6;i<9;i++)                      //draw the second row of three items under the stamp
      {
      QString str=stamp->text(i);
      if(str.length()>0)
        {
        txtHeight=m_fsize+1;                        //leave a 1mm space under previous row of text
        double swidth=HPDF_Page_TextWidth(pdfPage, qtCodec->fromUnicode(str));
        double txtXpos;

        if(i==6)                                                    //left text string under stamp
          txtXpos=xpos;
        else if(i==7)                                             //centre text string under stamp
          txtXpos=xpos+(stamp->width()-swidth)/2;
        else                                                       //right text string under stamp
          txtXpos=xpos+stamp->width()-swidth;

        HPDF_Page_BeginText(pdfPage);
        HPDF_Page_TextOut(pdfPage,txtXpos,
                          ypos-stampHeight-txtHeight-voffset,qtCodec->fromUnicode(str));
        HPDF_Page_EndText(pdfPage);
        }
      }
    }
  return  stampHeight+txtHeight+voffset ;
}


/************************************************************************************************/
CStamp::CStamp(STAMP_STYLE style,double width,double height,QString stampText[])
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: constructor for a new stamp
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_style=style;
  m_width=width;
  m_height=height;
  for(int i=0;i<9;i++)                       //a stamp can have up to 9 strings associated with it
    {
    m_text[i]=stampText[i];
    }
}


/************************************************************************************************/
CFormattedText::CFormattedText(int findex,double fsize,QString text,bool centre,
                               bool parseNewLines)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: constructor for a formatted text object - an object containing multiline text
                that has had escape characters substitued along with the font information and
                font size for the text.
   --------------------------------------------------------------------------------------------
    PARAMETERS:        findex: font index
                        fsize: font size
                         text: the text to be formattted
                       centre: true => centre the text on the page
                parseNewLines: true => split the text at new-line chars into multiple lines
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  m_findex=findex;
  m_fsize=fsize;
  m_centred=centre;

  QString buf="";
  QChar prevc;
  prevc=QChar::Null;

  for(int i=0;i<text.size();i++)
    {
    QChar c=text.at(i);
    if(prevc=='\\')                            //if the previous character was an escape character
      {
      if(c=='n' && parseNewLines==true)                              //if an escaped new-line char
        {
                                                                  //clean any trailing white space
        while(buf.length()>0 && buf.at(buf.length()-1).isSpace()==true)
          buf.chop(1);

        m_text.append(buf);                                                //save the line of text
        buf="";                                                                 //start a new line
        }
      else
        buf=buf+c;                //just keep the char - this will also keep escaped \ and " chars
      prevc=QChar::Null;                //set so that the previous character is not an escape char
      }
    else
      {
      prevc=c;                                                        //set the previous character
      if(c!='\\')
        buf=buf+c;                                                            //keep the character
      }
    }

  while(buf.length()>0 && buf.at(buf.length()-1).isSpace()==true) //clean any trailing white space
    buf.chop(1);
  m_text.append(buf);                                                      //save the line of text
}

