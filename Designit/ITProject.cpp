#include "ITProject.h"
#include "ITSurface.h"
#include "global.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"

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

void ITProject::setPoint(const int surfaceID, const int i, const int j, const float posX, const float posY, const float posZ, bool update)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_POINT");

	if ((j >= getSurface(surfaceID)->sizeX()) || (j < 0)) throw std::exception("NO_POINT");

	getSurface(surfaceID)->getControlPoint(i, j)->set_X(posX);
	getSurface(surfaceID)->getControlPoint(i, j)->set_Y(posY);
	getSurface(surfaceID)->getControlPoint(i, j)->set_Z(posZ);

	UnsavedChanges = true;

	if (update)
	{
		getSurface(surfaceID)->manageComputationOfInterpolatedPoints();
		w->updateAllTabs();
	}
}

void ITProject::movePoint(const int surfaceID, const int i, const int j, const float dX, const float dY, const float dZ, bool update)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_POINT");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_POINT");

	float tmpX, tmpY, tmpZ;

	tmpX = getSurface(surfaceID)->getControlPoint(i, j)->get_X();
	tmpY = getSurface(surfaceID)->getControlPoint(i, j)->get_Y();
	tmpZ = getSurface(surfaceID)->getControlPoint(i, j)->get_Z();

	tmpX += dX;
	tmpY += dY;
	tmpZ += dZ;

	getSurface(surfaceID)->getControlPoint(i, j)->set_X(tmpX);
	getSurface(surfaceID)->getControlPoint(i, j)->set_Y(tmpY);
	getSurface(surfaceID)->getControlPoint(i, j)->set_Z(tmpZ);

	UnsavedChanges = true;

	if (update)
	{
		getSurface(surfaceID)->manageComputationOfInterpolatedPoints();
		w->updateAllTabs();
	}
}

void ITProject::rotatePoint(const int surfaceID, const int i, const int j, const float x, const float y, const float z, const float angle, PLANE p, bool update)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_POINT");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_POINT");

	getSurface(surfaceID)->getControlPoint(i, j)->rotateAround(ITPoint(x, y, z), angle, p);

	UnsavedChanges = true;

	if (update) 
	{ 
		getSurface(surfaceID)->manageComputationOfInterpolatedPoints();
		w->updateAllTabs(); 
	}
}

