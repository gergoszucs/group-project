#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include <fstream> // std::ifstream
#include <QMessageBox> // QMessageBox

#include "ITIO.h"
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"
#include "ITControlSurface.h"
#include "ITPanel.h"
#include "ITWakePanel.h"
#include "ITPoint.h"
#include "ITVortex.h"
#include "ITBeam.h"
#include "ITBeamNode.h"

using namespace rapidjson;

void ITIO::readJSONInputFile(char *filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Start");

	FILE* fp = fopen(filenameWithPath, "r"); // non-Windows use "r"
	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	Document d;

	ParseResult ok = d.ParseStream(is);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Length of input stream: %i", strlen(readBuffer));
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

	// Organise surface index hierarchy chains for all surfaces.
	createSurfaceHierarchyChains();
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface hierarchy chains managed successfully.");

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of readJSONInputFile");
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
	const Value& pp = d["ITProject Data"]["FlowitCudaUnsteady Data"];

	project->set_MaxPropagationGeneration(pp["MaxPropagationGeneration"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MaxPropagationGeneration = %i", project->get_MaxPropagationGeneration());

	project->set_IsConstantSpeedTrajectories(pp["IsConstantSpeedTrajectories"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsConstantSpeedTrajectories = %i", project->get_IsConstantSpeedTrajectories());

	project->set_FlowitPressureEps(d["ITProject Data"]["FlowitCudaUnsteady Data"]["FlowitPressureEps"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FlowitPressureEps = %f", project->get_FlowitPressureEps());

	project->set_IsDoVelocityField(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsDoVelocityField"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsDoVelocityField = %i", project->get_IsDoVelocityField());

	project->set_RankineAlgorithm(d["ITProject Data"]["FlowitCudaUnsteady Data"]["RankineAlgorithm"].GetString());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "RankineAlgorithm = %s", project->get_RankineAlgorithm().c_str());

	project->set_RankineCoreRadius(d["ITProject Data"]["FlowitCudaUnsteady Data"]["RankineCoreRadius"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "RankineCoreRadius = %f", project->get_RankineCoreRadius());

	// Velocity Field parameters.
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

	// Physical constants.
	project->set_Rho(d["ITProject Data"]["FlowitCudaUnsteady Data"]["Rho"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Rho = %f", project->get_Rho());

	project->set_FramesPerSecond(d["ITProject Data"]["FlowitCudaUnsteady Data"]["FramesPerSecond"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FramesPerSecond = %f", project->get_FramesPerSecond());

	// Pressure Display Factors.
	project->set_PressureDisplayFactor(d["ITProject Data"]["FlowitCudaUnsteady Data"]["PressureDisplayFactor"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "PressureDisplayFactor = %f", project->get_PressureDisplayFactor());

	// Vorticity Display Factors.
	project->set_VorticityDisplayFactor(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VorticityDisplayFactor"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VorticityDisplayFactor = %f", project->get_VorticityDisplayFactor());

	// Velocity Field Display Factor.
	project->set_VelocityFieldDisplayFactor(d["ITProject Data"]["FlowitCudaUnsteady Data"]["VelocityFieldDisplayFactor"].GetDouble());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "VelocityFieldDisplayFactor = %f", project->get_VelocityFieldDisplayFactor());

	// Surface Hierarchy.
	project->set_IsSurfaceHierarchy(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsSurfaceHierarchy"].GetInt());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsSurfaceHierarchy = %i", project->get_IsSurfaceHierarchy());

	// IsGust
	// Because of the HasMember call, this parameter is optional, and need not be present in the json file.
	if (d["ITProject Data"]["FlowitCudaUnsteady Data"].HasMember("IsGust"))
	{
		project->set_IsGust(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsGust"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsGust = %i", project->get_IsGust());
	}
	else
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================================================= FuselageRadius NOT FOUND");
	}

	// ReplayDeltaTMSecs
	// Because of the HasMember call, this parameter is optional, and need not be present in the json file.
	if (d["ITProject Data"]["FlowitCudaUnsteady Data"].HasMember("ReplayDeltaTMSecs"))
	{
		project->set_ReplayDeltaTMSecs(d["ITProject Data"]["FlowitCudaUnsteady Data"]["ReplayDeltaTMSecs"].GetDouble());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ReplayDeltaTMSecs = %i", project->get_ReplayDeltaTMSecs());
	}
	else
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================================================= ReplayDeltaTMSecs NOT FOUND");
	}

	// IsActiveControlSurfaces
	// Because of the HasMember call, this parameter is optional, and need not be present in the json file.
	if (d["ITProject Data"]["FlowitCudaUnsteady Data"].HasMember("IsActiveControlSurfaces"))
	{
		project->set_IsActiveControlSurfaces(d["ITProject Data"]["FlowitCudaUnsteady Data"]["IsActiveControlSurfaces"].GetInt());
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "IsActiveControlSurfaces = %i", project->get_IsActiveControlSurfaces());
	}
	else
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================================================= IsActiveControlSurfaces NOT FOUND");
	}
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
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ITSurface %i created successfully.", k);

		// Create a new base surface object.
		ITSurface *bs = new ITSurface(); // This constructor creates a default array of control points. First thing we need to do is delete these default points and created our own.

		// Read ITSurface data from the JSON object.
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
		// This parameter is optional, and need not be present in the json file, so here we use the HasMember call to detect if it is present or not.
		if (d["ITSurfaces"][k]["Properties"].HasMember("FuselageRadius"))
		{
			s->set_FuselageRadius(d["ITSurfaces"][k]["Properties"]["FuselageRadius"].GetDouble());
			bs->set_FuselageRadius(d["ITSurfaces"][k]["Properties"]["FuselageRadius"].GetDouble());
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "FuselageRadius = %f", s->get_FuselageRadius());
		}
		else
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "========================================================= FuselageRadius NOT FOUND");
			s->set_FuselageRadius(0.0);
			bs->set_FuselageRadius(0.0);
		}

		// Push the surface into the array.
		project->get_MySurfaces()->push_back(s);
		project->get_MyBaseSurfaces()->push_back(bs);

		// Read the control points from the JSON document and initialise the control points for MySurfaces and MyBaseSurfaces.
		createControlPointsFromJSONObject(d, filenameWithPath, k);

		// Create the new trajectory curves from the JSON data (20160408).
		createTrajectoryCurvesFromJSON(d, k);
	}
}

void ITIO::createControlPointsFromJSONObject(Document& d, char * filenameWithPath, int k)
{
	//Find the number of rows and columns of control points in the k-th surface.
	int noOfRows = d["ITSurfaces"][k]["Properties"]["Number of rows of control points"].GetInt();
	int noOfCols = d["ITSurfaces"][k]["Properties"]["Number of cols of control points"].GetInt();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of rows = %i", noOfRows);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of cols = %i", noOfCols);

	int counter = 0;
	for (int i = 0; i < noOfRows; i++)
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> vb_dummy;

		for (int j = 0; j < noOfCols; j++)
		{
			float x = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["x"].GetDouble();
			float y = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["y"].GetDouble();
			float z = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["z"].GetDouble();
			float m = d["ITSurfaces"][k]["Control Points"][counter]["Properties"]["mass"].GetDouble();

			ITControlPoint* p = new ITControlPoint(x, y, z);
			ITControlPoint *pb = new ITControlPoint(x, y, z);

			v_dummy.push_back(p);
			vb_dummy.push_back(pb);

			counter++;
		}

		project->get_MySurfaces()->at(k)->get_MyControlPoints()->push_back(v_dummy);
		project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->push_back(vb_dummy);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No of rows in vector of vectors of control points = %i, %i", 
			project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(), project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->size());
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of createControlPointsFromJSONObject");
}

