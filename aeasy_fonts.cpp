/* --------------------------------------------------------------------------------------------
 *              aeasy_fonts.cpp
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Implementation of the font handling classes:
 *
 *              CFontManager  - maintains the relationship between fonts defined in the album
 *                              and the font file and code page used by libharu
 *              CFontFileInfo - object containing the font name, file name, and code pages
 *              CFontFileList - list of CFontFileInfo records for the available TTF files
 *              CFontPathList - list of paths to search for TTF files
 *
 *              NOTE: The current version of AlbumEasy only support the CP1250 to CP1258 code
 *                    pages.
 *
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
 * REFERENCES:  http://www.microsoft.com/typography/otspec/default.htm
 *              https://developer.apple.com/fonts/TTRefMan/RM06/Chap6.html
 * --------------------------------------------------------------------------------------------
 * REVISIONS:   Date          Version   Who    Comment
 *
 *              2011/04/26    3.0       cl     First QT Version
 * -------------------------------------------------------------------------------------------- */

#include "AlbumEasy.h"
#include "aeasy_ttf_structs.h"
#include "aeasy_flistwindow.h"
#include "aeasy_fonts.h"


/* ---------------------------------------------------------------------------------------------
/  Local Definitions
/  ---------------------------------------------------------------------------------------------*/

#define FONT_DIR "fonts"                     //font sub-directory relative to the application path


struct FONT_MAP
{
  QString  fontIdentifier;                                     //font identifier eg CN
  bool base14;                                                 //true  => PDF built-in base14 font
                                                               //false => TTF
  QString fontName;                                            //name of font
  int encoding;                                                //index into FONT_ENCODINGS array
  QString filePath;                                            //path for TT Font
  QString fileName;                                            //filename for TT Font

};


struct FONT_FILE_MAP
{
  QString fileName;
  QString fontName;
};


struct FONT_ENCODINGS
{
  QString encodingId;
  QString languageName;
  QString codec;
};


/* ---------------------------------------------------------------------------------------------
/  Local Globals
/  ---------------------------------------------------------------------------------------------*/


                             //the Haru library requires that TrueType fonts have all these tables
static const char * const requiredtags[]={"OS/2","cmap","cvt ","fpgm","glyf","head","hhea",
                                           "hmtx","loca","maxp","name","post","prep",""};


/* ---------------------------------------------------------------------------------------------
/  Static member variable definitions
/  ---------------------------------------------------------------------------------------------*/

QString CFontManager::m_error;

/* ---------------------------------------------------------------------------------------------
/  The FONT_MAP structure is used to map a font identifier to either a built in base14 font
/  or a TrueType font.
/
/  In the case of TTF, Each font identifer maps to a TTF file plus an encoding
/  As a result any particular TTF file may be mapped by more than one font identifier.
/
/  The libharu function HPDF_LoadTTFontFromFile which is used to load TTF files will generate an
/  error if an attempt is made to load the same file more than once on a per document basis.
/  Thus necessitating the FONT_FILE_MAP structure which is used to ensure that each TTF file
/  is only loaded once for each new PDF document.
/  ---------------------------------------------------------------------------------------------*/

#define NUM_USER_FONTS       12                                //Allow up to 12 user defined fonts
#define NUM_PREDEFINED_FONTS 12

static FONT_MAP fontMap[]=
{
  {"CN", true,   "Courier",               -1, "", ""},
  {"CB", true,   "Courier-Bold",          -1, "", ""},
  {"CI", true,   "Courier-Oblique",       -1, "", ""},
  {"CS", true,   "Courier-BoldOblique",   -1, "", ""},
  {"TN", true,   "Times-Roman",           -1, "", ""},
  {"TB", true,   "Times-Bold",            -1, "", ""},
  {"TI", true,   "Times-Italic",          -1, "", ""},
  {"TS", true,   "Times-BoldItalic",      -1, "", ""},
  {"HN", true,   "Helvetica",             -1, "", ""},
  {"HB", true,   "Helvetica-Bold",        -1, "", ""},
  {"HI", true,   "Helvetica-Oblique",     -1, "", ""},
  {"HS", true,   "Helvetica-BoldOblique", -1, "", ""},
  {"",false,"", -1, "", ""},                                 //enough room for NUM_USER_FONTS (12)
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""},
  {"",false,"", -1, "", ""}
};


static FONT_FILE_MAP fontFileMap[]=                          //enough room for NUM_USER_FONTS (12)
{
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",},
  {"","",}
};


/* ---------------------------------------------------------------------------------------------
/  Array that maps encoding identifiers to friendly language names and codec identifiers.
/  The later are used to identify the necessary codec for converting Unicode encoded QStrings
/  to an appropriate 8 bit encoding for the font.
/
/  NOTE: The current implementation of CFontFileInfo::availableCodePages is based on the sequence
/        of items in this array matching the bit sequence in the "OS/2" table ulCodePageRange1
/  ---------------------------------------------------------------------------------------------*/

