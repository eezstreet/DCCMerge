#include "INILoader.h"

using namespace std;
INILoader::INILoader(const string& sFileIn) {
	FILE* ptFile = fopen(sFileIn.c_str(), "r");
	bLoadedFile = false;
	if (!ptFile) {
		sErrorMessage = "could not find file";
		return;
	}

	// Get the file size and allocate a char array that can accomodate for it
	fseek(ptFile, 0L, SEEK_END);
	size_t szFileSize = ftell(ptFile);
	fseek(ptFile, 0L, SEEK_SET);

	if (szFileSize == 0) {
		sErrorMessage = "blank file";
		return;
	}

	char* sFile = new char[szFileSize];
	fread(sFile, sizeof(char), szFileSize, ptFile);
	fclose(ptFile);

	// Copy the contents of sFile into a stringstream (much more manageable)
	stringstream sFileContents(sFile);
	string sLine;
	delete[] sFile;

	// Loop through each line.
	int iLineNo = -1;
	stringstream ssBadSyntax("bad syntax: ");
	while (getline(sFileContents, sLine, '\n')) {
		const size_t szComment = sLine.find_first_of(';');
		const size_t szOpenBracket = sLine.find_first_of('[');
		const size_t szCloseBracket = sLine.find_first_of(']');
		const bool bHasComment = (szComment != string::npos);
		iLineNo++;

		if (szOpenBracket == string::npos || bHasComment && szComment < szOpenBracket) {
			// No valid [ found --> either not in this line or in a comment, so just continue
			continue;
		}
		else if (szCloseBracket == string::npos || bHasComment && szComment < szCloseBracket) {
			// No closing bracket or closing bracket occurs after a comment
			ssBadSyntax << "line " << iLineNo << ": found [ but no ]";
			sErrorMessage = ssBadSyntax.str();
			return;
		}
		else if (szCloseBracket < szOpenBracket) {
			// Bad bracketing
			ssBadSyntax << "line " << iLineNo << ": found ] before [";
			sErrorMessage = ssBadSyntax.str();
			return;
		}
		else if (szCloseBracket - szOpenBracket <= 1) {
			// No text in brackets
			ssBadSyntax << "line " << iLineNo << ": bad section name";
			sErrorMessage = ssBadSyntax.str();
			return;
		}

		// Get the text in the brackets
		const string sSectionName = sLine.substr(szOpenBracket + 1, szCloseBracket);
		const auto itSection = mCurrentFields.find(sSectionName);
		if (itSection == mCurrentFields.end()) {
			// We've already scanned this section once before. Don't scan it again.
			continue;
		}

		if (!ParseSection(sSectionName, stringstream(sFileContents.str()))) {
			return;
		}
	}

	bLoadedFile = true;
}

bool INILoader::ParseSection(const string& sSectionName, stringstream ss) {
	string sLine;
	sCurrentSection = sSectionName;
	while (getline(ss, sLine, '\n')) {
		const size_t szComment = sLine.find_first_of(';');
		const size_t szOpenBracket = sLine.find_first_of('[');
		const bool bHasComment = (szComment != string::npos);
		const bool bHasBracket = (szOpenBracket != string::npos);

		// Check and see if we have any brackets. If we do, stop.
		if (bHasBracket && !bHasComment) {
			break;
		}
		else if (szOpenBracket < szComment) {
			break;
		}

		if (!ParseLine(sLine)) {
			return false;
		}
	}

	return true;
}

bool INILoader::ParseLine(const string& sLine) {
	const size_t szComment = sLine.find_first_of(';');
	const bool bHasComment = szComment != string::npos;
	const string sExpression = bHasComment ? sLine.substr(0, szComment) : sLine;

	const size_t szAssign = sExpression.find_first_of('=');
	if (szAssign == string::npos && !bHasComment) {
		sErrorMessage = "no assignment detected: ''" + sLine + "''";
		return false;
	}
	else if (szAssign == string::npos) {
		// FIXME: possible to enter a string like:
		// something ;comment
		// without a proper assignment!!
		return true;
	}

	// Get values
	const string sRawAssignTo = sExpression.substr(0, szAssign);
	const string sRawAssignValue = sExpression.substr(szAssign + 1);

	// Strip whitespace
	remove_if(sRawAssignTo.begin(), sRawAssignTo.end(), isspace);
	remove_if(sRawAssignValue.begin(), sRawAssignValue.end(), isspace);

	// Determine the type
	INIField::INIFieldType_e eType = INIField::INIF_INT;
	const size_t szFirstQuote = sRawAssignTo.find_first_of("\"");
	const size_t szLastQuote = sRawAssignTo.find_last_of("\"");
	const size_t szDecimal = sRawAssignTo.find_first_of('.');
	if (szFirstQuote == szLastQuote && szFirstQuote != string::npos) {
		sErrorMessage = "bad value assigned: ''" + sLine + "''";
		return false;
	}
	else if (szFirstQuote != string::npos) {
		eType = INIField::INIF_STR;
	}
	else if (szDecimal != string::npos) {
		eType = INIField::INIF_FLOAT;
	}

	// Based on the type that we have determined, insert it into the fields
	auto ptSection = mCurrentFields.find(sCurrentSection);
	vector<INIField*> vFields;
	if (ptSection != mCurrentFields.end()) {
		vFields = ptSection->second;
	}

	INIField* ptField;
	switch (eType) {
	case INIField::INIF_INT:
	default:
		ptField = new INIField(sRawAssignTo, atoi(sRawAssignValue.c_str()));
		break;
	case INIField::INIF_STR:
		ptField = new INIField(sRawAssignTo, sRawAssignValue);
		break;
	case INIField::INIF_FLOAT:
		ptField = new INIField(sRawAssignTo, (float)atof(sRawAssignValue.c_str()));
		break;
	}
	vFields.push_back(ptField);
	mCurrentFields[sCurrentSection] = vFields;

	return true;
}

const bool INILoader::Loaded() {
	return bLoadedFile;
}

INIField* INILoader::GetField(const string& sSection, const string& sFieldIn) {
	const auto f = mCurrentFields.find(sSection);
	if (f == mCurrentFields.end()) {
		return nullptr;
	}
	auto it = (f->second).begin();
	for (auto it = f->second.begin(); it != f->second.end(); ++it) {
		if ((*it)->GetName() == sFieldIn) {
			return *it;
		}
	}
	return nullptr;
}

const string INILoader::GetErrorMessage() {
	return sErrorMessage;
}