void ITIO::createTrajectoryCurvesFromJSON(Document& d, int k)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Reading from createTrajectoryCurvesFromJSON");

	const Value& translationCurveControlPoints = d["ITSurfaces"][k]["Trajectory Translation Curve"]["Control Points"];

	// Instanciate the ITTrajectoryCurve curves.
	ITTrajectoryCurve* cx = new ITTrajectoryCurve();
	ITTrajectoryCurve* cy = new ITTrajectoryCurve();
	ITTrajectoryCurve* cz = new ITTrajectoryCurve();

	ITTrajectoryCurve* bcx = new ITTrajectoryCurve();
	ITTrajectoryCurve* bcy = new ITTrajectoryCurve();
	ITTrajectoryCurve* bcz = new ITTrajectoryCurve();

	// Loop over the translation curve JSON data to create the ITTrajectoryCurveSegments 
	// Note that the loop omits the last point (if there are N points, then there are N-1 segments).
	// Note that the ordinal of each segment end point is stored in the x instance variable of the ITPoint object.
	int i = 0;
	for (Value::ConstValueIterator itr = translationCurveControlPoints.Begin(); itr != translationCurveControlPoints.End() - 1; ++itr)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "translation segment %i", i);

		float p0x = (float)translationCurveControlPoints[i]["Properties"]["x"].GetDouble();
		float p0y = (float)translationCurveControlPoints[i]["Properties"]["y"].GetDouble();
		float p0z = (float)translationCurveControlPoints[i]["Properties"]["z"].GetDouble();

		int keyFrame0 = (float)translationCurveControlPoints[i]["Properties"]["KeyFrame"].GetInt();

		float p1x = (float)translationCurveControlPoints[i + 1]["Properties"]["x"].GetDouble();
		float p1y = (float)translationCurveControlPoints[i + 1]["Properties"]["y"].GetDouble();
		float p1z = (float)translationCurveControlPoints[i + 1]["Properties"]["z"].GetDouble();

		int keyFrame1 = (float)translationCurveControlPoints[i + 1]["Properties"]["KeyFrame"].GetInt();

		// x.
		ITTrajectoryCurveSegment *sx = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0xp = new ITPointTrajectory(p0x, 0.0, 0.0);
		ITPointTrajectory *p1xp = new ITPointTrajectory(p1x, 0.0, 0.0);
		sx->set_P0_p(p0xp);
		sx->set_P1_p(p1xp);
		sx->set_StartKeyFrame(keyFrame0);
		sx->set_EndKeyFrame(keyFrame1);
		cx->get_MyTrajectoryCurveSegments()->push_back(sx);

		// y.
		ITTrajectoryCurveSegment *sy = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0yp = new ITPointTrajectory(p0y, 0.0, 0.0);
		ITPointTrajectory *p1yp = new ITPointTrajectory(p1y, 0.0, 0.0);
		sy->set_P0_p(p0yp);
		sy->set_P1_p(p1yp);
		sy->set_StartKeyFrame(keyFrame0);
		sy->set_EndKeyFrame(keyFrame1);
		cy->get_MyTrajectoryCurveSegments()->push_back(sy);

		// z.
		ITTrajectoryCurveSegment *sz = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0zp = new ITPointTrajectory(p0z, 0.0, 0.0);
		ITPointTrajectory *p1zp = new ITPointTrajectory(p1z, 0.0, 0.0);
		sz->set_P0_p(p0zp);
		sz->set_P1_p(p1zp);
		sz->set_StartKeyFrame(keyFrame0);
		sz->set_EndKeyFrame(keyFrame1);
		cz->get_MyTrajectoryCurveSegments()->push_back(sz);

		// Increment the segment counter.
		i++;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Finished translation segments");

	// Compute the m0 and m1 tangent vectors for each of the segments in the translation curves.
	cx->computeMySegmentEndTangentVectors();
	cy->computeMySegmentEndTangentVectors();
	cz->computeMySegmentEndTangentVectors();

	// Push the translation curves into the surface.
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cx);
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cy);
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cz);

	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcx);
	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcy);
	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcz);

	// Do rotations.
	const Value& rotationCurveControlPoints = d["ITSurfaces"][k]["Trajectory Rotation Curve"]["Control Points"];

	// Instanciate the ITTrajectoryCurve curves.
	ITTrajectoryCurve* cr = new ITTrajectoryCurve();
	ITTrajectoryCurve* cp = new ITTrajectoryCurve();
	ITTrajectoryCurve* cyaw = new ITTrajectoryCurve();

	ITTrajectoryCurve* bcr = new ITTrajectoryCurve();
	ITTrajectoryCurve* bcp = new ITTrajectoryCurve();
	ITTrajectoryCurve* bcyaw = new ITTrajectoryCurve();

	// Loop over the rotation curve JSON data to create the ITTrajectoryCurveSegments 
	// Note that the loop omits the last point (if there are N points, then there are N-1 segments).
	i = 0;
	for (Value::ConstValueIterator itr = rotationCurveControlPoints.Begin(); itr != rotationCurveControlPoints.End() - 1; ++itr)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rotation segment %i", i);

		float p0r = (float)rotationCurveControlPoints[i]["Properties"]["x"].GetDouble();
		float p0p = (float)rotationCurveControlPoints[i]["Properties"]["y"].GetDouble();
		float p0y = (float)rotationCurveControlPoints[i]["Properties"]["z"].GetDouble();

		int keyFrame0 = (float)rotationCurveControlPoints[i]["Properties"]["KeyFrame"].GetInt();

		float p1r = (float)rotationCurveControlPoints[i + 1]["Properties"]["x"].GetDouble();
		float p1p = (float)rotationCurveControlPoints[i + 1]["Properties"]["y"].GetDouble();
		float p1y = (float)rotationCurveControlPoints[i + 1]["Properties"]["z"].GetDouble();

		int keyFrame1 = (float)rotationCurveControlPoints[i + 1]["Properties"]["KeyFrame"].GetInt();

		// roll.
		ITTrajectoryCurveSegment *sr = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0rp = new ITPointTrajectory(p0r, 0.0, 0.0);
		ITPointTrajectory *p1rp = new ITPointTrajectory(p1r, 0.0, 0.0);
		sr->set_P0_p(p0rp);
		sr->set_P1_p(p1rp);
		sr->set_StartKeyFrame(keyFrame0);
		sr->set_EndKeyFrame(keyFrame1);
		cr->get_MyTrajectoryCurveSegments()->push_back(sr);

		// pitch.
		ITTrajectoryCurveSegment *sp = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0pp = new ITPointTrajectory(p0p, 0.0, 0.0);
		ITPointTrajectory *p1pp = new ITPointTrajectory(p1p, 0.0, 0.0);
		sp->set_P0_p(p0pp);
		sp->set_P1_p(p1pp);
		sp->set_StartKeyFrame(keyFrame0);
		sp->set_EndKeyFrame(keyFrame1);
		cp->get_MyTrajectoryCurveSegments()->push_back(sp);

		// yaw.
		ITTrajectoryCurveSegment *syaw = new ITTrajectoryCurveSegment();
		// Set the p0 and p1 instance variables.
		ITPointTrajectory *p0yp = new ITPointTrajectory(p0y, 0.0, 0.0);
		ITPointTrajectory *p1yp = new ITPointTrajectory(p1y, 0.0, 0.0);
		syaw->set_P0_p(p0yp);
		syaw->set_P1_p(p1yp);
		syaw->set_StartKeyFrame(keyFrame0);
		syaw->set_EndKeyFrame(keyFrame1);
		cyaw->get_MyTrajectoryCurveSegments()->push_back(syaw);

		// Increment the segment counter.
		i++;
	}

	// Compute the m0 and m1 tangent vectors for each of the segments in the translation curves.
	cr->computeMySegmentEndTangentVectors();
	cp->computeMySegmentEndTangentVectors();
	cyaw->computeMySegmentEndTangentVectors();

	// Push the translation curves into the surface.
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cr);
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cp);
	project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(cyaw);

	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcr);
	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcp);
	project->get_MyBaseSurfaces()->at(k)->get_MyTrajectoryCurves()->push_back(bcyaw);

	// Read the centre of rotation point.
	float rotX = (float)d["ITSurfaces"][k]["Trajectory Rotation Curve"]["Centre of Rotation"]["x"].GetDouble();
	float rotY = (float)d["ITSurfaces"][k]["Trajectory Rotation Curve"]["Centre of Rotation"]["y"].GetDouble();
	float rotZ = (float)d["ITSurfaces"][k]["Trajectory Rotation Curve"]["Centre of Rotation"]["z"].GetDouble();

	project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_X(rotX);
	project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_Y(rotY);
	project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_Z(rotZ);

	project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_X(rotX);
	project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_Y(rotY);
	project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint()->set_Z(rotZ);

	// Finally update the project maxKeyFrame instance variable.
	// Set it to the end key frame of the last segment of the z trajectory of the last surface.
	project->set_MaxKeyFrame(project->get_MySurfaces()->back()->get_MyTrajectoryCurves()->back()->get_MyTrajectoryCurveSegments()->back()->get_EndKeyFrame());
}

