#ifndef H_GLD_TYPES
#define H_GLD_TYPES

#include <windows.h>
#include <algorithm>
#include <vector>
#include <gl/glew.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include "gld_constants.h"


struct Point {
	int x, y;
	void set(int sx, int sy);
};
Point point(int x, int y);
double distance(float x1, float y1, float x2, float y2);
double pointDistance(Point p1, Point p2);

class Box {
public:
	int top, left, bottom, right;
	void set(Box border);
	void set(int x1, int y1, int x2, int y2);
	void set(int x, int y, int size);
	bool contains(int x, int y);
	bool contains(Point position);
	void move(int vx, int vy);
	void moveTo(int x, int y);
	int getWidth();
	int getHeight();
	void setWidth(int newWidth);
	void setHeight(int newHeight);
	void setSize(int newWidth, int newHeight);
	Point getPosition();
	Point getCenterPosition();
	__declspec(property(get = getCenterPosition)) Point center;
	__declspec(property(get = getWidth, put = setWidth)) int width;
	__declspec(property(get = getHeight, put = setHeight)) int height;
};

class IOBox : public Box{
public:
	void save(std::ofstream & stream);
	void load(std::ifstream & stream);
};

extern Box canvas;
extern IOBox form;


/*****     M O U S E   I N P U T   M A N A G E R     *****/

enum mouseButton { mbLeft = 0, mbMiddle = 1, mbRight = 2 };
enum mouseEventType { EVENT_MOUSE_DOWN, EVENT_MOUSE_UP, EVENT_MOUSE_MOVE };

typedef void(*funcMousePosBtn)(int, int, mouseButton);
typedef void(*funcMousePos)(int, int);

struct inputManager {
private:
	bool mouseButtonPressed[3];
	Point mousePosition;
	
	std::vector<funcMousePosBtn> listenerPosBtnDown;
	std::vector<funcMousePosBtn> listenerPosBtnUp;
	std::vector<funcMousePos> listenerPos;
public:
	Point getMousePosition() {
		return mousePosition;
	};
	__declspec(property(get = getMousePosition)) Point mouse;
	bool mousePressed(mouseButton button) {
		return mouseButtonPressed[button];
	};
	void setMousePosition(int x, int y){
		if (mousePosition.x == x && mousePosition.y == y)
			return;
		mousePosition.x = x; mousePosition.y = y;
		for (unsigned i = 0; i < listenerPos.size(); i++)
			listenerPos[i](x, y);
	};
	void setMouseButtonDown(mouseButton button) {
		mouseButtonPressed[button] = true;
		for (unsigned i = 0; i < listenerPosBtnDown.size(); i++)
			listenerPosBtnDown[i](mousePosition.x, mousePosition.y, button);
	};
	void setMouseButtonUp(mouseButton button) {
		mouseButtonPressed[button] = false;
		for (unsigned i = 0; i < listenerPosBtnUp.size(); i++)
			listenerPosBtnUp[i](mousePosition.x, mousePosition.y, button);
	};
	void addEventOnMouseUp(funcMousePosBtn function) {
		listenerPosBtnUp.push_back(function);
	};
	void addEventOnMouseDown(funcMousePosBtn function) {
		listenerPosBtnDown.push_back(function);
	};
	void addEventOnMouseMove(funcMousePos function) {
		listenerPos.push_back(function);
	};
};
extern inputManager input;

bool fileExists(char* fileName);


/*****    F I E L D   C O N F I G U R A T I O N     *****/
struct FieldConfigRecord {
	FieldConfigRecord();
	IOBox border;
	unsigned type;
	std::vector<int> param;
	std::string paramText;
	void save(std::ofstream & stream);
	void load(std::ifstream & stream);
};
class FieldConfig {
	bool loaded;
public:
	std::vector<FieldConfigRecord> field;
	bool valid();
	void save();
	void load();
	void clear();
	bool fieldSetType(int f, int t);
	void debugOut();
};
extern FieldConfig configuration;


#endif