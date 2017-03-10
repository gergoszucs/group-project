#include "MyGLZTrajectoryView.h"
#include <QtOpenGL>
#include <GL/glu.h>
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"

MyGLZTrajectoryView::MyGLZTrajectoryView(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

	set_EditValue(0.0);
}

void MyGLZTrajectoryView::keyPressEvent(QKeyEvent * event)
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
		glZPanCentreY = glZPanCentreY + factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glZPanCentreY = glZPanCentreY - factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glZPanCentreX = glZPanCentreX - factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glZPanCentreX = glZPanCentreX + factor;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
}

void MyGLZTrajectoryView::mouseMoveEvent(QMouseEvent *event)
{
	// Check for shift key press for zoom.
	if (event->modifiers() & Qt::ShiftModifier)
	{
		if (MY_WIDGET_CONTROL == Z) // Only zoom if we have control.
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

			dx = factor * dx;
			dy = factor * dy;

			// Update zoom.
			glZViewHalfExtent = glZViewHalfExtent + dy;

			lastPos = event->pos();
		}
	}
	else if (!(event->modifiers())) // Just clicking without modifiers.
	{
		if (MY_EDIT_MODE == DRAG_TRAJECTORY_POINT)
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

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Previous mouse position: %i, %i", xold, yold);
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Current mouse position: %i, %i", x, y);

			GLdouble posX, posY, posZ;
			GLdouble old_posX, old_posY, old_posZ;
			GLint result;

			result = gluUnProject(xold, yold, 0, modelview, projection, viewport, &old_posX, &old_posY, &old_posZ);
			result = gluUnProject(x, y, 0, modelview, projection, viewport, &posX, &posY, &posZ);

			// Drag the Focus points
			dragFocusPoints(posX, posY, old_posX, old_posY);

			// Store the current mouse position, so we have it for the next mouse move event.
			lastPos = event->pos();

			UnsavedChanges = true;
		}
	} 

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	this->update();

	// updateGL();
	// Redraw other views.
	// w->updateAllTabs();
}

void MyGLZTrajectoryView::dragFocusPoints(float posX, float posY, float old_posX, float old_posY)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->size();

		if (n == 0)
		{
			continue;
		}

		int noOfSegments = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size();

		// Loop over focus end points.
		for (int t = 0; t < n; t++)
		{
			int index = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->at(t);
			ITPointTrajectory *p1 = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(index)->get_P1_p();

			p1->set_X(p1->get_X() + posY - old_posY);

			// Check if we need to update the initial point of the next segment too.
			// Only do this if we are not at the last segment.
			if (index < noOfSegments - 1)
			{
				ITPointTrajectory *p0 = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(index + 1)->get_P0_p();
				p0->set_X(p0->get_X() + posY - old_posY);
			}

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "New point: %f", p1->get_X());
		}

		// Update curve.
		project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->computeMySegmentEndTangentVectors();
	}
}

void MyGLZTrajectoryView::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
	if (MY_EDIT_MODE == DRAG_TRAJECTORY_POINT)
	{
		// Empty all focus index vectors.
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			int n = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->size();
			for (int t = 0; t < n; t++)
			{
				project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->pop_back();
			}
		}
	}

	// Release control.
	if (MY_WIDGET_CONTROL == Z)
	{
		// The semaphore is free, so we lock it.
		MY_WIDGET_CONTROL = NO;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();
}

void MyGLZTrajectoryView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		set_EditValue(0.0);

		if (MY_EDIT_MODE == DRAG_TRAJECTORY_POINT)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		} // End of if DRAG_TRAJECTORY_POINT
	}
	else
	{
		// Some modifier was pressed, so we need to take the "conn".
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Checking MY_WIDGET_CONTROL enum.");

		if (MY_WIDGET_CONTROL == NO)
		{
			// The semaphore is free, so we lock it.
			MY_WIDGET_CONTROL = Z;
		}
		else
		{
			// Someone else has control.
		}
	}
}

