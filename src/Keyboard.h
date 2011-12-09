
// Keyboard.h
// Defines the keyboard handler interface
// (c) 2002 Marty Dill


#ifndef KEYBOARD_H
#define KEYBOARD_H


// Represents the global state of the keyboard
struct KeyboardData
{
	unsigned char keys[256];
	unsigned char changedKeys[256];

	KeyboardData()
	{
		memset(changedKeys, 0, 256);
	}
};



// Platform-independent keyboard interface
class IKeyboard
{
public:
	
	// Called when the keyboard object is created
	virtual int initialize(void* initParameter) = 0;
	
	// Called every frame. Must update the KeyboardData struct that gets passed into it.
	virtual int update(KeyboardData* data) = 0;

	// Called when the keyboard object is destroyed
	virtual int destroy(void) = 0;

	virtual char getCharacterForKey(int scanCode) = 0;
};


#endif
