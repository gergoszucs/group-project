#include "MyXZView.h"
#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "UtililityFunctions.h"

MyXZView::MyXZView(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 270;
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
	ITControlPoint *p = new ITControlPoint(0.0, 0.0, 0.0);
	set_ScratchControlPoint(p);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);
}

MyXZView::~MyXZView(void)
{
	delete _ScratchControlPoint;
}

void MyXZView::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyXZView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	glRotatef((float)yRot, 0.0, 1.0, 0.0);
	glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
	glRotatef(0.0, 0.0, 0.0, 1.0);

	draw();
}

void MyXZView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyXZView::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(glXZPanCentreX - glXZViewHalfExtent,
		glXZPanCentreX + glXZViewHalfExtent,
		glXZPanCentreY - glXZViewHalfExtent / aspect,
		glXZPanCentreY + glXZViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyXZView::keyPressEvent(QKeyEvent * event)
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
		glXZPanCentreY = glXZPanCentreY + factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glXZPanCentreY = glXZPanCentreY - factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glXZPanCentreX = glXZPanCentreX - factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glXZPanCentreX = glXZPanCentreX + factor;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();

}

void MyXZView::draw()
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
		drawMyScratchControlPoint();
	}
}

void MyXZView::drawMyScratchControlPoint()
{
	if ((get_ScratchControlPoint()->get_K() > -1) || (get_ScratchControlPoint()->get_I() > -1) || (get_ScratchControlPoint()->get_J() > -1))
	{
		glTranslatef(get_ScratchControlPoint()->get_X(), 0.0, get_ScratchControlPoint()->get_Z());
		float radius = glXZViewHalfExtent / 50.0;
		drawSphere(radius, 15, 15, 0.0, 1.0, 0.0);
		glTranslatef(-get_ScratchControlPoint()->get_X(), 0.0, -get_ScratchControlPoint()->get_Z());
	}
}

void MyXZView::drawMyAnnotations()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				renderText(p->get_X(), p->get_Y(), p->get_Z(), QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(p->get_K()).arg(p->get_I()).arg(p->get_J()).arg(p->get_X()).arg(p->get_Y()).arg(p->get_Z()));
			}
		}
	}
}

void MyXZView::drawMyAxes()
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

void MyXZView::mouseReleaseEvent(QMouseEvent *event)
{
	if (MY_EDIT_MODE == PERSPECTIVE_ALL)
	{
		synchronizeBaseSurfaceCoordinates();
	}

	if (MY_EDIT_MODE == CENTRED_ROTATE)
	{
		if (get_SecondClicksFinished())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "_SecondClicksFinished is true so disabling priming flags.");

			synchronizeBaseSurfaceCoordinates();

			set_PrimedForFirstClick(false);
			set_PrimedForSecondClick(false);
			set_SecondClicksFinished(false);

			get_ScratchControlPoint()->set_I(-1);
			get_ScratchControlPoint()->set_J(-1);
			get_ScratchControlPoint()->set_K(-1);

			w->emptyFocusVectors();
		}
	}

	set_EditValue(0.0);

	w->updateAllTabs();

	// Update the spreadsheet.
	w->updateSpreadsheet();
}

void MyXZView::mousePressEvent(QMouseEvent *event)
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
		else if (MY_EDIT_MODE == ROTATE_ALL)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
		else if (MY_EDIT_MODE == CENTRED_ROTATE)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		}
	}
}

void MyXZView::AssignFocusPoint(QMouseEvent *event)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	const int x = event->x();
	const int y = viewport[3] - event->y();

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Here: %i, %i", x, y);

	GLdouble posX, posY, posZ;
	GLint result;
	result = gluUnProject(x, y, 0, modelview, projection, viewport, &posX, &posY, &posZ);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "3D point with POS: %f, %f, %f", posX, posY, posZ);

	// Find the vertex that is closest.
	int i, j, k;
	findControlPointIndicesNearMouse(posX, posY, posZ, &k, &i, &j);

	// Make sure lastPos is reset.
	lastPos = event->pos();

	// Make sure edit value is reset.
	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	if (MY_EDIT_MODE == CENTRED_ROTATE)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside CENTRED_ROTATE");
		// This edit mode is unique in that the centre of the rotation need not be a control point.
		if (get_PrimedForFirstClick())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside primed for first click");

			// Empty the focus vectors.
			w->emptyFocusVectors();

			// Save the centre of rotation.
			ITControlPoint *p = new ITControlPoint(posX, posY, posZ);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Centre of rotation scratch point captured. X: %f, Y: %f, Z: %f", p->get_X(), p->get_Y(), p->get_Z());

			get_ScratchControlPoint()->set_X(posX);
			get_ScratchControlPoint()->set_Y(0.0);
			get_ScratchControlPoint()->set_Z(posZ);

			get_ScratchControlPoint()->set_I(0);
			get_ScratchControlPoint()->set_J(0);
			get_ScratchControlPoint()->set_K(0);

			// Cancel primed for first click and prime for second click.
			set_PrimedForFirstClick(false);
			set_PrimedForSecondClick(true);
			set_SecondClicksFinished(false);
		}
		else if (get_PrimedForSecondClick())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Primed for second click is TRUE");

			if ((k > -1) && (i > -1) && (j > -1))
			{
				// User has clicked on a control point so push it onto the focus points.
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Focus point captured.");
			}
			else
			{
				// The user has finished clicking control points by clicking away from the surface, so do the rotation of the focus points about the scratch point.
				// Remember to delete the scratch point on mouse release.
				// Cancel primed for all clicks.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);
				set_SecondClicksFinished(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We've clicked away from the control points while primed for second click so set secondClicksFinished to TRUE.");
			}
		}
	}

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
		else if (MY_EDIT_MODE == ROTATE_ALL)
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
	}

	// Redraw everything.
	updateGL();

}

