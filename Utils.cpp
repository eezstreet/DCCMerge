#include "DCCMerge.h"

using namespace std;

void ParseRGB(const string& sRGBString, int* ptOutput) {
	if (ptOutput == nullptr) {
		cerr << "ParseRGB: bad 'ptOutput' parameter\n";
		return;
	}

	// Parse for hex
	string sFinalString;
	size_t szHexLetter;
	szHexLetter = sRGBString.find_first_of('x');
	if (szHexLetter != string::npos) {
		sFinalString = sRGBString.substr(szHexLetter + 1);
	} else {
		sFinalString = sRGBString;
	}

	// Check if the string has digits divisible by 3
	if (sFinalString.length() % 3 != 0) {
		cerr << "ParseRGB: number is not RGB compliant\n";
		return;
	}

	// Convert the string representation of RGB to three integers
	string sRGBDigits[] = {
		sFinalString.substr(0, sFinalString.length() / 3),
		sFinalString.substr(sFinalString.length() / 3, 2 * sFinalString.length() / 3),
		sFinalString.substr(2 * sFinalString.length() / 3)
	};

	stringstream ss;
	int iRGBDigits[3];
	double dRGBDigits[3];

	for (int i = 0; i < 3; i++) {
		ss << hex << sRGBDigits[i];
		ss >> iRGBDigits[i];
		ss.clear();

		// Normalize the number.
		dRGBDigits[i] = (((double)iRGBDigits[i] / 255.0f) - ((int)floor(iRGBDigits[i] / 255)));
		iRGBDigits[i] = dRGBDigits[i] * 255;
	}

	// The three sets of bytes form the output
	*ptOutput = (iRGBDigits[0] << 16) | (iRGBDigits[1] << 8) | (iRGBDigits[2]);
}

void ParseCSV(const string& sCSVString, vector<string>& vOutput) {
	size_t szNextComma = sCSVString.find_first_of(',');
	size_t szLastComma = 0;
	while (szNextComma != string::npos) {
		vOutput.push_back(sCSVString.substr(szLastComma, szNextComma));
		szLastComma = szNextComma + 1;
		szNextComma = sCSVString.find_first_of(',', szLastComma);
	}
}