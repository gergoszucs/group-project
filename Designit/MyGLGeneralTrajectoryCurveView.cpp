#include "MyGLGeneralTrajectoryCurveView.h"
#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "UtililityFunctions.h"

int globalI = 1;

MyGLGeneralTrajectoryCurveView::MyGLGeneralTrajectoryCurveView(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

	set_EditValue(0.0);

	set_ViewHalfExtent(50.0);
	set_PanCentreX(40.0);
	set_PanCentreY(0.0);
}

void MyGLGeneralTrajectoryCurveView::keyPressEvent(QKeyEvent * event)
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
		set_PanCentreY(get_PanCentreY() + factor);
	}
	else if (event->key() == Qt::Key_Down)
	{
		set_PanCentreY(get_PanCentreY() - factor);
	}
	else if (event->key() == Qt::Key_Left)
	{
		set_PanCentreX(get_PanCentreX() + factor);
	}
	else if (event->key() == Qt::Key_Right)
	{
		set_PanCentreX(get_PanCentreX() - factor);
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
}

void MyGLGeneralTrajectoryCurveView::mouseMoveEvent(QMouseEvent *event)
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

	if (event->buttons() & Qt::LeftButton)
	{
		// Check for shift key press for zoom.
		if (event->modifiers() & Qt::ShiftModifier)
		{
			// Update zoom.
			float tmp = get_ViewHalfExtent() + factor * dy;

			if (tmp >= 0.0) set_ViewHalfExtent(tmp);
		}
		else if (!(event->modifiers())) // Just clicking without modifiers.
		{
			if (MY_EDIT_MODE == DRAG_TRAJECTORY_POINT)
			{
				float posX, posY, old_posX, old_posY;

				getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), get_PanCentreX(), get_PanCentreY(), get_ViewHalfExtent());
				getInAxesPosition(old_posX, old_posY, lastPos.x(), lastPos.y(), this->width(), this->height(), get_PanCentreX(), get_PanCentreY(), get_ViewHalfExtent());

				// Drag the Focus points
				dragFocusPoints(posX, posY, old_posX, old_posY);

				UnsavedChanges = true;
			}
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		set_PanCentreX(get_PanCentreX() - factor * dx);
		set_PanCentreY(get_PanCentreY() + factor * dy);
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	this->update();
}

void MyGLGeneralTrajectoryCurveView::wheelEvent(QWheelEvent *event)
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), get_PanCentreX(), get_PanCentreY(), get_ViewHalfExtent());

	float tmp = get_ViewHalfExtent() - factor * dy;

	if (tmp >= 0.0) set_ViewHalfExtent(tmp);

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), get_PanCentreX(), get_PanCentreY(), get_ViewHalfExtent());

	set_PanCentreX(get_PanCentreX() + tmpAx - tmpBx);
	set_PanCentreY(get_PanCentreY() + tmpAy - tmpBy);

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyGLGeneralTrajectoryCurveView::dragFocusPoints(float posX, float posY, float old_posX, float old_posY)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->size();

		if (n == 0)
		{
			continue;
		}

		int noOfSegments = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size();

		// Loop over focus end points.
		for (int t = 0; t < n; t++)
		{
			ITPointTrajectory *p;
			int index = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->at(t);
			if (get_MyEndOfSegment() == 1)
			{
				p = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(index)->get_P1_p();
			}
			else
			{
				p = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(index)->get_P0_p();
			}

			p->set_X(p->get_X() + posY - old_posY);

			// Check if we need to update the initial point of the next segment too.
			if ((index < noOfSegments - 1) && (get_MyEndOfSegment() == 1))
			{
				ITPointTrajectory *p0 = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(index + 1)->get_P0_p();
				p0->set_X(p0->get_X() + posY - old_posY);
			}

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "New point: %f", p->get_X());
		}

		// Update curve.
		project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->computeMySegmentEndTangentVectors();
	}
}

void MyGLGeneralTrajectoryCurveView::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
	// Empty all focus index vectors.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->size();
		for (int t = 0; t < n; t++)
		{
			project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->pop_back();
		}
	}

	MY_EDIT_MODE = NONE;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();

	// Update the spreadsheet.
	w->updateTrajectorySpreadsheet();

	// Update change flag.
	UnsavedChanges = true;
}

void MyGLGeneralTrajectoryCurveView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		set_EditValue(0.0);

		AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.

		MY_EDIT_MODE = DRAG_TRAJECTORY_POINT;
	}
	else
	{
		// Some modifier was pressed, so we need to take the "conn".
	}

}