void ITIO::createSurfaceHierarchyChains()
{
	// Organise surface index hierarchy chains for all surfaces.
	if (project->get_IsSurfaceHierarchy())
	{
		// Loop through all the surfaces.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			ITSurface *s = project->get_MySurfaces()->at(k);

			// Check if the k-th surface has ancestors.
			int parentIndex = s->get_ParentSurfaceIndex();

			// Create ancestor chain for the k-th surface.
			int counter = 0;
			while ((parentIndex > -1) && (counter < 100))
			{
				s->get_MyAncestorSurfaceIndices()->push_back(parentIndex);
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Pushing %i onto AncestorSurfaceIndices vector.", s->get_MyAncestorSurfaceIndices()->back());
				parentIndex = project->get_MySurfaces()->at(parentIndex)->get_ParentSurfaceIndex();
				counter++; // Prevent infinite loop if JSON file has parent index logic errors.
			}

			// At this point the ancestor vector is filled with the oldest ancestor last.
			// So, the next line reverses the order of the indices in the vector, so the oldest ancestor is first.
			std::reverse(s->get_MyAncestorSurfaceIndices()->begin(), s->get_MyAncestorSurfaceIndices()->end());

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Ancestors for surface %i:", k);
			for (int kk = 0; kk < s->get_MyAncestorSurfaceIndices()->size(); kk++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Ancestor %i is %i", kk, s->get_MyAncestorSurfaceIndices()->at(kk));
			}
		}
	}
}

void ITIO::readJSONControlSurfaces(char *filenameWithPath)
{
	// The interpolated points and the ITPanels have been instanciated, so we can set Control Surface flags.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Start readJSONControlSurfaces. Filename: %s", filenameWithPath);

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

	// Close the JSON file.
	fclose(fp);

	// Now read the control surfaces of each ITSurface.
	assert(d.IsObject());
	assert(d.HasMember("ITSurfaces"));

	// Loop through the surfaces and instanciate ITSurface objects.
	for (int k = 0; k < d["ITSurfaces"].Size(); k++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Checking control surfaces for ITSurface: %i", k);

		// Loop over control surfaces for the current ITSurface.
		if (d["ITSurfaces"][k].HasMember("Control Surfaces"))
		{
			for (int n = 0; n < d["ITSurfaces"][k]["Control Surfaces"].Size(); n++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "        Reading control surface %i for ITSurface: %i", n, k);

				ITControlSurface *cs = new ITControlSurface();
				project->get_MySurfaces()->at(k)->get_MyControlSurfaces()->push_back(cs);

				ITControlSurface *csb = new ITControlSurface();
				project->get_MyBaseSurfaces()->at(k)->get_MyControlSurfaces()->push_back(csb);

				// Loop over the panels of the current 
				for (int g = 0; g < d["ITSurfaces"][k]["Control Surfaces"][n]["Panels"].Size(); g++)
				{
					int cs_row = d["ITSurfaces"][k]["Control Surfaces"][n]["Panels"][g]["row"].GetInt();
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============================================ cs_row = %i", cs_row);
					int cs_col = d["ITSurfaces"][k]["Control Surfaces"][n]["Panels"][g]["col"].GetInt();
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============================================ cs_col = %i", cs_col);

					// Set the ITPanel flag corresponding to this row and column.
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(cs_row).at(cs_col)->set_IsControlSurface(true);
					project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(cs_row).at(cs_col)->set_IsControlSurface(true);

					// Push the ITPanel onto the vector of ITPanels in the current ITControlSurface.
					cs->get_MyITPanels()->push_back(project->get_MySurfaces()->at(k)->get_MyPanels()->at(cs_row).at(cs_col));
					csb->get_MyITPanels()->push_back(project->get_MyBaseSurfaces()->at(k)->get_MyPanels()->at(cs_row).at(cs_col));
				}
			}
		}
	}
}

bool ITIO::doesFileExist(const char *filename)
{
	// Ref: http://stackoverflow.com/questions/13945341/c-check-if-a-file-exists-without-being-able-to-read-write-possible
	// Ref: http://stackoverflow.com/questions/3828835/how-can-we-check-if-a-file-exists-or-not-using-win32-program

	FILE *fp;
	fpos_t fsize = 0;

	if (!fopen_s(&fp, filename, "r"))
	{
		fseek(fp, 0, SEEK_END);
		fgetpos(fp, &fsize);
		fclose(fp);
	}

	return fsize > 0;
}

