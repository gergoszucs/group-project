#include "View2d.h"

#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.
#include <typeinfo> // For typeid
#include <string>

#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPhysics.h"
#include "UtililityFunctions.h"
#include <math.h>

View2d::View2d(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	switch (_plane)
	{
	case XY:
		xRot = 0;
		yRot = 0;
		zRot = 0;
		break;
	case XZ:
		xRot = 270;
		yRot = 0;
		zRot = 0;
		break;
	case YZ:
		xRot = 270;
		yRot = 270;
		zRot = 0;
		break;
	}

	setFocusPolicy(Qt::StrongFocus);

	// Instanciate the control point.
	ITControlPoint *point = new ITControlPoint(0.0, 0.0, 0.0);
	set_ScratchControlPoint(point);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);

	setMouseTracking(true);
}

View2d::~View2d(void)
{
	delete _ScratchControlPoint;
}

// Draw functions.
void View2d::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void View2d::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-eyeX*eyeZoom, -eyeY*eyeZoom, 0.0);
	glScalef(eyeZoom, eyeZoom, eyeZoom);

	// 20160510: Ordering of these rotation has been changed to give more intuitive rotation in response to mouse movement.

	switch (_plane)
	{
	case XY: // Z vertical.

		glRotatef((float)xRot, 1.0, 0.0, 0.0);
		glRotatef(0.0, 0.0, 1.0, 0.0);
		glRotatef((float)yRot, 0.0, 0.0, 1.0);

		break;
	case XZ: // Y vertical.

		glRotatef((float)yRot, 0.0, 1.0, 0.0);
		glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
		glRotatef(0.0, 0.0, 0.0, 1.0);

		break;
	case YZ: // X vertical.

		glRotatef((float)yRot, 0.0, 1.0, 0.0);
		glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
		glRotatef(0.0, 0.0, 0.0, 1.0);

		break;
	}

	draw();
}

void View2d::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	float aspect = (float)width / (float)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, +50.0, -50.0 / aspect, +50.0 / aspect, -50000, 50000.0);
	glMatrixMode(GL_MODELVIEW);

	draw();
}

void View2d::draw()
{
	if (IsDataLoaded)
	{
		drawMyAxes();

		if (drawControlPoints)
		{
			drawMyControlPointsNet();
		}

		if (drawInterpolatedPoints)
		{
			drawMyInterpolatedPointsNet();
		}

		if (drawGrids)
		{
			drawMyGrids();
		}

		if (drawDial)
		{
			drawAngleDial(15);
		}

		if (drawVector)
		{
			drawDragVector();
		}

		drawMyFocusControlPoints();
		drawMyFocusControlPoints_light();
		drawMyScratchControlPoint();

		QString mode;

		switch (_selectMode)
		{
		case POINT_M:
			mode = "SELECT MODE: POINT";
			break;
		case ROW_M:
			mode = "SELECT MODE: ROW";
			break;
		case COLUMN_M:
			mode = "SELECT MODE: COLUMN";
			break;
		case SURFACE_M:
			mode = "SELECT MODE: SURFACE";
			break;
		}

		glColor3f(0.0f, 0.0f, 0.0f);
		renderText(10, 10, mode);
	}
}

void View2d::drawMyScratchControlPoint()
{

	if (scrachPointReady)
	{
		float x = get_ScratchControlPoint()->get_X();
		float y = get_ScratchControlPoint()->get_Y();
		float z = get_ScratchControlPoint()->get_Z();

		switch (_plane)
		{
		case XY: 
			z = 0.0;
			break;
		case XZ:
			y = 0.0;
			break;
		case YZ:
			x = 0.0;
			break;
		}

		glTranslatef(x, y, z);
		drawSphere(1.5, 15, 15, 0.0, 1.0, 0.0);
		glTranslatef(-x, -y, -z);
	}
}

void View2d::drawMyGrids()
{
	float x1, y1, z1, x2, y2, z2;

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.5f, 0.5f, 0.5f); // grey

	// Draw horizontal lines.
	for (int i = -10; i < 11; i++)
	{
		x1 = (float)i*10.0;
		y1 = -100.0;
		z1 = 0.0;
		x2 = (float)i*10.0;
		y2 = 100.0;
		z2 = 0.0;

		glBegin(GL_LINE_STRIP);

		switch (_plane)
		{
		case XY:
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			break;
		case XZ:
			glVertex3f(x1, z1, y1);
			glVertex3f(x2, z2, y2);
			break;
		case YZ:
			glVertex3f(z1, y1, x1);
			glVertex3f(z2, y2, x2);
			break;
		}

		glEnd();
	}

	// Draw vertical lines.
	for (int j = -10; j < 11; j++)
	{
		x1 = -100.0;
		y1 = (float)j*10.0;
		z1 = 0.0;
		x2 = 100.0;
		y2 = (float)j*10.0;
		z2 = 0.0;

		glBegin(GL_LINE_STRIP);

		switch (_plane)
		{
		case XY:
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			break;
		case XZ:
			glVertex3f(x1, z1, y1);
			glVertex3f(x2, z2, y2);
			break;
		case YZ:
			glVertex3f(z1, y1, x1);
			glVertex3f(z2, y2, x2);
			break;
		}

		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);

}

void View2d::drawMyAxes()
{
	// Draw X axis.
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(30.0f, 0.0f, 0.0f);
	glEnd();

	// Draw Y axis.
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 30.0f, 0.0f);
	glEnd();

	// Draw Z axis.
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 30.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(30.0, 0.0, 0.0, QString("x-axis"));
	renderText(0.0, 30.0, 0.0, QString("y-axis"));
	renderText(0.0, 0.0, 30.0, QString("z-axis"));
}

void View2d::drawMyInterpolatedPointsNet()
{
	// Set the colour to blue.
	glColor3f(0.0f, 0.0f, 1.0f);

	// Draw each surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_Z());
			}
			glEnd();
		}

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(0).size(); i++) // i counts columns.
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); j++) // j counts rows.
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_Z());
			}
			glEnd();
		}
	}
}

