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
	xRot = 0;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	set_PrimedForFirstClick(false);
	set_PrimedForSecondClick(false);

	// Instanciate the control point.
	ITControlPoint *point = new ITControlPoint(0.0, 0.0, 0.0);
	set_ScratchControlPoint(point);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);

	glPanCenterX = 0.0;
	glPanCenterY = 0.0;
	glViewHalfExtent = 50.0;

	_plane = XY;
}

View2d::~View2d(void)
{
	delete _ScratchControlPoint;
}

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
	glTranslatef(0.0, 0.0, -10.0);

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
	
	//if (drawRotateXHorizontal)
	//{
	//	// X horizontal. (Checked)
	//	glRotatef((float)yRot, 0.0, 1.0, 0.0);
	//	glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
	//	glRotatef(0.0, 0.0, 0.0, 1.0);
	//}
	//else // if (drawRotateYHorizontal)
	//{
	//	// Y horizontal. This is actually Y vertical.
	//	glRotatef(0.0, 1.0, 0.0, 0.0); // Rotate about the X axis.
	//	glRotatef((float)yRot, 0.0, 1.0, 0.0);
	//	glRotatef((float)xRot, 0.0, 0.0, 1.0);
	//}

	draw();
}

void View2d::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void View2d::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(glPanCenterX - glViewHalfExtent,
		glPanCenterX + glViewHalfExtent,
		glPanCenterY - glViewHalfExtent / aspect,
		glPanCenterY + glViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void View2d::keyPressEvent(QKeyEvent * event)
{
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

	if (event->key() == Qt::Key_Up)
	{
		glPanCenterY += factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glPanCenterY -= factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glPanCenterX -= factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glPanCenterX += factor;
	}
	else if (event->key() == Qt::Key_V)
	{
		// User is holding the V key down, so restrict dragging to vertical only.
		IsVerticalDragOnly = true;
		IsHorizontalDragOnly = false;
	}
	else if (event->key() == Qt::Key_H)
	{
		// User is holding the H key down, so restrict dragging to horizontal only.
		IsVerticalDragOnly = false;
		IsHorizontalDragOnly = true;
	}
	else if (event->key() == Qt::Key_A)
	{
		// User is holding the A key down, so remove dragging restrictions.
		IsVerticalDragOnly = false;
		IsHorizontalDragOnly = false;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
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

		if (drawAnnotations)
		{
			//drawMyAnnotations();
		}

		if (drawGrids)
		{
			drawMyGrids();
		}

		//if (drawDial)
		//{
		//	drawAngleDial();
		//}

		drawMyFocusControlPoints();
		drawMyScratchControlPoint();
	}
}

void View2d::drawMyScratchControlPoint()
{

	if ((get_ScratchControlPoint()->get_K() > -1) || (get_ScratchControlPoint()->get_I() > -1) || (get_ScratchControlPoint()->get_J() > -1))
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
		float radius = glViewHalfExtent / 50.0;
		drawSphere(radius, 15, 15, 0.0, 1.0, 0.0);
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

void View2d::drawMyAnnotations()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->getSurface(k)->sizeX(); i++)
		{
			for (int j = 0; j < project->getSurface(k)->sizeY(); j++)
			{
				ITPoint *p = project->getSurface(k)->getControlPoint(i, j);

				renderText(p->get_X(), p->get_Y(), p->get_Z(), p->getAnnotationText());
			}
		}
	}
}

void View2d::finishEdit()
{
	w->resetModeButtons();

	focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());

	set_PrimedForFirstClick(true);
	set_PrimedForSecondClick(false);

	get_ScratchControlPoint()->set_X(0.0);
	get_ScratchControlPoint()->set_Y(0.0);
	get_ScratchControlPoint()->set_Z(0.0);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);

	MY_EDIT_MODE = NONE;
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