void ITIO::readMyVelocityFieldHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside readMyVelocityFieldHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".VFH".
	newfileNameWithPathString.append(".VFH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Velocity Field History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfRows;
		int noOfCols;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfFrames, &noOfRows, &noOfCols);
		fscanf(fp, "%s", headerString);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "noOfFrames: %i, noOfRows: %i, noOfCols: %i, headerString: %s", noOfFrames, noOfRows, noOfCols, headerString);

		// Read the data into vector of vectors.
		// Treat first frame separately.
		std::vector < std::vector <ITPoint*> > *dummyVectorOfVectorsOfVelocityFieldPointsFrame0 = new std::vector < std::vector <ITPoint*> >;
		// Store a snapshot of data.
		project->get_VelocityFieldDataHistory()->push_back(*(dummyVectorOfVectorsOfVelocityFieldPointsFrame0));

		// Read the array into memory.
		for (int n = 1; n < noOfFrames; n++)
		{
			std::vector < std::vector <ITPoint*> > *dummyVectorOfVectorsOfVelocityFieldPoints = new std::vector < std::vector <ITPoint*> >;
			int velocityFieldArrayIndex = 0;

			for (int i = 0; i < noOfRows; i++) // Loop over the rows of points in the current frame.
			{
				std::vector <ITPoint*> *dummyVectorOfVeocityFieldPoints = new std::vector <ITPoint*>;

				for (int j = 0; j < noOfCols; j++) // Loop over the cols of points in the current frame.
				{
					// Copy file data to new ITPoint object.  
					int ntemp;
					int itemp;
					int jtemp;

					float x;
					float y;
					float z;
					float vx;
					float vy;
					float vz;

					fscanf(fp, " %i %i %i %f %f %f %f %f %f", &ntemp, &itemp, &jtemp, &x, &y, &z, &vx, &vy, &vz);
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "ntemp: %i, itemp: %i, jtemp: %i, x: %f, y: %f, z: %f", ntemp, itemp, jtemp, x, y, z);

					ITPoint * p = new ITPoint(x, y, z);
					p->set_VX(vx);
					p->set_VY(vy);
					p->set_VZ(vz);

					// Store the velocity field ITPoint object in the vector.
					dummyVectorOfVeocityFieldPoints->push_back(p);

					velocityFieldArrayIndex++;
				}

				dummyVectorOfVectorsOfVelocityFieldPoints->push_back(*dummyVectorOfVeocityFieldPoints);
			}

			// Store a snapshot of data.
			project->get_VelocityFieldDataHistory()->push_back(*(dummyVectorOfVectorsOfVelocityFieldPoints));
		}
		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Velocity Field History has been read successfully!!!");
	}
}

void ITIO::readMyPressureHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside readMyPressureHistoryFromFile.");
	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".PDH".
	newfileNameWithPathString.append(".PDH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Pressure Distribution History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfRows;
		int noOfCols;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfFrames, &noOfRows, &noOfCols);
		fscanf(fp, "%s", headerString);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfFrames: %i, noOfRows: %i, noOfCols: %i, headerString: %s", noOfFrames, noOfRows, noOfCols, headerString);

		int ktemp, ntemp, itemp, jtemp;
		float currentPressure;

		// Read the data into vector of vectors.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			for (int n = 0; n < noOfFrames; n++)
			{
				std::vector < std::vector <float> > *dummyVectorOfVectorsOfPressures = new std::vector < std::vector <float> >;

				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsU() - 1; i++)
				{
					std::vector <float> *dummyVectorOfPressures = new std::vector <float>;

					for (int j = 0; j < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsV() - 1; j++)
					{
						fscanf(fp, "  %i %i %i %i %f", &ktemp, &ntemp, &itemp, &jtemp, &currentPressure);
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "ktemp: %i ntemp: %i, itemp: %i, jtemp: %i, currentPressure: %f", ktemp, ntemp, itemp, jtemp, currentPressure);
						// Store the pressure value from the JSON array.
						dummyVectorOfPressures->push_back(currentPressure);
					}

					dummyVectorOfVectorsOfPressures->push_back(*dummyVectorOfPressures);
				}

				// Store a snapshot of data for this frame.
				project->get_MySurfaces()->at(k)->get_MyPressureHistory()->push_back(*(dummyVectorOfVectorsOfPressures));
			}
		}
		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Pressure Distribution History has been read successfully!!!");
	}
}

void ITIO::readMyVorticityHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside readMyVorticityHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".VDH".
	newfileNameWithPathString.append(".VDH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Vorticity Distribution History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfRows;
		int noOfCols;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfFrames, &noOfRows, &noOfCols);
		fscanf(fp, "%s", headerString);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfFrames: %i, noOfRows: %i, noOfCols: %i, headerString: %s", noOfFrames, noOfRows, noOfCols, headerString);

		int ktemp, ntemp, itemp, jtemp;
		float currentVorticity;

		// Read the data into vector of vectors.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			for (int n = 0; n < noOfFrames; n++)
			{
				std::vector < std::vector <float> > *dummyVectorOfVectorsOfVorticities = new std::vector < std::vector <float> >;

				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsU() - 1; i++)
				{
					std::vector <float> *dummyVectorOfVorticities = new std::vector <float>;

					for (int j = 0; j < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsV() - 1; j++)
					{
						fscanf(fp, "  %i %i %i %i %f", &ktemp, &ntemp, &itemp, &jtemp, &currentVorticity);
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "ktemp: %i ntemp: %i, itemp: %i, jtemp: %i, currentVorticity: %f", ktemp, ntemp, itemp, jtemp, currentVorticity);

						// Store the pressure value from the JSON array.
						dummyVectorOfVorticities->push_back(currentVorticity);
					}

					dummyVectorOfVectorsOfVorticities->push_back(*dummyVectorOfVorticities);
				}

				// Store a snapshot of data for this frame.
				project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->push_back(*(dummyVectorOfVectorsOfVorticities));
			}
		}

		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Vorticity Distribution History has been read successfully!!!");
	}
}

