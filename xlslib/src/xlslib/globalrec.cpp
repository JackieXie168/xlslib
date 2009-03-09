/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * This file is part of xlslib -- A multiplatform, C/C++ library
 * for dynamic generation of Excel(TM) files.
 *
 * xlslib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * xlslib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with xlslib.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Copyright 2004 Yeico S. A. de C. V.
 * Copyright 2008 David Hoerl
 *  
 * $Source: /cvsroot/xlslib/xlslib/src/xlslib/globalrec.cpp,v $
 * $Revision: 1.12 $
 * $Author: dhoerl $
 * $Date: 2009/03/02 04:08:43 $
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * File description:
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <globalrec.h>

using namespace std;
using namespace xlslib_core;

#if 0
static CGlobalRecords	*m_pGlobalRecords;
#endif

/*
**********************************************************************
CGlobalRecords class implementation
**********************************************************************
*/
#if 0
CGlobalRecords& CGlobalRecords::Instance()
{
	if(m_pGlobalRecords == NULL)
		m_pGlobalRecords = new CGlobalRecords();
	
	return *m_pGlobalRecords;
}
#endif

CGlobalRecords::CGlobalRecords() :
	m_Fonts(),
	m_DefaultFonts(),
	m_Formats(),
	m_XFs(),
	m_DefaultXFs(),
	m_Styles(),
	m_BoundSheets(),
	m_window1(),
	m_palette(),

	defaultXF(NULL),
	  
#ifdef HAVE_ICONV
	iconv_code(),
