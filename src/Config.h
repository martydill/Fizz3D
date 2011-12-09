
// Config.h
// Contains definitions used by the configuratioin system (e.g. console variables)
// (c) 2002 Marty Dill


#ifndef CONFIG_H
#define CONFIG_H


#include "Fizz3d.h"
#include <map>


enum CVarType
{
	CVarType_String = 0,
	CVarType_Int = 1,
	CVarType_Float = 2,
	CVarType_Bool
};

class CVar
{
public:
	CVarType type;
	std::string name;
	std::string stringValue;
	int intValue;
	float floatValue;
	bool boolValue;
};


namespace Config
{
	static std::map< std::string, CVar* > cVarList;
	
	int readConfig(std::string fileName);
	int writeConfig(std::string fileName);

	int getIntValue(std::string varName);
	float getFloatValue(std::string varName);
	const std::string& getStringValue(std::string varName);
	bool getBoolValue(std::string varName);
}



#endif
