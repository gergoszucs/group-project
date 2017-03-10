#pragma once

#include "ITPoint.h"
#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

class ITPointTrajectory : public ITPoint
{

protected:
	int _KeyFrame;

public:
	// Constructor.
	ITPointTrajectory(float x, float y, float z);

	// Destructor.
	~ITPointTrajectory(void);

	// IO
	void serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d);

	// Accessors
	int get_KeyFrame();
	void set_KeyFrame(int i);
};