#endif
	m_DumpState(GLOBAL_INIT),
	font(),
	font_dflt(),
	fontIndex(0),
	format(),
	formatIndex(0),
	xf(),
	xf_dflt(),
	xfIndex(0),
	style(),
	bsheet()
{
	// set to what Excel 2004 on Mac outputs 12/12/2008

	// Initialize default fonts
	font_t *newfont, *font0, *font1, *font2, *font4;

	newfont = new font_t(*this, 0, "Verdana", 200, BOLDNESS_NORMAL, UNDERLINE_NONE, SCRIPT_NONE, ORIG_COLOR_BLACK, FONT_DFLT_ATTRIBUTES, FONT_DFLT_FAMILY, FONT_DFLT_CHARSET);
	newfont->MarkUsed(), newfont->MarkUsed();
	m_DefaultFonts.push_back(newfont);
	font0 = newfont;

	newfont = new font_t(*this, 1, "Verdana", 200, BOLDNESS_BOLD, UNDERLINE_NONE, SCRIPT_NONE, ORIG_COLOR_BLACK, FONT_ATTR_BOLD, FONT_DFLT_FAMILY, FONT_DFLT_CHARSET);
	newfont->MarkUsed(), newfont->MarkUsed();
	m_DefaultFonts.push_back(newfont);
	font1 = newfont;

	newfont = new font_t(*this, 2, "Verdana", 200, BOLDNESS_NORMAL, UNDERLINE_NONE, SCRIPT_NONE, ORIG_COLOR_BLACK, FONT_ATTR_ITALIC, FONT_DFLT_FAMILY, FONT_DFLT_CHARSET);
	newfont->MarkUsed(), newfont->MarkUsed();
	m_DefaultFonts.push_back(newfont);
	font2 = newfont;

	newfont = new font_t(*this, 3, "Verdana", 200, BOLDNESS_BOLD, UNDERLINE_NONE, SCRIPT_NONE, ORIG_COLOR_BLACK, FONT_ATTR_BOLD|FONT_ATTR_ITALIC, FONT_DFLT_FAMILY, FONT_DFLT_CHARSET);
	newfont->MarkUsed(), newfont->MarkUsed();
	m_DefaultFonts.push_back(newfont);

	// Excel spec for FONT says ignore 4
	newfont = new font_t(*this, 5, "Verdana", 200, BOLDNESS_NORMAL, UNDERLINE_NONE, SCRIPT_NONE, ORIG_COLOR_BLACK, FONT_DFLT_ATTRIBUTES, FONT_DFLT_FAMILY, FONT_DFLT_CHARSET);
	newfont->MarkUsed(), newfont->MarkUsed();
	m_DefaultFonts.push_back(newfont);
	font4 = newfont;

	fontIndex = 6;	// this will be 1 more than last standard font

	for(xfIndex=0; xfIndex<21; ++xfIndex)
	{
		xf_t*			newxf;
		font_t			*fnt;
		format_number_t	fmt;
		bool			is_cell;
		
		fnt		= font0;
		fmt		= FMT_GENERAL;
		is_cell	= XF_IS_STYLE;

		switch(xfIndex) {
		case 0:
			fnt		= NULL;
			break;
		case 1:
		case 2:
			fnt		= font1;
			break;
		case 3:
		case 4:
			fnt		= font2;
			break;
		case 15:
			fnt		= NULL;
			is_cell	= XF_IS_CELL;
			break;
		case 16:
			fnt		= font4;
			fmt		= FMT_CURRENCY7;
			break;
		case 17:
			fnt		= font4;
			fmt		= FMT_CURRENCY5;
			break;
		case 18:
			fnt		= font4;
			fmt		= FMT_CURRENCY8;
			break;
		case 19:
			fnt		= font4;
			fmt		= FMT_CURRENCY6;
			break;
		case 20:
			fnt		= font4;
			fmt		= FMT_PERCENT1;
			break;
		}

//		newxf = is_cell == XF_IS_CELL ? new xf_t(false) : new xf_t(xfi, false/*userXF*/, is_cell, xfIndex?false:true);
		newxf = new xf_t(*this, false/*userXF*/, is_cell, xfIndex?false:true);
				
		// override defaults
		if(fnt != NULL)			newxf->SetFont(fnt);
		if(fnt == font4)		newxf->ClearFlag(XF_ALIGN_ATRFONT);	// Ask Mr Bill why...Done to make binary the same
		if(fmt != FMT_GENERAL)	newxf->SetFormat(fmt);
		
		newxf->MarkUsed(), newxf->MarkUsed();
		m_DefaultXFs.push_back(newxf);
		
		if(xfIndex == XF_PROP_XF_DEFAULT_CELL) {
			newxf->SetIndex(XF_PROP_XF_DEFAULT_CELL);
			defaultXF = newxf;
		}
		newxf->SetIndex(xfIndex);	// for debugging - not really needed here
	}

	style_t* newstyle;
	newstyle = new style_t;
	newstyle->xfindex = 0x0010;
	newstyle->builtintype = 0x03;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	newstyle = new style_t;
	newstyle->xfindex = 0x0011; 
	newstyle->builtintype = 0x06;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	newstyle = new style_t;
	newstyle->xfindex = 0x0012;
	newstyle->builtintype = 0x04;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	newstyle = new style_t;
	newstyle->xfindex = 0x0013;
	newstyle->builtintype = 0x07;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	newstyle = new style_t;
	newstyle->xfindex = 0x0000;
	newstyle->builtintype = 0x00;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	newstyle = new style_t;
	newstyle->xfindex = 0x0014;
	newstyle->builtintype = 0x05;
	newstyle->level = 0xFF;
	m_Styles.push_back(newstyle);

	// Initialize former static variables
	font	= m_Fonts.begin();        
	format	= m_Formats.begin();    
	xf		= m_XFs.begin();            
	style	= m_Styles.begin();      
	bsheet	= m_BoundSheets.begin();
}
/*
****************************************
****************************************
*/
CGlobalRecords::~CGlobalRecords()
{
   // Delete dynamically created lists elements
   
   if(!m_DefaultFonts.empty())
   {
      for(Font_Vect_Itor_t fnt = m_DefaultFonts.begin(); fnt != m_DefaultFonts.end(); fnt++)
         delete *fnt;
      m_DefaultFonts.clear();
   }
   if(!m_Fonts.empty())
   {
      for(Font_Vect_Itor_t fnt = m_Fonts.begin(); fnt != m_Fonts.end(); fnt++)
         delete *fnt;
      m_Fonts.clear();
   }
   if(!m_Formats.empty())
   {
      for(Format_Vect_Itor_t fnt = m_Formats.begin(); fnt != m_Formats.end(); fnt++)
         delete *fnt;
      m_Formats.clear();
   }
   if(!m_DefaultXFs.empty())   
   {
      for(XF_Vect_Itor_t xfi = m_DefaultXFs.begin(); xfi != m_DefaultXFs.end(); xfi++)
         delete *xfi;
      m_DefaultXFs.clear();
   }
   if(!m_XFs.empty())   
   {
      for(XF_Vect_Itor_t xfi = m_XFs.begin(); xfi != m_XFs.end(); xfi++)
         delete *xfi;
      m_XFs.clear();
   }
   if(!m_Styles.empty())   
   {
      for(Style_Vect_Itor_t xfi = m_Styles.begin(); xfi != m_Styles.end(); xfi++)
         delete *xfi;
      m_Styles.clear();
   }

   if(!m_BoundSheets.empty())
   {
      for(Boundsheet_Vect_Itor_t xfi = m_BoundSheets.begin(); xfi != m_BoundSheets.end(); xfi++)
         delete *xfi;
      m_BoundSheets.clear();
   }
}
#if 0
void CGlobalRecords::Clean()
{	
	if(m_pGlobalRecords == NULL) return;

	delete m_pGlobalRecords;
	m_pGlobalRecords = NULL;
}
#endif
/*
****************************************
****************************************
*/
CUnit* CGlobalRecords::DumpData()
{
	CUnit*	m_pCurrentData	= NULL;
	bool	repeat			= false;

	XTRACE("CGlobalRecords::DumpData");

	do
	{
	  switch(m_DumpState)
	  {
         case GLOBAL_INIT:
            XTRACE("\tINIT");

            repeat = true;

            font		= m_Fonts.begin();        
            font_dflt	= m_DefaultFonts.begin();
            format		= m_Formats.begin();    
            xf			= m_XFs.begin();            
            xf_dflt		= m_DefaultXFs.begin();            
            style		= m_Styles.begin();      
            bsheet		= m_BoundSheets.begin();

            m_DumpState = GLOBAL_BOF;
            break;

         case GLOBAL_BOF:			// ********** STATE 1A *************
            XTRACE("\tBOF");

            repeat = false;

            //MSVC
            // Delete_Pointer(m_pCurrentData);
            m_pCurrentData = (CUnit*)(new CBof(BOF_TYPE_WBGLOBALS));
            m_DumpState = GLOBAL_CODEPAGE; // DFH GLOBAL_WINDOW1;
            break;

         case GLOBAL_CODEPAGE:		// ********** STATE 1B *************
           XTRACE("\tCODEPAGE");
 
			repeat = false;

            //Delete_Pointer(m_pCurrentData);
            m_pCurrentData = (CUnit*)(new CCodePage(1200));	// UTF-16
            m_DumpState = GLOBAL_WINDOW1;
            break;

         case GLOBAL_WINDOW1:		// ********** STATE 2A *************
            XTRACE("\tWINDOW1");
 
			repeat = false;

            //Delete_Pointer(m_pCurrentData);
			m_pCurrentData = (CUnit*)(new CWindow1(m_window1));
            m_DumpState = GLOBAL_DATEMODE; // GLOBAL_DEFAULTFONTS;
            break;

         case GLOBAL_DATEMODE:		// ********** STATE 2B *************
            XTRACE("\tDATEMODE");
 
			repeat = false;

            //Delete_Pointer(m_pCurrentData);
            m_pCurrentData = (CUnit*)(new CDateMode());
            m_DumpState = GLOBAL_DEFAULTFONTS;
            break;

         case GLOBAL_DEFAULTFONTS:	// ********** STATE 3A *************
			XTRACE("\tDEFAULTFONTS");
			
			repeat = false;
			
			m_pCurrentData = (CUnit*)(new CFont(*font_dflt));

			if(font_dflt != (--m_DefaultFonts.end()))
			{
			   // if it wasn't the last font from the list, increment to get the next one
			   font_dflt++;
			} else {
			   // if it was the last from the list, change the DumpState
			   m_DumpState = GLOBAL_FONTS;
			  // font_dflt = m_DefaultFonts.begin();
			}
			break;

         case GLOBAL_FONTS:// ********** STATE 3B *************
			XTRACE("\tFONTS");
			// First check if the list of fonts is not empty...
			if(!m_Fonts.empty())
			{
				m_pCurrentData = (CUnit*)(new CFont(*font));
				if(font != (--m_Fonts.end()))
				{
				  // if it was'nt the last font from the list, increment to get the next one
				  font++;
				} else {
				  // if it was the last from the list, change the DumpState
				  m_DumpState = GLOBAL_FORMATS;
				  font = m_Fonts.begin();
				}
				repeat = false;
			} else {
				// if the list is empty, change the dump state.
				m_DumpState = GLOBAL_FORMATS;
				//font = m_Fonts.begin();
				repeat = true;
			}
			break;

         case GLOBAL_FORMATS: // ********** STATE 4 *************
			XTRACE("\tFORMATS");

			if(!m_Formats.empty())
			{
				m_pCurrentData = (CUnit*)(new CFormat(*format));
				if(format != (--m_Formats.end()))
				{
				  // if it was'nt the last font from the list, increment to get the next one
				  format++;
				} else {
				  // if it was the last from the list, change the DumpState
				  m_DumpState = GLOBAL_DEFAULTXFS;
				  format = m_Formats.begin();
				}
				repeat = false;
			} else {
			   // if the list is empty, change the dump state.
			   m_DumpState = GLOBAL_DEFAULTXFS;
			   // format = m_Formats.begin();
			   repeat = true;
			}
			break;

         case GLOBAL_DEFAULTXFS: // ********** STATE 5a *************

            XTRACE("\tXDEFAULTFS");
            m_pCurrentData = (CUnit*)(new CExtFormat(*xf_dflt));

            if(xf_dflt != (--m_DefaultXFs.end()))
            {
			   // if it was'nt the last font from the list, increment to get the next one
               xf_dflt++;
               repeat = false;
            } else {
			   // if it was the last from the list, change the DumpState
               m_DumpState = GLOBAL_XFS;
               //xf_dflt = m_DefaultXFs.begin();
			   repeat = false;
            } 
            break;

         case GLOBAL_XFS: // ********** STATE 5b *************
			XTRACE("\tXFS");
			if(!m_XFs.empty())
			{
				m_pCurrentData = (CUnit*)(new CExtFormat(*xf));

				if(xf != (--m_XFs.end()))
				{
				  // if it wasn't the last font from the list, increment to get the next one
				  xf++;
				} else {   
				  // if it was the last from the list, change the DumpState
				  m_DumpState = GLOBAL_STYLES;
				  xf = m_XFs.begin();
				}
				repeat = false;
			} else {
			   // if the list is empty, change the dump state.
			   m_DumpState = GLOBAL_STYLES;
			   //xf = m_XFs.begin();
			   repeat = true;
			}
			break;

         case GLOBAL_STYLES: // ********** STATE 6 *************
            XTRACE("\tSTYLES");

            if(!m_Styles.empty())
            {
				// First check if the list of fonts is not empty...
				//Delete_Pointer(m_pCurrentData);
				m_pCurrentData = (CUnit*)(new CStyle(*style));

				if(style != (--m_Styles.end()))
				{
				  // if it was'nt the last font from the list, increment to get the next one
				  style++;
				} else {
				 // if it was the last from the list, change the DumpState
				  m_DumpState = GLOBAL_PALETTE;
				  //style = m_Styles.begin();
				}
				repeat = false;
            } else {
               // if the list is empty, change the dump state.
               m_DumpState = GLOBAL_PALETTE;
               //style = m_Styles.begin();
               repeat = true;
            }
            break;

         case GLOBAL_PALETTE: // ********** STATE 7 *************
            XTRACE("\tPALETTE");
 
			repeat = false;

            //Delete_Pointer(m_pCurrentData);
           // m_pCurrentData = (CUnit*)(new CPalette());
            m_pCurrentData = m_palette.GetData();
            m_DumpState = GLOBAL_BOUNDSHEETS;
            break;

         case GLOBAL_BOUNDSHEETS: // ********** STATE 8 *************
			XTRACE("\tBOUNDSHEETS");
			if(!m_BoundSheets.empty())
			{
				// First check if the list of sheets is not empty...
				//Delete_Pointer(m_pCurrentData);
				m_pCurrentData = (CUnit*)(new CBSheet(*bsheet));
				(*bsheet)->sheetData = (CBSheet *)m_pCurrentData;

				if(bsheet != (--m_BoundSheets.end()))
				{
				  // if it wasn't the last sheet from the list, increment to get the next one
				  bsheet++;
				} else {
				  // if it was the last from the list, change the DumpState
				  m_DumpState = GLOBAL_EOF;
				  bsheet = m_BoundSheets.begin();
				}
				repeat = false;
			} else {
			   // if the list is empty, change the dump state.
			   m_DumpState = GLOBAL_EOF;
			   bsheet = m_BoundSheets.begin();
			   repeat = true;
			}
			break;

         case GLOBAL_EOF:// ********** STATE 9 *************
            XTRACE("\tEOF");

            repeat = false;

            m_pCurrentData = (CUnit*)(new CEof());
            m_DumpState = GLOBAL_FINISH;
            break;

         case GLOBAL_FINISH: // ********** STATE 10 *************
            XTRACE("\tFINISH");

            repeat = false;

            m_pCurrentData = NULL;
            m_DumpState = GLOBAL_INIT;
            break;

         default:
            /* It shouldn't get here */
            XTRACE("\tDEFAULT");
            break;
      }
   } while(repeat);

   return m_pCurrentData;
}


