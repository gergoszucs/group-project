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

	setMouseTracking(true);
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
		eyeY += factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		eyeY -= factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		eyeX += factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		eyeX -= factor;
	}

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
		if (!(event->modifiers())) // Just clicking without modifiers.
		{
			if ((MY_EDIT_MODE == DRAG_TRAJECTORY_POINT) && ((surfaceID > -1) && (pointID > -1)))
			{
				float posX, posY, old_posX, old_posY;

				getAxesPos(posX, posY, event->x(), event->y());
				getAxesPos(old_posX, old_posY, lastPos.x(), lastPos.y());

				// Drag the Focus points
				project->moveTrajectoryPoint(surfaceID, get_MyCurveIndex(), pointID + 1, posY - old_posY, w->getSync());

				_EditValue += posY - old_posY;

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
	else
	{
		int k, i;
		float posX, posY;
		getAxesPos(posX, posY, event->x(), event->y());
		findTrajectoryPointIndicesNearMouse(posX, posY, 0.0, &k, &i);

		if ((k > -1) && (i > -1))
		{
			surfaceID_light = k;
			pointID_light = i;
		}
		else
		{
			surfaceID_light = -1;
			pointID_light = -1;
		}
	}

	lastPos = event->pos();

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

	getAxesPos(tmpAx, tmpAy, event->x(), event->y());

	float tmp = eyeZoom + 0.01 * factor * dy;

	if (tmp >= 0.0) eyeZoom = tmp;
	paintGL();

	getAxesPos(tmpBx, tmpBy, event->x(), event->y());

	eyeX += tmpAx - tmpBx;
	eyeY += tmpAy - tmpBy;

	// Redraw everything.
	update();
}

void MyGLGeneralTrajectoryCurveView::mouseReleaseEvent(QMouseEvent *releaseEvent)
{
	if (_EditValue != 0)
	{
		QStringList command, revCommand;

		command.push_back("moveTrajectoryPoint");
		command.push_back(QString::number(surfaceID));
		command.push_back(QString::number(get_MyCurveIndex()));
		command.push_back(QString::number(pointID + 1));
		command.push_back(QString::number(_EditValue));

		revCommand.push_back("moveTrajectoryPoint");
		revCommand.push_back(QString::number(surfaceID));
		revCommand.push_back(QString::number(get_MyCurveIndex()));
		revCommand.push_back(QString::number(pointID + 1));
		revCommand.push_back(QString::number(-_EditValue));

		if (w->getSync())
		{
			command.push_back(QString::number(1));
			revCommand.push_back(QString::number(1));
		}
		else
		{
			command.push_back(QString::number(0));
			revCommand.push_back(QString::number(0));
		}

		w->undoRedo.registerCommand(command, revCommand);
	}

	surfaceID = -1;
	pointID = -1;

	MY_EDIT_MODE = NONE;

	// Redraw everything.
	update();

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

		float posX, posY, posZ, old_posX, old_posY;

		posZ = 0.0;

		getAxesPos(posX, posY, event->x(), event->y());

		// Find the vertex that is closest.
		int k, i;
		findTrajectoryPointIndicesNearMouse(posX, posY, posZ, &k, &i);

		if ((k > -1) && (i > -1))
		{
			surfaceID = k;
			pointID = i;
		}
		else
		{
			surfaceID = -1;
			pointID = -1;
		}

		// Redraw everything.
		updateGL();

		MY_EDIT_MODE = DRAG_TRAJECTORY_POINT;
	}
}

void MyGLGeneralTrajectoryCurveView::findTrajectoryPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI)
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

void MyGLGeneralTrajectoryCurveView::resizeGL(int width, int height)
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

void MyGLGeneralTrajectoryCurveView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-eyeX*eyeZoom, -eyeY*eyeZoom, 0.0);
	glScalef(eyeZoom, eyeZoom, eyeZoom);

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

	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_MULTISAMPLE);
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

		// Draw focused point
		if ((surfaceID > -1) && (pointID > -1))
		{
			glTranslatef(project->getTrajectorySegment(surfaceID, get_MyCurveIndex(), pointID)->get_EndKeyFrame(), project->getTrajectoryPoint(surfaceID, get_MyCurveIndex(), pointID + 1)->get_X(), 0.0);
			drawSphere(1.5, 15, 15, 1.0, 0.0, 0.0);
			glTranslatef(-project->getTrajectorySegment(surfaceID, get_MyCurveIndex(), pointID)->get_EndKeyFrame(), -project->getTrajectoryPoint(surfaceID, get_MyCurveIndex(), pointID + 1)->get_X(), 0.0);
		}

		if ((surfaceID_light > -1) && (pointID_light > -1))
		{
			glTranslatef(project->getTrajectorySegment(surfaceID_light, get_MyCurveIndex(), pointID_light)->get_EndKeyFrame(), project->getTrajectoryPoint(surfaceID_light, get_MyCurveIndex(), pointID_light + 1)->get_X(), 0.0);
			drawSphere(1.5, 15, 15, 0.7, 0.0, 0.0);
			glTranslatef(-project->getTrajectorySegment(surfaceID_light, get_MyCurveIndex(), pointID_light)->get_EndKeyFrame(), -project->getTrajectoryPoint(surfaceID_light, get_MyCurveIndex(), pointID_light + 1)->get_X(), 0.0);
		}

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

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(get_MyCurveIndex())->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = currentSegment->get_EndKeyFrame();
			float plotY = currentSegment->get_P1_p()->get_X();

			glTranslatef(plotX, plotY, 0.0);
			drawSphere(1.0, 15, 15, 0.0, 0.0, 0.0);
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
				drawSphere(1.0, 15, 15, 1.0, 0.0, 0.0);
				glTranslatef(-s->get_EndKeyFrame(), -s->get_P1_p()->get_X(), 0.0);
			}
			else
			{
				glTranslatef(s->get_StartKeyFrame(), s->get_P0_p()->get_X(), 0.0);
				drawSphere(1.0, 15, 15, 1.0, 0.0, 0.0);
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
	eyeX = 40.0;
	eyeY = 0.0;
	eyeZoom = 1.0;

	update();
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

void MyGLGeneralTrajectoryCurveView::getAxesPos(float & pX, float & pY, const int x, const int y)
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

	pX = posX;
	pY = posY;
}

void MyGLGeneralTrajectoryCurveView::setSceneParameters(float eyeX, float eyeY, float eyeZoom)
{
	this->eyeX = eyeX;
	this->eyeY = eyeY;
	if (eyeZoom > 0) this->eyeZoom = eyeZoom;
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