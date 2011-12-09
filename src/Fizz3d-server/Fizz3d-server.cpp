// Fizz3d-server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetworkServer.h"

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		NetworkServer server;
		server.Start(NetworkPort);
	}
	catch(std::exception e)
	{
	
	}

	return 0;
}