//void View2d::drawAngleDial()
//{
//	dialSize = glXYViewHalfExtent * 0.25;
//
//	float beginX = dialCentreX + dialSize;
//	float beginY = dialCentreY;
//
//	float endX = dialCentreX + dialSize * cosf(dialAngle);
//	float endY = dialCentreY + dialSize * sinf(dialAngle);
//
//	glBegin(GL_LINE_STRIP);
//
//	glVertex2f(beginX, beginY);
//	glVertex2f(dialCentreX, dialCentreY);
//	glVertex2f(endX, endY);
//
//	glEnd();
//
//	float angle = 0;
//	float dAngle = 0.001;
//
//	float x = dialCentreX + dialSize * cosf(angle);
//	float y = dialCentreY + dialSize * sinf(angle);
//
//	glBegin(GL_LINE_STRIP);
//
//	if (dialAngle > 0)
//	{
//		while (angle < dialAngle)
//		{
//			glVertex2f(x, y);
//
//			x = dialCentreX + 0.9 * dialSize * cosf(angle);
//			y = dialCentreY + 0.9 * dialSize * sinf(angle);
//
//			angle += dAngle;
//		}
//	}
//	else {
//		dAngle = -dAngle;
//
//		while (angle > dialAngle)
//		{
//			glVertex2f(x, y);
//
//			x = dialCentreX + 0.9 * dialSize * cosf(angle);
//			y = dialCentreY + 0.9 * dialSize * sinf(angle);
//
//			angle += dAngle;
//		}
//	}
//
//
//	glEnd();
//
//	float textX = dialCentreX + dialSize * cosf(dialAngle / 2) * 1.2;
//	float textY = dialCentreY + dialSize * sinf(dialAngle / 2) * 1.2;
//
//	renderText(textX, textY, 0, QString("%1 deg").arg(dialAngle * 180 / 3.14));
//}

void View2d::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		set_EditValue(event->x()); // Used in perspective transformation.

		if ((MY_EDIT_MODE == DRAG) ||
			(MY_EDIT_MODE == DRAG_ROW) ||
			(MY_EDIT_MODE == DRAG_COL) ||
			(MY_EDIT_MODE == DRAG_ALL) ||
			(MY_EDIT_MODE == ROTATE_ALL) ||
			(MY_EDIT_MODE == RESIZE_ALL) ||
			(MY_EDIT_MODE == SHEAR_ALL) ||
			(MY_EDIT_MODE == PERSPECTIVE_ALL) ||
			(MY_EDIT_MODE == FLIP) ||
			(MY_EDIT_MODE == COPY_SURFACE) ||
			(MY_EDIT_MODE == DELETE_SURFACE) ||
			(MY_EDIT_MODE == INSERT_ROW) ||
			(MY_EDIT_MODE == DELETE_ROW) ||
			(MY_EDIT_MODE == DUPLICATE_ROW) ||
			(MY_EDIT_MODE == INSERT_COL) ||
			(MY_EDIT_MODE == DELETE_COL) ||
			(MY_EDIT_MODE == DUPLICATE_COL) ||
			(MY_EDIT_MODE == MATE_POINTS) ||
			(MY_EDIT_MODE == MERGE_SURFACES_BY_ROW) ||
			(MY_EDIT_MODE == MERGE_SURFACES_BY_ROW_REVERSE) ||
			(MY_EDIT_MODE == COPY_SURFACE_MIRROR) ||
			(MY_EDIT_MODE == MEASURE_DISTANCE) ||
			(MY_EDIT_MODE == CENTRED_ROTATE)
			)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}

		//if (MY_EDIT_MODE == ROTATE_ALL)
		//{
		//	dialAngle = 0;

		//	getInAxesPosition(dialCentreX, dialCentreY, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

		//	drawDial = true;
		//}
	}
}

