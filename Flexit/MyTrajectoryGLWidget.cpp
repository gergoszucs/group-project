#include "MyTrajectoryGLWidget.h"
#include <QtOpenGL>
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"

MyTrajectoryGLWidget::MyTrajectoryGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{}

void MyTrajectoryGLWidget::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyTrajectoryGLWidget::setViewOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 40, 0, 23, -150.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

void MyTrajectoryGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	if (IsDataLoaded)
	{
		plot2D();
	}
}

void MyTrajectoryGLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport(0, 0, width, height);

	setViewOrtho();

	if (IsDataLoaded)
	{
		plot2D();
	}
}


void MyTrajectoryGLWidget::plot2D()
{
	glPushMatrix();
	glTranslatef(1.0, 1.0, 1.0);
	drawMy2DAxes("x");
	drawMyCurves(0);
	drawMyCurveHandles(0);
	drawMyInterpolatedCurves(0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(14.0, 1.0, 1.0);
	drawMy2DAxes("y");
	drawMyCurves(1);
	drawMyCurveHandles(1);
	drawMyInterpolatedCurves(1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(28.0, 1.0, 1.0);
	drawMy2DAxes("z");
	drawMyCurves(2);
	drawMyCurveHandles(2);
	drawMyInterpolatedCurves(2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.0, 12.0, 1.0);
	drawMy2DAxes("roll");
	drawMyCurves(3);
	drawMyCurveHandles(3);
	drawMyInterpolatedCurves(3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(14.0, 12.0, 1.0);
	drawMy2DAxes("pitch");
	drawMyCurves(4);
	drawMyCurveHandles(4);
	drawMyInterpolatedCurves(4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(28.0, 12.0, 1.0);
	drawMy2DAxes("yaw");
	drawMyCurves(5);
	drawMyCurveHandles(5);
	drawMyInterpolatedCurves(5);
	glPopMatrix();
}


void MyTrajectoryGLWidget::drawMy2DAxes(char *yLabel)
{
	glColor3f(0.0f, 0.0f, 0.0f);

	// Draw X axis.
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glEnd();

	// Draw Y axis.
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glEnd();

	// Draw the axis titles.
	renderText(10.0, 0.0, 0.0, QString("Frame"));
	renderText(5.0, 10.0, 0.0, QString(yLabel));
}


void MyTrajectoryGLWidget::drawMyCurves(int curveIndex)
{
	int maxKey = -10000;
	float maxX = -10000.0;

	int minKey = 0; // The minimum key frame is always 0.
	float minX = 10000.0;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Loop though all the curve keyframe points to find the maximum and minimum X and Y values.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(i));

			if (currentSegment->get_EndKeyFrame() > maxKey)
			{
				maxKey = currentSegment->get_EndKeyFrame();
			}
			if (currentSegment->get_P0_p()->get_X() > maxX)
			{
				maxX = currentSegment->get_P0_p()->get_X();
			}
			if (currentSegment->get_P1_p()->get_X() > maxX)
			{
				maxX = currentSegment->get_P1_p()->get_X();
			}

			if (currentSegment->get_P0_p()->get_X() < minX)
			{
				minX = currentSegment->get_P0_p()->get_X();
			}
			if (currentSegment->get_P1_p()->get_X() < minX)
			{
				minX = currentSegment->get_P1_p()->get_X();
			}
		}
	}
	
	// We have now established the bounds of the plot.
	float deltaX = maxX - minX;
	int deltaKey = maxKey - minKey;

	// Render the y-axis numbers.
	for (int n = 0; n < 10; n++)
	{
		renderText(-0.6, (float)n, 0.0, QString::number(minX + deltaX*n / 10.0, 'f', 1));
	}

	// Plot the key frame digits.
	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		ITTrajectoryCurveSegment* currentSegment = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i);

		int currentEndKeyFrame = currentSegment->get_EndKeyFrame();
		float plotX = (currentEndKeyFrame - minKey)*10.0 / deltaKey;

		renderText(plotX, -0.5, 0.0, QString(" %1").arg(QString::number(currentEndKeyFrame)));
	}

	// Now plot the curves.
	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Plot the curve.
		glBegin(GL_LINE_STRIP);

		ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(0));

		float plotX = (currentSegment->get_StartKeyFrame() - minKey)*10.0 / deltaKey;
		float plotY = (currentSegment->get_P0_p()->get_X() - minX)*10.0 / deltaX;

		glVertex3f(plotX, plotY, 0.0);

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(i));

			float plotX = (currentSegment->get_EndKeyFrame() - minKey)*10.0 / deltaKey;
			float plotY = (currentSegment->get_P1_p()->get_X() - minX)*10.0 / deltaX;

			glVertex3f(plotX, plotY, 0.0);

		}
		glEnd();

		// Annotate.
		if (drawAnnotations)
		{
			ITTrajectoryCurveSegment *currentSegment = (ITTrajectoryCurveSegment *)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->back());
			float plotX = (currentSegment->get_EndKeyFrame() - minKey)*10.0 / deltaKey;
			float plotY = (currentSegment->get_P1_p()->get_X() - minX)*10.0 / deltaX;

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "k: %i, Plot x: %f, plot y: %f", k, plotX, plotY);

			renderText(plotX, plotY, 0.0, QString::number(k));
		}
	}

	// Draw the vertical key frame lines.
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0, 0.0, 0.0);

	for (int i = 0; i < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{
		glBegin(GL_LINE_STRIP);

		ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i));
		float plotX = (currentSegment->get_EndKeyFrame() - minKey)*10.0 / deltaKey;

		glVertex3f(plotX, 0.0, 0.0);
		glVertex3f(plotX, 10.0, 0.0);

		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);

	// Draw the line y=0.
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);

	float plotY = (0.0 - minX)*10.0 / deltaX;

	glVertex3f(0.0, plotY, 0.0);
	glVertex3f(10.0, plotY, 0.0);

	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);
}