#define ENCODING_LATIN_1  0                  //index of the Latin1 font in the fontEncodings array

static FONT_ENCODINGS fontEncodings[]=
{
{"CP1252", "Latin 1",        "Windows-1252"},
{"CP1250", "Latin 2",        "Windows-1250"},
{"CP1251", "Cyrillic",       "Windows-1251"},
{"CP1253", "Greek",          "Windows-1253"},
{"CP1254", "Turkish",        "Windows-1254"},
{"CP1255", "Hebrew",         "Windows-1255"},
{"CP1256", "Arabic",         "Windows-1256"},
{"CP1257", "Windows Baltic", "Windows-1257"},
{"CP1258", "Vietnamese",     "Windows-1258"}
};



/************************************************************************************************/
void CFontManager::initialise(void)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Font files have to be loaded for each PDF document, this function is used
                to clear out any font-file to font-name mappings from a previous document
                so that they will be loaded for the new document.
                It also clears any existing user defined fonts.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  none
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{
                                                            //clear the list of font-file mappings
  for(unsigned int i=0; i<(sizeof(fontFileMap)/sizeof(fontFileMap[0]));i++)
    {
    fontFileMap[i].fontName="";
    }
                                                            //clear the list of user defined fonts
  for(unsigned int i=NUM_PREDEFINED_FONTS; i<(NUM_PREDEFINED_FONTS+NUM_USER_FONTS);i++)
    {
    fontMap[i].fontIdentifier="";
    }
}



/************************************************************************************************/
bool CFontManager::addUserDefinedFont(CFontFileList *fontFiles,QString fontId,QString fontName,
                                      QString fontEncoding)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION:  Add a user defined font to the font mapping
   --------------------------------------------------------------------------------------------
    PARAMETERS:  fontFiles:    List of available font files
                 fontId:       User identifier for the font
                 fontName:     Name of the font
                 fontEncoding: Encoding is optional
   --------------------------------------------------------------------------------------------
       RETURNS:  true: error
                false: success
   -------------------------------------------------------------------------------------------- */
{
  bool error=false;
  m_error="";                                                                 //clear error string

  int id=-1;
                                                              //find an empty slot in the font map
  for(unsigned int i=0; i<(sizeof(fontMap)/sizeof(fontMap[0])) && error==false;i++)
    {
    if(fontMap[i].fontIdentifier.length()==0 && id<0)                        //found an empty slot
      id=i;
    else if(fontMap[i].fontIdentifier.compare(fontId,Qt::CaseInsensitive)==0)
      {
      m_error=QString(tr(" Duplicate definition of font %1.").arg(fontId));
      error=true;
      }
    }
  if(id<0 && error==false)
    {
    m_error=QString(tr(" Exceeded the maximum number of %1 user fonts").arg(NUM_USER_FONTS));
    error=true;
    }
  else if(error==false)
    {
    QString fileName;
    QString filePath;
                      //search the list of available font files for one that matches the font name
    if(fontFiles->find(fontName,fileName,filePath)==false)
      {
      error=true;
      m_error=QString(tr(" Error finding a True Type font file containing font %1").arg(fontName));
      }
    else
      {
      fontMap[id].fontIdentifier=fontId;
      fontMap[id].base14=false;
      fontMap[id].fontName=fontName;
      fontMap[id].encoding=-1;                      //encoding is optional, default to no encoding
      fontMap[id].filePath=filePath;
      fontMap[id].fileName=fileName;

      if(fontEncoding.size()>0)                                          //if an encoding supplied
        {                                                     //find it in the fontEncodings table
        for(int unsigned i=0;
            i<(sizeof(fontEncodings)/sizeof(fontEncodings[0])) && fontMap[id].encoding==-1; i++)
          {
          if(fontEncodings[i].encodingId==fontEncoding)
            {
            fontMap[id].encoding=i;
            }
          }
        if(fontMap[id].encoding==-1)                                         //if it was not found
          {
          m_error=QString(tr(" Specified encoding %1 is not supported.").arg(fontEncoding));
          error=true;
          }
        }
      if(error==false)
        {
        bool mapped=false;  //add font file with path to fontFileMap if not already in fontFileMap

        QString file=QDir::toNativeSeparators(fontMap[id].filePath+"/"+fontMap[id].fileName);

        for(unsigned int i=0; i<(sizeof(fontFileMap)/sizeof(fontFileMap[0])) && mapped==false;i++)
          {
          if(file==fontFileMap[i].fileName)
            mapped=true;
          }
        if(mapped==false)                                                  //not in map, so add it
          {
          for(unsigned int i=0;i<(sizeof(fontFileMap)/sizeof(fontFileMap[0])) && mapped==false;i++)
            {
            if(fontFileMap[i].fileName=="")
              {
              fontFileMap[i].fileName=file;                                         //add the file
              mapped=true;
              }
            }
          }
        if(mapped==false)                 //unable to find an empty fontFileMap entry for the file
          {
          m_error=QString(tr(" Exceeded the maximum number of %1 user font files. ")
                          .arg(NUM_USER_FONTS));
          error=true;
          }
        }
      }
    }

  return error;
}