void MyGLZTrajectoryView::AssignFocusPoint(QMouseEvent *event)
{
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
	int i, k;
	findTrajectoryPointIndicesNearMouse(posX, posY, posZ, &k, &i);

	if ((k > -1) && (i > -1))
	{
		if (MY_EDIT_MODE == DRAG_TRAJECTORY_POINT)
		{
			// We have found a control point, so add it to the focus vector.
			project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->push_back(i);
		}
	}

	// Redraw everything.
	updateGL();
}

void  MyGLZTrajectoryView::findTrajectoryPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI)
{
	float threshold = 1.0;
	float minDistance = 10000.0;
	*targetI = -1;
	*targetK = -1;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i));

			ITPointTrajectory *p = currentSegment->get_P1_p();

			float deltaX = posX - currentSegment->get_EndKeyFrame();
			float deltaY = posY - p->get_X();

			float distanceSquared = deltaX*deltaX + deltaY*deltaY;

			if ((distanceSquared < threshold) && (distanceSquared < minDistance))
			{
				minDistance = distanceSquared;
				*targetK = k;
				*targetI = i;

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "**********************************SUCCESS!!!!!");
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Indices of nearest trajectory point. k: %i, i: %i", *targetK, *targetI);
}

void MyGLZTrajectoryView::setViewOrtho(int width, int height)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "MyGLZTrajectoryView::setViewOrtho");

	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(glZPanCentreX - glZViewHalfExtent,
		glZPanCentreX + glZViewHalfExtent,
		glZPanCentreY - glZViewHalfExtent / aspect,
		glZPanCentreY + glZViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyGLZTrajectoryView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyGLZTrajectoryView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	// 20160510: Ordering of these rotation has been changed to give more intuitive rotation in response to mouse movement.
	// Z vertical.  (Checked)
	glRotatef((float)xRot, 1.0, 0.0, 0.0);
	glRotatef(0.0, 0.0, 1.0, 0.0);
	glRotatef((float)yRot, 0.0, 0.0, 1.0);

	draw();
}

void MyGLZTrajectoryView::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyGLZTrajectoryView::draw()
{
	if (IsDataLoaded)
	{
		drawMyAxes();
		drawMyCurve();
		drawMyFocusSegments();
		drawMyCurveHandles(2);
		drawMyInterpolatedCurve(2);

		//drawMyControlPointsNet();
		//drawMyInterpolatedPointsNet();
		//drawMyFocusControlPoints();
		drawMyAnnotations();
	}
}

void MyGLZTrajectoryView::findOrdinateRange(float *minOrdinate, float *maxOrdinate)
{
	*maxOrdinate = -10000.0;
	*minOrdinate = 10000.0;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "No of curve segments: %i", project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size());

		// Loop though all the curve keyframe points to find the maximum and minimum X and Y values.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i));

			if (currentSegment->get_P0_p()->get_X() > *maxOrdinate)
			{
				*maxOrdinate = currentSegment->get_P0_p()->get_X();
			}
			if (currentSegment->get_P1_p()->get_X() > *maxOrdinate)
			{
				*maxOrdinate = currentSegment->get_P1_p()->get_X();
			}

			if (currentSegment->get_P0_p()->get_X() < *minOrdinate)
			{
				*minOrdinate = currentSegment->get_P0_p()->get_X();
			}
			if (currentSegment->get_P1_p()->get_X() < *minOrdinate)
			{
				*minOrdinate = currentSegment->get_P1_p()->get_X();
			}
		}
	}
}

