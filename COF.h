#pragma once
#include <string>
#include <vector>

#define MAX_COF_COMPONENT	16
#define MAX_PART_STRING		3

struct COFComponent {
	bool bIsPresent;
	char sName[4];
	char* sWpnClass;
	char sFilePath[30];
	unsigned char ucColorMap[256];
	int iPalette;
	char sCMapName[256];
};

class COF {
public:
	COF(const char* sFileName);
	~COF();

	void PrintInformation();
private:
	int iLayer;
	unsigned char* ptTable;
	int iDirections;
	int iFramesPerDirection;

	unsigned char* ptCOFMemory;
	std::string sCOFName;
	COFComponent Layers[MAX_COF_COMPONENT];
};