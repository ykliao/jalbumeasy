/* --------------------------------------------------------------------------------------------
 *              aeasy_parse.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Class for parsing the album source file
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
#include "aeasy_fonts.h"
#include "aeasy_album.h"
#include "aeasy_config.h"
#include "aeasy_parse.h"

/* ---------------------------------------------------------------------------------------------
/  Arrray mapping commands to associated processing functions
/  ---------------------------------------------------------------------------------------------*/

typedef bool (CParser::*COMMAND_PROCESSOR_FUNC)(QString cmnd,QString parms);

struct COMMAND_MAP
{
  QString command;
  COMMAND_PROCESSOR_FUNC proc;
};

COMMAND_MAP commandMap[]=
{
  {"ALBUM_PAGES_SIZE",       &CParser::processPageSizeCommand},
  {"ALBUM_PAGES_MARGINS",    &CParser::processPageMarginsCommand},
  {"ALBUM_PAGES_MARGINSE",   &CParser::processPageMarginsCommand},
  {"ALBUM_PAGES_BORDER",     &CParser::processPageBorderCommand},
  {"ALBUM_PAGES_SPACING",    &CParser::processPageSpacingCommand},
  {"ALBUM_PAGES_TITLE",      &CParser::processPageTitleCommand},
  {"ALBUM_DEFINE_FONT",      &CParser::processDefineFontCommand},
  {"PAGE_START",             &CParser::processPageStartCommand},
  {"PAGE_START_VAR",         &CParser::processPageStartCommand},
  {"PAGE_TEXT",              &CParser::processPageTextCommand},
  {"PAGE_TEXT_CENTRE",       &CParser::processPageTextCommand},
  {"ROW_ALIGN_TOP",          &CParser::processRowAlignCommand},
  {"ROW_ALIGN_MIDDLE",       &CParser::processRowAlignCommand},
  {"ROW_ALIGN_BOTTOM",       &CParser::processRowAlignCommand},
  {"ROW_START_ES",           &CParser::processRowStartCommand},
  {"ROW_START_JS",           &CParser::processRowStartCommand},
  {"ROW_START_FS",           &CParser::processRowStartCommand},
  {"STAMP_ADD",              &CParser::processStampAddCommand},
  {"STAMP_ADDX",             &CParser::processStampAddCommand},
  {"STAMP_ADD_BLANK",        &CParser::processStampAddCommand},
  {"STAMP_ADD_TRIANGLE",     &CParser::processStampAddCommand},
  {"STAMP_ADD_TRIANGLE_INV", &CParser::processStampAddCommand},
  {"STAMP_ADD_DIAMOND",      &CParser::processStampAddCommand}
};


/************************************************************************************************/
bool CParser::parseFile(QFile *file,CAlbumData *album,QWidget *parent,CConfig *config,
                        CFontFileList *fontFiles)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Parse an album source file
   --------------------------------------------------------------------------------------------
    PARAMETERS:  file:     The text file to parse
                album:     The object which will receive that parsed album data
                parent:    Parent widget
                config:    Application configuration
                fontFiles: List of usable font files
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  QTextStream stream(file);

  if(config->unicodeMode()==true)             //in Unicode mode the input file is treated as UTF-8
    stream.setCodec("UTF-8");
  else                                   //else input file is treated as Latin 1 (actually CP1252)
    stream.setCodec("Windows-1252");

  QString line=stream.readLine();

  m_albumData=album;

  m_parent=parent;
  m_config=config;
  m_fontFiles=fontFiles;

  m_albumData->reset();
  m_currentLine=0;
  while(!line.isNull() && error==false)                              //read the file, line by line
    {
    m_currentLine++;

    cleanLine(line);                  //remove comments as well as leading and trailing whitespace
    if(!line.isEmpty())
      {
      while(line.endsWith("\"\\") && error==false)      //if a line ends with "\ then it continues
        {
        QString cline=stream.readLine();
        m_currentLine++;
        cleanLine(cline);

        if(!cline.startsWith("\""))                         //the continuation must start with a "
          error=true;
        else
          {
          line=line.left(line.size()-2);                    //remove the "\ at the end of the line
          cline=cline.right(cline.size()-1);        //remove the " from beginning of the next line
          line=line+cline;                                                           //concatenate
          }
        }
      if(error==true)
        {
        displayError(m_currentLine,tr("Bad line continuation."));
        }
      else
        {
        QString cmnd;
        QString parms;

        if((error=splitLine(line,cmnd,parms))==false)     //split line into command and parameters
          error=processCommand(cmnd,parms);                                  //process the command
                                                                                                                            }
      }
    line=stream.readLine();                              //read the next line from the source file
    }
  return error;
}



