#pragma once
#include "INICommon.h"
#include "INIField.h"

class INILoader {
private:
	std::unordered_map<std::string, std::vector<INIField*>> mCurrentFields;
	std::string sErrorMessage;
	bool bLoadedFile;

	std::string sCurrentSection;

	bool ParseSection(const string& sSectionName, std::stringstream SS);
	bool ParseLine(const string& line);
public:
	INIField* GetField(const std::string& section, const std::string& sFieldName);
	const bool Loaded();
	const std::string GetErrorMessage();

	INILoader(const std::string& sFileIn);
	~INILoader();
};