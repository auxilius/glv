#include "glv_ProfileSwitcher.h"
#include "glv_Constants.h"
#include "gld_types.h"

#define GLV_PROFILE_CAPTION_DEFAULT	"<default>"

ProfileSwitcher::ProfileSwitcher(FieldManager * fManage) {
	actualProfile = -1;
	fieldManager = fManage;
};

void ProfileSwitcher::init() {
	profileNames.clear();
	profileNames.push_back( GLV_PROFILE_CAPTION_DEFAULT );
	actualProfile = 0;
	fieldManager->load( getConfigFileName(0) );
	loadProfiles();	
};

unsigned ProfileSwitcher::count() {
	return profileNames.size();
};

void ProfileSwitcher::select(int id) {
	if (id < 0 || (int)count() <= id )
		return;
	if (actualProfile != id) {
		actualProfile = id;
		fieldManager->load( getConfigFileName(id) );
	}
	saveProfiles();
};

bool ProfileSwitcher::add(std::string name) {
	// if the same name of profile already exist - dont add
	if (std::find(profileNames.begin(), profileNames.end(), name) != profileNames.end() )
		return false;
	profileNames.push_back(name);
	select( profileNames.size()-1 );
	saveProfiles();
	fieldManager->save();
	return true;
};

std::string ProfileSwitcher::getName(int id) {
	if (0 <= id && id < (int)count() )
		return profileNames[id];
	else
		return "error_profile_id:"+std::to_string(id);
};

std::string ProfileSwitcher::getNameOfSelected() {
	if (actualProfile != -1)
		return profileNames[actualProfile];
	else
		return "error_profile_id:"+std::to_string(actualProfile);
};

void ProfileSwitcher::rename(int id, std::string name) {
	if ( id < 1 || (int)count() <= id)
		return;
	std::string oldConfigFile = getConfigFileName(id);
	profileNames[id] = name;
	std::rename(oldConfigFile.c_str(), getConfigFileName(id).c_str() );
	if (id == actualProfile)
		fieldManager->load( getConfigFileName(id) );
	saveProfiles();
};

void ProfileSwitcher::remove(int id) {
	// if id is valid and not default profile
	if ( id < 1 || (int)count() <= id)
		return;
	// erease from the list
	std::string configFile = getConfigFileName(id);
	profileNames.erase(profileNames.begin()+id);
	// remove the config file
	std::remove(configFile.c_str());
	// if was deleted this profile, select to default
	if (id == actualProfile)
		select(0);
	// decrease actual id if the predcestor was deleted
	if (id < actualProfile)
		actualProfile--;
	saveProfiles();
};


void ProfileSwitcher::saveProfiles() {
	std::ofstream oStream;
	oStream.open( pathToFile(GLV_FILE_PROFILES) );
	if (oStream.fail())
		MessageBox(0, "Cannot save profiles into file.", "GLD - save error", MB_OK | MB_ICONWARNING);
	for (unsigned i=1; i<count(); ++i) {
		if (actualProfile == (int)i)
			oStream << '>';
		oStream << profileNames[i] << std::endl;
	}
	oStream.close();
};

bool ProfileSwitcher::loadProfiles() {
	std::string fName = pathToFile(GLV_FILE_PROFILES);
	if (!fileExists(fName.c_str()))
		return false;
	std::ifstream inStream;
	inStream.open( fName );
	if (inStream.fail()) {
		MessageBox(0, "Error opening file containing profiles.", "GLD - loading error", MB_OK | MB_ICONWARNING);
		return false;
	}
	std::string cLine;
	while (! inStream.eof() ) {
		getline(inStream, cLine);
		cLine = trim(cLine);
		if ( (cLine == "") || (cLine[0] == '#') )
			continue;
		if ( cLine[0] == '>' ) {
			cLine.erase( cLine.begin() );
			profileNames.push_back( cLine );
			select(count()-1);
		} else
			profileNames.push_back( cLine );
	}
	inStream.close();
	return true;
};

std::string ProfileSwitcher::getConfigFileName(int id) {
	std::string fname = GLV_FILE_PROFILE_DEFAULT;
	if (id != 0)
		fname = getName(id) + GLV_FILE_EXT_CONFIG;
	return pathToFile(fname.c_str());
};