/************************************************************************************************/
void CParser::cleanLine(QString &buf)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Remove comments, leading and trailing whitespace and simplfy internal
                white space i.e replace sequences of spaces and tabs with a single space
   --------------------------------------------------------------------------------------------
    PARAMETERS: buf: The string to clean
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  buf=buf.trimmed();                                    //removed leading and trailing white space

  bool inString=false;
  bool inWhiteSpace=false;

  QChar prevc=QChar::Null;

  for(int i=0;i<buf.size();i++)                                                  //remove comments
    {
    QChar c=buf.at(i);

    if(c=='"' && inString==false)
      inString=true;
    else if(c=='"' && inString==true)
      {
      if(prevc!='\\')              //if the quote char wasn't escaped, it is the end of the string
        inString=false;
      }
    else if(c=='#' && inString==false)               //if a start of a comment and not in a string
      {
      buf=buf.left(i);                                               //remove the rest of the line
      break;
      }
    prevc=c;                                                          //set the previous character
    }
                                 //remove any remaining trailing white space after comment removal
  buf=buf.trimmed();

  inString=false;
  inWhiteSpace=false;

  QString tbuf=buf;
  buf="";

  prevc=QChar::Null;

  for(int i=0;i<tbuf.size();i++)                                   //simplify internal white space
    {
    QChar c=tbuf.at(i);

    if(c=='"' && inString==false)
      inString=true;
    else if(c=='"' && inString==true)
      {
      if(prevc!='\\')              //if the quote char wasn't escaped, it is the end of the string
        inString=false;
      }

    if((c==' ' || c=='\t') && inString==false)             //if a space or tab and not in a string
      {
      if(inWhiteSpace==false)
        {
        inWhiteSpace=true;
        buf=buf+' ';
        }
      }
    else                                                               //else retain the character
      {
      inWhiteSpace=false;
      buf=buf+c;
      }
    prevc=c;                                                          //set the previous character
    }

}

/************************************************************************************************/
bool CParser::splitLine(QString line,QString &cmnd,QString &parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Split the line of source text into the command and parameters. Check and
                remove parenthesis.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  line: Line containing the source text
                 cmnd: The returned command
                parms: The returned parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  cmnd=line;
  parms="";

  QChar prevc=QChar::Null;
  bool inString=false;
  bool foundParams=false;

  for(int i=0;i<line.size();i++)                                        //find opening parenthesis
    {
    QChar c=line.at(i);

    if(c=='"' && inString==false)
      inString=true;
    else if(c=='"' && inString==true)
      {
      if(prevc!='\\')              //if the quote char wasn't escaped, it is the end of the string
        inString=false;
      }
    else if( (c==' ' || c=='(') && inString==false)            //if a space or opening parenthesis
      {
      cmnd=line.left(i);                                       //command is first part of the line
      foundParams=true;
                                        //if char is a space next char must be opening parenthesis
      if(c==' ' && i<(line.size()-1) && line.at(i+1)=='(')
        i++;
      else if(c!='(')
        error=true;

      if(error==false && line.size()>i)                          //remove the leading  parenthesis
        parms=line.right(line.size()-i-1);
      else
        {
        error=true;
        displayError(m_currentLine,
                     tr("%1 command - Parameters require opening parenthesis.").arg(cmnd));
        }
      break;
      }
    prevc=c;                                                          //set the previous character
    }

  if(error==false && foundParams==true)                                 //find closing parenthesis
    {
    prevc=QChar::Null;

    inString=false;
    error=true;                        //indicate that closing parenthesis have not yet been found
    for(int i=0;i<parms.size();i++)
      {
      QChar c=parms.at(i);

      if(c=='"' && inString==false)
        inString=true;
      else if(c=='"' && inString==true)
        {
        if(prevc!='\\')            //if the quote char wasn't escaped, it is the end of the string
          inString=false;
        }
      else if(c==')' && inString==false)            //if a closing parenthesis and not in a string
        {
        error=false;                                         //indicate found trailing parenthesis
        parms=parms.left(i);                                                           //remove it
        break;
        }
      prevc=c;
      }
    if(error==true)
      {
      displayError(m_currentLine,
                   tr("%1 command - Parameters require closing parenthesis.").arg(cmnd));
      }
    }

  cmnd=cmnd.trimmed();
  parms=parms.trimmed();

  return error;
}