/************************************************************************************************/
int CFontManager::getFontIndex(QString fontId)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Find the font specified by identifier in the fontmap, returning the index.
   --------------------------------------------------------------------------------------------
    PARAMETERS:  fontId: The font identifier
   --------------------------------------------------------------------------------------------
       RETURNS:  >=0: index
                  <0: error, font identifier not found.
   -------------------------------------------------------------------------------------------- */
{
  int id=-1;

  for(unsigned int i=0; i<(sizeof(fontMap)/sizeof(fontMap[0])) && id<0;i++)
    {
    if(fontId==fontMap[i].fontIdentifier)
      id=i;
    }
  return id;
}



/************************************************************************************************/
HPDF_Font CFontManager::getFont(HPDF_Doc pdfDoc,int index)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the font corresponding to the specified fontMap index from the Haru library
                Load it first if necessary.
   --------------------------------------------------------------------------------------------
    PARAMETERS: pdfDoc
                index: The font identifier
   --------------------------------------------------------------------------------------------
       RETURNS:  >=0: index
                  <0: error, font identifier not found.
   -------------------------------------------------------------------------------------------- */
{
  HPDF_Font f=NULL;
  m_error="";                                                                 //clear error string

  if(index>=0 && index<(int)(sizeof(fontMap)/sizeof(fontMap[0])))
    {
    if(fontMap[index].base14==true)                                       //if a built-in PDF font
      {

      f=HPDF_GetFont(pdfDoc,fontMap[index].fontName.toLatin1(),              //get the font handle
                    fontEncodings[ENCODING_LATIN_1].encodingId.toLatin1());    //default to Latin1
      if(f==NULL)
        {
        m_error=QString(tr(" Unable to assign the specified PDF Font <b>%1</b> (Error No:%2)"))
                            .arg(fontMap[index].fontIdentifier)
                            .arg(HPDF_GetError(pdfDoc),0,16);
        }
      }
    else                                                                   //a true type font file
      {
      int fmidx=-1;                       //map the file name to the font name returned by libHaru


      QString file=QDir::toNativeSeparators(fontMap[index].filePath+"/"+fontMap[index].fileName);

      for(int i=0; i<(int)(sizeof(fontFileMap)/sizeof(fontFileMap[0])) && fmidx<0; i++)
        {
        if(file==fontFileMap[i].fileName)
          fmidx=i;
        }
      if(fmidx<0)                                             //if file to font name map not found
        {
        m_error=QString(tr(" Mapping for truetype font file %1 not found"))
                        .arg(fontFileMap[index].fileName);
        }
      else
        {
        if(fontFileMap[fmidx].fontName.length()==0)         //if font file has not yet been loaded
          {
                                                                                         //load it
          fontFileMap[fmidx].fontName=HPDF_LoadTTFontFromFile(pdfDoc,file.toLatin1(),HPDF_TRUE);
          }

        if(fontFileMap[fmidx].fontName.length()==0)     //if font file was not loaded successfully
          {
          m_error=QString(tr(" Failed to load TrueType font file <b>%1</b> (Error No:%2)"))
                          .arg(fontFileMap[fmidx].fileName)
                          .arg(HPDF_GetError(pdfDoc),0,16);
          }
        else                                                                 //loaded successfully
          {
          if(fontMap[index].encoding==-1)                                         //if no encoding
            f=HPDF_GetFont(pdfDoc,fontFileMap[fmidx].fontName.toLatin1(),
                           fontEncodings[ENCODING_LATIN_1].encodingId.toLatin1());//default Latin1
          else
            {
            int i=fontMap[index].encoding;                       //else use the encoding ID string
            f=HPDF_GetFont(pdfDoc,fontFileMap[fmidx].fontName.toLatin1(),
                           fontEncodings[i].encodingId.toLatin1());
            }
          if(f==NULL)
            {
            m_error=QString(tr(" Unable to assign the specified True Type Font <b>%1</b>"
                               " (Error No:%2)"))
                            .arg(fontMap[index].fontIdentifier).arg(HPDF_GetError(pdfDoc),0,16);
            }
          }
        }
      }
    }

  if(f==NULL)                                                              //if there was an error
    {
    HPDF_ResetError(pdfDoc);                     //clear the error so that processing may continue
    if(m_error=="")
      m_error=" Undefined error while loading a font.";
    }

  return f;
}

