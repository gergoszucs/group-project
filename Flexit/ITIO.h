#pragma once

#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

using namespace rapidjson;

class ITIO
{

public:

	ITIO(void);
	~ITIO(void);

	static void readJSONInputFile(char *filename); // Class method.
	static void readMyITProjectFromJSONObject(Document& d, char * filenameWithPath); // Class method.
	static void readMyITPointsFromFile(Document& d, char * filenameWithPath); // Class method.
	static void createControlPointsFromJSONObject(Document& d, char * filenameWithPath, int k); // Class method.
	static void createTrajectoryCurvesFromJSON(Document& d, int k); // Class method.
	static void createSurfaceHierarchyChains(); // Class method.
	static void readJSONControlSurfaces(char *filename); // Class method.

	static void readMyVelocityFieldHistoryFromFile(char * filenameWithPath);
	static void readMyPressureHistoryFromFile(char * filenameWithPath);
	static void readMyVorticityHistoryFromFile(char * filenameWithPath);
	static void readMyWakePanelHistoryFromFile(char * filenameWithPath);
	static void readMyBeamNodeHistoryFromFile(char * filenameWithPath);
	static void readMyOutputDataHistoryFromFile(char * filenameWithPath);
	static void readMyControlPointDeflectionHistoryFromFile(char * filenameWithPath);
	static void readJSONBeamNodes(char *filenameWithPath);

	static void writeMyProjectToFile(char *fileNameWithPath);

private:
	static void writeMyITSurfacesToFile(rapidjson::Document* d);
	static void writeCurrentSurfaceRotationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d);
	static void writeCurrentSurfaceControlSurfaces(int k, rapidjson::Value *controlSurfacesArray, rapidjson::Document *d);
	static void writeCurrentSurfaceTranslationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d);

	static void writeMyVelocityFieldHistoryToFile(char *fileNameWithPath);
	static void writeMyPressureHistoryToFile(char *fileNameWithPath);
	static void writeMyWakePanelHistoryToFile(char *fileNameWithPath);
	static void writeMyVorticityHistoryToFile(char *fileNameWithPath);
	static void writeMyBeamNodeHistoryToFile(char *fileNameWithPath);
	static void writeMyOutputDataHistoryToFile(char *fileNameWithPath);
	static void writeMyControlPointDeflectionHistoryToFile(char *fileNameWithPath);

	static bool doesFileExist(const char *filename);

};