#include "global.h"
#include "ITProject.h"
#include <stdarg.h>     // va_list, va_start, va_arg, va_end
#include <QDebug>       // qDebug.
#include <time.h>		// time, localtime, strftime
#include <iostream>		// For cerr.
#include "ITSurface.h"

using namespace rapidjson;

ITProject::ITProject(void)
{
	set_DebugLevel(2);
	set_MaxKeyFrame(500);

	// Instanciate the (empty) vector of ITSurfaces.
	_MySurfaces = new std::vector <ITSurface*>;
	_MyBaseSurfaces = new std::vector <ITSurface*>;

	// Set default instance variables.
	set_MaxPropagationGeneration(0);
	set_FlowitPressureEps(-2.0);
	set_IsConstantSpeedTrajectories(true);
	set_IsDoVelocityField(true);
	set_RankineAlgorithm("D");
	set_RankineCoreRadius(0.02);
	set_VelocityFieldConstantPlane("X");
	set_VelocityFieldNx(62);
	set_VelocityFieldNy(62);
	set_VelocityFieldNz(62);
	set_VelocityFieldMinx(-30.0);
	set_VelocityFieldMiny(-60.0);
	set_VelocityFieldMinz(-60.0);
	set_VelocityFieldMaxx(40.0);
	set_VelocityFieldMaxy(60.0);
	set_VelocityFieldMaxz(60.0);

	set_Rho(0.66);
	set_FramesPerSecond(100.0);
	set_PressureDisplayFactor(0.0002);
	set_VorticityDisplayFactor(0.2);
	set_VelocityFieldDisplayFactor(0.3);
	set_IsSurfaceHierarchy(0);
	set_IsGust(0);
	set_ReplayDeltaTMSecs(50);
	set_IsActiveControlSurfaces(0);
}

ITProject::~ITProject(void)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ITProject being destroyed.");

	// Delete the Surfaces.
	int noOfSurfaces = get_MySurfaces()->size();
	for (int i = 0; i < noOfSurfaces; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Calling delete on an ITSurface.");
		delete get_MySurfaces()->at(i);
	}

	get_MySurfaces()->clear();
	delete _MySurfaces;

	// Delete the Base Surfaces.
	int noOfBaseSurfaces = get_MyBaseSurfaces()->size();
	for (int i = 0; i < noOfBaseSurfaces; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Calling delete on an ITSurface.");
		delete get_MyBaseSurfaces()->at(i);
	}

	get_MyBaseSurfaces()->clear();
	delete _MyBaseSurfaces;
}

// Utilities.
void ITProject::currentDateTime(char* currentTime)
{
	time_t now = time(0);
	tm* local;

	local = localtime(&now);
	strftime(currentTime, 80, " %Y-%m-%d %H:%M:%S", local);

	return;
}

void ITProject::printDebug(const char *FILE, int LINE, const char *FUNC, int level, const char * format, ...)
{
#ifdef NDEBUG
	// Release code.

	if (MY_RUN_MODE == MYGUI)
	{
		// Running in release GUI mode.
		if (level <= get_DebugLevel())
		{
			char buffer[2256];
			va_list args;
			va_start(args, format);
			vsnprintf(buffer, 2255, format, args);
			va_end(args);

			qDebug("RG %s", buffer);
		}
	}
	else
	{
		// Running in release BATCH mode.
		if (level <= get_DebugLevel())
		{
			char buffer[2256];
			va_list args;
			va_start(args, format);
			vsnprintf(buffer, 2255, format, args);
			va_end(args);

			char  currentTime[120] = "";
			currentDateTime(currentTime);

			char s[256];
			sprintf(s, "RB [%s]", currentTime);

			qDebug("RB [%s:%d:%s()]:\t%i\t%s\t%s", FILE, LINE, FUNC, level, currentTime, buffer);

		}
	}

#else
	// Debug code.
	if (MY_RUN_MODE == MYGUI)
	{
		// Running in debug GUI mode.
		if (level <= get_DebugLevel())
		{
			char buffer[256];
			va_list args;
			va_start(args, format);
			vsnprintf(buffer, 255, format, args);
			va_end(args);

			char  currentTime[120] = "";
			currentDateTime(currentTime);

			qDebug("DG [%s:%d:%s()]:\t%i\t%s\t%s", FILE, LINE, FUNC, level, currentTime, buffer);

		}
	}
	else
	{
		// Running in debug BATCH mode.
		if (level <= get_DebugLevel())
		{
			char buffer[256];
			va_list args;
			va_start(args, format);
			vsnprintf(buffer, 255, format, args);
			va_end(args);

			char  currentTime[120] = "";
			currentDateTime(currentTime);

			qDebug("DB [%s:%d:%s()]:\t%i\t%s\t%s", FILE, LINE, FUNC, level, currentTime, buffer);

		}
	}

#endif // End of DEBUGGING

	return;

}

// Accessors.
int ITProject::get_DebugLevel() { return _debugLevel; }
void ITProject::set_DebugLevel(int d) { _debugLevel = d; }

int ITProject::get_MaxKeyFrame() { return _MaxKeyFrame; }
void ITProject::set_MaxKeyFrame(int d) { _MaxKeyFrame = d; }

std::string ITProject::get_ProgramName() { return _ProgramName; }
void ITProject::set_ProgramName(std::string s) { _ProgramName = s; }

std::string ITProject::get_ProgramNameWithPath() { return _ProgramNameWithPath; }
void ITProject::set_ProgramNameWithPath(std::string s) { _ProgramNameWithPath = s; }