/************************************************************************************************/
QTextCodec *CFontManager::getCodec(int index)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get the codec for converting a QString (Unicode) to an appropriate 8 bit encoded
                string for the font.
   --------------------------------------------------------------------------------------------
    PARAMETERS: index: The font identifier
   --------------------------------------------------------------------------------------------
       RETURNS:  QTextCodec *: The codec
                               0 => no matching codec foung
   -------------------------------------------------------------------------------------------- */
{
  QTextCodec *qtCodec=NULL;

  m_error="";                                                                 //clear error string

  if(index>=0 && index<(int)(sizeof(fontMap)/sizeof(fontMap[0])))
    {
    const char *codecName;

    int f=fontMap[index].encoding;
    if(f>=0)                                                //if an encoding was specified, use it
      codecName=fontEncodings[f].codec.toLatin1().data();
    else
      codecName=fontEncodings[ENCODING_LATIN_1].codec.toLatin1().data();  //else default to Latin1

    if((qtCodec=QTextCodec::codecForName(codecName))==0)
      m_error=QString(tr(" Font encoding \"%1\" was not found.")).arg(codecName);
    }
  else
    m_error=" Undefined error while loading font encoding.";

  return qtCodec;
}


/************************************************************************************************/
void CFontFileList::populate(QWidget *parent,bool includeSystemFonts)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: The CFontFileList maintains a list of font files and font names.
                This function populates the list.
   --------------------------------------------------------------------------------------------
    PARAMETERS: parent:             The parent widges
                includeSystemFonts: true  => search the system font directories
                                    false => only search the local "font" directory
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */
{

  if(m_populated==false)                 //could be a time consuming operation, so only do it once
    {
    m_populated=true;

    qint64 started=QDateTime::currentMSecsSinceEpoch();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QMessageBox msgBox(parent);                                    //display a message to the user
    msgBox.setWindowTitle("AlbumEasy");
    msgBox.setText(tr("Searching for fonts, please be patient."));
                                                              //remove title bar menu/close button
    msgBox.setWindowFlags((Qt::Dialog|Qt::WindowTitleHint|Qt::CustomizeWindowHint) &
                          ~Qt::WindowCloseButtonHint &  ~Qt::WindowSystemMenuHint);
    msgBox.setStandardButtons(0);                               //remove the message box OK button
    msgBox.show(); //use show instead of exec, show returns immediately so processing can continue


    CFontPathList fontPaths;                          //list of directories that may contain fonts
    fontPaths.populate(includeSystemFonts);

    foreach(const QString &path,fontPaths)                  //search each directory for font files
      {
      QStringList nameFilters;
      nameFilters.append("*.ttf");
                                                         //traverse, recursing into subdirectories
      QDirIterator dirIterator(path,nameFilters,
                              QDir::Files|QDir::NoSymLinks,QDirIterator::Subdirectories);

      while(dirIterator.hasNext())                 //process all font files found in the directory
        {
        qApp->processEvents();                                            //allow the UI to update

        QFileInfo fi(dirIterator.next());

        QString fileName=fi.fileName();
        QString filePath=fi.canonicalPath();

                  //don't parse files with duplicate names again, even if in different directories
                 //however the bad-files list is not checked for duplicates, so it is possible for
                 //duplicate file names to appear on the bad-files list

        if(contains(fileName)==false)                                         //if not a duplicate
          {
          parseFontFile(fileName,filePath);                                             //parse it
          }
        }
      }
                   //wait at least a few seconds so that the user has time to read the message box
    while(QDateTime::currentMSecsSinceEpoch()<(started+1500))
      qApp->processEvents();

    msgBox.close();                                                        //close the message box
    QApplication::restoreOverrideCursor();
    }
  qSort(m_fontFileInfoRecs.begin(),m_fontFileInfoRecs.end(),sortByName);
  m_badFontFiles->sort(Qt::CaseInsensitive);

}

/************************************************************************************************/
bool CFontFileList::find(QString fontName,QString &fileName,QString &filePath)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: find a font by name, returning the corresponding file and it's location.
   --------------------------------------------------------------------------------------------
    PARAMETERS: fontName:  Name of font to find
                fileName:  Variable to return the file name in
                filePath:  Variable to return the file location in
   --------------------------------------------------------------------------------------------
       RETURNS:  true: found
                false: not found
   -------------------------------------------------------------------------------------------- */
{
  for(int i=0;i<m_fontFileInfoRecs.size();i++)
    {
    CFontFileInfo *f=m_fontFileInfoRecs.at(i);
    if(fontName.compare(f->fontName(),Qt::CaseInsensitive)==0)
      {
      fileName=f->fileName();
      filePath=f->filePath();
      if(fileName.length()>0 && filePath.length()>0 )
        return true;
      }
    }
  return false;
}


/************************************************************************************************/
CFontFileInfo *CFontFileList::at(int idx)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Get a font information record by index from the list.
   --------------------------------------------------------------------------------------------
    PARAMETERS: idx: The index
   --------------------------------------------------------------------------------------------
       RETURNS: 0=> not found
                else the font information record
   -------------------------------------------------------------------------------------------- */
{
  if(idx>=0 && idx<m_fontFileInfoRecs.count())
    {
    return m_fontFileInfoRecs.at(idx);
    }
  else
    {
    return (CFontFileInfo *)0;
    }
}

