#ifndef __H_GLD_VIEW
#define __H_GLD_VIEW

#include "gld_types.h"

class View {
public:
	View(){};
	Box border;
	unsigned layer;
	void drawBackground(float r, float g, float b);
	void drawBorder(float r, float g, float b);
	void setBorder(Box newBorder);
};

#endif