void  MyXZView::findControlPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI, int *targetJ)
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

				float distanceSquared = deltaX*deltaX + deltaZ*deltaZ;

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

void MyXZView::mouseMoveEvent(QMouseEvent *event)
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
			float tmp = glXZViewHalfExtent + factor * dy;

			if (tmp >= 0.0) glXZViewHalfExtent = tmp;
		}
		else if (!(event->modifiers())) // Just clicking without modifiers.
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

			w->statusBar()->showMessage(QString("X: %1, Z: %2").arg(posX).arg(posZ));

			if ((MY_EDIT_MODE == DRAG) || (MY_EDIT_MODE == DRAG_ROW) || (MY_EDIT_MODE == DRAG_COL) || (MY_EDIT_MODE == DRAG_ALL))
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Here: %f, %f, %f", posX, posY, posZ);

				// Drag the Focus points
				for (int k = 0; k < project->get_MySurfaces()->size(); k++)
				{
					// Loop over focus points vector and add dx and dy
					for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
					{
						ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);

						p->set_X(p->get_X() + posX - old_posX);
						p->set_Z(p->get_Z() + posZ - old_posZ);

					}

					// Update interpolated points.
					project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

				}

				UnsavedChanges = true;

				// Display the distance moved.
				set_EditValueX(get_EditValueX() + (posX - old_posX));
				set_EditValueY(get_EditValueY() + (posZ - old_posZ));
				w->setMyTextDataField(QString("Distance dragged x: %1, z: %2").arg(get_EditValueX()).arg(get_EditValueY()));
			}
			else if (MY_EDIT_MODE == ROTATE_ALL)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. ROTATE_ALL");

				float thetaOld = atan2(old_posZ, old_posX);
				float thetaNew = atan2(posZ, posX);
				float dTheta = thetaNew - thetaOld;

				// Rotate the Focus points around the first point in the focus vector. 
				for (int k = 0; k < project->get_MySurfaces()->size(); k++)
				{
					if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() == 0)
					{
						return;
					}

					ITPoint *basePoint = new ITPoint(project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_X(),
						project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Y(),
						project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Z());

					// Loop over focus points vector and add dx and dy
					for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
					{
						ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);
						float pxOld = p->get_X() - basePoint->get_X();
						float pyOld = p->get_Y() - basePoint->get_Y();
						float pzOld = p->get_Z() - basePoint->get_Z();

						float pxNew = pxOld*cos(dTheta) - pzOld*sin(dTheta);
						float pzNew = pxOld*sin(dTheta) + pzOld*cos(dTheta);

						pxNew = pxNew + basePoint->get_X();
						pzNew = pzNew + basePoint->get_Z();

						p->set_X(pxNew);
						p->set_Z(pzNew);

					}

					delete basePoint;

					// Update interpolated points.
					project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
				}

				UnsavedChanges = true;
			}
			else if (MY_EDIT_MODE == CENTRED_ROTATE)
			{
				if (get_SecondClicksFinished())
				{
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside mouseMoveEvent. CENTRED_ROTATE");

					float thetaOld = atan2(old_posZ - get_ScratchControlPoint()->get_Z(), old_posX - get_ScratchControlPoint()->get_X());
					float thetaNew = atan2(posZ - get_ScratchControlPoint()->get_Z(), posX - get_ScratchControlPoint()->get_X());
					float dTheta = thetaNew - thetaOld;

					set_EditValue(get_EditValue() + dTheta*180.0 / PI);

					// Rotate the Focus points around the first point in the focus vector. 
					centredRotateFocusPoints(dTheta);

					UnsavedChanges = true;
					w->setMyTextDataField(QString::number(get_EditValue()));
				}
			}
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		glXZPanCentreX -= factor * dx;
		glXZPanCentreY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();

}