void View2d::drawMyFocusControlPoints()
{
	for (auto point : focusedPoints)
	{
		float x, y, z;

		x = point->get_X();
		y = point->get_Y();
		z = point->get_Z();

		glTranslatef(x, y, z);

		drawSphere(1.5, 15, 15, 1.0, 0.0, 0.0);

		glTranslatef(-x, -y, -z);
	}
}

void View2d::drawMyFocusControlPoints_light()
{
	for (auto point : focusedPoints_light)
	{
		float x, y, z;

		x = point->get_X();
		y = point->get_Y();
		z = point->get_Z();

		glTranslatef(x, y, z);

		drawSphere(1.5, 15, 15, 0.7, 0.0, 0.0);

		glTranslatef(-x, -y, -z);
	}
}

void View2d::drawMyControlPointsNet()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->getSurface(k)->sizeX(); i++)
		{
			glBegin(GL_LINE_STRIP);

			for (int j = 0; j < project->getSurface(k)->sizeY(); j++)
			{
				glVertex3f(project->getControlPoint(k, i, j)->get_X(),
					project->getControlPoint(k, i, j)->get_Y(),
					project->getControlPoint(k, i, j)->get_Z());
			}

			glEnd();
		}

		for (int j = 0; j < project->getSurface(k)->sizeY(); j++)
		{
			glBegin(GL_LINE_STRIP);

			for (int i = 0; i < project->getSurface(k)->sizeX(); i++)
			{
				glVertex3f(project->getControlPoint(k, i, j)->get_X(),
					project->getControlPoint(k, i, j)->get_Y(),
					project->getControlPoint(k, i, j)->get_Z());
			}

			glEnd();
		}
	}

	// Now draw spheres.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->getSurface(k)->sizeX(); i++)
		{
			for (int j = 0; j < project->getSurface(k)->sizeY(); j++)
			{
				ITControlPoint *point = project->getControlPoint(k, i, j);

				float x, y, z;

				x = point->get_X();
				y = point->get_Y();
				z = point->get_Z();

				glTranslatef(x, y, z);

				drawSphere(1.0, 15, 15, 0.0, 0.0, 0.0);

				glTranslatef(-x, -y, -z);
			}
		}
	}
}

void View2d::drawSphere(double r, int lats, int longs, float R, float GG, float B)
{
	// Ref: http://stackoverflow.com/questions/22058111/opengl-draw-sphere-using-glvertex3f
	glColor3f(R, GG, B);

	int i, j;
	for (i = 0; i <= lats; i++)
	{
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= longs; j++)
		{
			double lng = 2 * M_PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0 * r, y * zr0 * r, z0 * r);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1 * r, y * zr1 * r, z1 * r);
		}
		glEnd();
	}
}

void View2d::drawAngleDial(const float radius)
{
	if ((focusedPoints.size() != 0) && (scrachPointReady) && (w->getDial()))
	{
		Point3 center = _ScratchControlPoint->getCoordinates();

		float tmpAngle = angleRotated;

		while (tmpAngle > M_PI)
		{
			tmpAngle -= 2 * M_PI;
		}

		while (tmpAngle < -M_PI)
		{
			tmpAngle += 2 * M_PI;
		}

		if (!dialInitialized)
		{
			Point3 begin = focusedPoints[0]->getCoordinates();

			switch (_plane)
			{
			case XY:
				beginAngle = atan2f(begin.y - center.y, begin.x - center.x);
				break;
			case XZ:
				beginAngle = atan2f(begin.z - center.z, begin.x - center.x);
				break;
			case YZ:
				beginAngle = atan2f(begin.z - center.z, begin.y - center.y);
				break;
			}

			dialInitialized = true;
		}

		Point3 start, finish;

		start.toCartesian(1.1 * radius, beginAngle, _plane);
		start = start + center;
		finish.toCartesian(1.1 * radius, beginAngle + tmpAngle, _plane);
		finish = finish + center;

		// Draw main lines of dial
		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(2.5);
		glBegin(GL_LINE_STRIP);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(center.x, center.y, center.z);
		//glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(finish.x, finish.y, finish.z);
		glEnd();
		

		float angleStep = 0.001;
		Point3 drawPoint;

		drawPoint.toCartesian(radius, beginAngle, _plane);
		drawPoint = drawPoint + center;

		// Draw angle line
		glBegin(GL_LINE_STRIP);
		glVertex3f(drawPoint.x, drawPoint.y, drawPoint.z);

		if (tmpAngle >= 0)
		{
			for (float a = beginAngle; a < beginAngle + tmpAngle; a += angleStep)
			{
				drawPoint.toCartesian(radius, a, _plane);
				drawPoint = drawPoint + center;

				glVertex3f(drawPoint.x, drawPoint.y, drawPoint.z);
			}
		}
		else
		{
			for (float a = beginAngle; a > beginAngle + tmpAngle; a -= angleStep)
			{
				drawPoint.toCartesian(radius, a, _plane);
				drawPoint = drawPoint + center;

				glVertex3f(drawPoint.x, drawPoint.y, drawPoint.z);
			}
		}

		drawPoint.toCartesian(radius, beginAngle + tmpAngle, _plane);
		drawPoint = drawPoint + center;

		glVertex3f(drawPoint.x, drawPoint.y, drawPoint.z);
		glEnd();
		glLineWidth(1.0);

		// Draw angle rotated.
		float labelAngle = beginAngle + tmpAngle / 2;

		Point3 labelPoint;
		labelPoint.toCartesian(radius + 5, labelAngle, _plane);
		labelPoint = labelPoint + center;

		renderText(labelPoint.x, labelPoint.y, labelPoint.z, QString::number(RAD_TO_DEG(tmpAngle), 'g', 5));
	}
}