/*
****************************************
****************************************
*/
#if 0
void CGlobalRecords::AddBoundingSheet(unsigned32_t streampos,
                                      unsigned16_t attributes,
                                      string& sheetname)
{

   boundsheet_t* bsheetdef = new boundsheet_t;

   bsheetdef->worksheet  = (bool)((attributes & BSHEET_ATTR_WORKSHEET  ) == BSHEET_ATTR_WORKSHEET );
   bsheetdef->ex4macro   = (bool)((attributes & BSHEET_ATTR_EX4MACRO   ) == BSHEET_ATTR_EX4MACRO  );
   bsheetdef->chart      = (bool)((attributes & BSHEET_ATTR_CHART      ) == BSHEET_ATTR_CHART     );
   bsheetdef->vbmodule   = (bool)((attributes & BSHEET_ATTR_VBMODULE   ) == BSHEET_ATTR_VBMODULE  );
   bsheetdef->visible    = (bool)((attributes & BSHEET_ATTR_VISIBLE    ) == BSHEET_ATTR_VISIBLE   );
   bsheetdef->hidden     = (bool)((attributes & BSHEET_ATTR_HIDDEN     ) == BSHEET_ATTR_HIDDEN    );
   bsheetdef->veryhidden = (bool)((attributes & BSHEET_ATTR_VERYHIDDEN ) == BSHEET_ATTR_VERYHIDDEN);

   bsheetdef->asheetname = sheetname;
   bsheetdef->streampos  = streampos;

   m_BoundSheets.push_back(bsheetdef);

}
#endif
void CGlobalRecords::AddBoundingSheet(unsigned32_t streampos,
                                      unsigned16_t attributes,
                                      u16string& sheetname)
{
   boundsheet_t* bsheetdef = new boundsheet_t;

   bsheetdef->worksheet  = (bool)((attributes & BSHEET_ATTR_WORKSHEET  ) == BSHEET_ATTR_WORKSHEET );
   bsheetdef->ex4macro   = (bool)((attributes & BSHEET_ATTR_EX4MACRO   ) == BSHEET_ATTR_EX4MACRO  );
   bsheetdef->chart      = (bool)((attributes & BSHEET_ATTR_CHART      ) == BSHEET_ATTR_CHART     );
   bsheetdef->vbmodule   = (bool)((attributes & BSHEET_ATTR_VBMODULE   ) == BSHEET_ATTR_VBMODULE  );
   bsheetdef->visible    = (bool)((attributes & BSHEET_ATTR_VISIBLE    ) == BSHEET_ATTR_VISIBLE   );
   bsheetdef->hidden     = (bool)((attributes & BSHEET_ATTR_HIDDEN     ) == BSHEET_ATTR_HIDDEN    );
   bsheetdef->veryhidden = (bool)((attributes & BSHEET_ATTR_VERYHIDDEN ) == BSHEET_ATTR_VERYHIDDEN);

   bsheetdef->sheetname = sheetname;
   bsheetdef->streampos  = streampos;

   m_BoundSheets.push_back(bsheetdef);
}