/************************************************************************************************/
CFontFileList::~CFontFileList()
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: CFontFileList destructor
   --------------------------------------------------------------------------------------------
    PARAMETERS: none
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  while(m_fontFileInfoRecs.isEmpty()==false)
    {
    delete m_fontFileInfoRecs.takeFirst();
    }

 m_badFontFiles->clear();
 delete m_badFontFiles;
}


/************************************************************************************************/
bool CFontFileList::sortByName(CFontFileInfo *f1, CFontFileInfo *f2)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Comparison function used by qSort to aid in sorting the font information
                records by name.
   --------------------------------------------------------------------------------------------
    PARAMETERS: f1: First record to compare
                f2: Second record to compare
   -------------------------------------------------------------------------------------------- */
{
  if(f1->fontName()< f2->fontName())
    return(true);
  else
    return(false);

}

/************************************************************************************************/
bool CFontFileList::contains(const QString fileName)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Check if the list of font information records already contains a file
                with the same name as the specified file
   --------------------------------------------------------------------------------------------
    PARAMETERS: fileName: File to check for
   --------------------------------------------------------------------------------------------
       RETURNS:  true: found
                false: not found
   -------------------------------------------------------------------------------------------- */
{
  for(int i=0;i<m_fontFileInfoRecs.size();i++)
    {
    CFontFileInfo *f=m_fontFileInfoRecs.at(i);
    if(fileName.compare(f->fileName())==0)
      return true;
    }
  return false;
}


/************************************************************************************************/
void CFontFileList::parseFontFile(const QString fileName,const QString filePath)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Parse the specified TTF font file. If it is usable by Libharu extract the
                relevant information into a CFontFileInfo record and add it to the list
                else add it to the list of bad-font (incompatible) files
   --------------------------------------------------------------------------------------------
    PARAMETERS: fileName:  Name of the file to parse
                filePath:  Location of the file to parse
   --------------------------------------------------------------------------------------------
       RETURNS:  none
   -------------------------------------------------------------------------------------------- */