/************************************************************************************************/
bool CParser::processCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process a command
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  COMMAND_PROCESSOR_FUNC func=0;
  cmnd=cmnd.toUpper();                                                   //ignore the command case

                                                //find the command in the array of mapped commands
  for(unsigned int i=0; i<(sizeof(commandMap)/sizeof(commandMap[0])) && func==0;i++)
    {
    if(cmnd==commandMap[i].command)
      func=commandMap[i].proc;
    }
  if(func==0)                                                       //if the command was not found
    {
    error=true;
    displayError(m_currentLine,tr("Unrecognised Command."));
    }
  else
    error=(this->*func)(cmnd,parms);               //call the command specific processing function

  return error;
}



/************************************************************************************************/
bool CParser::parseBlankParameters(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Check that the parameter string is empty outputting an error message if not.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  if(parms.size()>0)
    {
    displayError(m_currentLine,tr("%1 command - Does not require parameters.").arg(cmnd));
    error=true;
    }

  return error;
}

/************************************************************************************************/
void CParser::extractParameterField(QString &parms,QString &field,bool toUpper)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Extract the the first field from the parameters string returning the
                remainder of the parameters string unaffected.
   --------------------------------------------------------------------------------------------
    PARAMETERS:   parms: The pre-cleaned string containing parameters seperated by a single space.
                  field: The string to return the field in
                toUpper: true  => convert the extracted field to upper case
                         false => don't change the case of the extracted field
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  bool inString=false;
  bool space=false;
  field="";
  QChar prevc=QChar::Null;

  int idx;
  for(idx=0;idx<parms.size();idx++)                            //find space - parameter delineator
    {
    QChar c=parms.at(idx);

    if(c=='"' && inString==false)
      inString=true;
    else if(c=='"' && inString==true)
      {
      if(prevc!='\\')              //if the quote char wasn't escaped, it is the end of the string
        inString=false;
      }
    else if(c==' ' && inString==false)                            //if a space and not in a string
      {
      space=true;
      break;
      }
    prevc=c;                                                          //set the previous character
    }
  idx=(space==false)?parms.size():idx;  //no space found, use remainder of the string as the field

  field=parms.left(idx);                                                           //get the field

  idx=(space==true)?idx+1:idx;     //remove field plus delineator space from the parameters string
  if(parms.size()>=idx)
    parms=parms.right(parms.size()-idx);

  if(toUpper==true)                                            //convert to uppercase if requested
    field=field.toUpper();
}


/************************************************************************************************/
int CParser::parseDoubleParameters(QString cmnd,QString &parms,int count,double *vals,
                                   bool displayErr)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Extract the requested number of double values from the start of the
                parameter string returning the remainder of the string unaffected.
   --------------------------------------------------------------------------------------------
    PARAMETERS:       cmnd: Command being processed
                     parms: Pre-cleaned string containing parameters seperated by a single space.
                     count: Number of values to extract
                      vals: Array large enough to hold the requested number of values
                displayErr: true  => display an error message if incorrect number of parameters
                            false => don't display the error message
   --------------------------------------------------------------------------------------------
       RETURNS: number of values returned
   -------------------------------------------------------------------------------------------- */
{
  int found=0;

  for(int i=0;i<count && parms.size()>0;i++)
    {
    QString val;
    extractParameterField(parms,val);
    if(val.size()>0)
      {
      *(vals+found)=val.toDouble();                                         //get the double value
      found++;
      }
    }
  if(found!=count && displayErr==true)
    {
    displayError(m_currentLine,
                 tr("%1 command - Incorrect number or format of parameters.").arg(cmnd));
    }
  return found;
}



