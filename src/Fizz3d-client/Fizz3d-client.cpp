// Fizz3d-client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Fizz3d.h"
#include "../Engine.h"
#include "../NetworkClient.h"

using namespace std;

int _tmain(int argc, char* argv[])
{
	CEngine engine(NULL);
	engine.InitEngine();
	engine.MainLoop();

	return 0;
}

