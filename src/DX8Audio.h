

// DX8Audio.h
// Defines our DirectX 8 audio class
// (c) 2002 Marty Dill


#ifndef DX8AUDIO_H
#define DX8AUDIO_H



#include "Audio.h"
#include "DirectX8.h"
#include <windows.h>
//#include <dmusici.h>
#include <dsound.h>


class DX8Audio : public IAudio
{
public:
	DX8Audio();
	~DX8Audio();

	int initialize(void);
	int loadFile(std::string fileName);
	int destroy(void);

private:
	//IDirectMusicPerformance8* performance;
	//IDirectMusicLoader8* loader;
	//IDirectMusicSegment8* segment;
};



#endif