void ITIO::readMyWakePanelHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside readMyWakePanelHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".WPH".
	newfileNameWithPathString.append(".WPH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Wake Panel History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfRows;
		int dummy;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfFrames, &dummy, &noOfRows);
		fscanf(fp, "%s", headerString);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfFrames: %i, dummy: %i noOfRows: %i, headerString: %s", noOfFrames, dummy, noOfRows, headerString);

		int ktemp, ntemp, gtemp, itemp;
		float vorticity, blx, bly, blz, brx, bry, brz, trx, trY, trz, tlx, tly, tlz, mpx, mpy, mpz, nx, ny, nz;

		for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop over the surfaces.
		{
			// Push an empty vector onto the instance variable for the first (0) frame.
			std::vector < std::vector <ITWakePanel*> > *dummyVectorOfVectors = new std::vector < std::vector <ITWakePanel*> >;
			project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->push_back(*(dummyVectorOfVectors));

			for (int n = 0; n < noOfFrames; n++) // Loop over the frames. Note that the data file only contains wake panels for frame 1 and beyond.
			{
				std::vector < std::vector <ITWakePanel*> > *dummyVectorOfVectorsOfWakes = new std::vector < std::vector <ITWakePanel*> >;

				// Account for truncated wakes.
				int noOfGenerationsInCurrentFrame = n + 1;
				if ((project->get_MaxPropagationGeneration() != 0) && (project->get_MaxPropagationGeneration() < n))
				{
					noOfGenerationsInCurrentFrame = project->get_MaxPropagationGeneration();
				}

				for (int g = 0; g < noOfGenerationsInCurrentFrame; g++) // Loop over generations in current frame.
				{
					std::vector <ITWakePanel*> *dummyVectorOfWakes = new std::vector <ITWakePanel*>;
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Reading wake panel history for frame: %i, generation: %i, number of rows: %i", 
						n, g, project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsU());

					// Loop along ITPanels on trailing edge. Note that different surfaces may have different numbers of rows of trailing edge panels.
					for (int i = 0; i < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsU() - 1; i++) 
					{
						fscanf(fp, " %i %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &ktemp, &ntemp, &gtemp, &itemp, &vorticity,
							&blx,
							&bly,
							&blz,
							&brx,
							&bry,
							&brz,
							&trx,
							&trY,
							&trz,
							&tlx,
							&tly,
							&tlz,
							&mpx,
							&mpy,
							&mpz,
							&nx,
							&ny,
							&nz
						);

						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "ktemp: %i, ntemp: %i gtemp: %i, itemp: %i", ktemp, ntemp, gtemp, itemp);
						// Copy file data to new ITWakePanel object.
						// Next get the mid point and normal.
						ITPoint * midPt = new ITPoint(mpx, mpy, mpz);
						ITPoint * normalPt = new ITPoint(nx, ny, nz);

						// Next get the vertices.
						ITPoint * bl = new ITPoint(blx, bly, blz);
						ITPoint * br = new ITPoint(brx, bry, brz);
						ITPoint * tr = new ITPoint(trx, trY, trz);
						ITPoint * tl = new ITPoint(tlx, tly, tlz);

						ITWakePanel * currentWakePanelObject = new ITWakePanel(bl, br, tr, tl, project->get_MySurfaces()->at(k), 1);

						currentWakePanelObject->set_MidPoint(midPt);
						currentWakePanelObject->set_Normal(normalPt);
						currentWakePanelObject->set_I(i);
						currentWakePanelObject->set_J(0);
						currentWakePanelObject->set_K(k);
						currentWakePanelObject->set_F(n);

						currentWakePanelObject->set_MyVorticity(vorticity);
						// Store the Wake Panel object in the vector.
						dummyVectorOfWakes->push_back(currentWakePanelObject);
					}

					dummyVectorOfVectorsOfWakes->push_back(*dummyVectorOfWakes);
				}

				// Store a snapshot of data.
				project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->push_back(*(dummyVectorOfVectorsOfWakes));
			}
		}

		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Wake Panel History has been read successfully!!!");
	}
}

void ITIO::readMyBeamNodeHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside readMyBeamNodeHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".BNH".
	newfileNameWithPathString.append(".BNH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Beam Node History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfSurfaces;
		int noOfRows;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfSurfaces, &noOfFrames, &noOfRows); // k, g, i
		fscanf(fp, "%s", headerString);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfSurfaces: %i, noOfFrames: %i noOfRows: %i, headerString: %s", noOfSurfaces, noOfFrames, noOfRows, headerString);

		int ktemp, gtemp, itemp;
		float x, y, z, wx, wy, wz, m;

		for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop over the surfaces.
		{
			// Create an empty vector of vectors.
			std::vector < std::vector <ITBeamNode*> > *dummyVectorOfVectors = new std::vector < std::vector <ITBeamNode*> >;

			// Push an empty vector onto the instance variable for the first (0) and second (1) frame.
			std::vector <ITBeamNode*> *dummyVector0 = new std::vector <ITBeamNode*>;
			project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->push_back(*(dummyVector0));
			std::vector <ITBeamNode*> *dummyVector1 = new std::vector <ITBeamNode*>;
			project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->push_back(*(dummyVector1));

			for (int n = 0; n < noOfFrames; n++) // Loop over the frames. Note that the data file only contains wake panels for frame 1 and beyond.
			{
				std::vector <ITBeamNode*> *dummyVectorOfNodes = new std::vector <ITBeamNode*>;

				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_NoOfInterpolatedPointsU() - 1; i++) // Loop along the beam. Note that different beams may have different numbers of nodes.
				{
					fscanf(fp, " %i %i %i %f %f %f %f %f %f %f", &ktemp, &gtemp, &itemp,
						&x,
						&y,
						&z,
						&wx,
						&wy,
						&wz,
						&m
					);

					// Copy file data to new ITBeamNode object.      
					ITBeamNode *currentNode = new ITBeamNode(x, y, z);

					currentNode->set_Mass(m);
					currentNode->get_W()->set_X(wx);
					currentNode->get_W()->set_X(wy);
					currentNode->get_W()->set_X(wz);

					// Store the Beam Node object in the vector.
					dummyVectorOfNodes->push_back(currentNode);
				}
				dummyVectorOfVectors->push_back(*dummyVectorOfNodes);
			}
			
			// Store data.
			project->get_MySurfaces()->at(k)->get_MyBeam()->set_MyNodeHistory(dummyVectorOfVectors);
		}

		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Beam Node History has been read successfully!!!");
	}
}

void ITIO::readMyOutputDataHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside readMyOutputDataHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".ODH".
	newfileNameWithPathString.append(".ODH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Output Data History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		char headerString[200];

		fscanf(fp, "%i", &noOfFrames);
		fscanf(fp, "%s", headerString);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfFrames: %i, headerString: %s", noOfFrames, headerString);

		int ktemp, ntemp;
		float x, y, z;

		// Read the data into vector of vectors.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			for (int n = 0; n < noOfFrames; n++)
			{
				fscanf(fp, "  %i %i %f %f %f", &ktemp, &ntemp, &x, &y, &z);
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "ktemp: %i ntemp: %i, x: %f, y: %f, z: %f", ktemp, ntemp, x, y, z);

				ITPoint *p = new ITPoint(x, y, z);
				project->get_MySurfaces()->at(k)->get_MyForceHistory()->push_back(p);
			}
		}

		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Output Data History has been read successfully!!!");
	}
}

void ITIO::readMyControlPointDeflectionHistoryFromFile(char * filenameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside readMyControlPointDeflectionHistoryFromFile.");

	// Create file path name string.
	// Create a std:string from filenameWithPath.
	std::string fileNameWithPathString(filenameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".CPH".
	newfileNameWithPathString.append(".CPH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "newfileNameWithPathString: %s", newfileNameWithPathString);

	int iii = ITIO::doesFileExist(newfileNameWithPathString.c_str());
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "doesFileExist return value: %i", iii);

	if (iii)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Control Point Deformation History file found!!!");

		// Open the file for reading.
		FILE *fp;
		fp = fopen(newfileNameWithPathString.c_str(), "r");

		int noOfFrames;
		int noOfRows;
		int noOfCols;
		char headerString[200];

		fscanf(fp, "%i, %i, %i", &noOfFrames, &noOfRows, &noOfCols);
		fscanf(fp, "%s", headerString);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "noOfFrames: %i, noOfRows: %i, noOfCols: %i, headerString: %s", noOfFrames, noOfRows, noOfCols, headerString);

		int ktemp, ntemp, itemp, jtemp;
		float x, y, z, vx, vy, vz;

		// Read the data into vector of vectors.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			for (int n = 0; n < noOfFrames; n++)
			{
				std::vector < std::vector <ITPoint*> > *dummyVectorOfVectorsOfControlPointDeflections = new std::vector < std::vector <ITPoint*> >;

				for (int i = 0; i < noOfRows; i++)
				{
					std::vector <ITPoint*> *dummyVectorOfControlPointDeflections = new std::vector <ITPoint*>;

					for (int j = 0; j < noOfCols; j++)
					{
						fscanf(fp, "  %i %i %i %i %f %f %f %f %f %f", &ktemp, &ntemp, &itemp, &jtemp, &x, &y, &z, &vx, &vy, &vz);
						// project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "ktemp: %i ntemp: %i, itemp: %i, jtemp: %i, x: %f, y: %f, z: %f", ktemp, ntemp, itemp, jtemp, x, y, z);

						// Create an ITPoint object and initialize its instance variables.
						ITPoint* p = new ITPoint(x, y, z);
						p->set_VX(vx);
						p->set_VY(vy);
						p->set_VZ(vz);

						// Store the pressure value from the JSON array.
						dummyVectorOfControlPointDeflections->push_back(p);

					}

					dummyVectorOfVectorsOfControlPointDeflections->push_back(*dummyVectorOfControlPointDeflections);
				}
				// Store a snapshot of data for this frame.
				project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->push_back(*(dummyVectorOfVectorsOfControlPointDeflections));
			}
		}
		
		// Close the file.
		fclose(fp);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "============= Control Point Deformation History has been read successfully!!!");
	}
}

