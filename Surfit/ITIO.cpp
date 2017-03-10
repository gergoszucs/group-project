#include "ITIO.h"

#include <fstream> // std::ifstream

#include "rapidjson/document.h" // will include "rapidjson/rapidjson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"

#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "ITPointTrajectory.h"

using namespace rapidjson;

void ITIO::readJSONInputFile(char *filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Start");

	FILE* fp = fopen(filenameWithPath, "r"); // non-Windows use "r"
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;

	ParseResult ok = d.ParseStream(is);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End %i", ok);

	if (!ok)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "RapidJSON Parse error = %s", d.GetParseError());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Error(offset %u)", (unsigned)d.GetErrorOffset());
	}

	fclose(fp);

	// Now go through and assign ITProject instance variables.
	readMyITProjectFromJSONObject(d, filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Project data read successfully.");

	// Now go through and read the control points.
	readMyITPointsFromFile(d, filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface data read successfully.");

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End");
}

void ITIO::readMyITProjectFromJSONObject(Document& d, char * filenameWithPath)
{
	assert(d.IsObject());
	assert(d.HasMember("ITProject Data"));

	// Read ITProject data.
	const Value& p = d["ITProject Data"]["General Data"];

	project->set_ProgramName(p["ProgramName"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ProgramName = %s", project->get_ProgramName().c_str());

	project->set_ProgramNameWithPath(p["ProgramNameWithPath"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ProgramNameWithPath = %s", project->get_ProgramNameWithPath().c_str());

	project->set_FileName(p["FileName"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FileName = %s", project->get_FileName().c_str());

	project->set_FileNameWithPath(p["FileNameWithPath"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FileNameWithPath = %s", project->get_FileNameWithPath().c_str());

	// Read FlowitCudaUnsteady data.
	const Value& ppp = d["ITProject Data"]["FlowitCudaUnsteady Data"];

	project->set_MaxPropagationGeneration(d["ITProject Data"]["FlowitCudaUnsteady Data"]["MaxPropagationGeneration"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MaxPropagationGeneration = %i", project->get_MaxPropagationGeneration());

	project->set_FlowitPressureEps(d["ITProject Data"]["FlowitCudaUnsteady Data"]["FlowitPressureEps"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FlowitPressureEps = %f", project->get_FlowitPressureEps());

	project->set_IsConstantSpeedTrajectories(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsConstantSpeedTrajectories"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsConstantSpeedTrajectories = %i", project->get_IsConstantSpeedTrajectories());

	project->set_IsDoVelocityField(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsDoVelocityField"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsDoVelocityField = %i", project->get_IsDoVelocityField());

	project->set_RankineAlgorithm(ppp["RankineAlgorithm"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "RankineAlgorithm = %s", project->get_RankineAlgorithm().c_str());

	project->set_RankineCoreRadius(d["ITProject Data"]["FlowitCudaUnsteady Data"]["RankineCoreRadius"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "RankineCoreRadius = %f", project->get_RankineCoreRadius());

	project->set_VelocityFieldConstantPlane(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldConstantPlane"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldConstantPlane = %s", project->get_VelocityFieldConstantPlane().c_str());

	project->set_VelocityFieldNx(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldNx"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldNx = %i", project->get_VelocityFieldNx());

	project->set_VelocityFieldNy(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldNy"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldNy = %i", project->get_VelocityFieldNy());

	project->set_VelocityFieldNz(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldNz"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldNz = %i", project->get_VelocityFieldNz());

	project->set_VelocityFieldMinx(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMinx"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMinx = %f", project->get_VelocityFieldMinx());

	project->set_VelocityFieldMiny(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMiny"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMiny = %f", project->get_VelocityFieldMiny());

	project->set_VelocityFieldMinz(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMinz"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMinz = %f", project->get_VelocityFieldMinz());

	project->set_VelocityFieldMaxx(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMaxx"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMaxx = %f", project->get_VelocityFieldMaxx());

	project->set_VelocityFieldMaxy(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMaxy"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMaxy = %f", project->get_VelocityFieldMaxy());

	project->set_VelocityFieldMaxz(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldMaxz"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldMaxz = %f", project->get_VelocityFieldMaxz());

}

void ITIO::readMyITPointsFromFile(Document& d, char * filenameWithPath)
{
	assert(d.IsObject());
	assert(d.HasMember("ITSurfaces"));

	// Loop through the surfaces and instanciate ITSurface objects.
	for (int k = 0; k < d["ITSurfaces"].Size(); k++)
	{
		// Create a new surface object.
		ITSurface *s = new ITSurface();

		// Create a new base surface object.
		ITSurface *bs = new ITSurface();

		s->set_NoOfInterpolatedPointsU(d["ITSurfaces"][k]["Properties"]["Number of rows of interpolated points"].GetInt());
		bs->set_NoOfInterpolatedPointsU(d["ITSurfaces"][k]["Properties"]["Number of rows of interpolated points"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "NoOfInterpolatedPointsU = %i", s->get_NoOfInterpolatedPointsU());

		s->set_NoOfInterpolatedPointsV(d["ITSurfaces"][k]["Properties"]["Number of cols of interpolated points"].GetInt());
		bs->set_NoOfInterpolatedPointsV(d["ITSurfaces"][k]["Properties"]["Number of cols of interpolated points"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "NoOfInterpolatedPointsV = %i", s->get_NoOfInterpolatedPointsV());

		// Read morphing variables.
		s->set_IsMorph(d["ITSurfaces"][k]["Properties"]["IsMorph"].GetInt());
		bs->set_IsMorph(d["ITSurfaces"][k]["Properties"]["IsMorph"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsMorph = %i", s->get_IsMorph());

		s->set_MorphStartFrame(d["ITSurfaces"][k]["Properties"]["MorphStartFrame"].GetInt());
		bs->set_MorphStartFrame(d["ITSurfaces"][k]["Properties"]["MorphStartFrame"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MorphStartFrame = %i", s->get_MorphStartFrame());

		s->set_MorphEndFrame(d["ITSurfaces"][k]["Properties"]["MorphEndFrame"].GetInt());
		bs->set_MorphEndFrame(d["ITSurfaces"][k]["Properties"]["MorphEndFrame"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MorphEndFrame = %i", s->get_MorphEndFrame());

		s->set_MorphType(d["ITSurfaces"][k]["Properties"]["MorphType"].GetString());
		bs->set_MorphType(d["ITSurfaces"][k]["Properties"]["MorphType"].GetString());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MorphType = %s", s->get_MorphType().c_str());

		// Decide whether or not this surface sheds a wake.
		s->set_IsWake(d["ITSurfaces"][k]["Properties"]["IsWake"].GetInt());
		bs->set_IsWake(d["ITSurfaces"][k]["Properties"]["IsWake"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsWake = %i", s->get_IsWake());

		// Decide whether or not this surface should have offset panels.
		s->set_IsPistolesiPanelOffset(d["ITSurfaces"][k]["Properties"]["IsPistolesiPanelOffset"].GetInt());
		bs->set_IsPistolesiPanelOffset(d["ITSurfaces"][k]["Properties"]["IsPistolesiPanelOffset"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsPistolesiPanelOffset = %i", s->get_IsPistolesiPanelOffset());

		// Define the offset fraction.
		s->set_PistolesiPanelOffsetFraction(d["ITSurfaces"][k]["Properties"]["PistolesiPanelOffsetFraction"].GetDouble());
		bs->set_PistolesiPanelOffsetFraction(d["ITSurfaces"][k]["Properties"]["PistolesiPanelOffsetFraction"].GetDouble());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "PistolesiPanelOffsetFraction = %f", s->get_PistolesiPanelOffsetFraction());

		// Save the surface parent index (used when project IsSurfaceHierarchy is true).
		s->set_ParentSurfaceIndex(d["ITSurfaces"][k]["Properties"]["ParentSurfaceIndex"].GetInt());
		bs->set_ParentSurfaceIndex(d["ITSurfaces"][k]["Properties"]["ParentSurfaceIndex"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ParentSurfaceIndexWake = %i", s->get_ParentSurfaceIndex());

		// Fuselage radius.
		// Because of the HasMember call, this parameter is optional, and need not be present in the json file.
		if (d["ITSurfaces"][k]["Properties"].HasMember("FuselageRadius"))
		{
			s->set_FuselageRadius(d["ITSurfaces"][k]["Properties"]["FuselageRadius"].GetDouble());
			bs->set_FuselageRadius(d["ITSurfaces"][k]["Properties"]["FuselageRadius"].GetDouble());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FuselageRadius = %f", s->get_FuselageRadius());
		}
		else
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================================================= FuselageRadius NOT FOUND");
		}

		// Push the surface into the vector.
		project->get_MySurfaces()->push_back(s);

		// Push the surface into the vector.
		project->get_MyBaseSurfaces()->push_back(bs);

		// Read the control points from the JSON document and initialise the control points for MySurfaces and MyBaseSurfaces.
		createControlPointsFromJSONObject(d, filenameWithPath, k);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "A");
	}
}

void ITIO::createControlPointsFromJSONObject(Document& d, char * filenameWithPath, int k)
{
	//Find the number of rows and columns of control points in the k-th surface.
	int noOfRows = d["ITSurfaces"][k]["Properties"]["Number of rows of control points"].GetInt();
	int noOfCols = d["ITSurfaces"][k]["Properties"]["Number of cols of control points"].GetInt();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of rows = %i", noOfRows);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of cols = %i", noOfCols);

	//Find the number of rows and columns of interpolated points in the k-th surface.
	int noOfIntRows = d["ITSurfaces"][k]["Properties"]["Number of rows of interpolated points"].GetInt();
	int noOfIntCols = d["ITSurfaces"][k]["Properties"]["Number of cols of interpolated points"].GetInt();

	project->get_MySurfaces()->at(k)->set_NoOfInterpolatedPointsU(noOfIntRows);
	project->get_MySurfaces()->at(k)->set_NoOfInterpolatedPointsV(noOfIntCols);

	int counter = 0;
	for (int i = 0; i < noOfRows; i++)
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> v_base_dummy;

		for (int j = 0; j < noOfCols; j++)
		{
			float x = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["x"].GetDouble();
			float y = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["y"].GetDouble();
			float z = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["z"].GetDouble();

			ITControlPoint* p = new ITControlPoint(x, y, z);
			ITControlPoint* pb = new ITControlPoint(x, y, z);

			p->set_K(k);
			p->set_I(i);
			p->set_J(j);

			pb->set_K(k);
			pb->set_I(i);
			pb->set_J(j);

			v_dummy.push_back(p);
			v_base_dummy.push_back(pb);

			counter++;
		} // End of loop over columns of control points.

		project->get_MySurfaces()->at(k)->get_MyControlPoints()->push_back(v_dummy);
		project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->push_back(v_base_dummy);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of rows in vector of vectors of control points = %i", project->get_MySurfaces()->at(k)->get_MyControlPoints()->size());
	} // End of loop over rows of control points
}

void ITIO::writeMyProjectToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside writeMyProjectToFile. fileName: %s", fileNameWithPath);

	std::ofstream myfile;

	rapidjson::Document d; // Null
	d.SetObject();

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

	// Project properties.============================================================
	rapidjson::Value object1(rapidjson::kObjectType);
	char  currentTime[120] = "";
	project->currentDateTime(currentTime);

	// Insert the date and time.
	Value key0("Date and Time", d.GetAllocator()); // copy string name
	object1.AddMember(key0, Value(currentTime, allocator).Move(), d.GetAllocator());

	// Debug level.
	object1.AddMember("Debug level", project->get_DebugLevel(), allocator);
	d.AddMember("Project Properties", object1, allocator);

	// ITProject data (big container for data for each program).======================
	rapidjson::Value objectITProjectData(rapidjson::kObjectType);

	// General Data. ==============================================================================
	rapidjson::Value objectGeneralData(rapidjson::kObjectType);

	// Insert the (data) FileName.
	Value key1("FileName", d.GetAllocator()); // copy string name
	objectGeneralData.AddMember(key1, Value(project->get_FileName().c_str(), allocator).Move(), d.GetAllocator());

	// Insert the (data) FileNameWithPath.
	Value key2("FileNameWithPath", d.GetAllocator()); // copy string name
	objectGeneralData.AddMember(key2, Value(project->get_FileNameWithPath().c_str(), allocator).Move(), d.GetAllocator());

	// Insert the ProgramName.
	Value key3("ProgramName", d.GetAllocator()); // copy string name
	objectGeneralData.AddMember(key3, Value(project->get_ProgramName().c_str(), allocator).Move(), d.GetAllocator());

	// Insert the ProgramNameWithPath.
	Value key4("ProgramNameWithPath", d.GetAllocator()); // copy string name
	objectGeneralData.AddMember(key4, Value(project->get_ProgramNameWithPath().c_str(), allocator).Move(), d.GetAllocator());

	// Put objectGeneralData into objectITProjectlData
	objectITProjectData.AddMember("General Data", objectGeneralData, allocator);

	// Design properties. ==============================================================
	rapidjson::Value objectDesignPropertiesData(rapidjson::kObjectType);
	objectDesignPropertiesData.AddMember("Number of Surfaces", project->get_MySurfaces()->size(), allocator);

	// Put objectDesignPropertiesData into objectITProjectlData
	objectITProjectData.AddMember("Design Properties", objectDesignPropertiesData, allocator);

	// FlowitCudaUnsteady. ==============================================================
	rapidjson::Value objectFlowitCudaUnsteadyData(rapidjson::kObjectType);
	objectFlowitCudaUnsteadyData.AddMember("MaxPropagationGeneration", project->get_MaxPropagationGeneration(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("FlowitPressureEps", project->get_FlowitPressureEps(), allocator);

	// This tortured code is for backwards compatibility of data files with OS X code.
	if (project->get_IsConstantSpeedTrajectories())
	{
		objectFlowitCudaUnsteadyData.AddMember("IsConstantSpeedTrajectories", 1, allocator);
	}
	else
	{
		objectFlowitCudaUnsteadyData.AddMember("IsConstantSpeedTrajectories", 0, allocator);
	}

	// This tortured code is for backwards compatibility of data files with OS X code.
	if (project->get_IsDoVelocityField())
	{
		objectFlowitCudaUnsteadyData.AddMember("IsDoVelocityField", 1, allocator);
	}
	else
	{
		objectFlowitCudaUnsteadyData.AddMember("IsDoVelocityField", 0, allocator);
	}

	Value key5("RankineAlgorithm", d.GetAllocator()); // copy string name
	objectFlowitCudaUnsteadyData.AddMember(key5, Value(project->get_RankineAlgorithm().c_str(), allocator).Move(), d.GetAllocator());
	objectFlowitCudaUnsteadyData.AddMember("RankineCoreRadius", project->get_RankineCoreRadius(), allocator);
	Value key6("VelocityFieldConstantPlane", d.GetAllocator()); // copy string name
	objectFlowitCudaUnsteadyData.AddMember(key6, Value(project->get_VelocityFieldConstantPlane().c_str(), allocator).Move(), d.GetAllocator());
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldNx", project->get_VelocityFieldNx(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldNy", project->get_VelocityFieldNy(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldNz", project->get_VelocityFieldNz(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMinx", project->get_VelocityFieldMinx(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMiny", project->get_VelocityFieldMiny(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMinz", project->get_VelocityFieldMinz(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMaxx", project->get_VelocityFieldMaxx(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMaxy", project->get_VelocityFieldMaxy(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldMaxz", project->get_VelocityFieldMaxz(), allocator);

	objectFlowitCudaUnsteadyData.AddMember("Rho", project->get_Rho(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("FramesPerSecond", project->get_FramesPerSecond(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("PressureDisplayFactor", project->get_PressureDisplayFactor(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VorticityDisplayFactor", project->get_VorticityDisplayFactor(), allocator);
	objectFlowitCudaUnsteadyData.AddMember("VelocityFieldDisplayFactor", project->get_VelocityFieldDisplayFactor(), allocator);

	// This tortured code is for backwards compatibility of data files with OS X code.
	if (project->get_IsSurfaceHierarchy())
	{
		objectFlowitCudaUnsteadyData.AddMember("IsSurfaceHierarchy", 1, allocator);
	}
	else
	{
		objectFlowitCudaUnsteadyData.AddMember("IsSurfaceHierarchy", 0, allocator);
	}

	// This tortured code is for backwards compatibility of data files with OS X code.
	if (project->get_IsGust())
	{
		objectFlowitCudaUnsteadyData.AddMember("IsGust", 1, allocator);
	}
	else
	{
		objectFlowitCudaUnsteadyData.AddMember("IsGust", 0, allocator);
	}

	objectFlowitCudaUnsteadyData.AddMember("ReplayDeltaTMSecs", project->get_ReplayDeltaTMSecs(), allocator);

	if (project->get_IsActiveControlSurfaces())
	{
		objectFlowitCudaUnsteadyData.AddMember("IsActiveControlSurfaces", 1, allocator);
	}
	else
	{
		objectFlowitCudaUnsteadyData.AddMember("IsActiveControlSurfaces", 0, allocator);
	}

	// Finally put objectFlowitCudaUnsteadyData into objectITProjectlData
	objectITProjectData.AddMember("FlowitCudaUnsteady Data", objectFlowitCudaUnsteadyData, allocator);

	// Finally put objectITProjectData into the document. ==================================================
	d.AddMember("ITProject Data", objectITProjectData, allocator);

	// Actually construct the ITSurfaces JSON data. =======================================================
	ITIO::writeMyITSurfacesToFile(&d);

	// Open the file for writing (don't append).
	myfile.open(fileNameWithPath, std::ofstream::out);

	//Convert JSON document to string
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	d.Accept(writer);
	const char* output = buffer.GetString();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "4");

	// Write the char* to the file.
	myfile << output;

	// Close the file.
	myfile.close();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyProjectToFile. Project has been written.");
}

void ITIO::writeMyITSurfacesToFile(rapidjson::Document* d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyITSurfacesToFile.");

	// must pass an allocator when the object may need to allocate memory
	rapidjson::Document::AllocatorType& allocator = d->GetAllocator();

	// Now set up a JSON array to contain the surfaces and add it to the project root.
	rapidjson::Value surfaceArray(rapidjson::kArrayType);

	for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop through all the surfaces in the v_s vector, adding data to the surfaceArray.
	{
		ITSurface *currentSurface = project->get_MySurfaces()->at(k);

		// Create the JSON object of the current Surface.
		rapidjson::Value currentSurfaceData(rapidjson::kObjectType);

		rapidjson::Value currentSurfaceProperties(rapidjson::kObjectType);

		Value key("Name", d->GetAllocator()); // copy string name
		char nameStr[10];
		sprintf(nameStr, "%i", k);
		currentSurfaceProperties.AddMember(key, Value(nameStr, allocator).Move(), d->GetAllocator());

		int NO_OF_ROWS = currentSurface->get_MyControlPoints()->size();
		int NO_OF_COLS = currentSurface->get_MyControlPoints()->at(0).size();

		currentSurfaceProperties.AddMember("Number of rows of control points", NO_OF_ROWS, allocator);
		currentSurfaceProperties.AddMember("Number of cols of control points", NO_OF_COLS, allocator);
		currentSurfaceProperties.AddMember("Number of rows of interpolated points", currentSurface->get_NoOfInterpolatedPointsU(), allocator);
		currentSurfaceProperties.AddMember("Number of cols of interpolated points", currentSurface->get_NoOfInterpolatedPointsV(), allocator);

		// This tortured code is for backwards compatibility of data files with OS X code.
		if (currentSurface->get_IsWake())
		{
			currentSurfaceProperties.AddMember("IsWake", 1, allocator);
		}
		else
		{
			currentSurfaceProperties.AddMember("IsWake", 0, allocator);
		}

		// This tortured code is for backwards compatibility of data files with OS X code.
		if (currentSurface->get_IsMorph())
		{
			currentSurfaceProperties.AddMember("IsMorph", 1, allocator);
		}
		else
		{
			currentSurfaceProperties.AddMember("IsMorph", 0, allocator);
		}

		currentSurfaceProperties.AddMember("MorphStartFrame", currentSurface->get_MorphStartFrame(), allocator);
		currentSurfaceProperties.AddMember("MorphEndFrame", currentSurface->get_MorphEndFrame(), allocator);

		Value key1("MorphType", d->GetAllocator()); // copy string name
		currentSurfaceProperties.AddMember(key1, Value(currentSurface->get_MorphType().c_str(), allocator).Move(), d->GetAllocator());

		currentSurfaceProperties.AddMember("ParentSurfaceIndex", currentSurface->get_ParentSurfaceIndex(), allocator);

		// This tortured code is for backwards compatibility of data files with OS X code.
		if (currentSurface->get_IsPistolesiPanelOffset())
		{
			currentSurfaceProperties.AddMember("IsPistolesiPanelOffset", 1, allocator);
		}
		else
		{
			currentSurfaceProperties.AddMember("IsPistolesiPanelOffset", 0, allocator);
		}

		currentSurfaceProperties.AddMember("PistolesiPanelOffsetFraction", currentSurface->get_PistolesiPanelOffsetFraction(), allocator);

		currentSurfaceProperties.AddMember("FuselageRadius", currentSurface->get_FuselageRadius(), allocator);

		// Add the properties to the surface object.
		currentSurfaceData.AddMember("Properties", currentSurfaceProperties, allocator);

		// Write control point data.
		// Now set up a JSON array to contain the control points.
		rapidjson::Value controlPointsArray(rapidjson::kArrayType);

		// Fill the array.
		for (int i = 0; i < NO_OF_ROWS; i++)
		{
			for (int j = 0; j < NO_OF_COLS; j++)
			{
				// Create the JSON char* of the current ITPoint.
				currentSurface->get_MyControlPoints()->at(i).at(j)->serializeMeAsJSONObject(k, i, j, &controlPointsArray, d);
			}
		}

		// Add the control points array to the surface object.
		currentSurfaceData.AddMember("Control Points", controlPointsArray, allocator);

		// Serialize the translation trajectory curve.
		rapidjson::Value translationCurveObject(rapidjson::kObjectType);
		writeDummyTranslationTrajectory(k, &translationCurveObject, d);
		// Add the translation curve object to the surface object.
		currentSurfaceData.AddMember("Trajectory Translation Curve", translationCurveObject, allocator);

		rapidjson::Value rotationCurveObject(rapidjson::kObjectType);
		writeDummyRotationTrajectory(k, &rotationCurveObject, d);
		// Add the rotation curve object to the surface object.
		currentSurfaceData.AddMember("Trajectory Rotation Curve", rotationCurveObject, allocator);

		// Finally add ITSurface to main array.
		surfaceArray.PushBack(currentSurfaceData, d->GetAllocator());

	} // End of k loop over surfaces.

	// Add the surface array to the document.
	d->AddMember("ITSurfaces", surfaceArray, allocator);
}

void ITIO::writeDummyTranslationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeDummyTranslationTrajectory");

	// Set up a JSON array to contain the control points.
	rapidjson::Value controlPointsArray(rapidjson::kArrayType);

	// Count through the segments.
	for (int i = 0; i < 4; i++)
	{
		// Create a new ITPointTrajectory point to do the serialization.
		ITPointTrajectory *tp = new ITPointTrajectory(i*5.0, 0, 0);

		tp->set_KeyFrame(20 * i);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeDummyTranslationTrajectory. KeyFrame: %i", tp->get_KeyFrame());

		// Write the point data to the controlPointsArray.
		tp->serializeMeAsJSONObject(k, i, 0, &controlPointsArray, d);

		// Delete temporary objects.
		delete tp;
	}

	// Serialize the final point in the trajectory.
	// Create a new ITPointTrajectory point to do the serialization.
	ITPointTrajectory *tp = new ITPointTrajectory(20, 0, 0);

	tp->set_KeyFrame(80);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeDummyTranslationTrajectory. KeyFrame: %i", tp->get_KeyFrame());

	// Write the point data to the controlPointsArray.
	tp->serializeMeAsJSONObject(k, 4, 0, &controlPointsArray, d);

	// Delete temporary objects.
	delete tp;

	// Add the control points array to the curveObject.
	trajectoryObject->AddMember("Control Points", controlPointsArray, d->GetAllocator());
}

void ITIO::writeDummyRotationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeDummyRotationTrajectory");

	// Serialize centre of rotation point.
	rapidjson::Value pointObject(rapidjson::kObjectType);
	pointObject.AddMember("x", 0.0, d->GetAllocator());
	pointObject.AddMember("y", 0.0, d->GetAllocator());
	pointObject.AddMember("z", 0.0, d->GetAllocator());

	// Add it to the curveObject.
	// Add the properties to the point node.
	trajectoryObject->AddMember("Centre of Rotation", pointObject, d->GetAllocator());

	// Set up a JSON array to contain the control points.
	rapidjson::Value controlPointsArray(rapidjson::kArrayType);

	// Count through the segments.
	for (int i = 0; i < 4; i++)
	{
		// Create a new ITPointTrajectory point to do the serialization.
		ITPointTrajectory *tp = new ITPointTrajectory(i*0.1, 0, 0);

		tp->set_KeyFrame(20 * i);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeDummyRotationTrajectory. KeyFrame: %i", tp->get_KeyFrame());

		// Write the point data to the controlPointsArray.
		tp->serializeMeAsJSONObject(k, i, 0, &controlPointsArray, d);

		// Delete temporary objects.
		delete tp;
	}

	// Serialize the final point in the trajectory.
	// Create a new ITPointTrajectory point to do the serialization.
	ITPointTrajectory *tp = new ITPointTrajectory(0.4, 0, 0);

	tp->set_KeyFrame(80);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeDummyRotationTrajectory. KeyFrame: %i", tp->get_KeyFrame());

	// Write the point data to the controlPointsArray.
	tp->serializeMeAsJSONObject(k, 4, 0, &controlPointsArray, d);

	// Delete temporary objects.
	delete tp;

	// Add the control points array to the curveObject.
	trajectoryObject->AddMember("Control Points", controlPointsArray, d->GetAllocator());
}