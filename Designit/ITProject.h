#pragma once

#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

// Forward declarations.
class ITSurface;

class ITProject
{
protected:

	// ITProject instance variables.
	int _debugLevel;
	int _MaxKeyFrame;

	// ITProject instance variables.
	std::string _ProgramName; // The program filename.
	std::string _ProgramNameWithPath; // The program filename with full path.
	std::string _FileName; // The data filename as read frm the JSON tag in the JSON file. Note that this is not necessarily the name of the file itself!
	std::string _FileNameWithPath; // The data filename with full path as read frm the JSON tag in the JSON file. Note that this is not necessarily the name of the file path itself!

	// The surfaces.
	std::vector <ITSurface*> *_MySurfaces;
	std::vector <ITSurface*> *_MyBaseSurfaces;

	int _MaxPropagationGeneration; // The maximum number of generations of wake panels to propagate (set to 0 if all generations are to be included).
	float _FlowitPressureEps; // The distance perpendicular to surface tangent where velocity is calculated when using Bernoulli to calculate pressure distribution.
	bool _IsConstantSpeedTrajectories; // Use the constant speed reparametrization algorithms.
	bool _IsDoVelocityField; // Whether or not to compute the velocity field.
	std::string _RankineAlgorithm; // Indicates which Rankine algorithm is to be used. (Either Appendix C or D from the UVLM Monograph).
	float _RankineCoreRadius; // Self-explanatory.

	// Velocity Field variables.
	std::string _VelocityFieldConstantPlane;
	int _VelocityFieldNx;
	int _VelocityFieldNy;
	int _VelocityFieldNz;
	float _VelocityFieldMinx;
	float _VelocityFieldMiny;
	float _VelocityFieldMinz;
	float _VelocityFieldMaxx;
	float _VelocityFieldMaxy;
	float _VelocityFieldMaxz;

	// Physical constants.
	float _rho; // Fluid density.
	float _framesPerSecond; // Obvious. Should be set to 1 always.

	// Display factors.
	float _PressureDisplayFactor; // Used for scaling the pressure nets in MyGLWidget drawing.
	float _VorticityDisplayFactor; // Used for scaling the pink surface vorticity net in MyGLWidget drawing.
	float _VelocityFieldDisplayFactor; // Used for scaling the velocity field in MyGLWidget drawing.

	// Surface Hierarchy.
	bool _IsSurfaceHierarchy; // Whether or not surface trajectories are to be piggy-backed on parent.

	bool _IsGust; // Indicated whether or not we shoulds include the gust in the simulation.
	float _ReplayDeltaTMSecs;
	bool _IsActiveControlSurfaces; // Indicates whether or nort control surface deflections should be modelled.

public:
	ITProject(void);
	~ITProject(void);

	void setPoint(const int surfaceID, const int i, const int j, const float posX, const float posY, const float posZ);
	void movePoint(const int surfaceID, const int i, const int j, const float dX, const float dY, const float dZ);

	// Admin methods.
	void currentDateTime(char * currentTime);
	void printDebug(const char *FILE, int LINE, const char *FUNC, int level, const char * format, ...);

	// Accessors for ITProject instance variables.
	int get_DebugLevel();
	void set_DebugLevel(int d);

	int get_MaxKeyFrame();
	void set_MaxKeyFrame(int d);

	std::string get_ProgramName();
	void set_ProgramName(std::string s);

	std::string get_ProgramNameWithPath();
	void set_ProgramNameWithPath(std::string s);

	std::string get_FileName();
	void set_FileName(std::string s);

	std::string get_FileNameWithPath();
	void set_FileNameWithPath(std::string s);

	std::vector <ITSurface*> *get_MySurfaces();
	void set_MySurfaces(std::vector <ITSurface*> *mySurfaces);

	std::vector <ITSurface*> *get_MyBaseSurfaces();
	void set_MyBaseSurfaces(std::vector <ITSurface*> *myBaseSurfaces);

	int get_MaxPropagationGeneration();
	void set_MaxPropagationGeneration(int i);

	float get_FlowitPressureEps();
	void set_FlowitPressureEps(float e);

	bool get_IsConstantSpeedTrajectories();
	void set_IsConstantSpeedTrajectories(bool b);

	bool get_IsDoVelocityField();
	void set_IsDoVelocityField(bool b);

	std::string get_RankineAlgorithm();
	void set_RankineAlgorithm(std::string s);

	float get_RankineCoreRadius();
	void set_RankineCoreRadius(float r);

	std::string get_VelocityFieldConstantPlane();
	void set_VelocityFieldConstantPlane(std::string c);

	int get_VelocityFieldNx();
	void set_VelocityFieldNx(int n);

	int get_VelocityFieldNy();
	void set_VelocityFieldNy(int n);

	int get_VelocityFieldNz();
	void set_VelocityFieldNz(int n);

	float get_VelocityFieldMinx();
	void set_VelocityFieldMinx(float x);

	float get_VelocityFieldMiny();
	void set_VelocityFieldMiny(float y);

	float get_VelocityFieldMinz();
	void set_VelocityFieldMinz(float z);

	float get_VelocityFieldMaxx();
	void set_VelocityFieldMaxx(float x);

	float get_VelocityFieldMaxy();
	void set_VelocityFieldMaxy(float y);

	float get_VelocityFieldMaxz();
	void set_VelocityFieldMaxz(float z);

	// Physical constants.
	float get_Rho();
	void set_Rho(float r);

	float get_FramesPerSecond();
	void set_FramesPerSecond(float f);

	// Display factors.
	float get_PressureDisplayFactor();
	void set_PressureDisplayFactor(float f);

	float get_VorticityDisplayFactor();
	void set_VorticityDisplayFactor(float f);

	float get_VelocityFieldDisplayFactor();
	void set_VelocityFieldDisplayFactor(float f);

	// Surface Hierarchy.
	bool get_IsSurfaceHierarchy();
	void set_IsSurfaceHierarchy(bool b);

	bool get_IsGust();
	void set_IsGust(bool b);

	float get_ReplayDeltaTMSecs();
	void set_ReplayDeltaTMSecs(float f);

	bool get_IsActiveControlSurfaces();
	void set_IsActiveControlSurfaces(bool b);

	ITSurface* getSurface(const int k);
};