void MyTrajectoryGLWidget::drawMyCurveHandles(int curveIndex)
{
	int maxKey = -10000;
	float maxZ = -10000.0;

	int minKey = 10000;
	float minZ = 10000.0;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Loop though all the segments for this curve for this surface.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(i));

			// Deal with the first point (P0).
			if (currentSegment->get_StartKeyFrame() > maxKey)
			{
				maxKey = currentSegment->get_StartKeyFrame();
			}
			if (currentSegment->get_P0_p()->get_X() > maxZ)
			{
				maxZ = currentSegment->get_P0_p()->get_X();
			}

			if (currentSegment->get_StartKeyFrame() < minKey)
			{
				minKey = currentSegment->get_StartKeyFrame();
			}
			if (currentSegment->get_P0_p()->get_X() < minZ)
			{
				minZ = currentSegment->get_P0_p()->get_X();
			}

			// Deal with the second point (P1).
			if (currentSegment->get_EndKeyFrame() > maxKey)
			{
				maxKey = currentSegment->get_EndKeyFrame();
			}
			if (currentSegment->get_P1_p()->get_X() > maxZ)
			{
				maxZ = currentSegment->get_P1_p()->get_X();
			}

			if (currentSegment->get_EndKeyFrame() < minKey)
			{
				minKey = currentSegment->get_EndKeyFrame();
			}
			if (currentSegment->get_P1_p()->get_X() < minZ)
			{
				minZ = currentSegment->get_P1_p()->get_X();
			}
		}
	}

	float deltaZ = maxZ - minZ;
	int deltaKey = maxKey - minKey;

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
			float plotX = (currentSegment->get_StartKeyFrame() - minKey)*10.0 / deltaKey;
			float plotY = offset + (currentSegment->get_P0_p()->get_X() - minZ)*10.0 / deltaZ;
			glVertex3f(plotX, plotY, 0.0);
			float plotXt = (currentSegment->get_StartKeyFrame() + currentSegment->get_m0_p()->get_X() - minKey)*10.0 / deltaKey;
			float plotYt = offset + (currentSegment->get_P0_p()->get_X() + currentSegment->get_m0_p()->get_Y() - minZ)*10.0 / deltaZ;
			glVertex3f(plotXt, plotYt, 0.0);
			glEnd();
		}

		glColor3f(0.0, 0.0, 0.0);
	}
}

void MyTrajectoryGLWidget::drawMyInterpolatedCurves(int curveIndex)
{
	int maxKey = -10000;
	float maxZ = -10000.0;

	int minKey = 10000;
	float minZ = 10000.0;

	// Loop though all the surfaces.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Loop though all the pitch curve segments for this surface.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size(); i++)
		{
			ITTrajectoryCurveSegment* currentSegment = (ITTrajectoryCurveSegment*)(project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(i));

			// Deal with the first point (P0).
			if (currentSegment->get_StartKeyFrame() > maxKey)
			{
				maxKey = currentSegment->get_StartKeyFrame();
			}
			if (currentSegment->get_P0_p()->get_X() > maxZ)
			{
				maxZ = currentSegment->get_P0_p()->get_X();
			}

			if (currentSegment->get_StartKeyFrame() < minKey)
			{
				minKey = currentSegment->get_StartKeyFrame();
			}
			if (currentSegment->get_P0_p()->get_X() < minZ)
			{
				minZ = currentSegment->get_P0_p()->get_X();
			}

			// Deal with the second point (P1).
			if (currentSegment->get_EndKeyFrame() > maxKey)
			{
				maxKey = currentSegment->get_EndKeyFrame();
			}
			if (currentSegment->get_P1_p()->get_X() > maxZ)
			{
				maxZ = currentSegment->get_P1_p()->get_X();
			}

			if (currentSegment->get_EndKeyFrame() < minKey)
			{
				minKey = currentSegment->get_EndKeyFrame();
			}
			if (currentSegment->get_P1_p()->get_X() < minZ)
			{
				minZ = currentSegment->get_P1_p()->get_X();
			}
		}
	}

	float deltaZ = maxZ - minZ;
	int deltaKey = maxKey - minKey;

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

			float plotX = (i - minKey)*10.0 / deltaKey;
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

			float plotY = (plotYa - minZ)*10.0 / deltaZ;

			glVertex3f(plotX, plotY, 0.0);
		}
		glEnd();
		glColor3f(0.0, 0.0, 0.0);

		delete translationPoint;
		delete rotationPoint;
	}
}