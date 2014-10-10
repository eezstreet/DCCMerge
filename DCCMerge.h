#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL.h>

typedef struct {
	std::string sGroupName, sBodyPart, sArmorLevel;
	std::vector<std::string> vCOFFiles;
} Section;

typedef struct {
	bool bIsComposite;
	std::vector<Section> vSections;
	int iDirections;
	int iTransRGB;
	int iNumSections;
} ProgramOptions;

void ParseRGB(const std::string& sRGBString, int* ptOutput);
void ParseCSV(const std::string& sCSVString, std::vector<string>& vOutput);