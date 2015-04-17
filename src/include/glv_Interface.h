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

	typedef std::function<void(void)> ButtonEventCallback;
	typedef std::function<void(bool)> CheckButtonEventCallback;

	class Button {
	public:
		ButtonEventCallback OnClick;
		GLfloat borderWidth;
		struct ButtonColors {
			Color *back;
			Color *hoverBack;
			Color *border;
			Color *hoverBorder;
			Color *text;
			Color *hoverText;
		} color;
		void onMouseDown(mouseButton button);
	public:
		Button();
		void draw(bool alsoHoverColors = true);
		bool isHovered();
		void setFont(Font *f);
		// CAPTION
		std::string getCaption() { return textOnButton; };
		void setCaption(std::string newCaption);
		__declspec(property(get = getCaption, put = setCaption)) std::string caption;
		// POSITION
		void setPosition(int x, int y) { border.moveTo(x, y); };
		Point getPosition() { return point(border.left, border.top); };
		__declspec(property(get = getPosition)) Point position;
		// WIDTH
		void setWidth(int newWidth) { border.setWidth(newWidth); };
		int getWidth() { return border.getWidth(); };
		__declspec(property(get = getWidth, put = setWidth)) int width;
		// HEIGHT
		void setHeight(int newHeight) { border.setHeight(newHeight); };
		int getHeight() { return border.getHeight(); };
		__declspec(property(get = getHeight, put = setHeight)) int height;
		// WIDTH + HEIGHT
		void setSize(int w, int h) { setWidth(w); setHeight(h); };
	protected:
		Font *font;
		Box border;
		std::string textOnButton;
		void loadDefaultColors();
		void recalcSize();
	protected:
		void drawBorder(bool alsoHoverColors);
		void drawText(bool alsoHoverColors);
	};


	class CheckButton : public Button {
	public:
		CheckButtonEventCallback OnChange;
		CheckButton();
		void onMouseDown(mouseButton button);
		// CHECKED
		void setChecked(bool state);
		bool getChecked() { return checkState; };
		__declspec(property(get = getChecked, put = setChecked)) bool checked;
	protected:
		bool checkState;
	};





	#define NULL_ITEM ""

	typedef std::function<void(int,std::string)> MenuClickEvent;
	typedef std::function<void(int,std::string,bool)> MenuChangeEvent;

	class PopupMenu {
	public:
		MenuClickEvent OnItemClick;
		bool checkable;
		PopupMenu();
		PopupMenu(std::vector<std::string> data);
		void onMouseDown(mouseButton button);
		int addItem(std::string caption);
		void clearAllItems();
		void draw(bool highlightHovered = true);
		bool isActive();
		void show(int x = -1, int y = -1);
		void hide();
	protected:
		int minItemWidth;
		Box activeArea, border;
		std::vector<Button> item;
		bool active;
		virtual Button newItem(std::string name);
	};

	
	class CheckPopupMenu : public PopupMenu {
	public:
		MenuChangeEvent OnItemChange;
		void onMouseDown(mouseButton button);
		void setCheckedOptions(std::vector<unsigned> * checkedOptions);
		int checkedItem();
		int uncheckedItem();
	protected:
		Button newItem(std::string name);
	};

	

};

#endif