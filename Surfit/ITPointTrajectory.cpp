// Dom's includes.
#include "ITPointTrajectory.h"
#include "global.h"
#include "ITProject.h"


ITPointTrajectory::ITPointTrajectory(float x, float y, float z) : ITPoint(x, y, z)
{
}


ITPointTrajectory::~ITPointTrajectory(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITPointTrajectory being destroyed.");

}

// Worker methods.
void ITPointTrajectory::serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d)
{
    project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITPointTrajectory::serializeMeAsJSONObject. KeyFrame: %i", get_KeyFrame());

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
	pointPropertiesObject.AddMember("vx", get_VX(), allocator);
	pointPropertiesObject.AddMember("vy", get_VY(), allocator);
	pointPropertiesObject.AddMember("vz", get_VZ(), allocator);
	pointPropertiesObject.AddMember("U", get_U(), allocator);
	pointPropertiesObject.AddMember("V", get_V(), allocator);
	pointPropertiesObject.AddMember("mass", get_Mass(), allocator);
	pointPropertiesObject.AddMember("KeyFrame", get_KeyFrame(), allocator);

	// Add the properties to the point node.
	pointObject.AddMember("Properties", pointPropertiesObject, allocator);

	// Finally add the point node to the array.
	controlPointsArray->PushBack(pointObject, allocator);

} // End of serializeMeAsJSONObject.



// Accessors.
int ITPointTrajectory::get_KeyFrame(){ return _KeyFrame; }
void ITPointTrajectory::set_KeyFrame(int i){ _KeyFrame = i; }