{
  QHash<QString, int> findingTags;                           //list of font table-tags to be found

  for(int i=0;;i++)                                            //add all required tags to the list
    {
    QString t(requiredtags[i]);
    if(t=="")
      break;
    findingTags[t]=false;                                                          //not yet found
    }

  QString fontName="";

  QString sfile=QDir::toNativeSeparators(filePath+ "/" +fileName);

  QFile ttfFile(sfile);

  TTF_OFFSET_TABLE offsetTable;
  offsetTable.numTables=0;                                              //default to no TTF tables

  if(ttfFile.exists()==true)                                                  //if the file exists
    {
    if(ttfFile.open(QIODevice::ReadOnly))
      {
      if(ttfFile.read((char *)&offsetTable, sizeof(TTF_OFFSET_TABLE))>=0)      //if read from file
        {
        offsetTable.type=qFromBigEndian(offsetTable.type);                //correct the byte order
        offsetTable.numTables=qFromBigEndian(offsetTable.numTables);
        if(offsetTable.type!=0x00010000)                  //if not a V1.0 (Windows compatible) TTF
          {
          offsetTable.numTables=0;                                        //clear number of tables
          }
        }
      }
    }

  TTF_TABLE_DIRECTORY nameTableDir;
  TTF_TABLE_DIRECTORY cmapTableDir;
  TTF_TABLE_DIRECTORY os2TableDir;

  nameTableDir.length=0;
  nameTableDir.offset=0;
  cmapTableDir.length=0;
  cmapTableDir.offset=0;
  os2TableDir.length=0;
  os2TableDir.offset=0;

  for(int i=0; i< offsetTable.numTables; i++)                  //read the tables from the TTF file
    {
    TTF_TABLE_DIRECTORY td;

                                            //find the "name", "cmap" and "OS/2" directory entries
    if(ttfFile.read((char *)&td,sizeof(TTF_TABLE_DIRECTORY))>=0)
      {
      QString tag(QByteArray(&td.tag[0],4));                            //the 4 byte directory tag

      if(findingTags.contains(tag))                        //if this is a tag we are interested in
        {
        findingTags[tag]=true;                                                  //flag it as found
        }

      if(QString::compare("name",tag,Qt::CaseInsensitive)==0 && nameTableDir.length==0)
        {
        nameTableDir.checkSum=qFromBigEndian(td.checkSum);
        nameTableDir.offset  =qFromBigEndian(td.offset);
        nameTableDir.length  =qFromBigEndian(td.length);
        }
      else if(QString::compare("cmap",tag,Qt::CaseInsensitive)==0 && cmapTableDir.length==0)
        {
        cmapTableDir.checkSum=qFromBigEndian(td.checkSum);
        cmapTableDir.offset  =qFromBigEndian(td.offset);
        cmapTableDir.length  =qFromBigEndian(td.length);
        }
      else if(QString::compare("OS/2",tag,Qt::CaseInsensitive)==0 && os2TableDir.length==0)
        {
        os2TableDir.checkSum=qFromBigEndian(td.checkSum);
        os2TableDir.offset  =qFromBigEndian(td.offset);
        os2TableDir.length  =qFromBigEndian(td.length);
        }
      }
    }

    bool usable=true;

    foreach(QString key,findingTags.keys())                //check if all required tags were found
      {
      bool found=findingTags.value(key);
      if(found==false)
        usable=false;
      }

#ifndef QT_NO_DEBUG
    if(usable==false)
      {
      DEBUGS(tr("Missing tags FontFile:%1:").arg(sfile));
      foreach(QString key,findingTags.keys())
        {
        DEBUGS(tr("Tag:%1 - Status:%2").arg(key).arg(findingTags.value(key)));
        }
      }
#endif

  TTF_NAME_TABLE_HEADER nameHeader;
  nameHeader.count=0;                                         //default to zero name records found
  if(usable && nameTableDir.length>0)                          //if a "name" table directory entry
    {

    if(ttfFile.seek(nameTableDir.offset)==true)                        //seek to name table header
      {
      if(ttfFile.read((char *)&nameHeader,sizeof(TTF_NAME_TABLE_HEADER))>=0)
        {
        nameHeader.format      =qFromBigEndian(nameHeader.format);
        nameHeader.count       =qFromBigEndian(nameHeader.count);
        nameHeader.stringOffset=qFromBigEndian(nameHeader.stringOffset);
        }

      }
    }

  TTF_NAME_RECORD nameRecord;

  for(int i=0; i<nameHeader.count;i++)                                     //read the name records
    {
    if(ttfFile.read((char *)&nameRecord,sizeof(TTF_NAME_RECORD))>=0)
      {
      qint64 pos=ttfFile.pos();          //save file position => the start of the next name record

      nameRecord.platformId=qFromBigEndian(nameRecord.platformId);
      nameRecord.encodingId=qFromBigEndian(nameRecord.encodingId);
      nameRecord.languageId=qFromBigEndian(nameRecord.languageId);
      nameRecord.nameId    =qFromBigEndian(nameRecord.nameId);
      nameRecord.length    =qFromBigEndian(nameRecord.length);
      nameRecord.offset    =qFromBigEndian(nameRecord.offset);

      QString pl1Nm4;
      QString pl3Nm4;

                                                                         //seek to the name string
      ttfFile.seek(nameTableDir.offset + nameRecord.offset + nameHeader.stringOffset);

      QByteArray ba=ttfFile.read(nameRecord.length);                        //read the name string

                                                                        //if Macintosh platform ID
      if(nameRecord.platformId==1 && nameRecord.encodingId==0 && nameRecord.nameId==4)
        {
        pl1Nm4=ba;
        }                                                               //if Microsoft platform ID
      else if(nameRecord.platformId==3 && nameRecord.encodingId==1 &&
              nameRecord.languageId==0x0409 && nameRecord.nameId==4)
        {
        QString sba;

        if(ba.size()>2)                                         //convert name from 16 bit unicode
          {
          char *rdata=ba.data();
          for(int u=1;u<ba.size();u=u+2)                //from second byte, read every second byte
            {
            sba.append(*(rdata+u));
            }
          }
        pl3Nm4=sba;
        }

      if(fontName.length()==0)          //if we haven't yet read the font name from the name table
        {
        if(pl1Nm4.length()>0)                   //if we read a platformId=1 nameId=4 name use that
          fontName=pl1Nm4;
        else if(pl3Nm4.length()>0)         //else if we read a platformId=3 nameId=4 name use that
          fontName=pl3Nm4;
        }
      ttfFile.seek(pos);                   //seek to saved position, the start of next name record
      }
    }

  fontName=fontName.trimmed();                             //trim leading and trainling whitespace

     //some font names that are not bold, italic etc. end with the redundant "Regular" - remove it
  if(fontName.endsWith("Regular",Qt::CaseInsensitive)==true)
    {
    fontName=fontName.left(fontName.length()-7);
    fontName=fontName.trimmed();
    }

  if(fontName.length()==0)
    usable=false;

  TTF_CMAP_TABLE_HEADER cmapHeader;
  cmapHeader.count=0;                                           //default to no cmap records found
  if(usable && cmapTableDir.length>0)                          //if a "cmap" table directory entry
    {
    if(ttfFile.seek(cmapTableDir.offset)==true)                        //seek to cmap table header
      {
      if(ttfFile.read((char *)&cmapHeader,sizeof(TTF_CMAP_TABLE_HEADER))>=0)
        {
        cmapHeader.version=qFromBigEndian(cmapHeader.version);
        cmapHeader.count  =qFromBigEndian(cmapHeader.count);
        }
      }

    if(cmapHeader.version!=0)                                            //only version 0 is valid
      usable=false;
    }

  TTF_CMAP_ENCODING_RECORD encodingRecord;
  TTF_CMAP_FORMAT_RECORD   formatRecord;

  bool unicodeFormat=false;

  for(int i=0; i<cmapHeader.count && usable==true && unicodeFormat==false ;i++)
    {
    if(ttfFile.read((char *)&encodingRecord,sizeof(TTF_CMAP_ENCODING_RECORD))>=0)
      {
      qint64 pos=ttfFile.pos();      //save file position => the start of the next encoding record

      encodingRecord.platformId=qFromBigEndian(encodingRecord.platformId);
      encodingRecord.encodingId=qFromBigEndian(encodingRecord.encodingId);
      encodingRecord.offset=qFromBigEndian(encodingRecord.offset);

      ttfFile.seek(cmapTableDir.offset+encodingRecord.offset);

      if(ttfFile.read((char *)&formatRecord,sizeof(TTF_CMAP_FORMAT_RECORD))>=0)
        {
        formatRecord.format=qFromBigEndian(formatRecord.format);
        formatRecord.length=qFromBigEndian(formatRecord.length);
        formatRecord.language=qFromBigEndian(formatRecord.language);
                                                                         //if format is MS Unicode
        if(encodingRecord.platformId==3 && encodingRecord.encodingId==1 && formatRecord.format==4)
          unicodeFormat=true;
        }
      if(unicodeFormat==false)
        ttfFile.seek(pos);    //seek to saved position in preparation for the next encoding record
      }
    }
  usable=(unicodeFormat==false)?false:usable;                 //libharu requires MS Unicode format

  TTF_OS2_TABLE os2Table;

  if(usable && os2TableDir.length>0)                           //if a "OS/2" table directory entry
    {
    usable=false;
    if(ttfFile.seek(os2TableDir.offset)==true)                         //seek to OS/2 table header
      {
      if(ttfFile.read((char *)&os2Table,sizeof(TTF_OS2_TABLE))>=0)
        {
        usable=true;

             //ulCodePageRange1 and ulCodePageRange2 should be 32 bit Big-Endian. For some reason,
             //perhaps a 2 byte offset error somewhere in this code, this gives incorrect results
             //when decoding the windows code pages from the the lower 9 bits of ulCodePageRange1.
             //The correct codepages are obtained if instead only the lower 16 bits of
             //ulCodePageRange1 are treated as 16 bit Big-Endian.

        quint16 tmp=(quint16)(os2Table.ulCodePageRange1 & 0xffff);  //big-endian the lower 16 bits
        tmp=qFromBigEndian(tmp);
        os2Table.ulCodePageRange1=(os2Table.ulCodePageRange1 & 0xffff0000) | tmp;

        //the three lines above should actually have been this:
           //os2Table.ulCodePageRange1=qFromBigEndian(os2Table.ulCodePageRange1);
        os2Table.ulCodePageRange2=qFromBigEndian(os2Table.ulCodePageRange2);
        }
      }
    }

  if(usable)
    {
    CFontFileInfo *finfo=new CFontFileInfo;

    finfo->setFileDetails(fontName,fileName,filePath,
                          os2Table.ulCodePageRange1,os2Table.ulCodePageRange2);

    m_fontFileInfoRecs.append(finfo);                 //add it to the list of available font files
    }
  else
     {
     m_badFontFiles->append(sfile);                    //add it to the list of unusable font files
     }
}


