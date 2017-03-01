#pragma once

#include "rapidjson/document.h" // https://github.com/miloyip/rapidjson

using namespace rapidjson;

class ITIO
{

public:

	ITIO(void);
	~ITIO(void);
	
	static void readJSONInputFile(char *filename); // Class method.
	static void writeMyProjectToFile(char *fileNameWithPath);

private:

	static void readMyITProjectFromJSONObject(Document& d, char * filenameWithPath); // Class method.
	static void readMyITPointsFromFile(Document& d, char * filenameWithPath); // Class method.
	static void createControlPointsFromJSONObject(Document& d, char * filenameWithPath, int k); // Class method.

	static void writeMyITSurfacesToFile(rapidjson::Document* d);
	static void writeDummyTranslationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d);
	static void writeDummyRotationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d);

};