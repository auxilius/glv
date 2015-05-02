#pragma once
#ifndef GLV_H_INTERFACE
#define GLV_H_INTERFACE

#include <string>
#include <vector>
#include <array>
#include <gl/glew.h>
#include <functional>

#include "gld_constants.h"
#include "gld_types.h"
#include "gld_drawing.h"

namespace Interface {

	const std::string NULL_ITEM = "";
	typedef std::function<void(mouseButton)> EventOnClick;
	typedef std::function<void(bool)> EventOnStateChange;
	typedef std::function<void(int,std::string)> EventOnMenuClick;
	typedef std::function<void(int,std::string,bool)> EventOnMenuStateChange;


	class Rect {
	public:
		Rect();
		// state
		virtual bool isHovered();
		// event notifiers
		virtual void onMouseDown(mouseButton button);
		virtual void onRender(bool alsoHoverColors = true);
	public:
		// callback pointers
		EventOnClick callOnClick;
		// properties
		bool visible;
		GLfloat outlineWidth;
		// colors
		Color *colorBack;
		Color *colorBackHover;
		Color *colorBorder;
		Color *colorBorderHover;
	public:
		// POSITION
		void setPosition(int x, int y);
		Point getPosition();
		__declspec(property(get = getPosition)) Point position;
		// WIDTH
		void setWidth(int newWidth);
		int getWidth();
		__declspec(property(get = getWidth, put = setWidth)) int width;
		// HEIGHT
		void setHeight(int newHeight);
		int getHeight();
		__declspec(property(get = getHeight, put = setHeight)) int height;
		// SIZE (WIDTH + HEIGHT)
		void setSize(int w, int h);
	protected:
		Box border;
		virtual void loadDefaultColors();
		virtual void drawBorder(bool alsoHoverColors);
	};
	

	/***   BUTTONS   ***/

	class Button : public Rect {
	public:
		// colors
		Color *colorText;
		Color *colorTextHover;
		// constructor
		Button();
		// event notifiers
		virtual void onRender(bool alsoHoverColors = true);
		// FONT
		Font* getFont();
		void setFont(Font *newFont);
		__declspec(property(get = getCaption, put = setCaption)) std::string font;
		// CAPTION
		std::string getCaption();
		void setCaption(std::string newCaption);
		__declspec(property(get = getCaption, put = setCaption)) std::string caption;
	protected:
		virtual void loadDefaultColors();
		virtual void drawText(bool alsoHoverColors);
	private:
		std::string textOnButton;
		Font *myFont;
		void fitSizeToText();
	};


	class CheckButton : public Button {
	public:
		EventOnStateChange callOnChange;
		// constructor
		CheckButton();
		// event notifiers
		virtual void onMouseDown(mouseButton button);
		// CHECKED
		void setChecked(bool state);
		bool getChecked();
		__declspec(property(get = getChecked, put = setChecked)) bool checked;
	private:
		bool checkState;
	};


	/***   MENU LISTS   ***/
	
	class Menu : public Rect {
	public:
		// callbacks
		EventOnMenuClick callOnItemClick;
		// constructor
		Menu();
		// event notifiers
		virtual void onMouseDown(mouseButton button);
		virtual void onRender(bool highlightHovered = true);
		// item list manipulation
		int addItem(std::string caption);
		std::string getItemCaption(int itemID);
		void clearAllItems();
		// visibility manipulation
		void show(int x, int y);
		void showAtMousePosition();
		void hide();
	protected:
		// list of items
		std::vector<Button> item;
		// creates an item which cam be added to the list of items
		virtual Button createItem(std::string name);
	};

	
	class CheckMenu : public Menu {
	public:
		// callbacks
		EventOnMenuStateChange callOnItemChange;
		// set the items to the states
		void setCheckedOptions(std::vector<unsigned> * checkedOptions);
		// event notifiers
		virtual void onMouseDown(mouseButton button);
	protected:
		// creates an item which cam be added to the list of items
		virtual Button createItem(std::string name);
	};


	/***   COMBO BOX   ***/

	class Select : public Button {
	public:
		// callback - when value of select was changed
		EventOnMenuClick callOnChange;
		// constructor
		Select();
		// add item to the list
		int addItem(std::string caption);
		// select specified item
		void selectItem(std::string item);
		// clear the list of items
		void clearAllItems();

		// event notifier - when mouse button was pressed
		virtual void onMouseDown(mouseButton button);
		// event notifier - when it is time for rendering
		virtual void onRender(void);
		// if the list is opened and user is selecting item
		bool isOpened();
	private:
		// list of items which can be selected
		Menu menuList;
		// actualy selected item
		int actualSelected;
		// event notifier - when *this button was clicked
		void onClick();
		// event notifier - when any item from the list was selected
		void onItemClick(int id, std::string caption);
	};

	

};

#endif