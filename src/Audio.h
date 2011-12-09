
// Audio.h
// Defines the audio handler interface
// (c) 2002 Marty Dill


#ifndef AUDIO_H
#define AUDIO_H



class IAudio
{
public:
	virtual int initialize(void) = 0;
	virtual int loadFile(std::string fileName) = 0;
	virtual int destroy(void) = 0;
};



#endif