/*
****************************************
****************************************
*/
void CGlobalRecords::AddBoundingSheet(boundsheet_t* bsheetdef)
{
   m_BoundSheets.push_back(bsheetdef);
}


/*
****************************************
It returns pointers to BoundingSheets one by one until
all are spanned, in which case the returned pointer is NULL
****************************************
*/
void CGlobalRecords::GetBoundingSheets(Boundsheet_Vect_Itor_t& bs)
{
   if(bs != m_BoundSheets.end())
      bs++;
   else
      bs = m_BoundSheets.begin();
}


Boundsheet_Vect_Itor_t CGlobalRecords::GetFirstBoundSheet()
{
   return m_BoundSheets.begin();
}

Boundsheet_Vect_Itor_t CGlobalRecords::GetBoundSheetAt(unsigned32_t idx)
{
	Boundsheet_Vect_Itor_t bs;

	bs = m_BoundSheets.begin();
	while(idx--) bs++;
	
	return bs;
}

Boundsheet_Vect_Itor_t CGlobalRecords::GetEndBoundSheet()
{
   return m_BoundSheets.end();
}

/*
****************************************
****************************************
*/
void CGlobalRecords::AddFormat(format_t* newformat)
{
	newformat->SetIndex(formatIndex++);
	m_Formats.push_back(newformat);
}