void View2d::AssignFocusPoint(QMouseEvent *event)
{
	float posX, posY;

	getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), glPanCenterX, glPanCenterY, glViewHalfExtent);

	// Find the vertex that is closest.
	int i, j, k;
	findControlPointIndicesNearMouse(posX, posY, &k, &i, &j);

	// Make sure lastPos is reset.
	lastPos = event->pos();

	// Make sure edit value is reset.
	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	int ret, surfaceID;

	if ((k > -1) && (i > -1) && (j > -1))
	{
		float dX, dY, dZ;

		switch (_plane)
		{
		case XY:
			dX = dY = 10.0;
			break;
		case XZ:
			dX = dZ = 10.0;
			break;
		case YZ:
			dZ = dY = 10.0;
			break;
		}

		QMessageBox msgBox;

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

			switch (ret)
			{
			case 0:
				project->flipSurfaceCentral(k, XY);
				break;
			case 1:
				project->flipSurfaceCentral(k, YZ);
				break;
			case 2:
				project->flipSurfaceCentral(k, XZ);
				break;
			}

			finishEdit();
			break;
		case COPY_SURFACE:
			project->copySurface(k, dX, dY, dZ);

			finishEdit();
			break;
		case DELETE_SURFACE:
			project->deleteSurface(k);

			finishEdit();
			break;
		case INSERT_ROW:
			if ((i == 0) || (i == project->getSurface(k)->sizeX() - 1))
			{
				// The user is trying to insert a row after the last row which is not possible, so display a warning dialogue.
				QMessageBox::StandardButton reply;
				reply = QMessageBox::warning(this, "Insert Row", "You cannot insert a row after the last row");
			}
			else
			{
				project->insertRow(k, i);
			}

			finishEdit();
			break;
		case DELETE_ROW:
			project->deleteRow(k, i);

			finishEdit();
			break;
		case DUPLICATE_ROW:
			project->duplicateRow(k, i);

			finishEdit();
			break;
		case INSERT_COL:
			if ((j == 0) || (j == project->getSurface(k)->sizeY() - 1))
			{
				// The user is trying to insert a row after the last row which is not possible, so display a warning dialogue.
				QMessageBox::StandardButton reply;
				reply = QMessageBox::warning(this, "Insert Column", "You cannot insert a column after the last column");
			}
			else
			{
				project->insertColumn(k, j);
			}

			finishEdit();
			break;
		case DELETE_COL:
			project->deleteColumn(k, j);

			finishEdit();
			break;
		case DUPLICATE_COL:
			project->duplicateColumn(k, j);

			finishEdit();
			break;
		case MATE_POINTS:
			if (_PrimedForFirstClick)
			{
				_ScratchControlPoint->set_K(k);
				_ScratchControlPoint->set_I(i);
				_ScratchControlPoint->set_J(j);

				_PrimedForFirstClick = false;
			}
			else
			{
				project->matePoints(_ScratchControlPoint->get_K(), _ScratchControlPoint->get_I(), _ScratchControlPoint->get_J(), k, i, j);

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

			project->copySurface(k, dX, dY, dZ);
			
			surfaceID = project->get_MySurfaces()->size() - 1;

			switch (ret)
			{
			case 0:
				project->flipSurfaceCentral(surfaceID, XY);
				break;
			case 1:
				project->flipSurfaceCentral(surfaceID, YZ);
				break;
			case 2:
				project->flipSurfaceCentral(surfaceID, XZ);
				break;
			}
			break;
		case MERGE_SURFACES_BY_ROW:
			finishEdit();
			break;
		case MERGE_SURFACES_BY_ROW_REVERSE:
			finishEdit();
			break;
		case MEASURE_DISTANCE:
			if (_PrimedForFirstClick)
			{
				_ScratchControlPoint->set_X(project->getControlPoint(k, i, j)->get_X());
				_ScratchControlPoint->set_Y(project->getControlPoint(k, i, j)->get_Y());
				_ScratchControlPoint->set_Z(project->getControlPoint(k, i, j)->get_Z());
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
		default:
			switch (_selectMode)
			{
			case POINT_SINGLE:
				focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
				focusedPoints.push_back(project->getControlPoint(k, i, j));
				break;
			case POINT_STACKING:
				focusedPoints.push_back(project->getControlPoint(k, i, j));
				break;
			case ROW_SINGLE:
				focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
				for (int it = 0; it < project->getSurface(k)->sizeY(); it++)
				{
					focusedPoints.push_back(project->getControlPoint(k, it, j));
				}
				break;
			case ROW_STACKING:
				for (int it = 0; it < project->getSurface(k)->sizeY(); it++)
				{
					focusedPoints.push_back(project->getControlPoint(k, i, it));
				}
				break;
			case COLUMN_SINGLE:
				focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
				for (int it = 0; it < project->getSurface(k)->sizeX(); it++)
				{
					focusedPoints.push_back(project->getControlPoint(k, it, j));
				}
				break;
			case COLUMN_STACKING:
				for (int it = 0; it < project->getSurface(k)->sizeX(); it++)
				{
					focusedPoints.push_back(project->getControlPoint(k, it, j));
				}
				break;
			case SURFACE_SINGLE:
				focusedPoints.erase(focusedPoints.begin(), focusedPoints.end());
				for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
				{
					for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
					{
						focusedPoints.push_back(project->getControlPoint(k, itX, itY));
					}
				}
				break;
			case SURFACE_STACKING:
				for (int itX = 0; itX < project->getSurface(k)->sizeX(); itX++)
				{
					for (int itY = 0; itY < project->getSurface(k)->sizeY(); itY++)
					{
						focusedPoints.push_back(project->getControlPoint(k, itX, itY));
					}
				}
				break;
			}
		}
	}
	
	//{
	//	if (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW)
	//	{
	//		if (get_PrimedForFirstClick())
	//		{
	//			// Empty the focus vectors.
	//			w->emptyFocusVectors();

	//			// Save the first point indices.
	//			get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
	//			get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
	//			get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

	//			get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
	//			get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
	//			get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

	//			// Cancel primed for first click and prime for second click.
	//			set_PrimedForFirstClick(false);
	//			set_PrimedForSecondClick(true);

	//			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First surface has been captured.");
	//		}
	//		else if (get_PrimedForSecondClick())
	//		{
	//			// Get second point and move it to the first one.
	//			ITControlPoint *firstPoint = get_ScratchControlPoint();
	//			ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
	//			ITControlPoint *secondBasePoint = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

	//			// Check that the number of columns for the two surfaces is the same.
	//			int k1 = firstPoint->get_K();
	//			int k2 = secondPoint->get_K();
	//			int noOfCols1 = project->get_MySurfaces()->at(k1)->get_MyControlPoints()->at(0).size();
	//			int noOfCols2 = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(0).size();
	//			if (noOfCols1 == noOfCols2)
	//			{
	//				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second surface has been captured.");

	//				mergeSurfacesByRow(firstPoint->get_K(), secondPoint->get_K());

	//				// Cancel primed for first click and cancel primed for second click.
	//				set_PrimedForFirstClick(false);
	//				set_PrimedForSecondClick(false);

	//				// Reset the buttons.
	//				w->resetModeButtons();

	//				// Reset the scratch point.
	//				get_ScratchControlPoint()->set_X(0.0);
	//				get_ScratchControlPoint()->set_Y(0.0);
	//				get_ScratchControlPoint()->set_Z(0.0);

	//				get_ScratchControlPoint()->set_K(-1);
	//				get_ScratchControlPoint()->set_I(-1);
	//				get_ScratchControlPoint()->set_J(-1);

	//				// Empty the focus vectors.
	//				w->emptyFocusVectors();

	//				// Finally recompute all the Bezier surface.
	//				for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++)
	//				{
	//					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Calling manageComputationOfInterpolatedPoints on surface: %i", kk);
	//					project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();
	//				}

	//				// Mark the project as changed.
	//				UnsavedChanges = true;
	//			}
	//			else
	//			{
	//				QMessageBox::StandardButton reply;
	//				reply = QMessageBox::warning(this, "Merge Surfaces", "You cannot merge surfaces with different numbers of control point columns.");

	//				// Reset the buttons.
	//				w->resetModeButtons();

	//				// Reset the edit mode.
	//				MY_EDIT_MODE = NONE;
	//			}
	//		}
	//	}
	//	else if (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW_REVERSE)
	//	{
	//		if (get_PrimedForFirstClick())
	//		{
	//			// Empty the focus vectors.
	//			w->emptyFocusVectors();

	//			// Save the first point indices.
	//			get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
	//			get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
	//			get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

	//			get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
	//			get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
	//			get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

	//			// Cancel primed for first click and prime for second click.
	//			set_PrimedForFirstClick(false);
	//			set_PrimedForSecondClick(true);

	//			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First point has been captured. k: %i", k);
	//		}
	//		else if (get_PrimedForSecondClick())
	//		{
	//			// Get second point and move it to the first one.
	//			ITControlPoint *firstPoint = get_ScratchControlPoint();
	//			ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

	//			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second point has been captured. k: %i", k);

	//			mergeSurfacesByRowReverse(firstPoint->get_K(), secondPoint->get_K());

	//			// Cancel primed for first click and cancel primed for second click.
	//			set_PrimedForFirstClick(false);
	//			set_PrimedForSecondClick(false);

	//			// Reset the scratch point.
	//			get_ScratchControlPoint()->set_X(0.0);
	//			get_ScratchControlPoint()->set_Y(0.0);
	//			get_ScratchControlPoint()->set_Z(0.0);

	//			get_ScratchControlPoint()->set_K(-1);
	//			get_ScratchControlPoint()->set_I(-1);
	//			get_ScratchControlPoint()->set_J(-1);

	//			// Reset the buttons.
	//			w->resetModeButtons();

	//			// Empty the focus vectors.
	//			w->emptyFocusVectors();

	//			// Finally recompute all the Bezier surface.
	//			for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++)
	//			{
	//				project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();
	//			}

	//			// Mark the project as changed.
	//			UnsavedChanges = true;

	//			// Reset the buttons.
	//			w->resetModeButtons();

	//			// Reset the edit mode.
	//			MY_EDIT_MODE = NONE;
	//		}
	//	}

	// Redraw everything.
	updateGL();
}