void MyGLZTrajectoryView::drawMyCurveHandles(int curveIndex)
{
	float minOrdinate;
	float maxOrdinate;
	findOrdinateRange(&minOrdinate, &maxOrdinate);

	float deltaOrdinate = maxOrdinate - minOrdinate;
	int deltaKey = project->get_MaxKeyFrame();

	float offset = 0.1;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Plot the segment end tangents.
		glColor3f(0.0, 1.0, 1.0);

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(i));

			glBegin(GL_LINE_STRIP);
			float plotX = currentSegment->get_StartKeyFrame();
			float plotY = offset + currentSegment->get_P0_p()->get_X();
			glVertex3f(plotX, plotY, 0.0);
			float plotXt = currentSegment->get_StartKeyFrame() + currentSegment->get_m0_p()->get_X();
			float plotYt = offset + currentSegment->get_P0_p()->get_X() + currentSegment->get_m0_p()->get_Y();
			glVertex3f(plotXt, plotYt, 0.0);
			glEnd();
		}

		glColor3f(0.0, 0.0, 0.0);
	}
}

void MyGLZTrajectoryView::drawMyCurve()
{
	float minOrdinate;
	float maxOrdinate;
	findOrdinateRange(&minOrdinate, &maxOrdinate);

	// We have now established the vertical bounds of the plot.
	float deltaOrdinate = maxOrdinate - minOrdinate;
	int deltaKey = project->get_MaxKeyFrame();

	// Now plot the curves.
	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Plot the curve.
		glBegin(GL_LINE_STRIP);

		ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(0));

		float plotX = currentSegment->get_StartKeyFrame();
		float plotY = currentSegment->get_P0_p()->get_X();

		glVertex3f(plotX, plotY, 0.0);

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			glVertex3f(plotX, plotY, 0.0);
		}
		glEnd();

		// Annotate the surface number.
		if (drawAnnotations)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->back());
			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, Plot x: %f, plot y: %f", k, plotX, plotY);

			renderText(plotX, plotY, 0.0, QString::number(k));
		}
	}

	// Now plot the dots.
	// Loop though all the surfaces.
	float radius = glZViewHalfExtent / 100.0;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			glTranslatef(plotX, plotY, 0.0);
			drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);
			glTranslatef(-plotX, -plotY, 0.0);
		}
	}
}

void MyGLZTrajectoryView::drawMyFocusSegments()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->size(); t++)
		{
			int index = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyFocusTrajectoryCurveSegmentIndices()->at(t);
			ITTrajectoryCurveSegment *s = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(index);

			glTranslatef(s->get_EndKeyFrame(), s->get_P1_p()->get_X(), 0.0);

			float radius = glZViewHalfExtent / 50.0;
			drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

			//if (t == 0) // Only dsraw indices for the first point.
			//{
			//	glColor3f(0.0, 0.0, 0.0);
			//	renderText(radius, 0, radius, QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(fp->get_K()).arg(fp->get_I()).arg(fp->get_J()).arg(fp->get_X()).arg(fp->get_Y()).arg(fp->get_Z()));
			//}

			glTranslatef(-s->get_EndKeyFrame(), -s->get_P1_p()->get_X(), 0.0);
		}
	}
}

void MyGLZTrajectoryView::drawMyAnnotations()
{
	float aspect = (float)myWidth / (float)myHeight; // Landscape is greater than 1.

	// Draw each surface.
	glColor3f(0.0f, 0.0f, 0.0f); // Black

	float minOrdinate;
	float maxOrdinate;
	findOrdinateRange(&minOrdinate, &maxOrdinate);

	// We have now established the vertical bounds of the plot.
	float deltaOrdinate = maxOrdinate - minOrdinate;
	int deltaKey = project->get_MaxKeyFrame();

	renderText(0.0, maxOrdinate + 5.0, 0.0, "Z-translation");

	// Render the y-axis numbers.
	for (int n = 0; n <= 10; n++)
	{
		renderText(-5.0, minOrdinate + deltaOrdinate*n / 10.0, 0.0, QString::number(minOrdinate + deltaOrdinate*n / 10.0, 'f', 1));
	}

	// Plot the key frame digits.
	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		ITTrajectoryCurveSegment* currentSegment = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i);

		int currentEndKeyFrame = currentSegment->get_EndKeyFrame();

		renderText((float)currentEndKeyFrame, -5, 0.0, QString(" %1").arg(QString::number(currentEndKeyFrame)));
	}
}

