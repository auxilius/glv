#pragma once 
#ifndef _H_FONT_ 
#define _H_FONT_

#include <windows.h>
#include <string>
#include <gl/glew.h>


enum FontFamily { 
	ffArial, ffVerdana, ffTimes, ffCourier, ffConsolas 
};

enum FontAlign {
	faLeft, faCenter, faRight
};

class Font {
private:
	int mySize;
	HFONT myFont;
	HDC myDC;
	GLuint myBitmapList;
	
	void glPrintText( std::string Text );
public:
	Font( HDC context, FontFamily family = ffArial, int size = 12, bool bold = false, bool italic = false);
	~Font();

	void textOut( int x, int y, std::string text , FontAlign align = faLeft);
	void textOutNormal( float x, float y, std::string text );

	int getWidth(std::string text);
	int getHeight();
	
	int getSize();
	bool isBold();
	bool isItalic();
};

#endif