void View2d::findControlPointIndicesNearMouse(double posX, double posY, int *targetK, int *targetI, int *targetJ)
{
	float x, y, z;
	float threshold = 0.2;
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

//void View2d::mergeSurfacesByRow(int k1, int k2)
//{
//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow");
//
//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. About to appended to k1. k1 i: %i", project->get_MySurfaces()->at(k1)->get_MyControlPoints()->size());
//
//	// The k1 surface forms the start of the new surface.
//	// Go through the second surface and add its points to the end of the first.
//	for (int i = 0; i < project->get_MySurfaces()->at(k2)->get_MyControlPoints()->size(); i++)
//	{
//		// Create new vectors that will contain the data for a row to be appended to k1.
//		std::vector <ITControlPoint *> dummy_v;
//		std::vector <ITControlPoint *> dummy_base_v;
//
//		// Loop over columns of the current row of k2.
//		for (int j = 0; j < project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).size(); j++)
//		{
//			float x = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_X();
//			float y = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_Y();
//			float z = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_Z();
//
//			ITControlPoint *p = new ITControlPoint(x, y, z);
//
//			ITControlPoint *pb = new ITControlPoint(x, y, z);
//
//			dummy_v.push_back(p);
//			dummy_base_v.push_back(pb);
//		}
//
//		project->get_MySurfaces()->at(k1)->get_MyControlPoints()->push_back(dummy_v);
//		project->get_MyBaseSurfaces()->at(k1)->get_MyControlPoints()->push_back(dummy_base_v);
//	}
//
//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. Data for k2 has been appended to k1. k1 i: %i", project->get_MySurfaces()->at(k1)->get_MyControlPoints()->size());
//
//	// Erase the second surface.
//	project->get_MySurfaces()->erase(project->get_MySurfaces()->begin() + k2);
//	project->get_MyBaseSurfaces()->erase(project->get_MyBaseSurfaces()->begin() + k2);
//
//	// Re-assign control point _K, _I and _J indices.
//	// Re-assign the _K, _I and _J variables.
//	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
//	{
//		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
//		{
//			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
//			{
//				ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
//				p->set_K(k);
//				p->set_I(i);
//				p->set_J(j);
//
//				ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
//				pb->set_K(k);
//				pb->set_I(i);
//				pb->set_J(j);
//			}
//		}
//	}
//
//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. All data has been merged, and indices have been reassigned.");
//}

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

	if (event->buttons() & Qt::LeftButton)
	{
		// Check for shift key press for zoom.
		if (event->modifiers() & Qt::ShiftModifier)
		{
			// Update zoom.
			float tmp = glViewHalfExtent + factor * dy;

			if (tmp >= 0.0) glViewHalfExtent = tmp;
		}
		else if (!(event->modifiers())) // Just clicking without modifiers.
		{
			float posX, posY, posZ, old_posX, old_posY, old_posZ;

			getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), glPanCenterX, glPanCenterY, glViewHalfExtent);
			getInAxesPosition(old_posX, old_posY, lastPos.x(), lastPos.y(), this->width(), this->height(), glPanCenterX, glPanCenterY, glViewHalfExtent);

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
			


			if (((MY_EDIT_MODE == DRAG) || (MY_EDIT_MODE == DRAG_ROW) || (MY_EDIT_MODE == DRAG_COL) || (MY_EDIT_MODE == DRAG_ALL)) && (focusedPoints.size() > 0))
			{
				// Drag the focused points
				for (int it = 0; it < focusedPoints.size(); it++)
				{
					project->movePoint(focusedPoints[it]->get_K(), focusedPoints[it]->get_I(), focusedPoints[it]->get_J(), diffX, diffY, diffZ, false);
				}
				
				lastPos = event->pos();

				UnsavedChanges = true;

				// Display the distance moved.
				//set_EditValueX(get_EditValueX() + (posX - old_posX));
				//set_EditValueY(get_EditValueY() + (posY - old_posY));
				//w->setMyTextDataField(QString("Distance dragged x: %1, y: %2").arg(get_EditValueX()).arg(get_EditValueY()));
			}
			else if (MY_EDIT_MODE == ROTATE_ALL)
			{
				//project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. ROTATE_ALL");

				//float thetaOld = atan2(old_posY - dialCentreY, old_posX - dialCentreX);
				//float thetaNew = atan2(posY - dialCentreY, posX - dialCentreX);
				//float dTheta = thetaNew - thetaOld;

				////dialAngle += dTheta;

				//set_EditValue(get_EditValue() + dTheta*180.0 / PI);

				//// Rotate the Focus points around the first point in the focus vector. 
				//rotateFocusPoints(dTheta);
				//lastPos = event->pos();

				//UnsavedChanges = true;
				//w->setMyTextDataField(QString::number(get_EditValue()));
			}
			else if (MY_EDIT_MODE == RESIZE_ALL)
			{
				//project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. RESIZE_ALL");

				//// Rotate the Focus points around the first point in the focus vector. 
				//resizeFocusPoints(y, yold);
				//lastPos = event->pos();

				//UnsavedChanges = true;
				//w->setMyTextDataField(QString::number(get_EditValue()));
			}
			else if (MY_EDIT_MODE == SHEAR_ALL)
			{
				/*project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. SHEAR_ALL");

				float dx = 0.001*(x - xold);

				set_EditValue(get_EditValue() + dx);
				shearFocusPoints(dx);

				lastPos = event->pos();
				UnsavedChanges = true;
				w->setMyTextDataField(QString::number(get_EditValue()));*/

			}
			else if (MY_EDIT_MODE == CENTRED_ROTATE)
			{
				//if (get_SecondClicksFinished())
				//{
				//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside mouseMoveEvent. CENTRED_ROTATE");

				//	float thetaOld = atan2(old_posY - get_ScratchControlPoint()->get_Y(), old_posX - get_ScratchControlPoint()->get_X());
				//	float thetaNew = atan2(posY - get_ScratchControlPoint()->get_Y(), posX - get_ScratchControlPoint()->get_X());
				//	float dTheta = thetaNew - thetaOld;

				//	set_EditValue(get_EditValue() + dTheta*180.0 / PI);

				//	// Rotate the Focus points around the first point in the focus vector. 
				//	centredRotateFocusPoints(dTheta);
				//	lastPos = event->pos();

				//	UnsavedChanges = true;
				//	w->setMyTextDataField(QString::number(get_EditValue()));
				//}
			}
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		glPanCenterX -= factor * dx;
		glPanCenterY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glPanCenterX, glPanCenterY, glViewHalfExtent);

	float tmp = glViewHalfExtent - factor * dy;

	if (tmp >= 0.0) glViewHalfExtent = tmp;

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), glPanCenterX, glPanCenterY, glViewHalfExtent);

	glPanCenterX += tmpAx - tmpBx;
	glPanCenterY += tmpAy - tmpBy;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void View2d::mouseReleaseEvent(QMouseEvent *event)
{
	//if (MY_EDIT_MODE == CENTRED_ROTATE)
	//{
	//	if (get_SecondClicksFinished())
	//	{
	//		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "_SecondClicksFinished is true so disabling priming flags.");

	//		synchronizeBaseSurfaceCoordinates();

	//		set_PrimedForFirstClick(false);
	//		set_PrimedForSecondClick(false);
	//		set_SecondClicksFinished(false);

	//		get_ScratchControlPoint()->set_I(-1);
	//		get_ScratchControlPoint()->set_J(-1);
	//		get_ScratchControlPoint()->set_K(-1);

	//		w->emptyFocusVectors();
	//	}

	//} // End of CENTRED_ROTATE

	//if (MY_EDIT_MODE == ROTATE_ALL)
	//{
	//	drawDial = false;
	//}

	for (int it = 0; it < project->get_MySurfaces()->size(); it++)
	{
		project->getSurface(it)->manageComputationOfInterpolatedPoints();
	}

	project->synchronizeSurfaceVectorsFromControl();

	w->updateAllTabs();

	set_EditValue(0.0);

	// Update the spreadsheet.
	w->updateSpreadsheet();

	finishEdit();
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
				switch (_plane)
				{
				case XY:
					x = point->get_X();
					y = point->get_Y();
					z = 0.0;
					break;
				case XZ:
					x = point->get_X();
					y = 0.0;
					z = point->get_Z();
					break;
				case YZ:
					x = 0.0;
					y = point->get_Y();
					z = point->get_Z();
					break;
				}

				glTranslatef(x, y, z);

				float radius = glViewHalfExtent / 100.0;
				drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);

				glTranslatef(-x, -y, -z);
			}
		}
	}
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
		switch (_plane)
		{
		case XY:
			x = point->get_X();
			y = point->get_Y();
			z = 0.0;
			break;
		case XZ:
			x = point->get_X();
			y = 0.0;
			z = point->get_Z();
			break;
		case YZ:
			x = 0.0;
			y = point->get_Y();
			z = point->get_Z();
			break;
		}

		glTranslatef(x, y, z);

		float radius = glViewHalfExtent / 50.0;
		drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

		glTranslatef(-x, -y, -z);
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

