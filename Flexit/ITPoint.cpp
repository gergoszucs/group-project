#include <string>
#include "ITPoint.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "global.h"

ITPoint::ITPoint(float x, float y, float z)
{
	set_X(x);
	set_Y(y);
	set_Z(z);

	set_U(0.0);
	set_V(0.0);

	set_VX(0.0f);
	set_VY(0.0f);
	set_VZ(0.0f);

	set_Name("");

	set_Mass(0.0);
}

ITPoint::~ITPoint(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside destructor. ITPoint being destroyed.");
}


float ITPoint::magnitude()
{
	return sqrt(get_X()*get_X() + get_Y()*get_Y() + get_Z()*get_Z());
}

void ITPoint::normalize()
{
	float tempMag = magnitude();

	set_X(get_X() / tempMag);
	set_Y(get_Y() / tempMag);
	set_Z(get_Z() / tempMag);
}

void ITPoint::propagateMeWithMorph(ITPoint* cp, ITPoint* rotationPoint, ITPoint* translationPoint, bool isMorph, int startFrame, int endFrame, std::string typeOfMorph)
{
	// Moves a temporary point from the base position during FlowitCudaUnsteady play out.
	// cp is the surface "datum" point about which rotations should take place.
	// rotationPoint is the point in roll-pitch-yaw space representing the required rotation.
	// translationPoint is the point in translation space representing the required translation.
	// This method operates on the coordinates of this ITPoint object in place.
	float x, y, z;

	// Do the morphing.
	if (isMorph)
	{
		if (typeOfMorph == "LinearReflectY")
		{
			if ((FrameNumber >= startFrame) && (FrameNumber <= endFrame))
			{
				int frameRange = endFrame - startFrame;
				int deltaFrame = FrameNumber - startFrame;

				set_Y(get_Y() - 2 * get_Y() * deltaFrame / frameRange);
			}
			else if (FrameNumber > endFrame)
			{
				set_Y(-get_Y());
			}

		}
	}

	// Morphing is complete.
	// Now do rigid body transformation.
	// Translate this point back to the datum point (in preparation for rotation).
	set_X(get_X() - cp->get_X());
	set_Y(get_Y() - cp->get_Y());
	set_Z(get_Z() - cp->get_Z());

	// Now rotate - Roll first.
	float rollAngleRad = rotationPoint->get_X();

	y = get_Y()*cos(rollAngleRad) - get_Z()*sin(rollAngleRad);
	z = get_Y()*sin(rollAngleRad) + get_Z()*cos(rollAngleRad);

	set_Y(y);
	set_Z(z);

	// Now rotate - Pitch second.
	float pitchAngleRad = -rotationPoint->get_Y();

	x = get_X()*cos(pitchAngleRad) - get_Z()*sin(pitchAngleRad);
	z = get_X()*sin(pitchAngleRad) + get_Z()*cos(pitchAngleRad);

	set_X(x);
	set_Z(z);

	// Now rotate - Yaw third.
	float yawAngleRad = rotationPoint->get_Z();

	x = get_X()*cos(yawAngleRad) - get_Y()*sin(yawAngleRad);
	y = get_X()*sin(yawAngleRad) + get_Y()*cos(yawAngleRad);

	set_X(x);
	set_Y(y);

	// Next, translate to the new location (includes undoing of the translation prior to rotation above).
	set_X(get_X() + cp->get_X() + translationPoint->get_X());
	set_Y(get_Y() + cp->get_Y() + translationPoint->get_Y());
	set_Z(get_Z() + cp->get_Z() + translationPoint->get_Z());
}