/************************************************************************************************/
bool CParser::parseTextField(QString cmnd,QString &parms,QString &text)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: The field to be extracted from the parameters string is
                quoted text e.g. "this is text". This is returned sans quotes. The remainder
                of the parameters string is returned unaffected.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command being processed
                parms: The pre-cleaned string containing parameters seperated by a single space
                text: The string to return the text in
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  extractParameterField(parms,text);

  bool error=true;

  if(text.size()>1)                  //string must be at least 2 chars long - start and end quotes
    {
    if(text.startsWith("\"") && text.endsWith("\""))                       //if the text is quoted
      {
      text=text.left(text.size()-1);                                           //remove the quotes
      text=text.right(text.size()-1);

      error=false;
      }
    }

  if(error==true)
    {
    displayError(m_currentLine,tr("%1 command - Quoted text expected.").arg(cmnd));
    }

  return error;
}


/************************************************************************************************/
bool CParser::parseFontParameters(QString cmnd,QString &parms,int &findex,double &size)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Extract the font information from the start of the parameter string,
                the remainder of the parameter string is returned unaffected.
                The font information is contained in two parameters:
                The FontID and size e.g. "TB 16"
   --------------------------------------------------------------------------------------------
    PARAMETERS:   cmnd: Command being processed
                 parms: The pre-cleaned string containing parameters seperated by a single space.
                findex: return the index into font mapping table for the parsed font
                  size: return the font point size
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  findex=-1;                                                                   //no font found yet
  size=0;

  QString font;
  extractParameterField(parms,font);

  if(font.size()>0)
    {
    findex=CFontManager::getFontIndex(font);
    }
  if(findex<0)                                                           //matching font not found
    {
    error=true;
    if(font.size()>0)
      displayError(m_currentLine,
                   tr("%1 command - Specified font \"%2\" not found.").arg(cmnd).arg(font));
    else
      displayError(m_currentLine,tr("%1 command - No font provided.").arg(cmnd));
    }
  else
    {
    double vals[1];

    int n=parseDoubleParameters(cmnd,parms,1,vals,false);

    if(n==1 && vals[0]>0.0)                                                 //if a font point size
      size=vals[0];
    else
      {
      error=true;
      displayError(m_currentLine,tr("%1 command - Font size not specified.").arg(cmnd));
      }
    }
  return error;
}


/************************************************************************************************/
bool CParser::pageRequired(QString cmnd)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Some commands are only valid once a page has been declared.
                This function checks if a pages has been declared and issues an error message
                if not.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command being processed
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  if(m_albumData->hasPage()==false)
    {
    error=true;
    displayError(m_currentLine,
                 tr("%1 command - Attempting to add a row before a page has been "
                    "declared.").arg(cmnd));
    }
  return error;
}


/************************************************************************************************/
bool CParser::rowRequired(QString cmnd)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Stamps can only be added once a row has been declared.
                This function checks that there is a row, issuing an error message if not.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command being processed
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  if(m_albumData->hasRow()==false)
    {
    error=true;
    displayError(m_currentLine,
                 tr("%1 command - Attempting to add a stamp before a stamp row has been "
                 "declared.").arg(cmnd));
    }
  return error;
}


/************************************************************************************************/
bool CParser::processPageSizeCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process command:
               ALBUM_PAGES_SIZE (width height)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  double vals[2];
  int n=parseDoubleParameters(cmnd,parms,2,vals);
  if(n!=2)
    error=true;
  else
    {
    m_albumData->setPageSize(vals[0],vals[1]);          //set the album page size - width & height
    }

  return error;
}



/************************************************************************************************/
bool CParser::processPageMarginsCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands:
               ALBUM_PAGES_MARGINS (left right top bottom)
               ALBUM_PAGES_MARGINSE (left right top bottom)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  double vals[4];
  int n=parseDoubleParameters(cmnd,parms,4,vals);
  if(n!=4)
    error=true;
  else
    {
    bool odd=true;
    if(cmnd=="ALBUM_PAGES_MARGINSE")             //if setting the margins for even numbered  pages
      odd=false;
    m_albumData->setMargins(vals[0],vals[1],vals[2],vals[3],odd);     //set the album page margins
    }

  return error;
}

/************************************************************************************************/
bool CParser::processPageBorderCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process command:
               ALBUM_PAGES_BORDER (outer inner spacing)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  double vals[3];
  int n=parseDoubleParameters(cmnd,parms,3,vals);
  if(n!=3)
    error=true;
  else
    {
    m_albumData->setBorder(vals[0],vals[1],vals[2]);                   //set the album page border
    }

  return error;
}

