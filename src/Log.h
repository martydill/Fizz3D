
// Log.h
// Definition of CLog class
// (c) 2002 Marty Dill


#ifndef LOG_H
#define LOG_H


#include "Fizz3d.h"


#define Trace Log.currentModule = moduleName; Log.writeModuleDebugMessage


class CLog
{
public:
	CLog();
	CLog(std::string fileName);
	~CLog();

	int writeModuleDebugMessage(const char* fmt, ...);
	int writeDebugMessage(const char* fmt, ...);

	const char* currentModule;

private:
	std::ofstream outFile;
};


extern CLog Log;



#endif
