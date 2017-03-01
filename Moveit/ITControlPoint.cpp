// Dom's includes.
#include "ITControlPoint.h"
#include "global.h"
#include "ITProject.h"

ITControlPoint::ITControlPoint(float x, float y, float z) : ITPoint(x, y, z)
{
	// Set all the initial deflections to zero.
	_W = new ITPoint(0.0, 0.0, 0.0);

}


ITControlPoint::~ITControlPoint(void)
{
	delete _W;
}

void ITControlPoint::serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d)
{
    project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITPoint serializeMeAsJSONObject");

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = d->GetAllocator();

	rapidjson::Value pointObject(rapidjson::kObjectType);
	rapidjson::Value pointIndexObject(rapidjson::kObjectType);
	rapidjson::Value pointPropertiesObject(rapidjson::kObjectType);

	pointIndexObject.AddMember("row", i, allocator);
	pointIndexObject.AddMember("col", j, allocator);
	pointIndexObject.AddMember("surface", k, allocator);

	// Add the indices to the point node.
	pointObject.AddMember("Indices", pointIndexObject, allocator);

	pointPropertiesObject.AddMember("Name", "", allocator);
	pointPropertiesObject.AddMember("x", get_X(), allocator);
	pointPropertiesObject.AddMember("y", get_Y(), allocator);
	pointPropertiesObject.AddMember("z", get_Z(), allocator);
	pointPropertiesObject.AddMember("vx", 0.0, allocator);
	pointPropertiesObject.AddMember("vy", 0.0, allocator);
	pointPropertiesObject.AddMember("vz", 0.0, allocator);
	pointPropertiesObject.AddMember("U", 0.0, allocator);
	pointPropertiesObject.AddMember("V", 0.0, allocator);
	pointPropertiesObject.AddMember("mass", 0.0, allocator);

	// Add the properties to the point node.
	pointObject.AddMember("Properties", pointPropertiesObject, allocator);

	// Finally add the point node to the array.
	controlPointsArray->PushBack(pointObject, allocator);

} // End of serializeMeAsJSONObject.


// Accessors
ITPoint *ITControlPoint::get_W(){ return _W; }
void ITControlPoint::set_W(ITPoint *w){ _W = w; }
