/* --------------------------------------------------------------------------------------------
 *              aeasy_ttf_structs.h
 * --------------------------------------------------------------------------------------------
 * DESCRIPTION: Structures used while parsing TrueType Font files
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

#ifndef _AEASY_TTF_STRUCTS_H_
#define _AEASY_TTF_STRUCTS_H_


struct TTF_OFFSET_TABLE
{
  quint32 type;
  quint16 numTables;
  quint16 searchRange;
  quint16 entrySelector;
  quint16 rangeShift;
};


struct TTF_TABLE_DIRECTORY
{
  char tag[4];
  quint32 checkSum;
  quint32 offset;
  quint32 length;
};


struct TTF_NAME_TABLE_HEADER
{
  quint16 format;
  quint16 count;
  quint16 stringOffset;
};


struct TTF_NAME_RECORD
{
  quint16 platformId;
  quint16 encodingId;
  quint16 languageId;
  quint16 nameId;
  quint16 length;
  quint16 offset;
};


struct TTF_CMAP_TABLE_HEADER
{
  quint16 version;
  quint16 count;
};


struct TTF_CMAP_ENCODING_RECORD
{
  quint16 platformId;
  quint16 encodingId;
  quint32 offset;
};

struct TTF_CMAP_FORMAT_RECORD
{
  quint16 format;
  quint16 length;
  quint16 language;
};


struct TTF_OS2_TABLE
{
  quint16 	version;
  qint16  	xAvgCharWidth;
  quint16 	usWeightClass;
  quint16 	usWidthClass;
  quint16 	fsType;
  qint16  	ySubscriptXSize;
  qint16  	ySubscriptYSize;
  qint16  	ySubscriptXOffset;
  qint16  	ySubscriptYOffset;
  qint16  	ySuperscriptXSize;
  qint16  	ySuperscriptYSize;
  qint16  	ySuperscriptXOffset;
  qint16  	ySuperscriptYOffset;
  qint16  	yStrikeoutSize;
  qint16  	yStrikeoutPosition;
  qint16  	sFamilyClass;
  quint8 	  panose[10];
  quint32 	ulUnicodeRange1;
  quint32 	ulUnicodeRange2;
  quint32 	ulUnicodeRange3;
  quint32 	ulUnicodeRange4;
  qint8 	  achVendID[4];
  quint16 	fsSelection;
  quint16 	usFirstCharIndex;
  quint16 	usLastCharIndex;
  qint16  	sTypoAscender;
  qint16  	sTypoDescender;
  qint16  	sTypoLineGap;
  quint16 	usWinAscent;
  quint16 	usWinDescent;
  quint32 	ulCodePageRange1;
  quint32 	ulCodePageRange2;
  qint16  	sxHeight;
  qint16  	sCapHeight;
  quint16 	usDefaultChar;
  quint16 	usBreakChar;
  quint16 	usMaxContext;
};


#endif // _AEASY_TTF_STRUCTS_H_