std::string ITProject::get_FileName() { return _FileName; }
void ITProject::set_FileName(std::string s) { _FileName = s; }

std::string ITProject::get_FileNameWithPath() { return _FileNameWithPath; }
void ITProject::set_FileNameWithPath(std::string s) { _FileNameWithPath = s; }

std::vector <ITSurface*> * ITProject::get_MySurfaces() { return _MySurfaces; }
void ITProject::set_MySurfaces(std::vector <ITSurface*> *mySurfaces) { _MySurfaces = mySurfaces; }

std::vector <ITSurface*> * ITProject::get_MyBaseSurfaces() { return _MyBaseSurfaces; }
void ITProject::set_MyBaseSurfaces(std::vector <ITSurface*> *myBaseSurfaces) { _MyBaseSurfaces = myBaseSurfaces; }

int ITProject::get_MaxPropagationGeneration() { return _MaxPropagationGeneration; }
void ITProject::set_MaxPropagationGeneration(int i) { _MaxPropagationGeneration = i; }

float ITProject::get_FlowitPressureEps() { return _FlowitPressureEps; }
void ITProject::set_FlowitPressureEps(float e) { _FlowitPressureEps = e; }

bool ITProject::get_IsConstantSpeedTrajectories() { return _IsConstantSpeedTrajectories; }
void ITProject::set_IsConstantSpeedTrajectories(bool b) { _IsConstantSpeedTrajectories = b; }

bool ITProject::get_IsDoVelocityField() { return _IsDoVelocityField; }
void ITProject::set_IsDoVelocityField(bool b) { _IsDoVelocityField = b; }

std::string ITProject::get_RankineAlgorithm() { return _RankineAlgorithm; }
void ITProject::set_RankineAlgorithm(std::string s) { _RankineAlgorithm = s; }

float ITProject::get_RankineCoreRadius() { return _RankineCoreRadius; }
void ITProject::set_RankineCoreRadius(float r) { _RankineCoreRadius = r; }

std::string ITProject::get_VelocityFieldConstantPlane() { return _VelocityFieldConstantPlane; }
void ITProject::set_VelocityFieldConstantPlane(std::string c) { _VelocityFieldConstantPlane = c; }

int ITProject::get_VelocityFieldNx() { return _VelocityFieldNx; }
void ITProject::set_VelocityFieldNx(int n) { _VelocityFieldNx = n; }

int ITProject::get_VelocityFieldNy() { return _VelocityFieldNy; }
void ITProject::set_VelocityFieldNy(int n) { _VelocityFieldNy = n; }

int ITProject::get_VelocityFieldNz() { return _VelocityFieldNz; }
void ITProject::set_VelocityFieldNz(int n) { _VelocityFieldNz = n; }

float ITProject::get_VelocityFieldMinx() { return _VelocityFieldMinx; }
void ITProject::set_VelocityFieldMinx(float x) { _VelocityFieldMinx = x; }

float ITProject::get_VelocityFieldMiny() { return _VelocityFieldMiny; }
void ITProject::set_VelocityFieldMiny(float y) { _VelocityFieldMiny = y; }

float ITProject::get_VelocityFieldMinz() { return _VelocityFieldMinz; }
void ITProject::set_VelocityFieldMinz(float z) { _VelocityFieldMinz = z; }

float ITProject::get_VelocityFieldMaxx() { return _VelocityFieldMaxx; }
void ITProject::set_VelocityFieldMaxx(float x) { _VelocityFieldMaxx = x; }

float ITProject::get_VelocityFieldMaxy() { return _VelocityFieldMaxy; }
void ITProject::set_VelocityFieldMaxy(float y) { _VelocityFieldMaxy = y; }

float ITProject::get_VelocityFieldMaxz() { return _VelocityFieldMaxz; }
void ITProject::set_VelocityFieldMaxz(float z) { _VelocityFieldMaxz = z; }

float ITProject::get_Rho() { return _rho; }
void ITProject::set_Rho(float r) { _rho = r; }

float ITProject::get_FramesPerSecond() { return _framesPerSecond; }
void ITProject::set_FramesPerSecond(float f) { _framesPerSecond = f; }

float ITProject::get_PressureDisplayFactor() { return _PressureDisplayFactor; }
void ITProject::set_PressureDisplayFactor(float f) { _PressureDisplayFactor = f; }

float ITProject::get_VorticityDisplayFactor() { return _VorticityDisplayFactor; }
void ITProject::set_VorticityDisplayFactor(float f) { _VorticityDisplayFactor = f; }

float ITProject::get_VelocityFieldDisplayFactor() { return _VelocityFieldDisplayFactor; }
void ITProject::set_VelocityFieldDisplayFactor(float f) { _VelocityFieldDisplayFactor = f; }

bool ITProject::get_IsSurfaceHierarchy() { return _IsSurfaceHierarchy; }
void ITProject::set_IsSurfaceHierarchy(bool b) { _IsSurfaceHierarchy = b; }

bool ITProject::get_IsGust() { return _IsGust; }
void ITProject::set_IsGust(bool b) { _IsGust = b; }

float ITProject::get_ReplayDeltaTMSecs() { return _ReplayDeltaTMSecs; }
void ITProject::set_ReplayDeltaTMSecs(float f) { _ReplayDeltaTMSecs = f; }

bool ITProject::get_IsActiveControlSurfaces() { return _IsActiveControlSurfaces; }
void ITProject::set_IsActiveControlSurfaces(bool b) { _IsActiveControlSurfaces = b; }