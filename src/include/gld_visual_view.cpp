#include "gld_visual_view.h"
#include "gld_drawing.h"

void View::setBorder(Box newBorder) {
	border = newBorder;
};
void View::drawBackground(float r, float g, float b) {
	glColor3f(r, g, b);
	drawRect(GL_QUADS, border);
};
void View::drawBorder(float r, float g, float b) {
	glLineWidth(1.0f);
	glColor3f(r, g, b);
	drawRect(GL_LINE_LOOP, border);
};
