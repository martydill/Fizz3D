
// Config.cpp
// Functions which handle configuration and settings
// (c) 2002 Marty Dill

#include "Fizz3d.h"
#include "Engine.h"
#include "Config.h"
#include "Log.h"
#include "Platform.h"

using namespace std;

static const char* moduleName = "Config";

// Number of valid config file variables
static const int numValidVariables = 13;


static CVar variables[] = 
{
	{CVarType_Int, "Width"},
	{CVarType_Int, "Height"},
	{CVarType_Int, "PatchDetail"},
	{CVarType_Int, "ColorDepth"},
	{CVarType_Bool, "Fullscreen"},
	{CVarType_Bool, "UseHardwareGamma"},
	{CVarType_Int, "MouseSpeed"},
	{CVarType_Bool, "InvertMouse"},
	{CVarType_String, "RenderingDevice"},
	{CVarType_Bool, "UseAudio"},
	{CVarType_Bool, "UseMipmaps"},
	{CVarType_Bool, "DrawCrosshair"},
	{CVarType_Bool,"ShowFramerate"},
};



// Loads the specified configuration file
int Config::readConfig(string fileName)
{
	Trace("Loading configuration file %s", fileName.c_str());
	ifstream inFile;
	inFile.open(fileName.c_str());

	// Check for errors
	if(inFile.fail())
	{
		CEngine::CriticalError("Unable to open configuration file!");
	}

	char lineData[255];

	// While there is data to read ...
	while(!inFile.eof())
	{
		// Grab a line
		inFile.getline(lineData, 255);
		
		// Ignore comments
		if(lineData[0] == '#') { }

		// Ignore blank lines
		else if(!strcmp(lineData, "")) { }

		// Valid data
		else
		{
			char* s = strtok(lineData, " \t");
			string  name = s;
			char* p = lineData + strlen(s);
			while(!isalnum(*p))
				p++;

			// Check if the variable is valid ...
			for(int i = 0; i < numValidVariables; ++i)
			{
				if(variables[i].name == s)
				{
					Config::cVarList[s] = &variables[i];
					switch(variables[i].type)
					{
					case CVarType_String:
						variables[i].stringValue = p;
						break;
					case CVarType_Int:
						variables[i].intValue = atoi(p);
						break;
					case CVarType_Bool:
						variables[i].boolValue = !strcmp(p, "1") || !STRCMPI(p, "true");
						break;
					case CVarType_Float:
						variables[i].floatValue = atof(p);
						break;
					}

					break;
				}
			}
		}
	}

	inFile.close();
	Trace("Finished reading config file");

	return 0;
}


// Saves the current configuration to a file
int Config::writeConfig(string fileName)
{
	Trace("Saving configuration as %s", fileName.c_str());
	ofstream outFile;
	outFile.open(fileName.c_str());

	// Check for errors
	if(outFile.fail())
	{
		CEngine::CriticalError("Unable to open file for writing!");
	}

	outFile << "\n# Fizz3d configuration file\n\n";

	// Loop through each item and output it
	map< string, CVar* >::iterator i;	
	for(i = cVarList.begin(); i != cVarList.end(); ++i)
	{
		outFile << i->second->name << "\t" << i->second->stringValue << endl;
	}

	outFile.close();
	Trace("Finished saving configuration file");

	return 0;
}


// Gets integer data corresponding to a single variable
int Config::getIntValue(string varName)
{
	Assert(cVarList[varName]->type == CVarType_Int);
	return cVarList[varName]->intValue;
}


// Gets float data corresponding to a single variable
float Config::getFloatValue(string varName)
{
	Assert(cVarList[varName]->type == CVarType_Float);
	return cVarList[varName]->floatValue;
}


// Gets string data corresponding to a single variable
const string& Config::getStringValue(string varName)
{
	Assert(cVarList[varName]->type == CVarType_String);
	return cVarList[varName]->stringValue;
}


// Gets string data corresponding to a single variable
bool Config::getBoolValue(string varName)
{
	Assert(cVarList[varName]->type == CVarType_Bool);
	return cVarList[varName]->boolValue;
}