void ITIO::readJSONBeamNodes(char *filenameWithPath)
{
	// The interpolated points and the ITPanels have been instanciated, so we can set beam node data.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Start readJSONBeamNodes. Filename: %s", filenameWithPath);

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

	// Close the JSON file.
	fclose(fp);

	// Now read the control surfaces of each ITSurface.
	assert(d.IsObject());
	assert(d.HasMember("ITSurfaces"));

	// Loop through the surfaces and instanciate ITSurface objects.
	for (int k = 0; k < d["ITSurfaces"].Size(); k++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Checking beam nodes for ITSurface: %i", k);

		// Loop over control surfaces for the current ITSurface.
		if (d["ITSurfaces"][k].HasMember("Beam"))
		{
			for (int n = 0; n < d["ITSurfaces"][k]["Beam"]["Beam Nodes"].Size(); n++)
			{
				int index = d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["Index"].GetInt();
				float x = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["x"].GetDouble());
				float y = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["y"].GetDouble());
				float z = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["z"].GetDouble());
				float m = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["Mass"].GetDouble());
				float E = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["E"].GetDouble());
				float Ix = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["Ix"].GetDouble());
				float Iz = (float)(d["ITSurfaces"][k]["Beam"]["Beam Nodes"][n]["Iz"].GetDouble());

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Beam node %i, index: %i", n, index);

				ITBeamNode *node = new ITBeamNode(x, y, z);
				node->set_Mass(m);

				node->set_MassPerUnitLength(m / 1.4);
				node->set_E(E);

				node->set_Ix(Ix);
				node->set_Iz(Iz);

				project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->push_back(node);
			}
		}
		else
		{
			// Beam data for this surface was not present.
			if (IsModeEulerBernoulli)
			{
				// Disable EB computations and warn user.
				if (MY_RUN_MODE == MYGUI)
				{
					QMessageBox::information(0, "Beam data", "Beam data is missing for one or more surfaces. \n\n Euler-Bernoulli computations will be disabled.");
				}
				w->on_actionEuler_Bernoulli_triggered();
			}
		}
	}
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

	//  ==================================================================
	// FlowitCudaUnsteady data.
	//  ==================================================================
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

	// Write the Rankine Algorithm.
	Value key5("RankineAlgorithm", d.GetAllocator()); // copy string name
	objectFlowitCudaUnsteadyData.AddMember(key5, Value(project->get_RankineAlgorithm().c_str(), allocator).Move(), d.GetAllocator());

	// Rankine core radius.
	objectFlowitCudaUnsteadyData.AddMember("RankineCoreRadius", project->get_RankineCoreRadius(), allocator);

	// Write the velocity field parameters.
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

	// IsGust
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

	// Put objectFlowitCudaUnsteadyData into objectITProjectlData
	objectITProjectData.AddMember("FlowitCudaUnsteady Data", objectFlowitCudaUnsteadyData, allocator);

	// ==============================================================
	// Design properties. 
	// ==============================================================
	rapidjson::Value objectDesignPropertiesData(rapidjson::kObjectType);
	objectDesignPropertiesData.AddMember("Number of Surfaces", project->get_MySurfaces()->size(), allocator);

	// Put objectDesignPropertiesData into objectITProjectlData
	objectITProjectData.AddMember("Design Properties", objectDesignPropertiesData, allocator);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "2");

	// Put objectITProjectData into the document.
	d.AddMember("ITProject Data", objectITProjectData, allocator);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "3");

	// ==============================================================
	// Actually construct the ITSurfaces JSON data. 
	// ==============================================================
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

	if (project->get_VelocityFieldDataHistory()->at(1).size() > 0)
	{
		ITIO::writeMyVelocityFieldHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyPressureHistory()->size() > 0)
	{
		ITIO::writeMyPressureHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->size() > 0)
	{
		ITIO::writeMyWakePanelHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyVorticityHistory()->size() > 0)
	{
		ITIO::writeMyVorticityHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyBeam()->get_MyNodeHistory()->size() > 0)
	{
		ITIO::writeMyBeamNodeHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyForceHistory()->size() > 0)
	{
		ITIO::writeMyOutputDataHistoryToFile(fileNameWithPath);
	}

	if (project->get_MySurfaces()->at(0)->get_MyControlPointDeflectionHistory()->size() > 0)
	{
		ITIO::writeMyControlPointDeflectionHistoryToFile(fileNameWithPath);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyProjectToFile. Project has been written.");

}

void ITIO::writeMyITSurfacesToFile(rapidjson::Document* d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside writeMyITSurfacesToFile.");

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
		// We need to make sure the surface is back at the base location (and undeformed geometry).
		currentSurface->moveMeBackToBase(k);

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
		writeCurrentSurfaceTranslationTrajectory(k, &translationCurveObject, d);
		// Add the translation curve object to the surface object.
		currentSurfaceData.AddMember("Trajectory Translation Curve", translationCurveObject, allocator);

		// Serialize the rotation trajectory curve.
		rapidjson::Value rotationCurveObject(rapidjson::kObjectType);
		writeCurrentSurfaceRotationTrajectory(k, &rotationCurveObject, d);
		// Add the rotation curve object to the surface object.
		currentSurfaceData.AddMember("Trajectory Rotation Curve", rotationCurveObject, allocator);

		// Serialize the control surfaces.
		rapidjson::Value controlSurfacesArray(rapidjson::kArrayType);
		writeCurrentSurfaceControlSurfaces(k, &controlSurfacesArray, d);
		// Add the control surfaces object to the surface object.
		currentSurfaceData.AddMember("Control Surfaces", controlSurfacesArray, allocator);

		// Finally add ITSurface to main array.
		surfaceArray.PushBack(currentSurfaceData, d->GetAllocator());
	}
	
	// Add the surface array to the document.
	d->AddMember("ITSurfaces", surfaceArray, allocator);
}

