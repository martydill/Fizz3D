// Fizz3d-client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Fizz3d.h"
#include "../Engine.h"
#include "../NetworkClient.h"

using namespace std;

std::string TCHARToString(const TCHAR* ptsz)
{
	if(ptsz == NULL)
		return "";

	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2*len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2*len + 1);
	std::string s = psz;
	delete [] psz;
	return s;
}


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CEngine engine(NULL);
		engine.InitEngine();
		engine.MainLoop();
		engine.Destroy();
	}
	catch(exception e)
	{
		//MessageBox(NULL, e.what(), _T("Critical error"), MB_OK);
	}
	return 0;
}

