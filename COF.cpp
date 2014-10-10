#include "COF.h"
#include "DCCMerge.h"

using namespace std;

const char sComponents[MAX_COF_COMPONENT][MAX_PART_STRING] = {
		{ "HD" },
		{ "TR" },
		{ "LG" },
		{ "RA" },
		{ "LA" },
		{ "RH" },
		{ "LH" },
		{ "SH" },
		{ "S1" },
		{ "S2" },
		{ "S3" },
		{ "S4" },
		{ "S5" },
		{ "S6" },
		{ "S7" },
		{ "S8" }
};

COF::COF(const char* sFileName) {
	FILE* ptFile = fopen(sFileName, "rb+");
	if (ptFile == nullptr) {
		cerr << "could not open " << sFileName;
badFile:
		ptTable = nullptr;
		iLayer = 0;
		iDirections = 0;
		iFramesPerDirection = 0;
		return;
	}
	size_t szCOFSize;

	fseek(ptFile, 0L, SEEK_END);
	szCOFSize = ftell(ptFile);
	fseek(ptFile, 0L, SEEK_SET);

	if (szCOFSize <= 0) {
		cerr << "COF file " << sFileName << " is empty\n";
		goto badFile;
	}

	ptCOFMemory = new unsigned char[szCOFSize];
	if (ptCOFMemory == nullptr) {
		cerr << "could not allocate " << szCOFSize << " bytes for COF file " << sFileName << ", aborting..\n";
		goto badFile;
	}
	fread(ptCOFMemory, 1, szCOFSize, ptFile);
	fclose(ptFile);

	iLayer = ptCOFMemory[0];
	iFramesPerDirection = ptCOFMemory[1];
	iDirections = ptCOFMemory[2];
	ptTable = ptCOFMemory + 28;
	sCOFName = sFileName;

	for (int i = 0; i < iLayer; i++) {
		const int pTable = *ptTable;
		COFComponent* ptComponent = &Layers[pTable];
		ptComponent->bIsPresent = true;
		ptComponent->sWpnClass = (char*)(ptTable + 5);
		ptComponent->sWpnClass[3] = '\0';
		strupr(ptComponent->sWpnClass);
		ptTable += 9;
	}
	ptTable += iFramesPerDirection;
}

COF::~COF() {
	if (ptCOFMemory != nullptr) {
		delete[] ptCOFMemory;
		ptCOFMemory = nullptr;
	}
}

void COF::PrintInformation() {
	cout << "COF:\t\t\t\t\t\t\t" << sCOFName << endl;
	cout << "# directions:\t\t\t\t\t\t" << iDirections << endl;
	cout << "# frames per direction:\t" << iFramesPerDirection << endl;
	cout << "# layers:\t\t\t\t\t" << iLayer << endl;
	cout << "Layers present:" << endl;
	for (int i = 0; i < MAX_COF_COMPONENT; i++) {
		if (!Layers[i].bIsPresent) {
			cout << "------------------" << endl;
			cout << "Layer " << i << " (component " << sComponents[i] << ")" << endl;
			cout << "File Path: " << Layers[i].sFilePath << endl;
			cout << "Name: " << Layers[i].sName << endl;
			cout << "Weapon Class: " << Layers[i].sWpnClass << endl;
			cout << "------------------" << endl;
		}
	}
}