void MyXZView::wheelEvent(QWheelEvent *event)
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glXZPanCentreX, glXZPanCentreY, glXZViewHalfExtent);

	float tmp = glXZViewHalfExtent - factor * dy;

	if (tmp >= 0.0) glXZViewHalfExtent = tmp;

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), glXZPanCentreX, glXZPanCentreY, glXZViewHalfExtent);

	glXZPanCentreX += tmpAx - tmpBx;
	glXZPanCentreY += tmpAy - tmpBy;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyXZView::drawMyControlPointsNet()
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

				glTranslatef(p->get_X(), 0.0, p->get_Z());

				float radius = glXZViewHalfExtent / 100.0;
				drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);

				glTranslatef(-p->get_X(), 0.0, -p->get_Z());
			}
		}
	}
}

void MyXZView::drawMyInterpolatedPointsNet()
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

void MyXZView::drawMyGrids()
{
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.5f, 0.5f, 0.5f); // grey

	// Draw horizontal lines.
	for (int i = -10; i < 11; i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f((float)i*10.0, 0.0, -100.0);
		glVertex3f((float)i*10.0, 0.0, 100.0);
		glEnd();
	}

	// Draw vertical lines.
	for (int j = -10; j < 11; j++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(-100.0, 0.0, (float)j*10.0);
		glVertex3f(100.0, 0.0, (float)j*10.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);
}

void MyXZView::drawMyFocusControlPoints()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); t++)
		{
			ITControlPoint *fp = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t);

			glTranslatef(fp->get_X(), 0.0, fp->get_Z());

			float radius = glXZViewHalfExtent / 50.0;
			drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

			if (t == 0)
			{
				glColor3f(0.0, 0.0, 0.0);
				renderText(radius, 0, radius, QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(fp->get_K()).arg(fp->get_I()).arg(fp->get_J()).arg(fp->get_X()).arg(fp->get_Y()).arg(fp->get_Z()));
			}

			glTranslatef(-fp->get_X(), 0.0, -fp->get_Z());
		}
	}
}

void MyXZView::drawSphere(double r, int lats, int longs, float R, float GG, float B)
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

void MyXZView::synchronizeBaseSurfaceCoordinates()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n > 0)
		{
			// Update Base points and recalculate.
			for (int i = 0; i < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint *p = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

					p->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				}
			}
		}

		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}
}

void MyXZView::centredRotateFocusPoints(float dTheta)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "centredRotateFocusPoints. Number of surfaces: %i", project->get_MySurfaces()->size());

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() == 0)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "No Focus points for surface: %i", k);
			continue;
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rotateFocusPoints. Focus points found for surface: %i", k);
		ITPoint *basePoint = new ITPoint(get_ScratchControlPoint()->get_X(), get_ScratchControlPoint()->get_Y(), get_ScratchControlPoint()->get_Z());

		// Loop over focus points vector and rotate each point.
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
		{
			ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);
			float pxOld = p->get_X() - basePoint->get_X();
			float pyOld = p->get_Y() - basePoint->get_Y();
			float pzOld = p->get_Z() - basePoint->get_Z();

			float pxNew = pxOld*cos(dTheta) - pzOld*sin(dTheta);
			float pzNew = pxOld*sin(dTheta) + pzOld*cos(dTheta);

			pxNew = pxNew + basePoint->get_X();
			pzNew = pzNew + basePoint->get_Z();

			p->set_X(pxNew);
			p->set_Z(pzNew);
		}

		delete basePoint;

		// Update interpolated points.
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}
}

// Accessors.
float MyXZView::get_EditValue() { return _EditValue; }
void MyXZView::set_EditValue(float a) { _EditValue = a; }

float MyXZView::get_EditValueX() { return _EditValueX; }
void MyXZView::set_EditValueX(float a) { _EditValueX = a; }

float MyXZView::get_EditValueY() { return _EditValueY; }
void MyXZView::set_EditValueY(float a) { _EditValueY = a; }

int MyXZView::get_Sense() { return _Sense; }
void MyXZView::set_Sense(int a) { _Sense = a; }

bool MyXZView::get_PrimedForFirstClick() { return _PrimedForFirstClick; }
void MyXZView::set_PrimedForFirstClick(bool p) { _PrimedForFirstClick = p; }

bool MyXZView::get_PrimedForSecondClick() { return _PrimedForSecondClick; }
void MyXZView::set_PrimedForSecondClick(bool p) { _PrimedForSecondClick = p; }

bool MyXZView::get_SecondClicksFinished() { return _SecondClicksFinished; }
void MyXZView::set_SecondClicksFinished(bool p) { _SecondClicksFinished = p; }

ITControlPoint *MyXZView::get_ScratchControlPoint() { return _ScratchControlPoint; }
void MyXZView::set_ScratchControlPoint(ITControlPoint *p) { _ScratchControlPoint = p; }