void View2d::drawDragVector()
{
	if ((focusedPoints.size() != 0) && (w->getVector()))
	{
		Point3 pointNow = focusedPoints[0]->getCoordinates();

		Point3 pointBegin(pointNow.x - draggedX, pointNow.y - draggedY, pointNow.z - draggedZ);

		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(2.5);
		glBegin(GL_LINE_STRIP);
		glVertex3f(pointBegin.x, pointBegin.y, pointBegin.z);
		glVertex3f(pointNow.x, pointNow.y, pointNow.z);
		glEnd();

		// Draw arrow head

		float length = 5;
		float angle = DEG_TO_RAD(15.0);
		float mainAngle;

		switch (_plane)
		{
		case XY:
			mainAngle = atan2(-draggedY, -draggedX);
			break;
		case XZ:
			mainAngle = atan2(-draggedZ, -draggedX);
			break;
		case YZ:
			mainAngle = atan2(-draggedZ, -draggedY);
			break;
		}
		
		Point3 arrow1, arrow2;
		arrow1.toCartesian(length, mainAngle + angle, _plane);
		arrow2.toCartesian(length, mainAngle - angle, _plane);

		arrow1 = arrow1 + pointNow;
		arrow2 = arrow2 + pointNow;

		glBegin(GL_LINE_STRIP);
		glVertex3f(arrow1.x, arrow1.y, arrow1.z);
		glVertex3f(pointNow.x, pointNow.y, pointNow.z);
		glVertex3f(arrow2.x, arrow2.y, arrow2.z);
		glEnd();
		glLineWidth(1.0);

		Point3 labelPoint;
		labelPoint.x = pointBegin.x + draggedX / 2;
		labelPoint.y = pointBegin.y + draggedY / 2;
		labelPoint.z = pointBegin.z + draggedZ / 2;

		QString text;

		switch (_plane)
		{
		case XY:
			labelPoint.x += 5;
			labelPoint.y += 5;
			text = tr("(") + QString::number(draggedX, 'g', 5) + tr(", ") + QString::number(draggedY, 'g', 5) + tr(")");
			break;
		case XZ:
			labelPoint.x += 5;
			labelPoint.z += 5;
			text = tr("(") + QString::number(draggedX, 'g', 5) + tr(", ") + QString::number(draggedZ, 'g', 5) + tr(")");
			break;
		case YZ:
			labelPoint.y += 5;
			labelPoint.z += 5;
			text = tr("(") + QString::number(draggedY, 'g', 5) + tr(", ") + QString::number(draggedZ, 'g', 5) + tr(")");
			break;
		}

		renderText(labelPoint.x, labelPoint.y, labelPoint.z, text);

		glTranslatef(pointBegin.x, pointBegin.y, pointBegin.z);
		drawSphere(1.0, 15, 15, 0.0, 1.0, 0.0);
		glTranslatef(-pointBegin.x, -pointBegin.y, -pointBegin.z);
	}
}

// Event handling.
void View2d::keyPressEvent(QKeyEvent * event)
{
	float factor;

	// Check for fine movement.
	if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)
	{
		// Fine movement.
		factor = 0.05;
	}
	else
	{
		// Coarse movement.
		factor = 1.0;
	}

	if (event->key() == Qt::Key_Up)
	{
		eyeY += factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		eyeY -= factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		eyeX -= factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		eyeX += factor;
	}
	else if (event->key() == Qt::Key_Alt)
	{
		switch (_selectMode)
		{
		case POINT_M:
			_selectMode = ROW_M;
			break;
		case ROW_M:
			_selectMode = COLUMN_M;
			break;
		case COLUMN_M:
			_selectMode = SURFACE_M;
			break;
		case SURFACE_M:
			_selectMode = POINT_M;
			break;
		}

		w->updateAllTabs();
	}

	// Force redraw.
	update();
}