/************************************************************************************************/
void CFontPathList::populate(bool includeSystemFonts)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: This class maintains a list of directories that may contain font files.
                These directories include the application local "font" directory and optionally
                any installed system fonts.
   --------------------------------------------------------------------------------------------
    PARAMETERS: includeSystemFonts: true  => include system font directories
                                    false => only include the local "font" directory
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{

  clear();                                                               //empty any existing list

                                          //first append the local directory containing font files
  append(QDir::toNativeSeparators(QApplication::applicationDirPath()+"/" + FONT_DIR));


  if(includeSystemFonts==true)                //if including the installed system font directories
    {

    //On Linux the USER font directory is located in the user home directory e.g. /home/xyz/.fonts
    //this appears to be empty by default. So instead most applications also allow the user to
    //choose from the installed SYSTEM fonts.
    //On Windows which does not differentiate between USER and SYSTEM fonts, the USER font
    //directory as provided by QT e.g. C:/Windows/Fonts, allows access to all installed fonts

                                        //get paths to the USER font directories as provided by QT
    foreach(const QString &fdir,QStandardPaths::standardLocations(QStandardPaths::FontsLocation))
      {
      append(fdir);
      }

#ifdef Q_OS_LINUX               //add the location of the SYSTEM font directories on Linux systems

           //many Linux systems have the locations of the font directories stored in a config file
           //try parsing known config files.
    parseLinuxFontConfigFile("/etc/fonts/fonts.conf");
    parseLinuxFontConfigFile("/etc/fonts/local.conf");
                                                         //also try appending some known locations
    append(QString("/usr/share/fonts"));
    append(QString("/usr/local/share/fonts"));
    append(QString("~/.fonts"));
#endif


#ifdef Q_OS_MAC                   //add the location of the SYSTEM font directories on MAC systems

    ;                                   //NOT  YET IMPLMENTED, TO BE DONE WHEN WE HAVE A MAC BUILD

#endif
    }

}


