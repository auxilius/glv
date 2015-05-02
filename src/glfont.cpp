#include "glfont.h"

#include <WTypes.h>
#include "gld_types.h"

LPCSTR getFamilySting(FontFamily family) {
	switch (family)
	{
	case ffArial:
		return "Arial";
	case ffVerdana:
		return "Verdana";
	case ffTimes:
		return "Times New Roman";
	case ffCourier:
		return "Courier New";
	case ffConsolas:
		return "Consolas";
	default:
		return "";
	}
};


Font::Font( HDC context, FontFamily family, int size, bool bold, bool italic ) {
	
	mySize = size;
	myDC = context;

	myFont = CreateFont(  
		-size,                              // height
		0,									// width
		0,                                  // escapement angle
		0,                                  // orientation angle
		(bold) ? 1 : 0,                     // bold
		(italic) ? 1 : 0,					// italic
		0,                                  // underlined
		0,                                  // crossed
		ANSI_CHARSET,                       // charset
		OUT_TT_PRECIS,                      // output precision
		CLIP_DEFAULT_PRECIS,                // clip precision
		ANTIALIASED_QUALITY,                // output quality
		FF_DONTCARE | DEFAULT_PITCH,        // family and pitch
		getFamilySting(family)
	);
	SelectObject( myDC, myFont );
	myBitmapList = glGenLists(96);
	wglUseFontBitmaps( myDC, 32, 96, myBitmapList );
}

Font::~Font() {
	glDeleteLists( myBitmapList, 96 );
}

void Font::glPrintText( std::string Text ) {
	SelectObject( myDC, myFont );
	if ( Text.empty() ) 
		return;
	glPushAttrib( GL_LIST_BIT );
	glListBase(myBitmapList - 32);
	glCallLists( Text.length(), GL_UNSIGNED_BYTE, Text.c_str() );
	glPopAttrib();
}


void Font::textOut( int x, int y, std::string text, FontAlign align)
{
	glPushAttrib( GL_TRANSFORM_BIT | GL_ENABLE_BIT );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );
	glMatrixMode( GL_PROJECTION );
	
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, canvas.width, canvas.height, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	
	glPushMatrix();
	glLoadIdentity();
	GLfloat xPos = (float)x;
	if (align == faCenter)
		xPos -= (float)getWidth(text)/2.0f;
	else if (align == faRight) 
		xPos -= (float)getWidth(text)/2.0f;
	glRasterPos2f( xPos, y + 0.7f * mySize );	
	glPrintText( text );

	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glPopAttrib();
}


void Font::textOutNormal( float x, float y, std::string text ) {
	textOut( (int)(x * canvas.width), (int)(y * canvas.height), text );
}

int Font::getSize() {
	return mySize;
}

int Font::getWidth(std::string text) {
	SelectObject( myDC, myFont );
	int width=0, val=0;
	for (unsigned i=0; i < text.length(); i++) {
		GetCharWidth32(myDC, text[i], text[i], &val);
		width += val;
	}
	return width;
}

int Font::getHeight() {
	return mySize;
}