void View2d::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		float posX, posY;

		getAxesPos(posX, posY, event->x(), event->y());

		// Find the vertex that is closest.
		int i, j, k;
		findControlPointIndicesNearMouse(posX, posY, &k, &i, &j);

		// Make sure lastPos is reset.
		lastPos = event->pos();

		int ret, surfaceID;


		if ((MY_EDIT_MODE == ROTATE) && (!primedForClick))
		{
			_ScratchControlPoint->set_K(0);
			_ScratchControlPoint->set_I(0);
			_ScratchControlPoint->set_J(0);

			switch (_plane)
			{
			case XY:
				_ScratchControlPoint->set_X(posX);
				_ScratchControlPoint->set_Y(posY);
				_ScratchControlPoint->set_Z(0.0);
				break;
			case XZ:
				_ScratchControlPoint->set_X(posX);
				_ScratchControlPoint->set_Y(0.0);
				_ScratchControlPoint->set_Z(posY);
				break;
			case YZ:
				_ScratchControlPoint->set_X(0.0);
				_ScratchControlPoint->set_Y(posX);
				_ScratchControlPoint->set_Z(posY);
				break;
			}

			drawDial = true;
			scrachPointReady = true;
			primedForClick = true;
		}
		else if ((MY_EDIT_MODE == SHEAR) && (!primedForClick) && ((k > -1) && (i > -1) && (j > -1)))
		{
			ITControlPoint* point = project->getControlPoint(k, i, j);

			_ScratchControlPoint->set_K(k);
			_ScratchControlPoint->set_I(i);
			_ScratchControlPoint->set_J(j);

			_ScratchControlPoint->set_X(point->get_X());
			_ScratchControlPoint->set_Y(point->get_Y());
			_ScratchControlPoint->set_Z(point->get_Z());

			scrachPointReady = true;
			primedForClick = true;
		}
		else if ((MY_EDIT_MODE == SHEAR) && (primedForClick) && ((k > -1) && (i > -1) && (j > -1)))
		{
			if (k == _ScratchControlPoint->get_K())
			{
				bool flag = false;

				switch (_plane)
				{
				case XY:
					flag = (i != _ScratchControlPoint->get_I());
					break;
				case XZ:
					flag = (i != _ScratchControlPoint->get_I());
					break;
				case YZ:
					flag = (j != _ScratchControlPoint->get_J());
					break;
				}

				if (flag)
				{
					memoryK = k;
					memoryI = i;
					memoryJ = j;

					focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
					addFocusPoint(project->getControlPoint(k, i, j));

					readyToShear = true;
				}
			}
		}
		else if ((k > -1) && (i > -1) && (j > -1))
		{
			QMessageBox msgBox;
			QStringList command, revCommand;
			ITSurface * tmp;
			ITSurface * tmpBase;
			std::vector<ITControlPoint*> tmpVec;
			Point3 tmpPoint;
			bool status;
			float data;

			//handle click-based modes
			switch (MY_EDIT_MODE)
			{
			case FLIP:
				msgBox.setWindowTitle(tr("Flipling surface"));
				msgBox.setText(tr("Choose plane"));
				msgBox.addButton(tr("XY"), QMessageBox::AcceptRole);
				msgBox.addButton(tr("YZ"), QMessageBox::AcceptRole);
				msgBox.addButton(tr("XZ"), QMessageBox::AcceptRole);

				ret = msgBox.exec();

				command.push_back("flipSurfaceCentral");
				command.push_back(QString::number(k));

				switch (ret)
				{
				case 0:
					command.push_back("XY");
					break;
				case 1:
					command.push_back("YZ");
					break;
				case 2:
					command.push_back("XZ");
					break;
				}

				w->executeCommand("FLIP", command, true);

				finishEdit();
				break;
			case COPY_SURFACE:
				command.push_back("copySurface");
				command.push_back(QString::number(k));
				command.push_back("0");
				command.push_back("0");
				command.push_back("0");

				w->executeCommand("COPY SURFACE", command, true);

				finishEdit();
				break;
			case DELETE_SURFACE:
				command.push_back("deleteSurface");
				command.push_back(QString::number(k));

				w->executeCommand("DELETE SURFACE", command, true);

				finishEdit();
				break;
			case INSERT_ROW:
				if (i == project->getSurface(k)->sizeX() - 1)
				{
					QMessageBox::StandardButton reply;
					reply = QMessageBox::warning(this, "Insert Row", "You cannot insert a row after the last row");
				}
				else
				{
					project->insertRow(k, i);

					command.push_back("insertRow");
					command.push_back(QString::number(k));
					command.push_back(QString::number(i));

					w->executeCommand("INSERT ROW", command, true);
				}

				finishEdit();
				break;
			case DELETE_ROW:
				if (project->getSurface(k)->sizeX() == 1)
				{
					command.push_back("deleteSurface");
					command.push_back(QString::number(k));
				}
				else
				{
					command.push_back("deleteRow");
					command.push_back(QString::number(k));
					command.push_back(QString::number(i));
				}

				w->executeCommand("DELETE ROW", command, true);

				finishEdit();
				break;
			case DUPLICATE_ROW:
				command.push_back("duplicateRow");
				command.push_back(QString::number(k));
				command.push_back(QString::number(i));

				w->executeCommand("DUPLICATE ROW", command, true);

				finishEdit();
				break;
			case INSERT_COL:
				if (j == project->getSurface(k)->sizeY() - 1)
				{
					// The user is trying to insert a row after the last row which is not possible, so display a warning dialogue.
					QMessageBox::StandardButton reply;
					reply = QMessageBox::warning(this, "Insert Column", "You cannot insert a column after the last column");
				}
				else
				{
					command.push_back("insertColumn");
					command.push_back(QString::number(k));
					command.push_back(QString::number(j));

					w->executeCommand("INSERT COLUMN", command, true);
				}

				finishEdit();
				break;
			case DELETE_COL:
				if (project->getSurface(k)->sizeY() == 1)
				{
					command.push_back("deleteSurface");
					command.push_back(QString::number(k));
				}
				else
				{
					command.push_back("deleteColumn");
					command.push_back(QString::number(k));
					command.push_back(QString::number(j));
				}

				w->executeCommand("DELETE COLUMN", command, true);

				finishEdit();
				break;
			case DUPLICATE_COL:
				command.push_back("duplicateColumn");
				command.push_back(QString::number(k));
				command.push_back(QString::number(j));

				w->executeCommand("DUPLICATE COLUMN", command, true);

				finishEdit();
				break;
			case MATE_POINTS:
				if (!primedForClick)
				{
					_ScratchControlPoint->set_K(k);
					_ScratchControlPoint->set_I(i);
					_ScratchControlPoint->set_J(j);

					tmpPoint = project->getControlPoint(k, i, j)->getCoordinates();

					_ScratchControlPoint->set_X(tmpPoint.x);
					_ScratchControlPoint->set_Y(tmpPoint.y);
					_ScratchControlPoint->set_Z(tmpPoint.z);

					primedForClick = true;
					scrachPointReady = true;
				}
				else
				{
					command.push_back("matePoints");
					command.push_back(QString::number(_ScratchControlPoint->get_K()));
					command.push_back(QString::number(_ScratchControlPoint->get_I()));
					command.push_back(QString::number(_ScratchControlPoint->get_J()));
					command.push_back(QString::number(k));
					command.push_back(QString::number(i));
					command.push_back(QString::number(j));

					w->executeCommand("MATE POINTS", command, true);

					finishEdit();
				}
				break;
			case COPY_SURFACE_MIRROR:
				msgBox.setWindowTitle(tr("Coping surface with flipping"));
				msgBox.setText(tr("Choose plane"));
				msgBox.addButton(tr("XY"), QMessageBox::AcceptRole);
				msgBox.addButton(tr("YZ"), QMessageBox::AcceptRole);
				msgBox.addButton(tr("XZ"), QMessageBox::AcceptRole);

				ret = msgBox.exec();

				command.push_back("copySurfaceMirror");
				command.push_back(QString::number(k));
				command.push_back("0");
				command.push_back("0");
				command.push_back("0");

				switch (ret)
				{
				case 0:
					command.push_back("XY");
					break;
				case 1:
					command.push_back("YZ");
					break;
				case 2:
					command.push_back("XZ");
					break;
				}

				w->executeCommand("COPY SURFACE MIRROR", command, true);

				finishEdit();
				break;
			case MERGE_SURFACES_BY_ROW:
				if (!primedForClick)
				{
					_ScratchControlPoint->set_K(k);

					for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
					{
						for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
						{
							addFocusPoint(project->getControlPoint(k, itX, itY));
						}
					}

					primedForClick = true;
				}
				else
				{
					if (_ScratchControlPoint->get_K() == k)
					{
						QMessageBox::warning(this, "Merge surface", "You cannot merge surface with itself");

						finishEdit();
					}
					else if (project->getSurface(_ScratchControlPoint->get_K())->sizeY() != project->getSurface(k)->sizeY())
					{
						QMessageBox::warning(this, "Merge surface", "You cannot merge surfaces with diffrent number of columns");

						finishEdit();
					}
					else
					{
						auto tmp = project->getSurface(k);
						auto tmpBase = project->getBaseSurface(k);

						QStringList items;
						items << tr("First to first") << tr("First to last") << tr("Last to first") << tr("Last to last");

						bool ok;
						QString item = QInputDialog::getItem(this, tr("Merge surface"),
							tr("Choose how to merge surfaces rowwise:"), items, 0, false, &ok);

						if (ok && !item.isEmpty())
						{
							command.push_back("mergeSurface");
							command.push_back(QString::number(_ScratchControlPoint->get_K()));

							if (QString::compare(item, items[0], Qt::CaseInsensitive) == 0)
							{
								command.push_back("first");
								command.push_back(QString::number(k));
								command.push_back("first");
							}
							else if (QString::compare(item, items[1], Qt::CaseInsensitive) == 0)
							{
								command.push_back("first");
								command.push_back(QString::number(k));
								command.push_back("last");
							}
							else if (QString::compare(item, items[2], Qt::CaseInsensitive) == 0)
							{
								command.push_back("last");
								command.push_back(QString::number(k));
								command.push_back("first");
							}
							else if (QString::compare(item, items[3], Qt::CaseInsensitive) == 0)
							{
								command.push_back("last");
								command.push_back(QString::number(k));
								command.push_back("last");
							}
							
							w->executeCommand("MERGE SURFACE", command, true);

							finishEdit();
						}
						else
						{
							finishEdit();
						}
					}
				}
				break;
			case MERGE_SURFACES_BY_ROW_REVERSE:
				if (!primedForClick)
				{
					_ScratchControlPoint->set_K(k);

					for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
					{
						for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
						{
							addFocusPoint(project->getControlPoint(k, itX, itY));
						}
					}

					primedForClick = true;
				}
				else
				{
					if (_ScratchControlPoint->get_K() == k)
					{
						QMessageBox::warning(this, "Merge surface reversed", "You cannot merge surface with itself");

						finishEdit();
					}
					else if (project->getSurface(_ScratchControlPoint->get_K())->sizeY() != project->getSurface(k)->sizeY())
					{
						QMessageBox::warning(this, "Merge surface reversed", "You cannot merge surfaces with diffrent number of columns");

						finishEdit();
					}
					else
					{
						auto tmp = project->getSurface(k);
						auto tmpBase = project->getBaseSurface(k);

						QStringList items;
						items << tr("First to first") << tr("First to last") << tr("Last to first") << tr("Last to last");

						bool ok;
						QString item = QInputDialog::getItem(this, tr("Merge surface reversed"),
							tr("Choose how to merge surfaces rowwise:"), items, 0, false, &ok);

						if (ok && !item.isEmpty())
						{
							command.push_back("mergeSurfaceReversed");
							command.push_back(QString::number(_ScratchControlPoint->get_K()));

							if (QString::compare(item, items[0], Qt::CaseInsensitive) == 0)
							{
								command.push_back("first");
								command.push_back(QString::number(k));
								command.push_back("first");
							}
							else if (QString::compare(item, items[1], Qt::CaseInsensitive) == 0)
							{
								command.push_back("first");
								command.push_back(QString::number(k));
								command.push_back("last");
							}
							else if (QString::compare(item, items[2], Qt::CaseInsensitive) == 0)
							{
								command.push_back("last");
								command.push_back(QString::number(k));
								command.push_back("first");
							}
							else if (QString::compare(item, items[3], Qt::CaseInsensitive) == 0)
							{
								command.push_back("last");
								command.push_back(QString::number(k));
								command.push_back("last");
							}

							w->executeCommand("MERGE SURFACE REVERSED", command, true);

							finishEdit();
						}
						else
						{
							finishEdit();
						}
					}
				}
				break;
			case MEASURE_DISTANCE:
				if (!scrachPointReady)
				{
					_ScratchControlPoint->set_X(project->getControlPoint(k, i, j)->get_X());
					_ScratchControlPoint->set_Y(project->getControlPoint(k, i, j)->get_Y());
					_ScratchControlPoint->set_Z(project->getControlPoint(k, i, j)->get_Z());

					scrachPointReady = true;
				}
				else
				{
					float dx = project->getControlPoint(k, i, j)->get_X() - _ScratchControlPoint->get_X();
					float dy = project->getControlPoint(k, i, j)->get_Y() - _ScratchControlPoint->get_Y();
					float dz = project->getControlPoint(k, i, j)->get_Z() - _ScratchControlPoint->get_Z();
					float dist = sqrtf(dx*dx + dy*dy + dz*dz);

					w->setMyTextDataField(QString("Distance: %1, dx: %2, dy: %3, dz: %4").arg(dist).arg(dx).arg(dy).arg(dz));

					finishEdit();
				}
				break;
			case RESIZE:
				data = QInputDialog::getDouble(this, tr("Scaling"), tr("Enter scaling factor:"), 1.0, 0.001, 2147483647, 3, &status);
				if ((status) && (data > 0))
				{
					command.push_back("resizeSurface");
					command.push_back(QString::number(k));
					command.push_back(QString::number(data));

					w->executeCommand("RESIZE SURFACE", command, true);
				}
				break;
			default:
				if (singleSelect) { focusedPoints.erase(focusedPoints.begin(), focusedPoints.end()); }

				switch (_selectMode)
				{
				case POINT_M:
					addFocusPoint(project->getControlPoint(k, i, j));
					break;
				case ROW_M:
					for (int it = 0; it < project->getSurface(k)->sizeY(); it++)
					{
						addFocusPoint(project->getControlPoint(k, i, it));
					}
					break;
				case COLUMN_M:
					for (int it = 0; it < project->getSurface(k)->sizeX(); it++)
					{
						addFocusPoint(project->getControlPoint(k, it, j));
					}
					break;
				case SURFACE_M:
					for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
					{
						for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
						{
							addFocusPoint(project->getControlPoint(k, itX, itY));
						}
					}
					break;
				}
			}
		}
		else
		{
			focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());

			if (scrachPointReady)
			{
				primedForClick = false;

				get_ScratchControlPoint()->set_X(0.0);
				get_ScratchControlPoint()->set_Y(0.0);
				get_ScratchControlPoint()->set_Z(0.0);

				get_ScratchControlPoint()->set_K(-1);
				get_ScratchControlPoint()->set_I(-1);
				get_ScratchControlPoint()->set_J(-1);

				scrachPointReady = false;
				readyToShear = false;
			}
		}

		// Redraw everything.
		update();
	}
}