void MyGLGeneralTrajectoryCurveView::AssignFocusPoint(QMouseEvent *event)
{
	float posX, posY, posZ, old_posX, old_posY;

	posZ = 0.0;

	getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), get_PanCentreX(), get_PanCentreY(), get_ViewHalfExtent());

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "3D point with POS: %f, %f, %f", posX, posY, posZ);

	// Find the vertex that is closest.
	int i, k;
	findTrajectoryPointIndicesNearMouse(posX, posY, posZ, &k, &i);

	if ((k > -1) && (i > -1))
	{
			// We have found a control point, so add it to the focus vector.
			project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->push_back(i);
	}

	// Redraw everything.
	updateGL();
}

void  MyGLGeneralTrajectoryCurveView::findTrajectoryPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI)
{
	float threshold = 1.0;
	float minDistance = 10000.0;
	*targetI = -1;
	*targetK = -1;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(i));

			ITPointTrajectory *p = currentSegment->get_P1_p();

			float deltaX = posX - currentSegment->get_EndKeyFrame();
			float deltaY = posY - p->get_X();

			float distanceSquared = deltaX*deltaX + deltaY*deltaY;

			if ((distanceSquared < threshold) && (distanceSquared < minDistance))
			{
				minDistance = distanceSquared;
				*targetK = k;
				*targetI = i;

				set_MyEndOfSegment(1);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "**********************************SUCCESS!!!!!");

				break;
			}

			// check for p0 of first segment as a special case.
			if (i == 0)
			{
				ITPointTrajectory *p = currentSegment->get_P0_p();

				float deltaX = posX - currentSegment->get_StartKeyFrame();
				float deltaY = posY - p->get_X();

				float distanceSquared = deltaX*deltaX + deltaY*deltaY;

				if ((distanceSquared < threshold) && (distanceSquared < minDistance))
				{
					minDistance = distanceSquared;
					*targetK = k;
					*targetI = i;

					set_MyEndOfSegment(0);

					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "**********************************SUCCESS. We've found the start point of the first segment!!!!!");

					break;
				}
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Indices of nearest trajectory point. k: %i, i: %i", *targetK, *targetI);
}

void MyGLGeneralTrajectoryCurveView::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(get_PanCentreX() - get_ViewHalfExtent(),
		get_PanCentreX() + get_ViewHalfExtent(),
		get_PanCentreY() - get_ViewHalfExtent() / aspect,
		get_PanCentreY() + get_ViewHalfExtent() / aspect,
		-50000.0,
		50000.0);


	glMatrixMode(GL_MODELVIEW);
}

void MyGLGeneralTrajectoryCurveView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyGLGeneralTrajectoryCurveView::paintGL()
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

void MyGLGeneralTrajectoryCurveView::initializeGL()
{
	qglClearColor(Qt::gray);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.

	// 20161215 New Stuff.
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_MULTISAMPLE);
}

void MyGLGeneralTrajectoryCurveView::draw()
{
	if (IsDataLoaded)
	{
		drawMyAxes();
		drawMyCurve();
		drawMyFocusSegments();
		drawMyCurveHandles(get_MyCurveIndex());
		drawMyInterpolatedCurve(get_MyCurveIndex());

		//drawMyControlPointsNet();
		//drawMyInterpolatedPointsNet();
		//drawMyFocusControlPoints();
		drawMyAnnotations();
	}
}

void MyGLGeneralTrajectoryCurveView::findOrdinateRange(float *minOrdinate, float *maxOrdinate)
{
	*maxOrdinate = -10000.0;
	*minOrdinate = 10000.0;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "No of curve segments: %i",
			project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size());

		// Loop though all the curve keyframe points to find the maximum and minimum X and Y values.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(i));

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

void MyGLGeneralTrajectoryCurveView::drawMyCurveHandles(int curveIndex)
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

void MyGLGeneralTrajectoryCurveView::drawMyCurve()
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

		ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(0));

		float plotX = currentSegment->get_StartKeyFrame();
		float plotY = currentSegment->get_P0_p()->get_X();

		glVertex3f(plotX, plotY, 0.0);

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			glVertex3f(plotX, plotY, 0.0);
		}
		glEnd();

		// Annotate the surface number.
		if (drawAnnotations)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->back());
			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, Plot x: %f, plot y: %f", k, plotX, plotY);

			renderText(plotX, plotY, 0.0, QString::number(k));
		}
	}

	// Now plot the dots.
	// Loop though all the surfaces.
	float radius = glXViewHalfExtent / 100.0;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			glTranslatef(plotX, plotY, 0.0);
			drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);
			glTranslatef(-plotX, -plotY, 0.0);
		}
	}
}

