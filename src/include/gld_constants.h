#ifndef H_GLD_CONSTANTS
#define H_GLD_CONSTANTS


#define PI 3.141528

#define MENU_HEIGHT 30

#define FIELD_SCALEHANDLE_SIZE	8
#define FIELD_MIN_SIZE_TO_SHOW	20
#define FIELD_MIN_SIZE			90

#define GRID_SIZE				30
#define GRID_SNAP_DIST			5

#define FILE_EXT_CONFIG			".conf"
#define FILE_PROFILE_DEFAULT	"default.conf"
#define FILE_PROFILES			"profiles.txt"
#define FILE_FONT				"font_arial.glf"

#define PROFILE_CAPTION_DEFAULT		"<default>";

#define CONFIG_FILE_VERSION "v 1.14.11." // version.year.month.build

#define FIELD_TYPE_NONE		0
#define FIELD_TYPE_TEXTURE	1
#define FIELD_TYPE_MODEL	2
#define FIELD_TYPE_VALUE	3

#define COLOR_MAP_RAINBOW	0
#define COLOR_MAP_BLUERED	1


#define FIELD_NAME_NONE				"-x-"
#define FIELD_NAME_ABR_NONE			"-x-"
#define FIELD_NAME_TEXTURE			"Texture Field"
#define FIELD_NAME_ABR_TEXTURE		"TEX"
#define FIELD_NAME_MODEL			"Mesh Visual"
#define FIELD_NAME_ABR_MODEL		"MES"
#define FIELD_NAME_VALUE			"Variable Watch"
#define FIELD_NAME_ABR_VALUE		"VAR"

#define MENUITEM_FIELD_TO_SQUARE	"Set To Square"
#define MENUITEM_FIELD_DELETE		"Delete"
#define MENUITEM_FIELD_ADD			"Add Field"
#define MENUITEM_SAVE_RUN			"Save & Finish"


////   ERRORS   ////

#define ERRORTEXT_HEADER L"   GLD Error"
#define ERRORTEXT_FONT_MISSING L" Font cannot be loaded from working directory, font file is probably missing. Place the font file into working directory to prevent rendering errors."
#define ERRORTEXT_WORKDIR_DONT_EXIST L"Working directory set by gldInit() function does not exist. Already existing directory needs to be set."
#define ERRORTEXT_SHADERFILE_DONT_EXIST L"One or more shader files does not exist."
#define ERRORTEXT_SHADER_COMPILE_ERROR L"There were some errors compiling shader. See console output for more specific info."
#define ERRORTEXT_CANNOT_SWITCH_MODE L"Create at least one field and select its type to proceed to visualization mode."

static char * visualizationTypeCaption[4] = {
	FIELD_NAME_NONE,	
	FIELD_NAME_TEXTURE,
	FIELD_NAME_MODEL,
	FIELD_NAME_VALUE	
};

#endif