void View2d::mouseMoveEvent(QMouseEvent *event)
{
	float dx = (float)(event->x() - lastPos.x());
	float dy = (float)(event->y() - lastPos.y());

	float factor = 0.0;

	// Check for fine movement.
	if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)
	{
		// Fine movement.
		factor = 0.05;
	}
	else
	{
		// Coarse movement.
		factor = 1.0;
	}

	focusedPoints_light.erase(focusedPoints_light.begin(), focusedPoints_light.end());

	if (event->buttons() & Qt::LeftButton)
	{
		if (!(event->modifiers())) // Just clicking without modifiers.
		{
			float posX, posY, posZ, old_posX, old_posY, old_posZ;

			getAxesPos(posX, posY, event->x(), event->y());
			getAxesPos(old_posX, old_posY, lastPos.x(), lastPos.y());

			float diffX, diffY, diffZ;

			switch (_plane)
			{
			case XY:
				diffX = posX - old_posX;
				diffY = posY - old_posY;
				diffZ = 0;
				break;
			case XZ:
				diffX = posX - old_posX;
				diffY = 0;
				diffZ = posY - old_posY;
				break;
			case YZ:
				diffX = 0;
				diffY = posX - old_posX;
				diffZ = posY - old_posY;
				break;
			}

			if ((MY_EDIT_MODE == DRAG) && (focusedPoints.size() > 0))
			{
				// Drag the focused points.
				for (auto it : focusedPoints)
				{
					project->movePoint(it->get_K(), it->get_I(), it->get_J(), diffX, diffY, diffZ, false);
				}

				draggedX += diffX;
				draggedY += diffY;
				draggedZ += diffZ;

				drawVector = true;
				UnsavedChanges = true;
			}
			else if ((MY_EDIT_MODE == ROTATE) && (scrachPointReady) && (focusedPoints.size() > 0))
			{
				Point3 center = _ScratchControlPoint->getCoordinates();

				float thetaOld;
				float thetaNew;

				switch (_plane)
				{
				case XY:
					thetaOld = atan2(old_posY - center.y, old_posX - center.x);
					thetaNew = atan2(posY - center.y, posX - center.x);
					break;
				case XZ:
					thetaOld = atan2(old_posY - center.z, old_posX - center.x);
					thetaNew = atan2(posY - center.z, posX - center.x);
					break;
				case YZ:
					thetaOld = atan2(old_posY - center.z, old_posX - center.y);
					thetaNew = atan2(posY - center.z, posX - center.y);
					break;
				}
				
				float dTheta = thetaNew - thetaOld;

				// Rotate the focused points.
				for (auto it : focusedPoints)
				{
					project->rotatePoint(it->get_K(), it->get_I(), it->get_J(), center.x, center.y, center.z, dTheta, _plane, false);
				}

				angleRotated += dTheta;

				drawDial = true;
				UnsavedChanges = true;
			}
			else if ((MY_EDIT_MODE == SHEAR) && (readyToShear))
			{
				float diff;

				switch (_plane)
				{
				case XY:
					diff = diffX;
					draggedX += diffX;
					break;
				case XZ:
					diff = diffX;;
					draggedX += diffX;
					break;
				case YZ:
					diff = diffY;;
					draggedY += diffY;
					break;
				}

				project->sheer(_ScratchControlPoint->get_K(), _ScratchControlPoint->get_I(), _ScratchControlPoint->get_J(), memoryI, memoryJ, diff, _plane);

				drawVector = true;
				UnsavedChanges = true;

			}
			else if ((MY_EDIT_MODE == CENTRED_ROTATE) && (focusedPoints.size() > 0))
			{
				Point3 center;
				getCenterOfFocused(center.x, center.y, center.z);

				_ScratchControlPoint->set_K(0);
				_ScratchControlPoint->set_I(0);
				_ScratchControlPoint->set_J(0);

				_ScratchControlPoint->set_X(center.x);
				_ScratchControlPoint->set_Y(center.y);
				_ScratchControlPoint->set_Z(center.z);

				scrachPointReady = true;

				float thetaOld;
				float thetaNew;

				switch (_plane)
				{
				case XY:
					thetaOld = atan2(old_posY - center.y, old_posX - center.x);
					thetaNew = atan2(posY - center.y, posX - center.x);
					break;
				case XZ:
					thetaOld = atan2(old_posY - center.z, old_posX - center.x);
					thetaNew = atan2(posY - center.z, posX - center.x);
					break;
				case YZ:
					thetaOld = atan2(old_posY - center.z, old_posX - center.y);
					thetaNew = atan2(posY - center.z, posX - center.y);
					break;
				}

				float dTheta = thetaNew - thetaOld;

				// Rotate the focused points.
				for (auto it : focusedPoints)
				{
					project->rotatePoint(it->get_K(), it->get_I(), it->get_J(), center.x, center.y, center.z, dTheta, _plane, false);
				}

				angleRotated += dTheta;

				drawDial = true;
				UnsavedChanges = true;
			}
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		eyeX -= factor * dx;
		eyeY += factor * dy;
	}
	
	setPointHighlight(event);

	lastPos = event->pos();

	// Redraw everything.
	update();

	// Redraw other views.
	w->updateAllTabs();
}