void MyGLZTrajectoryView::drawMyAxes()
{
	float minOrdinate;
	float maxOrdinate;
	findOrdinateRange(&minOrdinate, &maxOrdinate);

	// We have now established the vertical bounds of the plot.
	float deltaOrdinate = maxOrdinate - minOrdinate;
	int deltaKey = project->get_MaxKeyFrame();

	// Draw X axis.
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f((float)(project->get_MaxKeyFrame()), 0.0f, 0.0f);
	glEnd();

	// Draw Y axis.
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, minOrdinate, 0.0f);
	glVertex3f(0.0f, maxOrdinate, 0.0f);
	glEnd();

	float aspect = (float)myWidth / (float)myHeight; // When window is Landscape then aspect is greater than 1.
	glColor3f(0.0f, 0.0f, 0.0f); // Black
	renderText((float)(project->get_MaxKeyFrame()), 0.0, 0.0, QString("Frames"));

	// Draw the vertical key frame lines.
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0, 0.0, 0.0);

	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		glBegin(GL_LINE_STRIP);

		ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(i));
		float plotX = currentSegment->get_EndKeyFrame();

		glVertex3f(plotX, minOrdinate, 0.0);
		glVertex3f(plotX, maxOrdinate, 0.0);

		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);

	// Draw the line y=0.
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);

	glVertex3f(0.0, 0, 0.0);
	glVertex3f(project->get_MaxKeyFrame(), 0, 0.0);

	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);
}

void MyGLZTrajectoryView::drawMyInterpolatedCurve(int curveIndex)
{
	float minOrdinate;
	float maxOrdinate;
	findOrdinateRange(&minOrdinate, &maxOrdinate);

	// We have now established the vertical bounds of the plot.
	float deltaOrdinate = maxOrdinate - minOrdinate;
	int deltaKey = project->get_MaxKeyFrame();

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		ITPoint* translationPoint = new ITPoint();
		ITPoint* rotationPoint = new ITPoint();

		// Plot the curve.
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < project->get_MaxKeyFrame(); i++)
		{
			project->get_MySurfaces()->at(k)->computeTrajectoryPointsAtFrame(i, k, translationPoint, rotationPoint);

			float plotX = i;

			float plotYa = 0.0;

			if (curveIndex == 0)
			{
				plotYa = translationPoint->get_X();
			}
			else if (curveIndex == 1)
			{
				plotYa = translationPoint->get_Y();
			}
			else if (curveIndex == 2)
			{
				plotYa = translationPoint->get_Z();
			}
			else if (curveIndex == 3)
			{
				plotYa = rotationPoint->get_X();
			}
			else if (curveIndex == 4)
			{
				plotYa = rotationPoint->get_Y();
			}
			else if (curveIndex == 5)
			{
				plotYa = rotationPoint->get_Z();
			}

			float plotY = plotYa;

			glVertex3f(plotX, plotY, 0.0);
		}
		glEnd();
		glColor3f(0.0, 0.0, 0.0);

		delete translationPoint;
		delete rotationPoint;
	}
}

void MyGLZTrajectoryView::drawSphere(double r, int lats, int longs, float R, float GG, float B)
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

void MyGLZTrajectoryView::resetMyView()
{
	glZViewHalfExtent = 50.0;
	glZPanCentreX = 40.0;
	glZPanCentreY = 0.0;
}

// Accessors.
float MyGLZTrajectoryView::get_EditValue() { return _EditValue; }
void MyGLZTrajectoryView::set_EditValue(float a) { _EditValue = a; }

float MyGLZTrajectoryView::get_MaxY() { return _MaxY; }
void MyGLZTrajectoryView::set_MaxY(float y) { _MaxY = y; }