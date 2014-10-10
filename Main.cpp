#include "INILoader.h"
#include "DCCMerge.h"

#define MIN_ARGS	2

using namespace std;

static ProgramOptions po;

// Prints the usage of this app
void usage() {
	cout << "usage: DCCMerge <ini file>\n";
}

// Loads the program options from an INI
void loadPO(INILoader* ptINI) {
	if (ptINI == nullptr) {
		cerr << "loadPO: null ptINI\n";
		return;
	}

	// The actual meat of the function
	INIField* field;

	// Composite field
	field = ptINI->GetField("DCCMerge", "Composite");
	if (field == nullptr) {
		cerr << "field 'Composite' not found\n";
		return;
	}
	po.bIsComposite = field->GetInteger() != 0;

	// TransRGB field
	field = ptINI->GetField("DCCMerge", "TransRGB");
	if (field == nullptr) {
		cerr << "field 'TransRGB' not found\n";
		return;
	}
	ParseRGB(field->GetStr(), &po.iTransRGB);

	// NumSections field
	field = ptINI->GetField("DCCMerge", "NumSections");
	if (field == nullptr) {
		cerr << "field 'NumSections' not found\n";
		return;
	}
	po.iNumSections = field->GetInteger();

	// NumDirections field
	field = ptINI->GetField("DCCMerge", "NumDirections");
	if (field == nullptr) {
		cerr << "field 'NumDirections' not found\n";
		return;
	}
	po.iDirections = field->GetInteger();

	for (int i = 0; i < po.iNumSections; i++) {
		Section curSection;
		field = ptINI->GetField("Section" + i, "ArmorLevel");
		if (field == nullptr) {
			cerr << "section " << i << " does not have ArmorLevel set\n";
			return;
		}
		curSection.sArmorLevel = field->GetStr();

		field = ptINI->GetField("Section" + i, "BodyPart");
		if (field == nullptr) {
			cerr << "section " << i << " does not have BodyPart set\n";
			return;
		}
		curSection.sBodyPart = field->GetStr();

		field = ptINI->GetField("Section" + i, "GroupName");
		if (field == nullptr) {
			cerr << "section " << i << " does not have GroupName set\n";
			return;
		}
		curSection.sGroupName = field->GetStr();

		field = ptINI->GetField("Section" + i, "COFFiles");
		if (field == nullptr) {
			cerr << "section " << i << " has no COF files\n";
			return;
		}
		ParseCSV(field->GetStr(), curSection.vCOFFiles);
		po.vSections.push_back(curSection);
	}
}

// Entry point of the program
int main(int argc, char** argv) {
	// Print usage, if necessary
	if (argc < MIN_ARGS) {
		usage();
		return 0;
	}

	// Load the INI and print any parsing errors
	INILoader* ini = new INILoader(argv[0]);
	if (!ini->Loaded()) {
		cout << ini->GetErrorMessage();
		return 0;
	}

	// Load the program options from the INI
	loadPO(ini);
}