void View2d::mouseReleaseEvent(QMouseEvent *event)
{
	QStringList command, revCommand;
	int id;
	ITControlPoint* p;
	float diff;
	QString plane;

	switch (MY_EDIT_MODE)
	{
	case DRAG:

		if (focusedPoints.size() != 0)
		{
			id = w->undoRedo.registerPointsGroupMove(focusedPoints, draggedX, draggedY, draggedZ);

			command.push_back(QString("redoPointGroupMove"));
			command.push_back(QString::number(id));

			revCommand.push_back(QString("undoPointGroupMove"));
			revCommand.push_back(QString::number(id));

			w->undoRedo.registerCommand(command, revCommand);
		}

		draggedX = 0.0;
		draggedY = 0.0;
		draggedZ = 0.0;

		drawVector = false;
		break;
	case ROTATE:
		if (focusedPoints.size() != 0)
		{
			id = w->undoRedo.registerPointsGroupRotate(focusedPoints, angleRotated, _ScratchControlPoint->get_X(), _ScratchControlPoint->get_Y(), _ScratchControlPoint->get_Z(), _plane);

			command.push_back(QString("redoPointGroupRotate"));
			command.push_back(QString::number(id));

			revCommand.push_back(QString("undoPointGroupRotate"));
			revCommand.push_back(QString::number(id));

			w->undoRedo.registerCommand(command, revCommand);
		}

		drawDial = false;
		dialInitialized = false;
		angleRotated = 0.0;
		break;
	case CENTRED_ROTATE:
		if (focusedPoints.size() != 0)
		{
			id = w->undoRedo.registerPointsGroupRotate(focusedPoints, angleRotated, _ScratchControlPoint->get_X(), _ScratchControlPoint->get_Y(), _ScratchControlPoint->get_Z(), _plane);

			command.push_back(QString("redoPointGroupRotate"));
			command.push_back(QString::number(id));

			revCommand.push_back(QString("undoPointGroupRotate"));
			revCommand.push_back(QString::number(id));

			w->undoRedo.registerCommand(command, revCommand);
		}

		drawDial = false;
		dialInitialized = false;
		angleRotated = 0.0;
		break;
	case SHEAR:
		if ((draggedX != 0.0) || (draggedY != 0.0) || (draggedZ != 0.0))
		{
			p = _ScratchControlPoint;

			switch (_plane)
			{
			case XY:
				diff = draggedX;
				plane = "xy";
				break;
			case XZ:
				diff = draggedX;
				plane = "xz";
				break;
			case YZ:
				diff = draggedY;
				plane = "yz";
				break;
			}

			command.push_back(QString("sheer"));
			command.push_back(QString::number(p->get_K()));
			command.push_back(QString::number(p->get_I()));
			command.push_back(QString::number(p->get_J()));
			command.push_back(QString::number(memoryI));
			command.push_back(QString::number(memoryJ));
			command.push_back(QString::number(diff));
			command.push_back(plane);

			revCommand.push_back(QString("sheer"));
			revCommand.push_back(QString::number(p->get_K()));
			revCommand.push_back(QString::number(p->get_I()));
			revCommand.push_back(QString::number(p->get_J()));
			revCommand.push_back(QString::number(memoryI));
			revCommand.push_back(QString::number(memoryJ));
			revCommand.push_back(QString::number(-diff));
			revCommand.push_back(plane);

			w->undoRedo.registerCommand(command, revCommand);
		}

		draggedX = 0.0;
		draggedY = 0.0;
		draggedZ = 0.0;

		drawVector = false;
		break;
	case MERGE_SURFACES_BY_ROW:
		break;
	case MERGE_SURFACES_BY_ROW_REVERSE:
		break;
	}

	for (int it = 0; it < project->get_MySurfaces()->size(); it++)
	{
		project->getSurface(it)->manageComputationOfInterpolatedPoints();
	}

	project->synchronizeSurfaceVectorsFromControl();

	w->updateAllTabs();

	// Update the spreadsheet.
	w->updateSpreadsheet();

	//finishEdit();
}

