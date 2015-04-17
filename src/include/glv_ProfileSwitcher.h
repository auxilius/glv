#pragma once
#ifndef GLV_H_PROFILE_SWITCHER
#define GLV_H_PROFILE_SWITCHER

#include <string>
#include <vector>
#include "glv_FieldManager.h"


class ProfileSwitcher {
public:
	ProfileSwitcher(FieldManager * fManage);
	// initialize the profile switcher
	void init();
	// number of loaded profiles
	unsigned count();
	// load profile with this 'id'
	void select(int id);
	// add new profile and 'name' it
	bool add(std::string name);
	// get name of the profile with specified 'id'
	std::string getName(int id);
	// get name of actualy selected profile
	std::string getNameOfSelected();
	// rename profile with this 'id' to the 'name'
	void rename(int id, std::string name);
	// delete profile with this 'id'
	void remove(int id);

private:
	FieldManager *fieldManager;
	int actualProfile;
	std::vector<std::string> profileNames;
	void saveProfiles();
	bool loadProfiles();
	std::string getConfigFileName(int id);
};


#endif