
// ModelManager.cpp
// The model manager is responsible for the loading, caching, and destroying of game models.
// (c) 2002 Marty Dill


#include "Fizz3d.h"
#include "ModelManager.h"
#include "MD3Model.h"
#include "Log.h"
#include "MD3PlayerModel.h"
#include "MD3WeaponModel.h"

static const char* moduleName = "ModelManager";


ModelManager* ModelManager::instance = NULL;



// Standard constructor
ModelManager::ModelManager() :
	numModels(0)
{
}


// Standard destructor
ModelManager::~ModelManager()
{
}


// Creates an instance, if necessary, and returns it
ModelManager* ModelManager::getInstance(void)
{
	if(instance == NULL)
		instance = new ModelManager();
	
	return instance;
}


// Loads a model and returns its id
ModelID ModelManager::loadModel(std::string fileName)
{
	ModelID id;

	id = modelIdList[fileName];

	// Model has already been loaded - return its id
	if(modelIdList[fileName] != 0)
	{
		Trace("Using cached model for %s", fileName.c_str());
		return id;
	}

	IModel* model;

	// Figure out what kind of model it is
	if(fileName.find("players") != string::npos)
		model = new MD3PlayerModel();
	else if(fileName.find("weapons") != string::npos)
		model = new MD3WeaponModel();
	else
		model = new MD3GenericModel();

	Trace("Loading model %s", fileName.c_str());
	model->load(fileName);

	// Store the model info
	id = ++numModels;
	modelIdList[fileName] = id;
	modelList[id] = model;

	return id;
}


// Returns a pointer to a model, given an id
IModel* ModelManager::getModel(ModelID id)
{
	IModel* model = NULL;

	// Look up the id in the map
	if(modelList[id] != 0)
		model = modelList[id];

	return model;
}


// Frees all of the cached models
int ModelManager::freeAllModels(void)
{
	std::map< ModelID, IModel* >::iterator i;

	// Delete each IModel* in the list
	for(i = modelList.begin(); i != modelList.end(); ++i)
		delete i->second;
	
	// Clear the maps
	modelList.clear();
	modelIdList.clear();
	numModels = 0;
	
	return 0;
}
