
// ModelManager.h
// Defines the model manager class
// (c) 2002 Marty Dill


#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "Model.h"


typedef unsigned int ModelID;


class ModelManager
{
public:
	static ModelManager* getInstance(void);
	
	ModelID loadModel(std::string fileName);
	bool freeModel(ModelID id);
	int freeAllModels(void);
	IModel* getModel(ModelID id);

private:
	ModelManager();
	~ModelManager();
	static ModelManager* instance;

	std::map< std::string, ModelID > modelIdList;
	std::map< ModelID, IModel* > modelList;

	int numModels;
};



#endif
