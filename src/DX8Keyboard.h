
// DX8Keyboard.h
// Defines our DirectX 8 keyboard class
// (c) 2002 Marty Dill


#ifndef DX8KEYBOARD_H
#define DX8KEYBOARD_H



#include "Keyboard.h"
#include "DirectX8.h"
#include "Fizz3d.h"


class DX8Keyboard : public IKeyboard
{
public:
	DX8Keyboard();
	~DX8Keyboard();

	int initialize(void* initData);
	int update(KeyboardData* data);
	int destroy(void);
	char getCharacterForKey(int scanCode);

private:
	IDirectInputDevice8* device;
	unsigned char keys[256];
	unsigned char lastKeys[256];
	bool acquired;
};



#endif