void ITProject::setColumn(const int surfaceID, const int j, const float posX, const float posY, const float posZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_COLUMN");

	//begin of column i = 0, j = j; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ, diffX, diffY, diffZ;

	ITPoint* orgin = getSurface(surfaceID)->getControlPoint(0, j);
	centerX = orgin->get_X();
	centerY = orgin->get_Y();
	centerZ = orgin->get_Z();

	setPoint(surfaceID, 0, j, posX, posY, posZ, false);

	for (int i = 1; i < getSurface(surfaceID)->sizeX(); i++)
	{
		ITPoint* tmp = getSurface(surfaceID)->getControlPoint(i, j);

		diffX = tmp->get_X() - centerX;
		diffY = tmp->get_Y() - centerY;
		diffZ = tmp->get_Z() - centerZ;

		setPoint(surfaceID, i, j, posX + diffX, posY + diffY, posZ + diffZ, false);
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::moveColumn(const int surfaceID, const int j, const float dX, const float dY, const float dZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_COLUMN");

	//begin of column i = 0, j = j; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ;

	ITPoint* orgin = getSurface(surfaceID)->getControlPoint(0, j);
	centerX = orgin->get_X();
	centerY = orgin->get_Y();
	centerZ = orgin->get_Z();

	setColumn(surfaceID, j, centerX + dX, centerY + dY, centerY + dY);

	UnsavedChanges = true;
}

void ITProject::rotateColumn(const int surfaceID, const int j, const float x, const float y, const float z, const float angle, PLANE p)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_COLUMN");

	for (int i = 0; i < getSurface(surfaceID)->sizeX(); i++)
	{
		rotatePoint(surfaceID, i, j, x, y, z, angle, p, false);
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::setRow(const int surfaceID, const int i, const float posX, const float posY, const float posZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_ROW");

	//begin of row i = i, j = 0; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ, diffX, diffY, diffZ;

	ITPoint* orgin = getSurface(surfaceID)->getControlPoint(i, 0);
	centerX = orgin->get_X();
	centerY = orgin->get_Y();
	centerZ = orgin->get_Z();

	setPoint(surfaceID, i, 0, posX, posY, posZ, false);

	for (int j = 1; j < getSurface(surfaceID)->sizeY(); j++)
	{
		ITPoint* tmp = getSurface(surfaceID)->getControlPoint(i, j);

		diffX = tmp->get_X() - centerX;
		diffY = tmp->get_Y() - centerY;
		diffZ = tmp->get_Z() - centerZ;

		setPoint(surfaceID, i, j, posX + diffX, posY + diffY, posZ + diffZ, false);
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::moveRow(const int surfaceID, const int i, const float dX, const float dY, const float dZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_ROW");

	//begin of column i = 0, j = j; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ;

	ITPoint* orgin = getSurface(surfaceID)->getControlPoint(i, 0);
	centerX = orgin->get_X();
	centerY = orgin->get_Y();
	centerZ = orgin->get_Z();

	setRow(surfaceID, i, centerX + dX, centerY + dY, centerY + dY);

	UnsavedChanges = true;
}

void ITProject::rotateRow(const int surfaceID, const int i, const float x, const float y, const float z, const float angle, PLANE p)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_ROW");

	for (int j = 0; j < getSurface(surfaceID)->sizeY(); j++)
	{
		rotatePoint(surfaceID, i, j, x, y, z, angle, p, false);
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::setSurface(const int surfaceID, const float posX, const float posY, const float posZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	//set position of center of surface; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ, diffX, diffY, diffZ;

	getSurface(surfaceID)->getCenter(centerX, centerY, centerZ);

	for (int i = 0; i < getSurface(surfaceID)->sizeX(); i++)
	{
		for (int j = 0; j < getSurface(surfaceID)->sizeY(); j++)
		{
			ITPoint* tmp = getSurface(surfaceID)->getControlPoint(i, j);

			diffX = tmp->get_X() - centerX;
			diffY = tmp->get_Y() - centerY;
			diffZ = tmp->get_Z() - centerZ;

			setPoint(surfaceID, i, j, posX + diffX, posY + diffY, posZ + diffZ, false);
		}
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::moveSurface(const int surfaceID, const float dX, const float dY, const float dZ)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	float centerX, centerY, centerZ;

	getSurface(surfaceID)->getCenter(centerX, centerY, centerZ);

	setSurface(surfaceID, centerX + dX, centerY + dY, centerY + dY);

	UnsavedChanges = true;
}

void ITProject::rotateSurface(const int surfaceID, const float x, const float y, const float z, const float angle, PLANE p)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	_MySurfaces->at(surfaceID)->rotateAround(x, y, z, angle, p);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::rotateSurfaceCentral(const int surfaceID, const float angle, PLANE p)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	float x, y, z;

	_MySurfaces->at(surfaceID)->getCenter(x, y, z);

	_MySurfaces->at(surfaceID)->rotateAround(x, y, z, angle, p);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::resizeSurface(const int surfaceID, const float factor)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	//set position of center of surface; compute vector of diffrance between orgin point and rest of column
	float centerX, centerY, centerZ, diffX, diffY, diffZ;

	getSurface(surfaceID)->getCenter(centerX, centerY, centerZ);

	for (int i = 0; i < getSurface(surfaceID)->sizeX(); i++)
	{
		for (int j = 0; j < getSurface(surfaceID)->sizeY(); j++)
		{
			ITPoint* tmp = getSurface(surfaceID)->getControlPoint(i, j);

			diffX = tmp->get_X() - centerX;
			diffY = tmp->get_Y() - centerY;
			diffZ = tmp->get_Z() - centerZ;

			setPoint(surfaceID, i, j, centerX + factor * diffX, centerY + factor * diffY, centerZ + factor * diffZ, false);
		}
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::deleteSurface(const int surfaceID)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	// We have found a control point, so delete the k-th surface.
	project->get_MySurfaces()->erase(project->get_MySurfaces()->begin() + surfaceID);
	project->get_MyBaseSurfaces()->erase(project->get_MyBaseSurfaces()->begin() + surfaceID);

	// Re-assign control point _K, _I and _J indices.
	// Re-assign the _K, _I and _J variables.
	for (int k = 0; k < _MySurfaces->size(); k++)
	{
		for (int i = 0; i < getSurface(k)->sizeX(); i++)
		{
			for (int j = 0; j < getSurface(k)->sizeY(); j++)
			{
				ITPoint* p = getSurface(k)->getControlPoint(i, j);;
				p->set_K(k);
				p->set_I(i);
				p->set_J(j);

				ITPoint* pb = getBaseSurface(k)->getControlPoint(i, j);;
				pb->set_K(k);
				pb->set_I(i);
				pb->set_J(j);
			}
		}
	}

	UnsavedChanges = true;

	w->updateAllTabs();
}

void ITProject::addSurface()
{
	ITSurface *s = new ITSurface();
	_MySurfaces->push_back(s);

	s->set_NoOfInterpolatedPointsU(20);
	s->set_NoOfInterpolatedPointsV(20);

	ITSurface *sb = new ITSurface();
	_MyBaseSurfaces->push_back(sb);

	sb->set_NoOfInterpolatedPointsU(20);
	sb->set_NoOfInterpolatedPointsV(20);

	int k = _MySurfaces->size() - 1;

	// Create control points for asymmetric NACA LPQXX aerofoil with 1m chord.
	// Ref: https://en.wikipedia.org/wiki/NACA_airfoil
	// Ref: http://www.pdas.com/naca456mean3.html

	float p = 0.20;
	float m = 0.29;
	float k1 = 6.643;
	float c = 1.0;

	for (int i = 0; i < 6; i++) // Step along span (5 metre span).
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> v_base_dummy;

		for (int j = 0; j < 7; j++) // j steps along chord.
		{
			float x = (float)j / 6.0; // Distance along chord.

									  // The camber line offset (z coordinate).
			float yc = 0.0;
			if ((0.0 <= x) && (x <= p))
			{
				yc = (k1 / 6.0) * (x*x*x - 3 * m*x*x + m*m*(3.0 - m)*x);
			}
			else
			{
				yc = (k1 / 6.0)*m*m*m*(1 - x);
			}

			// Span is parallel to y-axis.
			ITControlPoint *p = new ITControlPoint(-x, (float)i, yc);

			p->set_K(k);
			p->set_I(i);
			p->set_J(j);

			v_dummy.push_back(p);

			ITControlPoint *pb = new ITControlPoint(-x, (float)i, yc);

			pb->set_K(k);
			pb->set_I(i);
			pb->set_J(j);

			v_base_dummy.push_back(pb);

		}

		getSurface(k)->get_MyControlPoints()->push_back(v_dummy);
		getBaseSurface(k)->get_MyControlPoints()->push_back(v_base_dummy);

	}


	getSurface(k)->manageComputationOfInterpolatedPoints();

	createNewTrajectoryCurve(k);

	IsDataLoaded = true;
	UnsavedChanges = true;

	w->updateSpreadsheet();
	w->updateTrajectorySpreadsheet();
	w->updateAllTabs();
}

void ITProject::sheer(const int surfaceID, const int orginI, const int orginJ, const int refI, const int refJ, const float diff, PLANE plane)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((orginI >= getSurface(surfaceID)->sizeX()) || (orginI < 0)) throw std::exception("NO_POINT");

	if ((orginJ >= getSurface(surfaceID)->sizeY()) || (orginJ < 0)) throw std::exception("NO_POINT");

	if ((refI >= getSurface(surfaceID)->sizeX()) || (refI < 0)) throw std::exception("NO_POINT");

	if ((refJ >= getSurface(surfaceID)->sizeY()) || (refJ < 0)) throw std::exception("NO_POINT");

	float orginX, orginY, orginZ;
	float tmpX, tmpY, tmpZ;
	float refX, refY, refZ;

	orginX = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_X();
	orginY = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_Y();
	orginZ = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_Z();

	refX = getSurface(surfaceID)->getControlPoint(refI, refJ)->get_X() - orginX;
	refY = getSurface(surfaceID)->getControlPoint(refI, refJ)->get_Y() - orginY;
	refZ = getSurface(surfaceID)->getControlPoint(refI, refJ)->get_Z() - orginZ;

	for (int i = 0; i < getSurface(surfaceID)->sizeX(); i++)
	{
		for (int j = 0; j < getSurface(surfaceID)->sizeY(); j++)
		{
			ITPoint* p = getSurface(surfaceID)->getControlPoint(i, j);

			// Translate surface back to origin.
			tmpX = p->get_X() - orginX;
			tmpY = p->get_Y() - orginY;
			tmpZ = p->get_Z() - orginZ;

			// Shear to first axis of plane.
			switch (plane)
			{
			case XY:
				tmpX += tmpY * diff / refY;
				break;
			case XZ:
				tmpX += tmpZ * diff / refZ;
				break;
			case YZ:
				tmpY += tmpZ * diff / refZ;
				break;
			}

			// Translate surface back again.
			p->set_X(tmpX + orginX);
			p->set_Y(tmpY + orginY);
			p->set_Z(tmpZ + orginZ);
		}
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::sheerD(const int surfaceID, const int orginI, const int orginJ, const float diff, PLANE p)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((orginI >= getSurface(surfaceID)->sizeX()) || (orginI < 0)) throw std::exception("NO_POINT");

	if ((orginJ >= getSurface(surfaceID)->sizeY()) || (orginJ < 0)) throw std::exception("NO_POINT");

	sheer(surfaceID, orginI, orginJ, getSurface(surfaceID)->sizeX() - 1, orginJ, diff, p);

	UnsavedChanges = true;
}

void ITProject::flipSurface(const int surfaceID, const int x, const int y, const int z, PLANE plane)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	float tmpX, tmpY, tmpZ;

	for (int i = 0; i < getSurface(surfaceID)->sizeX(); i++)
	{
		for (int j = 0; j < getSurface(surfaceID)->sizeY(); j++)
		{
			ITPoint* p = getSurface(surfaceID)->getControlPoint(i, j);

			tmpX = p->get_X();
			tmpY = p->get_Y();
			tmpZ = p->get_Z();

			switch (plane)
			{
			case XY:
				tmpZ -= z;
				tmpZ = z - tmpZ;
				break;
			case XZ:
				tmpY -= y;
				tmpY = y - tmpY;
				break;
			case YZ:
				tmpX -= x;
				tmpX = x - tmpX;
				break;
			}

			p->set_X(tmpX);
			p->set_Y(tmpY);
			p->set_Z(tmpZ);
		}
	}

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::flipSurfacePoint(const int surfaceID, const int orginI, const int orginJ, PLANE plane)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((orginI >= getSurface(surfaceID)->sizeX()) || (orginI < 0)) throw std::exception("NO_POINT");

	if ((orginJ >= getSurface(surfaceID)->sizeY()) || (orginJ < 0)) throw std::exception("NO_POINT");

	float x, y, z;

	x = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_X();
	y = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_Y();
	z = getSurface(surfaceID)->getControlPoint(orginI, orginJ)->get_Z();

	flipSurface(surfaceID, x, y, z, plane);

	UnsavedChanges = true;
}

void ITProject::flipSurfaceCentral(const int surfaceID, PLANE plane)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	float x, y, z;

	getSurface(surfaceID)->getCenter(x, y, z);

	flipSurface(surfaceID, x, y, z, plane);

	UnsavedChanges = true;
}

void ITProject::copySurface(const int surfaceID, const int x, const int y, const int z)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	int k = project->get_MySurfaces()->size();

	get_MySurfaces()->push_back( getSurface(surfaceID)->getCopyTranslated(k, x, y, z) );
	get_MyBaseSurfaces()->push_back( getBaseSurface(surfaceID)->getCopyTranslated(k, x, y, z) );
	
	IsDataLoaded = true;
	UnsavedChanges = true;

	getSurface(k)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::insertRow(const int surfaceID, const int i)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX() - 1) || (i < 0)) throw std::exception("NO_ROW");

	getSurface(surfaceID)->addRow(i);
	getBaseSurface(surfaceID)->addRow(i);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::duplicateRow(const int surfaceID, const int i)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX() - 1) || (i < 0)) throw std::exception("NO_ROW");

	getSurface(surfaceID)->duplicateRow(i);
	getBaseSurface(surfaceID)->duplicateRow(i);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::deleteRow(const int surfaceID, const int i)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((i >= getSurface(surfaceID)->sizeX()) || (i < 0)) throw std::exception("NO_ROW");

	getSurface(surfaceID)->deleteRow(i);
	getBaseSurface(surfaceID)->deleteRow(i);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::insertColumn(const int surfaceID, const int j)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY() - 1) || (j < 0)) throw std::exception("NO_COLUMN");

	getSurface(surfaceID)->addColumn(j);
	getBaseSurface(surfaceID)->addColumn(j);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::duplicateColumn(const int surfaceID, const int j)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY() - 1) || (j < 0)) throw std::exception("NO_COLUMN");

	getSurface(surfaceID)->duplicateColumn(j);
	getBaseSurface(surfaceID)->duplicateColumn(j);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::deleteColumn(const int surfaceID, const int j)
{
	if ((surfaceID >= _MySurfaces->size()) || (surfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((j >= getSurface(surfaceID)->sizeY()) || (j < 0)) throw std::exception("NO_COLUMN");

	getSurface(surfaceID)->deleteColumn(j);
	getBaseSurface(surfaceID)->deleteColumn(j);

	UnsavedChanges = true;

	getSurface(surfaceID)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();
}

void ITProject::matePoints(const int baseSurfaceID, const int baseI, const int baseJ, const int targetSurfaceID, const int targetI, const int targetJ)
{
	if ((baseSurfaceID >= _MySurfaces->size()) || (baseSurfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((baseI >= getSurface(baseSurfaceID)->sizeX()) || (baseI < 0)) throw std::exception("NO_POINT");

	if ((baseJ >= getSurface(baseSurfaceID)->sizeY()) || (baseJ < 0)) throw std::exception("NO_POINT");

	if ((targetSurfaceID >= _MySurfaces->size()) || (targetSurfaceID < 0)) throw std::exception("NO_SURFACE");

	if ((targetI >= getSurface(targetSurfaceID)->sizeX()) || (targetI < 0)) throw std::exception("NO_POINT");

	if ((targetJ >= getSurface(targetSurfaceID)->sizeY()) || (targetJ < 0)) throw std::exception("NO_POINT");

	float x = getSurface(baseSurfaceID)->getControlPoint(baseI, baseJ)->get_X();
	float y = getSurface(baseSurfaceID)->getControlPoint(baseI, baseJ)->get_Y();
	float z = getSurface(baseSurfaceID)->getControlPoint(baseI, baseJ)->get_Z();

	setPoint(targetSurfaceID, targetI, targetJ, x, y, z, false);

	UnsavedChanges = true;

	for (int i = 0; i < _MySurfaces->size(); i++)
	{
		getSurface(i)->manageComputationOfInterpolatedPoints();
	}

	w->updateAllTabs();
}

Point3 ITProject::getPointData(const int surfaceID, const int i, const int j)
{
	Point3 tmp;

	tmp.x = getSurface(surfaceID)->getControlPoint(i, j)->get_X();
	tmp.y = getSurface(surfaceID)->getControlPoint(i, j)->get_Y();
	tmp.z = getSurface(surfaceID)->getControlPoint(i, j)->get_Z();

	return tmp;
}

void ITProject::synchronizeSurfaceVectorsFromControl()
{
	for (int k = 0; k < _MySurfaces->size(); k++)
	{
		for (int i = 0; i < getSurface(k)->sizeX(); i++)
		{
			for (int j = 0; j < getSurface(k)->sizeY(); j++)
			{
				ITPoint* p = getSurface(k)->getControlPoint(i, j);
				ITPoint* pBase = getBaseSurface(k)->getControlPoint(i, j);

				pBase->set_X(p->get_X());
				pBase->set_Y(p->get_Y());
				pBase->set_Z(p->get_Z());
			}
		}
	}
}

void ITProject::createNewTrajectoryCurve(const int k)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Creating new trajectory curve");

	// Instanciate the ITTrajectoryCurve curves.
	ITTrajectoryCurve* cx = new ITTrajectoryCurve();
	ITTrajectoryCurve* cy = new ITTrajectoryCurve();
	ITTrajectoryCurve* cz = new ITTrajectoryCurve();

	// Note that the loop omits the last point (if there are N points, then there are N-1 segments).
	// Note that the ordinal of each segment end point is stored in the x instance variable of the ITPoint object.
	for (int i = 0; i < 4; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "translation segment %i", i);

		cx->addSegment(i*5.0, 0.0, 0.0, 20 * i, 0.0, 0.0, (i + 1)*5.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
		cy->addSegment(0.0, 0.0, 0.0, 20 * i, 0.0, 0.0, 0.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
		cz->addSegment(0.0, 0.0, 0.0, 20 * i, 0.0, 0.0, 0.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Finished translation segments");

	// Compute the m0 and m1 tangent vectors for each of the segments in the translation curves.
	cx->computeMySegmentEndTangentVectors();
	cy->computeMySegmentEndTangentVectors();
	cz->computeMySegmentEndTangentVectors();

	// Push the translation curves into the surface.
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cx);
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cy);
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cz);

	// Instanciate the ITTrajectoryCurve curves.
	ITTrajectoryCurve* cr = new ITTrajectoryCurve();
	ITTrajectoryCurve* cp = new ITTrajectoryCurve();
	ITTrajectoryCurve* cyaw = new ITTrajectoryCurve();

	// Note that the loop omits the last point (if there are N points, then there are N-1 segments).
	for (int i = 0; i < 4; i++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rotation segment %i", i);

		cr->addSegment(0.0, 0.0, 0.0, 20 * i, 0.0, 0.0, 0.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
		cp->addSegment(0.0, 0.0, 0.0, 20 * i, 0.0, 0.0, 0.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
		cyaw->addSegment(0.0, 0.0, 0.0, 20 * i, 0.0, 0.0, 0.0, 0.0, 0.0, 20 * (i + 1), 0.0, 0.0);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Finished rotation segments");

	// Compute the m0 and m1 tangent vectors for each of the segments in the translation curves.
	cr->computeMySegmentEndTangentVectors();
	cp->computeMySegmentEndTangentVectors();
	cyaw->computeMySegmentEndTangentVectors();

	// Push the translation curves into the surface.
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cr);
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cp);
	getSurface(k)->get_MyTrajectoryCurves()->push_back(cyaw);

	// Centre of rotation point.
	getSurface(k)->get_MyCentreOfRotationPoint()->set_X(0.0);
	getSurface(k)->get_MyCentreOfRotationPoint()->set_Y(0.0);
	getSurface(k)->get_MyCentreOfRotationPoint()->set_Z(0.0);

	// Finally update the project maxKeyFrame instance variable.
	// Set it to the end key frame of the last segment of the z trajectory of the last surface.
	_MaxKeyFrame = _MySurfaces->back()->get_MyTrajectoryCurves()->back()->get_MyTrajectoryCurveSegments()->back()->get_EndKeyFrame();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Max key frame: %i", project->get_MaxKeyFrame());
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

ITSurface* ITProject::getSurface( const int k ) 
{ 
	if ((k >= _MySurfaces->size()) || (k < 0)) throw std::exception("NO_SURFACE");

	return _MySurfaces->at(k); 
};

ITSurface* ITProject::getBaseSurface(const int k)
{
	if ((k >= _MyBaseSurfaces->size()) || (k < 0)) throw std::exception("NO_SURFACE");

	return _MyBaseSurfaces->at(k);
};