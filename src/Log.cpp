
// Log.cpp
// Implementation of Logging class
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "Log.h"


CLog Log("Engine.log");


CLog::CLog()
{
}


// Standard constructor - opens the log file
CLog::CLog(std::string fileName)
{
	outFile.open(fileName.c_str());
	outFile << "Log opened." << std::endl;
}


// Standard destructor - closes the log file
CLog::~CLog()
{
	outFile << "Log closed." << std::endl;
	outFile.close();
}



// Output an event to the log, first writing the name of the current module
int CLog::writeModuleDebugMessage(const char* fmt, ...)
{
	va_list args;
	char str[1024];

	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	outFile << "[" << currentModule << "] " << str << std::endl;
	outFile.flush();

	return 0;
}


// Output an event to the log
int CLog::writeDebugMessage(const char* fmt, ...)
{
	va_list args;
	char str[255];

	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	outFile << str << std::endl;
	outFile.flush();

	return 0;
}