void ITPoint::propagateMeWithMorphForTrajectorySpeed(ITPoint* cp, ITPoint* rotationPoint, ITPoint* translationPoint, bool isMorph, int startFrame, int endFrame, std::string typeOfMorph, int k, int myFrameNumber)
{
	// Moves a temporary point from the base position during FlowitCudaUnsteady play out.
	// cp is the surface "datum" point about which rotations should take place.
	// rotationPoint is the point in roll-pitch-yaw space representing the required rotation.
	// translationPoint is the point in translation space representing the required translation.
	// This method operates on the coordinates of this ITPoint object in place.

	float x, y, z;

	// Do the morphing.
	if (isMorph)
	{
		if (typeOfMorph == "LinearReflectY")
		{
			if ((FrameNumber >= startFrame) && (FrameNumber <= endFrame))
			{
				int frameRange = endFrame - startFrame;
				int deltaFrame = FrameNumber - startFrame;

				set_Y(get_Y() - 2 * get_Y() * deltaFrame / frameRange);
			}
			else if (FrameNumber > endFrame)
			{
				set_Y(-get_Y());
			}
		}
	}

	// Morphing is complete.
	// Now do rigid body transformation.
	// Translate this point back to the datum point (in preparation for rotation).
	set_X(get_X() - cp->get_X());
	set_Y(get_Y() - cp->get_Y());
	set_Z(get_Z() - cp->get_Z());

	// Now rotate - Roll first.
	float rollAngleRad = rotationPoint->get_X();

	y = get_Y()*cos(rollAngleRad) - get_Z()*sin(rollAngleRad);
	z = get_Y()*sin(rollAngleRad) + get_Z()*cos(rollAngleRad);

	set_Y(y);
	set_Z(z);

	// Now rotate - Pitch second.
	float pitchAngleRad = -rotationPoint->get_Y();

	x = get_X()*cos(pitchAngleRad) - get_Z()*sin(pitchAngleRad);
	z = get_X()*sin(pitchAngleRad) + get_Z()*cos(pitchAngleRad);

	set_X(x);
	set_Z(z);

	// Now rotate - Yaw third.
	float yawAngleRad = rotationPoint->get_Z();

	x = get_X()*cos(yawAngleRad) - get_Y()*sin(yawAngleRad);
	y = get_X()*sin(yawAngleRad) + get_Y()*cos(yawAngleRad);

	set_X(x);
	set_Y(y);

	// Next, translate to the new location (includes undoing of the translation prior to rotation above).
	set_X(get_X() + cp->get_X() + translationPoint->get_X());
	set_Y(get_Y() + cp->get_Y() + translationPoint->get_Y());
	set_Z(get_Z() + cp->get_Z() + translationPoint->get_Z());

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i", k);

	// FIXME: Finally apply deformation.
	if (IsModeEulerBernoulli)
	{
		// Find the nearest control point and get its x, y and z (in the earth frame).
		float dist = 100000.0;
		int myI = 0;
		int myJ = 0;

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				float currentDist = this->distanceFrom(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));

				if (currentDist < dist)
				{
					dist = currentDist;
					myI = i;
					myJ = j;
				}
			}
		}
		
		// Finally apply EULER-BERNOULLI deformations of the surface.
		if (myFrameNumber == FrameNumber)
		{
			set_X(get_X() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_X());
			set_Y(get_Y() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_Y());
			set_Z(get_Z() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_Z());
		}
		else if (FrameNumber > 1)
		{
			//set_X(get_X() + project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(myFrameNumber).at(myI).at(myJ)->get_X());
			//set_Y(get_Y() + project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(myFrameNumber).at(myI).at(myJ)->get_Y());
			//set_Z(get_Z() + project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(myFrameNumber).at(myI).at(myJ)->get_Z());
			set_X(get_X() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_X());
			set_Y(get_Y() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_Y());
			set_Z(get_Z() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(myI).at(myJ)->get_W()->get_Z());
		}
	}
}

float ITPoint::distanceFrom(ITPoint* secondPoint)
{
	return sqrt(
		(this->get_X() - secondPoint->get_X())*(this->get_X() - secondPoint->get_X())
		+ (this->get_Y() - secondPoint->get_Y())*(this->get_Y() - secondPoint->get_Y())
		+ (this->get_Z() - secondPoint->get_Z())*(this->get_Z() - secondPoint->get_Z())
	);
}

float ITPoint::dot(ITPoint *otherPt)
{
	float d = get_X()*otherPt->get_X()
		+ get_Y()*otherPt->get_Y()
		+ get_Z()*otherPt->get_Z();

	return d;
}

ITPoint *ITPoint::cross(ITPoint *otherPt, ITPoint *result)
{

	result->set_X(get_Y() * otherPt->get_Z() - get_Z() * otherPt->get_Y());
	result->set_Y(get_Z() * otherPt->get_X() - get_X() * otherPt->get_Z());
	result->set_Z(get_X() * otherPt->get_Y() - get_Y() * otherPt->get_X());

	return result;
}

void ITPoint::serializeMeAsJSONObject(int k, int i, int j, rapidjson::Value *controlPointsArray, rapidjson::Document *d)
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
	pointPropertiesObject.AddMember("vx", get_VX(), allocator);
	pointPropertiesObject.AddMember("vy", get_VY(), allocator);
	pointPropertiesObject.AddMember("vz", get_VZ(), allocator);
	pointPropertiesObject.AddMember("U", get_U(), allocator);
	pointPropertiesObject.AddMember("V", get_V(), allocator);
	pointPropertiesObject.AddMember("mass", get_Mass(), allocator);

	// Add the properties to the point node.
	pointObject.AddMember("Properties", pointPropertiesObject, allocator);

	// Finally add the point node to the array.
	controlPointsArray->PushBack(pointObject, allocator);
}

float ITPoint::get_X() { return _X; }
void ITPoint::set_X(float x) { _X = x; }

float ITPoint::get_Y() { return _Y; }
void ITPoint::set_Y(float y) { _Y = y; }

float ITPoint::get_Z() { return _Z; }
void ITPoint::set_Z(float z) { _Z = z; }

float ITPoint::get_VX() { return _VX; }
void ITPoint::set_VX(float vx) { _VX = vx; }

float ITPoint::get_VY() { return _VY; }
void ITPoint::set_VY(float vy) { _VY = vy; }

float ITPoint::get_VZ() { return _VZ; }
void ITPoint::set_VZ(float vz) { _VZ = vz; }

float ITPoint::get_Mass() { return _Mass; }
void ITPoint::set_Mass(float m) { _Mass = m; }

int ITPoint::get_I() { return _i; }
void ITPoint::set_I(int i) { _i = i; }

int ITPoint::get_J() { return _j; }
void ITPoint::set_J(int j) { _j = j; }

int ITPoint::get_K() { return _k; }
void ITPoint::set_K(int k) { _k = k; }

std::string ITPoint::get_Name() { return _Name; }
void ITPoint::set_Name(std::string s) { _Name = s; }

float ITPoint::get_U() { return _U; }
void ITPoint::set_U(float u) { _U = u; }

float ITPoint::get_V() { return _V; }
void ITPoint::set_V(float v) { _V = v; }