/************************************************************************************************/
void CFontPathList::append(QString path)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Append a path to the list of font paths if it is not already in the list
   --------------------------------------------------------------------------------------------
    PARAMETERS: path : The path to append
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
                //paths may contain ~/ which would be expanded by shell globbing to the users home
                                                  //so the equivalent globbing has to be done here
  if(path.startsWith ("~/"))
    path.replace(0,1,QDir::homePath());

  QDir d(path);                              //for comparison purposes convert to a canonical path
  path=d.canonicalPath();  //converting to a canonical path also ensures that the directory exists

  if(!path.isEmpty() && contains(path)==false)  //don't add empty path or path already in the list
    {
    QStringList::append(path);
    }
}


/************************************************************************************************/
void CFontPathList::parseLinuxFontConfigFile(QString fname)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Parse A Linux font configuration file to ascertain the location of font files.
                paths in the config file are demarcated by a <dir> .. </dir> pair.
   --------------------------------------------------------------------------------------------
    PARAMETERS: fname : The name of the config file to parse.
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{
  QFile file(fname);
  if(file.exists() && file.open(QFile::ReadOnly|QFile::Text))
    {
    QTextStream stream(&file);
    QString line=stream.readLine();

    while(!line.isNull())                                            //read the file, line by line
      {
      line=line.trimmed();                               //remove leading and trailing white space
      if(!line.isEmpty())
        {
        bool error=false;

        int pos=line.indexOf("<dir>",Qt::CaseInsensitive);        //find first directory delimiter

        while(pos>=0 && error==false)                 //a line may contain more than one directory
          {
          error=true;

          int remainder=line.size()-pos-5;           //number of chars in the line after the <dir>

          if(remainder>0)
            {
            line=line.right(remainder);

            pos=line.indexOf("</dir>",Qt::CaseInsensitive);         //look for closing path marker
            if(pos>0)
              {
              QString path=line.left(pos);
              append(path);                                    //add the path to the list of paths

              remainder=line.size()-pos-6;          //number of chars in the line after the </dir>
              if(remainder>0)
                {
                line=line.right(remainder);
                pos=line.indexOf("<dir>",Qt::CaseInsensitive);   //look for next path in this line
                error=false;
                }
              }
            }
          }
        }
      line=stream.readLine();                                   //read the next line from the file
      }
    file.close();
    }
}


/************************************************************************************************/
void CFontFileInfo::setFileDetails(QString fontName,QString fileName,QString filePath,
                                   quint32 codePageRange1,quint32 codePageRange2)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Set the information record details for the font file
   --------------------------------------------------------------------------------------------
    PARAMETERS: fontName       : The name of the font
                fileName       : corresponding file name
                filePath       : location of the file.
                codePageRange1 : 64 bit codepage range from the TTF file "OS/2" page is split
                codePageRange2 : into two 32 bit variables.
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
 {
   m_fontName=fontName;
   m_fileName=fileName;
   m_filePath=filePath;
   m_codePageRange1=codePageRange1;
   m_codePageRange2=codePageRange2;
}



/************************************************************************************************/
void CFontFileInfo::availableCodePages(QString &codePages)
/* --------------------------------------------------------------------------------------------
   DESCRIPTION: Create a string containing the names of the code pages supported by this font
                separated by new line characters.
                NOTE: This function expects the fontEncodings[] array defined near the top of
                      this file to be in the same sequence as the bit sequence in the
                      TrueType Font "OS/2" table ulCodePageRange1
   --------------------------------------------------------------------------------------------
    PARAMETERS: codePages : variable in which to return the code pages.
   --------------------------------------------------------------------------------------------
       RETURNS: none
   -------------------------------------------------------------------------------------------- */
{

  codePages="";
  quint32 mask=0x01;


  for(int i=0;i<9;i++)                                //process the first 9 bits in codePageRange1
    {
    if(m_codePageRange1 & mask)                                                //if the bit is set
      {
      QString cp=fontEncodings[i].encodingId + " (" + fontEncodings[i].languageName + ")";
      if(codePages.length()!=0)
        codePages=codePages+"\n";
      codePages=codePages+cp;
      }
    mask=mask<<1;                                                               //get the next bit
    }
}