void MyGLGeneralTrajectoryCurveView::drawMyFocusSegments()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->size(); t++)
		{
			int index = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyFocusTrajectoryCurveSegmentIndices()->at(t);
			ITTrajectoryCurveSegment *s = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(index);

			if (get_MyEndOfSegment() == 1)
			{
				glTranslatef(s->get_EndKeyFrame(), s->get_P1_p()->get_X(), 0.0);

				float radius = glXViewHalfExtent / 50.0;
				drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

				glTranslatef(-s->get_EndKeyFrame(), -s->get_P1_p()->get_X(), 0.0);
			}
			else
			{
				glTranslatef(s->get_StartKeyFrame(), s->get_P0_p()->get_X(), 0.0);

				float radius = glXViewHalfExtent / 50.0;
				drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

				glTranslatef(-s->get_StartKeyFrame(), -s->get_P0_p()->get_X(), 0.0);
			}
		}
	}
}

void MyGLGeneralTrajectoryCurveView::drawMyAnnotations()
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

	renderText(0.0, maxOrdinate + 5.0, 0.0, QString(get_MyChar()).append(QString("-curve")));

	// Render the y-axis numbers.
	for (int n = 0; n <= 10; n++)
	{
		renderText(-5.0, minOrdinate + deltaOrdinate*n / 10.0, 0.0, QString::number(minOrdinate + deltaOrdinate*n / 10.0, 'f', 1));
	}

	// Plot the key frame digits.
	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		ITTrajectoryCurveSegment* currentSegment = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i);
		int currentEndKeyFrame = currentSegment->get_EndKeyFrame();

		renderText((float)currentEndKeyFrame, -5, 0.0, QString(" %1").arg(QString::number(currentEndKeyFrame)));
	}
}

void MyGLGeneralTrajectoryCurveView::drawMyAxes()
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

	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		glBegin(GL_LINE_STRIP);

		ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i));
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

	float plotY = (0.0 - minOrdinate)*10.0 / deltaOrdinate;

	glVertex3f(0.0, 0, 0.0);
	glVertex3f(project->get_MaxKeyFrame(), 0, 0.0);

	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);
}

void MyGLGeneralTrajectoryCurveView::drawMyInterpolatedCurve(int curveIndex)
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

void MyGLGeneralTrajectoryCurveView::resetMyView()
{
	set_ViewHalfExtent(50.0);
	set_PanCentreX(40.0);
	set_PanCentreY(0.0);
}

void MyGLGeneralTrajectoryCurveView::drawSphere(double r, int lats, int longs, float R, float GG, float B)
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
float MyGLGeneralTrajectoryCurveView::get_EditValue() { return _EditValue; }
void MyGLGeneralTrajectoryCurveView::set_EditValue(float a) { _EditValue = a; }

float MyGLGeneralTrajectoryCurveView::get_MaxY() { return _MaxY; }
void MyGLGeneralTrajectoryCurveView::set_MaxY(float y) { _MaxY = y; }

float MyGLGeneralTrajectoryCurveView::get_ViewHalfExtent() { return _ViewHalfExtent; }
void MyGLGeneralTrajectoryCurveView::set_ViewHalfExtent(float a) { _ViewHalfExtent = a; }

float MyGLGeneralTrajectoryCurveView::get_PanCentreX() { return _PanCentreX; }
void MyGLGeneralTrajectoryCurveView::set_PanCentreX(float a) { _PanCentreX = a; }

float MyGLGeneralTrajectoryCurveView::get_PanCentreY() { return _PanCentreY; }
void MyGLGeneralTrajectoryCurveView::set_PanCentreY(float a) { _PanCentreY = a; }

int MyGLGeneralTrajectoryCurveView::get_MyCurveIndex() { return _MyCurveIndex; }
void MyGLGeneralTrajectoryCurveView::set_MyCurveIndex(int a) { _MyCurveIndex = a; }

char MyGLGeneralTrajectoryCurveView::get_MyChar() { return _MyChar; }
void MyGLGeneralTrajectoryCurveView::set_MyChar(char c) { _MyChar = c; }

int MyGLGeneralTrajectoryCurveView::get_MyEndOfSegment() { return _MyEndOfSegment; }
void MyGLGeneralTrajectoryCurveView::set_MyEndOfSegment(int a) { _MyEndOfSegment = a; }