void ITIO::writeMyVelocityFieldHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyVelocityFieldHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".VFH".
	newfileNameWithPathString.append(".VFH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_VelocityFieldDataHistory()->size(), project->get_VelocityFieldDataHistory()->at(1).size(), project->get_VelocityFieldDataHistory()->at(1).at(0).size()); // n, i, j.
	fprintf(fp, "n,i,j,X,Y,Z,VX,VY,VZ\n");

	// Loop through the frames.
	for (int n = 0; n < project->get_VelocityFieldDataHistory()->size(); n++)
	{
		// Loop through the rows of velocity vector ITPoint objects.
		for (int i = 0; i < project->get_VelocityFieldDataHistory()->at(n).size(); i++)
		{
			// Loop through columns of velocity vector ITPoint objects.
			for (int j = 0; j < project->get_VelocityFieldDataHistory()->at(n).at(i).size(); j++)
			{
				fprintf(fp, "%i %i %i %f %f %f %f %f %f\n", n, i, j, project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_X(),
					project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_Y(),
					project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_Z(),
					project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_VX(),
					project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_VY(),
					project->get_VelocityFieldDataHistory()->at(n).at(i).at(j)->get_VZ());

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "%i, %i, %i", n, i, j);
			}
		}
	}

	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyVelocityFieldHistoryToFile finished successfully.");
}

void ITIO::writeMyPressureHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyPressureHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".PDH".
	newfileNameWithPathString.append(".PDH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_MySurfaces()->at(0)->get_MyPressureHistory()->size(), project->get_MySurfaces()->at(0)->get_MyPressureHistory()->at(0).size(), project->get_MySurfaces()->at(0)->get_MyPressureHistory()->at(0).at(0).size()); // n, i, j.
	fprintf(fp, "k,n,i,j,p\n");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyPressureHistory()->size(); n++)
		{
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(n).size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(n).at(i).size(); j++)
				{
					fprintf(fp, "%i %i %i %i %f\n", k, n, i, j, project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(n).at(i).at(j));
				}
			}
		}
	}
	
	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyPressureHistoryToFile finished successfully.");
}

void ITIO::writeMyWakePanelHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyWakePanelHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".WPH".
	newfileNameWithPathString.append(".WPH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Stuff 3: %i",
		project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->size());

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->size(),
		project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->at(1).size(),
		project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->at(1).at(0).size()); // n, i, j. Remember that the first generation contains no wake panels.
	fprintf(fp, "k,n,g,i,vorticity,bl,br,tr,tl,midpt,normal\n");


	for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop over surfaces.
	{
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->size(); n++) // Loop over frames. Note that the first frame has no wake panels and is empty.
		{
			for (int g = 0; g < project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(n).size(); g++) // Loop over generations.
			{
				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(n).at(g).size(); i++) // Loop down the trailing edges.
				{
					ITWakePanel *currentWakePanel = project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(n).at(g).at(i);

					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "currentWakePanel vorticity: %f, %f", currentWakePanel->get_MyVorticity(),
						currentWakePanel->get_MyVortices()->at(0)->get_StartPoint()->get_X());

					fprintf(fp, "%i %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", k, n, g, i, currentWakePanel->get_MyVorticity(),
						currentWakePanel->get_BottomLeftPoint()->get_X(),
						currentWakePanel->get_BottomLeftPoint()->get_Y(),
						currentWakePanel->get_BottomLeftPoint()->get_Z(),
						currentWakePanel->get_BottomRightPoint()->get_X(),
						currentWakePanel->get_BottomRightPoint()->get_Y(),
						currentWakePanel->get_BottomRightPoint()->get_Z(),
						currentWakePanel->get_TopRightPoint()->get_X(),
						currentWakePanel->get_TopRightPoint()->get_Y(),
						currentWakePanel->get_TopRightPoint()->get_Z(),
						currentWakePanel->get_TopLeftPoint()->get_X(),
						currentWakePanel->get_TopLeftPoint()->get_Y(),
						currentWakePanel->get_TopLeftPoint()->get_Z(),
						currentWakePanel->get_MidPoint()->get_X(),
						currentWakePanel->get_MidPoint()->get_Y(),
						currentWakePanel->get_MidPoint()->get_Z(),
						currentWakePanel->get_Normal()->get_X(),
						currentWakePanel->get_Normal()->get_Y(),
						currentWakePanel->get_Normal()->get_Z()
					);
				}
			}
		}
	}
	
	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyWakeHistoryToFile finished successfully.");
}

void ITIO::writeMyVorticityHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyVorticityHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".PDH".
	newfileNameWithPathString.append(".VDH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_MySurfaces()->at(0)->get_MyVorticityHistory()->size(), project->get_MySurfaces()->at(0)->get_MyVorticityHistory()->at(0).size(), project->get_MySurfaces()->at(0)->get_MyVorticityHistory()->at(0).at(0).size()); // n, i, j.
	fprintf(fp, "k,n,i,j,p\n");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->size(); n++)
		{
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->at(n).size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->at(n).at(i).size(); j++)
				{
					fprintf(fp, "%i %i %i %i %f\n", k, n, i, j, project->get_MySurfaces()->at(k)->get_MyVorticityHistory()->at(n).at(i).at(j));
				}
			}
		}
	}
	
	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyVorticityHistoryToFile finished successfully.");
}

void ITIO::writeMyBeamNodeHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyBeamNodeHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".BNH".
	newfileNameWithPathString.append(".BNH");

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_MySurfaces()->size(), project->get_MySurfaces()->at(0)->get_MyBeam()->get_MyNodeHistory()->size(), project->get_MySurfaces()->at(0)->get_MyBeam()->get_MyNodeHistory()->at(0).size()); // n, i.
	fprintf(fp, "k,g,i,x,y,z,wx,wy,wz,m\n");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int g = 0; g < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->size(); g++)
		{
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(g).size(); i++)
			{
				ITBeamNode *currentNode = project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(g).at(i);

				fprintf(fp, "%i %i %i %f %f %f %f %f %f %f\n", k, g, i,
					currentNode->get_X(),
					currentNode->get_Y(),
					currentNode->get_Z(),
					currentNode->get_W()->get_X(),
					currentNode->get_W()->get_Y(),
					currentNode->get_W()->get_Z(),
					currentNode->get_Mass()
				);
			}
		}
	}
	
	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyBeamNodeHistoryToFile finished successfully.");
}

void ITIO::writeMyOutputDataHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyOutputDataHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".ODH".
	newfileNameWithPathString.append(".ODH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i\n", project->get_MySurfaces()->at(0)->get_MyForceHistory()->size()); // n.
	fprintf(fp, "k,f,X,Y,Z\n");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop over surfaces.
	{
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++) // Loop over frames. Note that the first frame has no wake panels and is empty.
		{
			fprintf(fp, "%i %i %f %f %f\n", k, n, project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_X(),
				project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Y(),
				project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Z());
		}
	}

	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyOutputDataHistoryToFile finished successfully.");
}