void View2d::wheelEvent(QWheelEvent *event)
{
	float factor = 0.0;
	float dy = event->delta() / 8;

	// Check for fine movement.
	if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier) == true)
	{
		// Fine movement.
		factor = 0.05;
	}
	else
	{
		// Coarse movement.
		factor = 1.0;
	}

	float tmpAx, tmpAy, tmpBx, tmpBy;

	getAxesPos(tmpAx, tmpAy, event->x(), event->y());

	float tmp = eyeZoom + 0.01 * factor * dy;

	if (tmp >= 0.0) eyeZoom = tmp;

	getAxesPos(tmpBx, tmpBy, event->x(), event->y());

	eyeX += tmpAx - tmpBx;
	eyeY += tmpAy - tmpBy;

	// Redraw everything.
	update();
}

// Utility functions.
void View2d::getAxesPos(float & pX, float & pY, const int x, const int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	winZ = 0.0;

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	switch (_plane)
	{
	case XY:
		pX = posX;
		pY = posY;
		break;
	case XZ:
		pX = posX;
		pY = posZ;
		break;
	case YZ:
		pX = posY;
		pY = posZ;
		break;
	}
}

void View2d::addFocusPoint(ITControlPoint * p)
{
	auto ptr = std::find(focusedPoints.begin(), focusedPoints.end(), p);
	
	if (ptr == focusedPoints.end()) focusedPoints.push_back(p);
}