/************************************************************************************************/
bool CParser::processPageSpacingCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process command:
               ALBUM_PAGES_SPACING (horizontal vertical)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  double vals[2];
  int n=parseDoubleParameters(cmnd,parms,2,vals);
  if(n!=2)
    error=true;
  else
    {
    m_albumData->setSpacing(vals[0],vals[1]);     //set album page horizontal and vertical spacing
    }

  return error;
}

/************************************************************************************************/
bool CParser::processPageTitleCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process command:
                ALBUM_PAGES_TITLE (Font Fontsize Title)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  int findex;
  double size;

  if((error=parseFontParameters(cmnd,parms,findex,size))==false)              //get the title font
    {
    QString title;
    if((error=parseTextField(cmnd,parms,title))==false)                       //get the title text
      m_albumData->setTitle(findex,size,title);
    }

  return error;
}

/************************************************************************************************/
bool CParser::processDefineFontCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process command:
                ALBUM_DEFINE_FONT (fontId "fontName")
                ALBUM_DEFINE_FONT (fontId "fontName" Encoding)
                                  optional third parameter specifying the encoding.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  QString fontId;
  QString fontName;
  QString fontEncoding;
                                                                      //get up to three parameters
  extractParameterField(parms,fontId,true);
  if((error=parseTextField(cmnd,parms,fontName))==false)    //name can contain spaces => in quotes
    {
    extractParameterField(parms,fontEncoding);

    if(fontId.size()==0 || fontName.size()==0)                //the first two fields are mandatory
      {
      error=true;
      displayError(m_currentLine,
                   tr("%1 command - Incorrect number or format of parameters.").arg(cmnd));
      }
    else if(fontId.size()<2 || fontId.size()>8)
      {
      error=true;
      displayError(m_currentLine,
                  tr("%1 command - User defined font identifiers must be 2 to 8 chars long.")
                  .arg(cmnd));
      }
    else
      {
                                                                //populate list of available fonts
      m_fontFiles->populate(m_parent,m_config->includeSystemFonts());

      if((error=CFontManager::addUserDefinedFont(m_fontFiles,fontId,fontName,fontEncoding)==true))
        {
        displayError(m_currentLine,tr("%1 command - %2").arg(cmnd).arg(CFontManager::getError()));
        }
      }
    }
  return error;
}


/************************************************************************************************/
bool CParser::processPageStartCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands:
               PAGE_START
               PAGE_START_VAR (horizontal vertical)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  if(cmnd=="PAGE_START")
    {
    if((error=parseBlankParameters(cmnd,parms))==false)
      m_albumData->startNewPage();
    }
  else //PAGE_START_VAR
    {
    double vals[2];
    int n=parseDoubleParameters(cmnd,parms,2,vals);
    if(n!=2)
      error=true;
    else
      {
                               //start new page with the specified horizontal and vertical spacing
      m_albumData->startNewPage(vals[0],vals[1]);
      }
    }
  return error;
}

/************************************************************************************************/
bool CParser::processPageTextCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands for adding text to the page:
                PAGE_TEXT (Font Fontsize Text)
                PAGE_TEXT_CENTRE (Font Fontsize Text)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  int findex;
  double size;

  if((error=parseFontParameters(cmnd,parms,findex,size))==false)       //get the font for the text
    {
    QString text;
    if((error=parseTextField(cmnd,parms,text))==false)                              //get the text
      {
      if((error=pageRequired(cmnd))==false)
        {
        bool centre=(cmnd=="PAGE_TEXT_CENTRE")?true:false;
        m_albumData->addPageTextToPage(findex,size,text,centre);
        }
      }
    }

  return error;
}



/************************************************************************************************/
bool CParser::processRowAlignCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands:
               ROW_ALIGN_TOP
               ROW_ALIGN_MIDDLE
               ROW_ALIGN_BOTTOM
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  if((error=parseBlankParameters(cmnd,parms))==false)
    {
    if(cmnd=="ROW_ALIGN_TOP")
      m_albumData->setRowAlignment(ROW_ALIGN_TOP);
    else if(cmnd=="ROW_ALIGN_MIDDLE")
      m_albumData->setRowAlignment(ROW_ALIGN_MIDDLE);
    else
      m_albumData->setRowAlignment(ROW_ALIGN_BOTTOM);
    }

  return error;
}