void ITIO::writeMyControlPointDeflectionHistoryToFile(char *fileNameWithPath)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeMyControlPointDeflectionHistoryToFile.");

	// Create a std:string from fileNameWithPath.
	std::string fileNameWithPathString(fileNameWithPath);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "fileNameWithPathString: %s", fileNameWithPathString);

	// Remove extension.
	std::string newfileNameWithPathString = fileNameWithPathString.substr(0, fileNameWithPathString.find("."));
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Append ".CPH".
	newfileNameWithPathString.append(".CPH");
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "newfileNameWithPathString: %s", newfileNameWithPathString);

	// Open the file.
	FILE *fp;
	fp = fopen(newfileNameWithPathString.c_str(), "w");

	// Write header.
	fprintf(fp, "%i, %i, %i\n", project->get_MySurfaces()->at(0)->get_MyControlPointDeflectionHistory()->size(), project->get_MySurfaces()->at(0)->get_MyControlPointDeflectionHistory()->at(0).size(), project->get_MySurfaces()->at(0)->get_MyControlPointDeflectionHistory()->at(0).at(0).size()); // n, i, j.
	fprintf(fp, "k,n,i,j,wx,wy,wz,wvx,wvy,wvz\n");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->size(); n++)
		{
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).size(); j++)
				{
					fprintf(fp, "%i %i %i %i %f %f %f %f %f %f\n", k, n, i, j, project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_X(),
						project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_Y(),
						project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_Z(),
						project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_VX(),
						project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_VY(),
						project->get_MySurfaces()->at(k)->get_MyControlPointDeflectionHistory()->at(n).at(i).at(j)->get_VZ());
				}
			}
		}
	}
	
	// Flush the file writing.
	fflush(fp);

	// Close the file.
	fclose(fp);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "writeMyControlPointDeflectionHistoryToFile finished successfully.");
}

void ITIO::writeCurrentSurfaceTranslationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeCurrentSurfaceTranslationTrajectory");

	// Get a pointer to the current surface curve array.
	ITSurface* currentSurface = project->get_MySurfaces()->at(k);

	// Set up a JSON array to contain the control points.
	rapidjson::Value controlPointsArray(rapidjson::kArrayType);

	// Count through the segments.
	for (int i = 0; i < currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		// Get the translation coordinates.
		float x = currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
		float y = currentSurface->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
		float z = currentSurface->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();

		// Create a new ITPointTrajectory point to do the serialization.
		ITPointTrajectory *tp = new ITPointTrajectory(x, y, z);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeCurrentSurfaceTranslationTrajectory. KeyFrame: %i", 
			currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());

		tp->set_KeyFrame(currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());

		// Write the point data to the controlPointsArray.
		tp->serializeMeAsJSONObject(k, i, 0, &controlPointsArray, d);

		// Delete temporary objects.
		delete tp;
	}

	// Serialize the final point in the trajectory.
	// Get the translation coordinates.
	float x = currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();
	float y = currentSurface->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();
	float z = currentSurface->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();

	// Create a new ITPointTrajectory point to do the serialization.
	ITPointTrajectory *tp = new ITPointTrajectory(x, y, z);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeCurrentSurfaceTranslationTrajectory. KeyFrame: %i", 
		currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->back()->get_StartKeyFrame());

	tp->set_KeyFrame(currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->back()->get_EndKeyFrame());

	// Write the point data to the controlPointsArray.
	tp->serializeMeAsJSONObject(k, currentSurface->get_MyTrajectoryCurves()->size() + 1, 0, &controlPointsArray, d);

	// Delete temporary objects.
	delete tp;

	// Add the control points array to the curveObject.
	trajectoryObject->AddMember("Control Points", controlPointsArray, d->GetAllocator());
}

void ITIO::writeCurrentSurfaceControlSurfaces(int k, rapidjson::Value *controlSurfacesArray, rapidjson::Document *d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeCurrentSurfaceControlSurfaces ================================== index: %i", k);

	// Get a pointer to the current surface.
	ITSurface* currentSurface = project->get_MySurfaces()->at(k);

	// Count through the segments.
	for (int i = 0; i < currentSurface->get_MyControlSurfaces()->size(); i++)
	{
		ITControlSurface *cs = currentSurface->get_MyControlSurfaces()->at(i);

		rapidjson::Value currentControlSurfaceObject(rapidjson::kObjectType);
		currentControlSurfaceObject.AddMember("Name", "A", d->GetAllocator());

		rapidjson::Value currentControlSurfacePanelArray(rapidjson::kArrayType);

		for (int j = 0; j < cs->get_MyITPanels()->size(); j++)
		{
			rapidjson::Value currentPanel(rapidjson::kObjectType);
			currentPanel.AddMember("row", cs->get_MyITPanels()->at(j)->get_I(), d->GetAllocator());
			currentPanel.AddMember("col", cs->get_MyITPanels()->at(j)->get_J(), d->GetAllocator());
			currentControlSurfacePanelArray.PushBack(currentPanel, d->GetAllocator());
		}
		
		// Push the currentControlSurfacePanelArray into the currentControlSurfaceObject.
		currentControlSurfaceObject.AddMember("Panels", currentControlSurfacePanelArray, d->GetAllocator());

		// Push the currentControlSurfaceObject into the controlSurfaceArray.
		controlSurfacesArray->PushBack(currentControlSurfaceObject, d->GetAllocator());

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeCurrentSurfaceControlSurfaces ================================== control surface index: %i", i);
	}
}

void ITIO::writeCurrentSurfaceRotationTrajectory(int k, rapidjson::Value *trajectoryObject, rapidjson::Document *d)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside ITIO::writeCurrentSurfaceRotationTrajectory");

	// Get a pointer to the current surface curve array.
	ITSurface* currentSurface = project->get_MySurfaces()->at(k);

	// Serialize centre of rotation point.
	rapidjson::Value pointObject(rapidjson::kObjectType);
	pointObject.AddMember("x", project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->get_X(), d->GetAllocator());
	pointObject.AddMember("y", project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->get_Y(), d->GetAllocator());
	pointObject.AddMember("z", project->get_MySurfaces()->at(k)->get_MyCentreOfRotationPoint()->get_Z(), d->GetAllocator());

	// Add it to the curveObject.
	// Add the properties to the point node.
	trajectoryObject->AddMember("Centre of Rotation", pointObject, d->GetAllocator());

	// Set up a JSON array to contain the control points.
	rapidjson::Value controlPointsArray(rapidjson::kArrayType);

	// Count through the segments.
	for (int i = 0; i < currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		// Get the translation coordinates.
		float x = currentSurface->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
		float y = currentSurface->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();
		float z = currentSurface->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(i)->get_P0_p()->get_X();

		// Create a new ITPointTrajectory point to do the serialization.
		ITPointTrajectory *tp = new ITPointTrajectory(x, y, z);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeCurrentSurfaceRotationTrajectory. KeyFrame: %i", currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());

		tp->set_KeyFrame(currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_StartKeyFrame());

		// Write the point data to the controlPointsArray.
		tp->serializeMeAsJSONObject(k, i, 0, &controlPointsArray, d);

		// Delete temporary objects.
		delete tp;
	}

	// Serialize the final point in the trajectory.
	// Get the translation coordinates.
	float x = currentSurface->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();
	float y = currentSurface->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();
	float z = currentSurface->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->back()->get_P1_p()->get_X();

	// Create a new ITPointTrajectory point to do the serialization.
	ITPointTrajectory *tp = new ITPointTrajectory(x, y, z);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside writeCurrentSurfaceRotationTrajectory. KeyFrame: %i", currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->back()->get_StartKeyFrame());

	tp->set_KeyFrame(currentSurface->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->back()->get_EndKeyFrame());

	// Write the point data to the controlPointsArray.
	tp->serializeMeAsJSONObject(k, currentSurface->get_MyTrajectoryCurves()->size() + 1, 0, &controlPointsArray, d);

	// Delete temporary objects.
	delete tp;

	// Add the control points array to the curveObject.
	trajectoryObject->AddMember("Control Points", controlPointsArray, d->GetAllocator());
}