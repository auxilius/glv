#ifndef H_GLD_CONSTANTS
#define H_GLD_CONSTANTS


#define PI 3.141528

#define MENU_HEIGHT 30

#define FIELD_SCALEHANDLE_SIZE	8
#define FIELD_MIN_SIZE			30
#define GRID_SIZE			30
#define GRID_SNAP_DIST		5

#define FILE_CONFIG "user.conf"
#define FILE_FONT   "font_arial.glf"

#define FIELD_TYPE_NONE		0
#define FIELD_TYPE_TEXTURE	1
#define FIELD_TYPE_MODEL	2
#define FIELD_TYPE_VALUE	3

#define FIELD_NAME_NONE				"N/A"
#define FIELD_NAME_TEXTURE			"Texture Field"
#define FIELD_NAME_MODEL			"Mesh Field"
#define FIELD_NAME_VALUE			"Variable Display"
#define MENUITEM_FIELD_TO_SQUARE	"Set To Square"
#define MENUITEM_FIELD_DELETE		"Delete"
#define MENUITEM_FIELD_ADD			"Add Field"
#define MENUITEM_SAVE_RUN			"Save & Finish"

static char * visualizationTypeCaption[4] = {
	FIELD_NAME_NONE,	
	FIELD_NAME_TEXTURE,
	FIELD_NAME_MODEL,
	FIELD_NAME_VALUE	
};

#endif