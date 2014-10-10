#pragma once
#include "INICommon.h"
class INIField {
public:
	enum INIFieldType_e {
		INIF_INT,
		INIF_FLOAT,
		INIF_STR
	};
private:
	union {
		int ivalue;
		float fvalue;
		char* svalue;
	};

	INIFieldType_e eType;
	const std::string& sName;
public:
	~INIField() {
		if (eType == INIF_STR) {
			delete[] svalue;
		}
	}

	INIField(const string& _sName, const int value) :
		ivalue(value), eType(INIF_INT), sName(_sName) { }
	INIField(const string& _sName, float value) :
		fvalue(value), eType(INIF_FLOAT), sName(_sName) { }
	INIField(const string& _sName, const string& value) :
		eType(INIF_STR), sName(_sName) {
		svalue = new char[value.length()];
		strcpy(svalue, value.c_str());
	}

	const int GetInteger() { return ivalue; }
	const float GetValue() { return fvalue; }
	const char* GetStr() { return svalue; }
	const std::string GetName() { return sName; }
	INIFieldType_e GetType() { return eType; }
};