/*
****************************************
****************************************
*/
void CGlobalRecords::AddFont(font_t* newfont )
{
	newfont->SetIndex(fontIndex++);
	m_Fonts.push_back(newfont);
}
font_t* CGlobalRecords::GetDefaultFont() const
{
   return *m_DefaultFonts.begin();
}

/*
****************************************
****************************************
*/
unsigned16_t CGlobalRecords::AddXFormat(xf_t* xfi)
{
	m_XFs.push_back(xfi);
	
	return xfIndex++;	// how xf_t's get their index
}

/*
****************************************
****************************************
*/
bool CGlobalRecords::SetColor(unsigned8_t r, unsigned8_t g, unsigned8_t b, unsigned8_t idx)
{
	return m_palette.setColor(r, g, b, idx);
}

/*
****************************************
****************************************
*/
xf_t* CGlobalRecords::GetDefaultXF() const
{
	return defaultXF;
}
/*
***********************************
***********************************
*/
font_t* CGlobalRecords::fontdup(unsigned8_t fontnum) const
{
   return font_t::fontDup(m_DefaultFonts[fontnum]);
}
/*
***********************************
***********************************
*/
#ifdef HAVE_ICONV
void  CGlobalRecords::wide2str16(const ustring& str1, u16string& str2)
{
	size_t					resultSize, inbytesleft, outbytesleft;
	const wchar_t			*inbuf;
	iconv_t					cd;
	unsigned16_t			*outbuf, *origOutbuf;
	static unsigned16_t		convFail[] = { 'i', 'c', 'o', 'n', 'v', ' ', 'f', 'a', 'i', 'l', 'e', 'd', '!' };
	
	cd = iconv_open("UCS-2-INTERNAL", iconv_code.c_str());
	// no need to test return code as we ALREADY did this when setting iconv_code in workbook
	
	inbytesleft		= str1.size() * sizeof(uchar_t);
	outbytesleft	= inbytesleft * 4 * sizeof(unsigned16_t); // Unicode expansion 4 to 2 ???

	inbuf		= str1.c_str();
	origOutbuf	= (unsigned16_t *)calloc(outbytesleft, 1);
	outbuf		= origOutbuf;

	resultSize = iconv(cd, (char **)&inbuf, &inbytesleft, (char **)&outbuf, &outbytesleft);
	iconv_close(cd);

	if(resultSize == (size_t)-1) {
		str2 = convFail;
	} else {
		str2.assign(origOutbuf, outbuf - origOutbuf);
	}
	return;
}
#else
void  CGlobalRecords::wide2str16(const ustring& str1, u16string& str2)
{
	ustring::const_iterator	cBegin, cEnd;
	size_t	len;

	// if character size of both strings the same, well, we should be able to just assign them
	if(sizeof(uchar_t) == sizeof(unsigned16_t)) {
		str2 = str1;
		return;
	}
	
	len = str1.length();
	str2.reserve(len);

	cBegin	= str1.begin();
	cEnd	= str1.end();
	
	while(cBegin != cEnd) {
		str2.push_back((unsigned16_t)*cBegin++);		
	}
	return;
}
#endif
void  CGlobalRecords::char2str16(const string& str1, u16string& str2)
{
	string::const_iterator	cBegin, cEnd;
	size_t	len;
	
	len = str1.length();
	str2.reserve(len);

	cBegin	= str1.begin();
	cEnd	= str1.end();
	
	while(cBegin != cEnd) {
		str2.push_back((unsigned16_t)*cBegin++);		
	}
	return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * $Log: globalrec.cpp,v $
 * Revision 1.12  2009/03/02 04:08:43  dhoerl
 * Code is now compliant to gcc  -Weffc++
 *
 * Revision 1.11  2009/01/23 16:09:55  dhoerl
 * General cleanup: headers and includes. Fixed issues building mainC and mainCPP
 *
 * Revision 1.10  2009/01/09 15:04:26  dhoerl
 * GlobalRec now used only as a reference.
 *
 * Revision 1.9  2009/01/09 03:23:12  dhoerl
 * GlobalRec references tuning
 *
 * Revision 1.8  2009/01/08 22:16:06  dhoerl
 * January Rework
 *
 * Revision 1.7  2009/01/08 02:52:59  dhoerl
 * December Rework
 *
 * Revision 1.6  2008/12/20 15:49:05  dhoerl
 * 1.2.5 fixes
 *
 * Revision 1.5  2008/12/11 21:12:40  dhoerl
 * Cleanup
 *
 * Revision 1.4  2008/12/10 03:34:31  dhoerl
 * m_usage was 16bit and wrapped
 *
 * Revision 1.3  2008/12/06 01:42:57  dhoerl
 * John Peterson changes along with lots of tweaks. Many bugs that causes Excel crashes fixed.
 *
 * Revision 1.2  2008/10/25 18:39:54  dhoerl
 * 2008
 *
 * Revision 1.1.1.1  2004/08/27 16:31:56  darioglz
 * Initial Import.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