/************************************************************************************************/
bool CParser::processRowStartCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands for adding a new row of stamps to the page:
                  ROW_START_ES (Font Fontsize border)
                  ROW_START_JS (Font Fontsize border)
                  ROW_START_FS (Font Fontsize border spacing)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  ROW_STYLE style=ROW_STYLE_EQUAL;

  if(cmnd=="ROW_START_JS")
    style=ROW_STYLE_JUSTIFY;
  else if(cmnd=="ROW_START_FS")
    style=ROW_STYLE_FIXED;

  int findex;
  double size;

  if((error=parseFontParameters(cmnd,parms,findex,size))==false)       //parse the font parameters
    {
    double border[1];
    int n=parseDoubleParameters(cmnd,parms,1,border);                       //get the border width
    if(n!=1)
      error=true;
    else
      {
      double spacing[1];
      spacing[0]=0.0;
      if(style==ROW_STYLE_FIXED)                          //get the spacing for fixed spacing rows
        {
        n=parseDoubleParameters(cmnd,parms,1,spacing);
        if(n!=1)
          error=true;
        }
      if(error==false)
        {
        if((error=pageRequired(cmnd))==false)
          {
          m_albumData->addStampRowToPage(findex,size,border[0],style,spacing[0]);
          }
        }
      }

    }

  return error;
}




/************************************************************************************************/
bool CParser::processStampAddCommand(QString cmnd,QString parms)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Process commands for adding a new stamp to a row:
                  STAMP_ADD              (width height txt0 txt1 txt2 txtBL txtBC txtBR)
                  STAMP_ADDX             (width height txt0 txt1 txt2 txtBL txtBC txtBR
                                          txt1BL txt1BC txt1BR)
                  STAMP_ADD_BLANK        (width height)
                  STAMP_ADD_TRIANGLE     (width height txt0 txt1 txt2 txtBL txtBC txtBR)
                  STAMP_ADD_TRIANGLE_INV (width height txt0 txt1 txt2 txtBL txtBC txtBR)
                  STAMP_ADD_DIAMOND      (width height txt0 txt1 txt2 txtBL txtBC txtBR)
   --------------------------------------------------------------------------------------------
    PARAMETERS:  cmnd: The command to process
                parms: The command parameters
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;

  double ssize[2];                                                              //width and height
  QString stampText[9];                             //up to 9 text items associated with the stamp

  if((error=rowRequired(cmnd))==false)
    {
    int n=parseDoubleParameters(cmnd,parms,2,ssize);              //get the stamp width and height
    if(n!=2)
      error=true;
    else if(cmnd!="STAMP_ADD_BLANK")    //all stamps other than a BLANK stamp require 6 text items
      {
      int i;
      for(i=0;i<6 && error==false;i++)
        error=parseTextField(cmnd,parms,stampText[i]);
      if(cmnd=="STAMP_ADDX")                 //this command requires an additional three paramters
        {
        for(;i<9 && error==false;i++)
          error=parseTextField(cmnd,parms,stampText[i]);
        }
      }
    }
  if(error==false)
    {
    for(int i=0;i<9;i++)                         //parse all stamp strings for escaped quote chars
      {
      QChar prevc=QChar::Null;
      QString tmp=stampText[i];
      stampText[i]="";

      for(int idx=0;idx<tmp.size();idx++)
        {
        QChar c=tmp.at(idx);
        if(c!='\\')                                                   //if not an escape character
          {
          stampText[i]=stampText[i]+c;                                                 //output it
          prevc=c;
          }
        else if(prevc=='\\')                                  //if the previous char was an escape
          {
          stampText[i]=stampText[i]+c;                                   //output the current char
          prevc=QChar::Null;      //2nd escape char in a sequence of two can not be an escape char
          }
        else
          prevc=c;
        }
      }

    STAMP_STYLE style=STAMP_STYLE_BLANK;
    style=(cmnd=="STAMP_ADD")             ?STAMP_STYLE_BLOCK       :style;
    style=(cmnd=="STAMP_ADDX")            ?STAMP_STYLE_BLOCKX      :style;
    style=(cmnd=="STAMP_ADD_TRIANGLE")    ?STAMP_STYLE_TRIANGLE    :style;
    style=(cmnd=="STAMP_ADD_TRIANGLE_INV")?STAMP_STYLE_TRIANGLE_INV:style;
    style=(cmnd=="STAMP_ADD_DIAMOND")     ?STAMP_STYLE_DIAMOND     :style;

    m_albumData->addStampToRow(style,ssize[0],ssize[1],stampText);
    }
  return error;
}
