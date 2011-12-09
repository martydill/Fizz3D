
// Model.h
// Defines the Model interface
// (c) 2002 Marty Dill

#ifndef FIZZMODEL_H
#define FIZZMODEL_H

#include "Fizz3d.h"



class IModel
{
public:
	virtual int load(std::string fileName) = 0;
	virtual int draw(void) = 0;
};



#endif