// Accessors.
float View2d::get_EditValue() { return _EditValue; }
void View2d::set_EditValue(float a) { _EditValue = a; }

float View2d::get_EditValueX() { return _EditValueX; }
void View2d::set_EditValueX(float a) { _EditValueX = a; }

float View2d::get_EditValueY() { return _EditValueY; }
void View2d::set_EditValueY(float a) { _EditValueY = a; }

int View2d::get_Sense() { return _Sense; }
void View2d::set_Sense(int a) { _Sense = a; }

bool View2d::get_PrimedForFirstClick() { return _PrimedForFirstClick; }
void View2d::set_PrimedForFirstClick(bool p) { _PrimedForFirstClick = p; }

bool View2d::get_PrimedForSecondClick() { return _PrimedForSecondClick; }
void View2d::set_PrimedForSecondClick(bool p) { _PrimedForSecondClick = p; }

bool View2d::get_SecondClicksFinished() { return _SecondClicksFinished; }
void View2d::set_SecondClicksFinished(bool p) { _SecondClicksFinished = p; }

ITControlPoint *View2d::get_ScratchControlPoint() { return _ScratchControlPoint; }
void View2d::set_ScratchControlPoint(ITControlPoint *p) { _ScratchControlPoint = p; }

void View2d::set_plane(PLANE p)
{
	_plane = p;
}