void View2d::setPointHighlight(QMouseEvent *event)
{
	float posX, posY;

	getAxesPos(posX, posY, event->x(), event->y());

	// Find the vertex that is closest.
	int i, j, k;
	findControlPointIndicesNearMouse(posX, posY, &k, &i, &j);

	if ((k > -1) && (i > -1) && (j > -1))
	{
		if ((MY_EDIT_MODE == COPY_SURFACE) || (MY_EDIT_MODE == DELETE_SURFACE) || (MY_EDIT_MODE == COPY_SURFACE_MIRROR) || (MY_EDIT_MODE == FLIP) || (MY_EDIT_MODE == RESIZE) || (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW) || (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW_REVERSE))
		{
			for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
			{
				for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
				{
					focusedPoints_light.push_back(project->getControlPoint(k, itX, itY));
				}
			}
		}
		else if ((MY_EDIT_MODE == INSERT_COL) || (MY_EDIT_MODE == DUPLICATE_COL) || (MY_EDIT_MODE == DELETE_COL))
		{
			for (int it = 0; it < project->getSurface(k)->sizeX(); it++)
			{
				focusedPoints_light.push_back(project->getControlPoint(k, it, j));
			}
		}
		else if ((MY_EDIT_MODE == INSERT_ROW) || (MY_EDIT_MODE == DUPLICATE_ROW) || (MY_EDIT_MODE == DELETE_ROW))
		{
			for (int it = 0; it < project->getSurface(k)->sizeY(); it++)
			{
				focusedPoints_light.push_back(project->getControlPoint(k, i, it));
			}
		}
		else if ((MY_EDIT_MODE == MATE_POINTS) || (MY_EDIT_MODE == MEASURE_DISTANCE) || (MY_EDIT_MODE == SHEAR))
		{
			focusedPoints_light.push_back(project->getControlPoint(k, i, j));
		}
		else if ((MY_EDIT_MODE == DRAG) || (MY_EDIT_MODE == ROTATE) || (MY_EDIT_MODE == CENTRED_ROTATE))
		{
			switch (_selectMode)
			{
			case POINT_M:
				focusedPoints_light.push_back(project->getControlPoint(k, i, j));
				break;
			case ROW_M:
				for (int it = 0; it < project->getSurface(k)->sizeY(); it++)
				{
					focusedPoints_light.push_back(project->getControlPoint(k, i, it));
				}
				break;
			case COLUMN_M:
				for (int it = 0; it < project->getSurface(k)->sizeX(); it++)
				{
					focusedPoints_light.push_back(project->getControlPoint(k, it, j));
				}
				break;
			case SURFACE_M:
				for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
				{
					for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
					{
						focusedPoints_light.push_back(project->getControlPoint(k, itX, itY));
					}
				}
				break;
			}

		}
	}
}

void View2d::getCenterOfFocused(float & posX, float & posY, float & posZ)
{
	posX = 0;
	posY = 0;
	posZ = 0;

	if (focusedPoints.size() != 0)
	{
		for (auto it : focusedPoints)
		{
			posX += it->get_X();
			posY += it->get_Y();
			posZ += it->get_Z();
		}

		posX /= focusedPoints.size();
		posY /= focusedPoints.size();
		posZ /= focusedPoints.size();
	}
}

void View2d::findControlPointIndicesNearMouse(double posX, double posY, int *targetK, int *targetI, int *targetJ)
{
	float x, y, z;
	float threshold = 0.5;
	float minDistance = 10000.0;
	*targetI = -1;
	*targetJ = -1;
	*targetK = -1;

	x = y = z = 0.0;

	switch (_plane)
	{
	case XY:
		x = posX;
		y = posY;
		break;
	case XZ:
		x = posX;
		z = posY;
		break;
	case YZ:
		y = posX;
		z = posY;
		break;
	}

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->getSurface(k)->sizeX(); i++)
		{
			for (int j = 0; j < project->getSurface(k)->sizeY(); j++)
			{
				ITControlPoint *currentPoint = project->getControlPoint(k, i, j);

				float deltaX = x - currentPoint->get_X();
				float deltaY = y - currentPoint->get_Y();
				float deltaZ = z - currentPoint->get_Z();

				float distanceSquared;

				switch (_plane)
				{
				case XY:
					distanceSquared = deltaX*deltaX + deltaY*deltaY;
					break;
				case XZ:
					distanceSquared = deltaX*deltaX + deltaZ*deltaZ;
					break;
				case YZ:
					distanceSquared = deltaY*deltaY + deltaZ*deltaZ;
					break;
				}

				if ((distanceSquared < threshold) && (distanceSquared < minDistance))
				{
					minDistance = distanceSquared;
					*targetK = k;
					*targetI = i;
					*targetJ = j;
				}
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Indices of nearest control point. k: %i, i: %i, j: %i", *targetK, *targetI, *targetJ);
}

void View2d::finishEdit()
{
	w->resetModeButtons();
	project->manageComputationOfInterpolatedPoints();
	project->synchronizeSurfaceVectorsFromControl();

	focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
	focusedPoints_light.erase(focusedPoints_light.begin(), focusedPoints_light.end());

	primedForClick = false;

	get_ScratchControlPoint()->set_X(0.0);
	get_ScratchControlPoint()->set_Y(0.0);
	get_ScratchControlPoint()->set_Z(0.0);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);

	scrachPointReady = false;

	angleRotated = 0.0;
	draggedX = 0.0;
	draggedY = 0.0;
	draggedZ = 0.0;

	drawDial = false;
	dialInitialized = false;

	drawVector = false;

	memoryK = -1;
	memoryI = -1;
	memoryJ = -1;

	readyToShear = false;

	MY_EDIT_MODE = NONE;

	w->updateAllTabs();
}

// Accessors.
ITControlPoint *View2d::get_ScratchControlPoint() { return _ScratchControlPoint; }
void View2d::set_ScratchControlPoint(ITControlPoint *p) { _ScratchControlPoint = p; }

void View2d::set_plane(PLANE p)
{
	_plane = p;

	switch (_plane)
	{
	case XY:
		xRot = 0;
		yRot = 0;
		zRot = 0;
		break;
	case XZ:
		xRot = 270;
		yRot = 0;
		zRot = 0;
		break;
	case YZ:
		xRot = 270;
		yRot = 270;
		zRot = 0;
		break;
	}
}

void View2d::setSceneParameters(float eyeX, float eyeY, float eyeZoom)
{
	this->eyeX = eyeX;
	this->eyeY = eyeY;
	if (eyeZoom > 0) this->eyeZoom = eyeZoom;
}
