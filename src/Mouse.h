
// Mouse.h
// Defines the mouse handler interface
// (c) 2002 Marty Dill


#ifndef MOUSE_H
#define MOUSE_H



// Mouse constraints - ensure that we don't flip over while looking up or down
const int MAX_MOUSE_ELEVATION = 435;
const int MIN_MOUSE_ELEVATION =	275;

// Global mouse speed scaler
const float MOUSE_SCALE_FACTOR = 0.1f;


struct MouseData
{
	// Delta x, y, z - how far we have moved since the last time
	long dx, dy, dz;

	// Mouse buttons
	unsigned char leftButton;
	unsigned char rightButton;
	unsigned char middleButton;
};


class IMouse
{
public:

	// Called when the mouse object is created
	virtual int initialize(void* initParameter) = 0;
	
	// Called every frame. Must update the MouseData struct that gets passed into it.
	virtual int update(MouseData* data) = 0;

	// Called when the mouse object is destroyed
	virtual int destroy(void) = 0;
};



#endif
