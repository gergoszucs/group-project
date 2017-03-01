#pragma once

#include <string>
#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

#include "ITPoint.h"

class ITControlPoint : public ITPoint
{

private:

	ITPoint* _W; // The current deflection vector.

public:

	ITControlPoint(float x, float y, float z);
	~ITControlPoint(void);

	// Utilities.

	// IO
	void serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d);


	// Accessors.	
	ITPoint *get_W();
	void set_W(ITPoint *w);

};