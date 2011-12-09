
// DX8Mouse.h
// Defines our DirectX 8 mouse class
// (c) 2002 Marty Dill


#ifndef DX8MOUSE_H
#define DX8MOUSE_H



#include "Mouse.h"
#include "DirectX8.h"
#include "Fizz3d.h"


class DX8Mouse : public IMouse
{
public:
	DX8Mouse();
	~DX8Mouse();

	int initialize(void* initParameter);
	int update(MouseData* data);
	int destroy(void);

private:
	IDirectInputDevice8* device;
	DIMOUSESTATE state;
	bool acquired;
};



#endif