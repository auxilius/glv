#ifndef H_GLD_CONSTANTS
#define H_GLD_CONSTANTS

#define CONFIG_FILE_VERSION "v 1.14.11." 

#define PI 3.141528

#define FILE_EXT_CONFIG			".conf"
#define FILE_PROFILE_DEFAULT	"default.conf"
#define FILE_PROFILES			"profiles.txt"
#define FILE_FONT				"font_arial.glf"

#define PROFILE_CAPTION_DEFAULT		"<default>"
#define PROFILE_CAPTION_ADDNEW		"Create New Profile"

#define FIELD_TYPE_NONE		0
#define FIELD_TYPE_TEXTURE	1
#define FIELD_TYPE_MODEL	2
#define FIELD_TYPE_VALUE	3

#define COLOR_MAP_RAINBOW	0
#define COLOR_MAP_BLUERED	1

#define FIELD_SCALEHANDLE_SIZE	8
#define FIELD_MIN_SIZE_TO_SHOW	30
#define FIELD_MIN_SIZE			90


////   ERRORS   ////

#define ERRORTEXT_HEADER "GLD Error"
#define ERRORTEXT_FONT_MISSING "Font cannot be loaded from working directory, font file is probably missing. Place the font file into working directory to prevent rendering errors."
#define ERRORTEXT_WORKDIR_DONT_EXIST "Working directory set by gldInit() function does not exist. Already existing directory needs to be set."
#define ERRORTEXT_SHADERFILE_DONT_EXIST "One or more shader files does not exist."
#define ERRORTEXT_SHADER_COMPILE_ERROR "There were some errors compiling shader-> See console output for more specific info."
#define ERRORTEXT_CANNOT_SWITCH_MODE "Create at least one field and select its type to proceed to visualization mode."


//   SPECIAL KEYS   //

#define KEY_BACKSPACE	8
#define KEY_ENTER		13
#define KEY_SHIFT		16
#define KEY_ESCAPE		27
#define KEY_SPACE		32

#endif