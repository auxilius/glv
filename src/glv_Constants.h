#pragma once
#ifndef GLV_H_CONSTANTS
#define GLV_H_CONSTANTS

//#define CONFIG_FILE_VERSION "v 1.14.11." 
#define CONFIG_FILE_VERSION "v 1.15.5" 

#define PI 3.141528

#define GLV_FILE_EXT_CONFIG			".conf"
#define GLV_FILE_PROFILE_DEFAULT	"default.conf"
#define GLV_FILE_PROFILES			"profiles.txt"

#define GLV_FIELD_TYPE_NONE		0
#define GLV_FIELD_TYPE_TEXTURE	1
#define GLV_FIELD_TYPE_MODEL	2
#define GLV_FIELD_TYPE_VALUE	3

#define GLV_COLOR_MAP_RAINBOW	0
#define GLV_COLOR_MAP_BLUERED	1

#define GLV_FIELD_SCALEHANDLE_SIZE	8
#define GLV_FIELD_MIN_SIZE_TO_SHOW	30
#define GLV_FIELD_MIN_SIZE			90

#define GLV_MENU_HEIGHT	30



////   ERRORS   ////

#define GLV_ERRORTEXT_HEADER "GLViewer Library Error"


//   SPECIAL KEYS   //

#define GLV_KEY_BACKSPACE	8
#define GLV_KEY_ENTER		13
#define GLV_KEY_SHIFT		16
#define GLV_KEY_ESCAPE		27
#define GLV_KEY_SPACE		32

#endif