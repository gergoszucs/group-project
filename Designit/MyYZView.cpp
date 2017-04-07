#include "MyYZView.h"
#include <QtOpenGL>
#include <GL/glu.h>
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "UtililityFunctions.h"

MyYZView::MyYZView(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 270;
	yRot = 270;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	set_PrimedForFirstClick(false);
	set_PrimedForSecondClick(false);
}

void MyYZView::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyYZView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	glRotatef((float)yRot, 0.0, 1.0, 0.0);
	glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
	glRotatef(0.0, 0.0, 0.0, 1.0);

	draw();
}

void MyYZView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyYZView::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(glYZPanCentreX - glYZViewHalfExtent,
		glYZPanCentreX + glYZViewHalfExtent,
		glYZPanCentreY - glYZViewHalfExtent / aspect,
		glYZPanCentreY + glYZViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyYZView::keyPressEvent(QKeyEvent * event)
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
		glYZPanCentreY = glYZPanCentreY + factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glYZPanCentreY = glYZPanCentreY - factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glYZPanCentreX = glYZPanCentreX - factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glYZPanCentreX = glYZPanCentreX + factor;
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

void MyYZView::draw()
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
			drawMyAnnotations();
		}

		if (drawGrids)
		{
			drawMyGrids();
		}

		drawMyFocusControlPoints();
	}
}

void MyYZView::drawMyAxes()
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

void MyYZView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		if (MY_EDIT_MODE == DRAG)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
		else if (MY_EDIT_MODE == DRAG_ROW)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
		else if (MY_EDIT_MODE == DRAG_COL)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
		else if (MY_EDIT_MODE == DRAG_ALL)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
		else if (MY_EDIT_MODE == MEASURE_DISTANCE)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
	}
}

void MyYZView::AssignFocusPoint(QMouseEvent *event)
{
	// Empty focus point vectors
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int N = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();
		for (int n = 0; n < N; n++)
		{
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->pop_back();
		}
	}

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	const int x = event->x();
	const int y = viewport[3] - event->y();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Here: %i, %i", x, y);

	GLdouble posX, posY, posZ;
	GLint result;
	result = gluUnProject(x, y, 0, modelview, projection, viewport, &posX, &posY, &posZ);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "3D point with POS: %f, %f, %f", posX, posY, posZ);

	// Find the vertex that is closest.
	int i, j, k;
	findControlPointIndicesNearMouse(posX, posY, posZ, &k, &i, &j);

	// Make sure lastPos is reset.
	lastPos = event->pos();

	// Make sure edit value is reset.
	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	if ((k > -1) && (i > -1) && (j > -1))
	{
		if (MY_EDIT_MODE == DRAG)
		{
			// We have found a control point, so add it to the focus vector.
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
		}
		else if (MY_EDIT_MODE == DRAG_ROW)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
			}
		}
		else if (MY_EDIT_MODE == DRAG_COL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
			}
		}
		else if (MY_EDIT_MODE == DRAG_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
		}
		else if (MY_EDIT_MODE == MEASURE_DISTANCE)
		{
			if (get_PrimedForFirstClick())
			{
				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Save the first point.
				set_ScratchControlPoint(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));

				// Cancel primed for first click and prime for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First point has been captured.");
			}
			else if (get_PrimedForSecondClick())
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second point has been captured.");

				// Get second point and move it to the first one.
				ITControlPoint *firstPoint = get_ScratchControlPoint();
				ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				// Find the distance.
				float dist = firstPoint->distanceFrom(secondPoint);

				float dx = secondPoint->get_X() - firstPoint->get_X();
				float dy = secondPoint->get_Y() - firstPoint->get_Y();
				float dz = secondPoint->get_Z() - firstPoint->get_Z();

				// Display it in the ui field.
				w->setMyTextDataField(QString("Total: %1, dx %2, dy: %3, dz: %4 ").arg(dist).arg(dx).arg(dy).arg(dz));

				// Cancel primed for first click and cancel primed for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);

				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();
			}
		}
	}

	// Redraw everything.
	updateGL();
}

void  MyYZView::findControlPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI, int *targetJ)
{
	float threshold = 0.1;
	float minDistance = 10000.0;
	*targetI = -1;
	*targetJ = -1;
	*targetK = -1;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *currentPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				float deltaX = posX - currentPoint->get_X();
				float deltaY = posY - currentPoint->get_Y();
				float deltaZ = posZ - currentPoint->get_Z();

				float distanceSquared = deltaY*deltaY + deltaZ*deltaZ;

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

void MyYZView::mouseMoveEvent(QMouseEvent *event)
{
	// The shift key was pressed, so zoom.
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

	//check for left mouse button events
	if (event->buttons() & Qt::LeftButton)
	{
		// Check for shift key press for zoom.
		if (event->modifiers() & Qt::ShiftModifier)
		{
			// Update zoom.
			float tmp = glYZViewHalfExtent + factor * dy;

			if (tmp >= 0.0) glYZViewHalfExtent = tmp;
		}
		else if (!(event->modifiers())) // Just clicking without modifiers.
		{
			if ((MY_EDIT_MODE == DRAG) || (MY_EDIT_MODE == DRAG_ROW) || (MY_EDIT_MODE == DRAG_COL) || (MY_EDIT_MODE == DRAG_ALL))
			{
				GLint viewport[4];
				GLdouble modelview[16];
				GLdouble projection[16];
				glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
				glGetDoublev(GL_PROJECTION_MATRIX, projection);
				glGetIntegerv(GL_VIEWPORT, viewport);

				const int x = event->x();
				const int y = viewport[3] - event->y();

				const int xold = lastPos.x();
				const int yold = viewport[3] - lastPos.y();

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Here: %i, %i", x, y);

				GLdouble posX, posY, posZ;
				GLdouble old_posX, old_posY, old_posZ;
				GLint result;

				result = gluUnProject(xold, yold, 0, modelview, projection, viewport, &old_posX, &old_posY, &old_posZ);
				result = gluUnProject(x, y, 0, modelview, projection, viewport, &posX, &posY, &posZ);

				// Drag the Focus points
				dragFocusPoints(posY, posZ, old_posY, old_posZ);

				lastPos = event->pos();

				UnsavedChanges = true;

				// Display the distance moved.
				set_EditValueX(get_EditValueX() + (posY - old_posY));
				set_EditValueY(get_EditValueY() + (posZ - old_posZ));
				w->setMyTextDataField(QString("Distance dragged y: %1, z: %2").arg(get_EditValueX()).arg(get_EditValueY()));
			}
		}

	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		glYZPanCentreX -= factor * dx;
		glYZPanCentreY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();
}

void MyYZView::wheelEvent(QWheelEvent *event)
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glYZPanCentreX, glYZPanCentreY, glYZViewHalfExtent);

	float tmp = glYZViewHalfExtent - factor * dy;

	if (tmp >= 0.0) glYZViewHalfExtent = tmp;

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), glYZPanCentreX, glYZPanCentreY, glYZViewHalfExtent);

	glYZPanCentreX += tmpAx - tmpBx;
	glYZPanCentreY += tmpAy - tmpBy;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyYZView::dragFocusPoints(float posY, float posZ, float old_posY, float old_posZ)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n == 0)
		{
			continue;
		}

		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() > 0)
		{
			// Loop over focus points vector and add dx and dy
			for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);

				if (IsVerticalDragOnly)
				{
					p->set_Z(p->get_Z() + (posZ - old_posZ));
				}
				else if (IsHorizontalDragOnly)
				{
					p->set_Y(p->get_Y() + (posY - old_posY));
				}
				else
				{
					p->set_Y(p->get_Y() + (posY - old_posY));
					p->set_Z(p->get_Z() + (posZ - old_posZ));
				}
			}

			// Update interpolated points.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		}
	}
}

void MyYZView::drawMyControlPointsNet()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	// Curves of constant u.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Do the plotting.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);

			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());
			}

			glEnd();
		}

		// Curves of constant v.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size(); i++)
		{
			glBegin(GL_LINE_STRIP);

			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_Z());
			}

			glEnd();
		}
	}

	// Now draw spheres.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				glTranslatef(0.0, p->get_Y(), p->get_Z());

				float radius = glYZViewHalfExtent / 100.0;
				drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);

				glTranslatef(0.0, -p->get_Y(), -p->get_Z());
			}
		}
	}
}

void MyYZView::drawMyInterpolatedPointsNet()
{
	// Set the colour to blue.
	glColor3f(0.0f, 0.0f, 1.0f);

	// Draw each surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// First the curves of constant u
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
		// Next the curves of constant v
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

void MyYZView::drawMyGrids()
{
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.5f, 0.5f, 0.5f); // grey

	// Draw horizontal lines.
	for (int i = -10; i < 11; i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)i*10.0, -100.0);
		glVertex3f(0.0, (float)i*10.0, 100.0);
		glEnd();
	}

	// Draw vertical lines.
	for (int j = -10; j < 11; j++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, -100.0, (float)j*10.0);
		glVertex3f(0.0, 100.0, (float)j*10.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);
}

void MyYZView::drawMyAnnotations()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Do the plotting.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				renderText(p->get_X(), p->get_Y(), p->get_Z(), 
					QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(p->get_K()).arg(p->get_I()).arg(p->get_J()).arg(p->get_X()).arg(p->get_Y()).arg(p->get_Z()));
			}
		}
	}
}

void MyYZView::drawMyFocusControlPoints()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); t++)
		{
			ITControlPoint *fp = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t);

			glTranslatef(0.0, fp->get_Y(), fp->get_Z());

			float radius = glYZViewHalfExtent / 50.0;
			drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

			glColor3f(0.0, 0.0, 0.0);
			renderText(radius, 0, radius, 
				QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(fp->get_K()).arg(fp->get_I()).arg(fp->get_J()).arg(fp->get_X()).arg(fp->get_Y()).arg(fp->get_Z()));

			glTranslatef(0.0, -fp->get_Y(), -fp->get_Z());
		}
	}
}

void MyYZView::drawSphere(double r, int lats, int longs, float R, float GG, float B)
{
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
float MyYZView::get_EditValue() { return _EditValue; }
void MyYZView::set_EditValue(float a) { _EditValue = a; }

float MyYZView::get_EditValueX() { return _EditValueX; }
void MyYZView::set_EditValueX(float a) { _EditValueX = a; }

float MyYZView::get_EditValueY() { return _EditValueY; }
void MyYZView::set_EditValueY(float a) { _EditValueY = a; }

bool MyYZView::get_PrimedForFirstClick() { return _PrimedForFirstClick; }
void MyYZView::set_PrimedForFirstClick(bool p) { _PrimedForFirstClick = p; }

bool MyYZView::get_PrimedForSecondClick() { return _PrimedForSecondClick; }
void MyYZView::set_PrimedForSecondClick(bool p) { _PrimedForSecondClick = p; }

ITControlPoint *MyYZView::get_ScratchControlPoint() { return _ScratchControlPoint; }
void MyYZView::set_ScratchControlPoint(ITControlPoint *p) { _ScratchControlPoint = p; }