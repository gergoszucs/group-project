#include "MyOutputGraphicsGLWidget.h"

// Reference: http://www.bogotobogo.com/Qt/Qt5_OpenGL_QGLWidget.php

// Dom's includes.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPointTrajectory.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPanel.h"

MyOutputGraphicsGLWidget::MyOutputGraphicsGLWidget(QWidget *parent)
	: QGLWidget(parent)
{

}

MyOutputGraphicsGLWidget::~MyOutputGraphicsGLWidget()
{

}

void MyOutputGraphicsGLWidget::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyOutputGraphicsGLWidget::setViewOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 40, 0, 23, -150.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

void MyOutputGraphicsGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);
	plot2D();
}

void MyOutputGraphicsGLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport(0, 0, width, height);

	setViewOrtho();

	plot2D();

}

void MyOutputGraphicsGLWidget::plot2D()
{

	if (IsDataLoaded)
	{
		drawForceHistories();

		//drawPressureDistributions();

		drawSpanForceDistributions();

		drawCoefficientHistories();

	} // End of if dataIsLoaded.

} // End of plot2D

void MyOutputGraphicsGLWidget::drawCoefficientHistories()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "drawCoefficientHistories");

	// Move to the correct position in the window for the CL plot.
	glPushMatrix();
	glTranslatef(1.0, 12.0, 1.0);

	char *ylabelX = "CL";
	drawMy2DAxes(ylabelX);


	// Find the maximum value and scale the y-axis accoringly.
	float yMax = 0.0f;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyCLHistory()->size(); n++)
		{

			if (fabs(project->get_MySurfaces()->at(k)->get_MyCLHistory()->at(n)) > yMax)
			{
				yMax = fabs(project->get_MySurfaces()->at(k)->get_MyCLHistory()->at(n));
			}
		}

	} // End of loop over surfaces.


	  // Round up yMax to the nearest 1.
	float roundedYMax = ceil(yMax);

	// Print the abscissa scale
	int noOfAbscissas = roundedYMax / 10;


	// Print the abscissa scale
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int n = 0; n<10; n++)
	{
		renderText(-0.8, (float)n, 0.0, QString::number(roundedYMax*n / 10.0, 'f', 1));

		// Draw the horizontal grid lines.

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)n, 0.0);
		glVertex3f(10.0, (float)n, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);


	// Loop over surfaces, plotting the CL history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);

		// Plot the points.
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyCLHistory()->size(); n++)
		{
			glVertex3f((float)n*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyCLHistory()->at(n))*10.0 / roundedYMax, 0.0f);
		} // End of loop over frames.

		glEnd();

		if (drawAnnotations)
		{
			int historySize = project->get_MySurfaces()->at(k)->get_MyCLHistory()->size();

			if ((FrameNumber > 0) && (FrameNumber < project->get_MaxKeyFrame()) && (historySize > 0)) // To avoid Qt recursive repaint run-time error.
			{
				renderText((float)(historySize)*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyCLHistory()->back())*10.0 / roundedYMax, 0.0f, QString::number(k));
			}
		}


	} // End of surface loop.

	  // Remove the window translation.
	glPopMatrix();









	// Move to the correct position in the window for the CD plot.
	glPushMatrix();
	glTranslatef(15.0, 12.0, 1.0);

	ylabelX = "CD";
	drawMy2DAxes(ylabelX);


	// Find the maximum value and scale the y-axis accoringly.
	yMax = -100000.0f;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		for (int n = 2; n<project->get_MySurfaces()->at(k)->get_MyCDHistory()->size(); n++)
		{

			if (fabs(project->get_MySurfaces()->at(k)->get_MyCDHistory()->at(n)) > yMax)
			{
				yMax = fabs(project->get_MySurfaces()->at(k)->get_MyCDHistory()->at(n));
			}
		}

	} // End of loop over surfaces.


	  // Round up yMax to the nearest 1.
	roundedYMax = ceil(yMax);

	// Print the abscissa scale

	// Print the abscissa scale
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int n = 0; n<10; n++)
	{
		renderText(-0.8, (float)n, 0.0, QString::number(roundedYMax*n / 10.0, 'f', 1));

		// Draw the horizontal grid lines.

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)n, 0.0);
		glVertex3f(10.0, (float)n, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);


	// Loop over surfaces, plotting the CD history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);

		// Plot the points.
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyCDHistory()->size(); n++)
		{
			glVertex3f((float)n*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyCDHistory()->at(n))*10.0 / roundedYMax, 0.0f);
		} // End of loop over frames.

		glEnd();

		if (drawAnnotations)
		{
			int historySize = project->get_MySurfaces()->at(k)->get_MyCDHistory()->size();

			if ((FrameNumber > 0) && (FrameNumber < project->get_MaxKeyFrame()) && (historySize > 0)) // To avoid Qt recursive repaint run-time error.
			{
				renderText((float)(historySize)*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyCDHistory()->back())*10.0 / roundedYMax, 0.0f, QString::number(k));
			}
		}


	} // End of surface loop.

	  // Remove the window translation.
	glPopMatrix();

} // End of drawCoefficientHistories.



void MyOutputGraphicsGLWidget::drawMy2DAxes(char *yLabel)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "drawMy2DAxes");

	// Draw X axis.
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glEnd();

	// Draw Y axis.
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glEnd();

	// Draw the axis titles.
	glColor3f(0.0f, 0.0f, 0.0f);
	renderText(10.0, 0.0, 0.0, QString("Frame"));
	renderText(0.0, 10.0, 0.0, QString(yLabel));

	// Draw the x axis annotation.

	for (int i = 0; i<project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); i++)
	{

		float plotX = (float)(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame())*10.0 / (float)(project->get_MaxKeyFrame());

		renderText(plotX, -0.5, 0.0, QString(" %1").arg(QString::number(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(i)->get_EndKeyFrame())));

		// Print the abscissa scale
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(3, 0xAAAA);
		glColor3f(0.0f, 0.0f, 0.0f);

		// Draw the vertical grid line.
		glBegin(GL_LINE_STRIP);
		glVertex3f(plotX, 0.0, 0.0);
		glVertex3f(plotX, 9.0, 0.0);
		glEnd();

		glColor3f(0.0, 0.0, 0.0);
		glDisable(GL_LINE_STIPPLE);

	}


} // End of drawMy2DAxes.


void MyOutputGraphicsGLWidget::drawForceHistories()
{

	// Move to the correct position in the window for the x plot.
	glPushMatrix();
	glTranslatef(1.0, 1.0, 1.0);

	char *ylabelX = "abs(x-force)(N)";
	drawMy2DAxes(ylabelX);


	// Find the maximum value and scale the y-axis accoringly.
	float yMax = 0.0f;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{

			if (fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_X()) > yMax)
			{
				yMax = fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_X());
			}
		}

	} // End of loop over surfaces.


	  // Round up yMax to the nearest 1.
	float roundedYMax = ceil(yMax);

	// Print the abscissa scale
	int noOfAbscissas = roundedYMax / 10;


	// Print the abscissa scale
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int n = 0; n<10; n++)
	{
		renderText(-0.6, (float)n, 0.0, QString::number(roundedYMax*n / 10.0, 'e', 1));

		// Draw the horizontal grid lines.

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)n, 0.0);
		glVertex3f(10.0, (float)n, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);


	// Loop over surfaces, plotting the x force history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);

		// Plot the points.
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{
			glVertex3f((float)n*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_X())*10.0 / roundedYMax, 0.0f);
		} // End of loop over frames.

		glEnd();

		if (drawAnnotations)
		{
			int historySize = project->get_MySurfaces()->at(k)->get_MyForceHistory()->size();

			if ((FrameNumber > 0) && (FrameNumber < project->get_MaxKeyFrame()) && (historySize > 0)) // To avoid Qt recursive repaint run-time error.
			{
				renderText((float)(historySize)*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->back()->get_X())*10.0 / roundedYMax, 0.0f, QString::number(k));
			}
		}


	} // End of surface loop.

	  // Remove the window translation.
	glPopMatrix();







	// Move to the correct position in the window for the y plot.
	glPushMatrix();
	glTranslatef(15.0, 1.0, 1.0);

	ylabelX = "abs(y-force)(N)";
	drawMy2DAxes(ylabelX);


	// Find the maximum value and scale the y-axis accoringly.
	yMax = 0.0f;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{

			if (fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Y()) > yMax)
			{
				yMax = fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Y());
			}
		}

	} // End of loop over surfaces.


	  // Round up yMax to the nearest 1.
	roundedYMax = ceil(yMax);

	// Print the abscissa scale
	noOfAbscissas = roundedYMax / 10;


	// Print the abscissa scale
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int n = 0; n<10; n++)
	{
		renderText(-1.9, (float)n, 0.0, QString::number(roundedYMax*n / 10.0, 'e', 1));

		// Draw the horizontal grid lines.

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)n, 0.0);
		glVertex3f(10.0, (float)n, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);


	// Loop over surfaces, plotting the y force history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);

		// Plot the points.
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{
			glVertex3f((float)n*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Y())*10.0 / roundedYMax, 0.0f);
		} // End of loop over frames.

		glEnd();

		if (drawAnnotations)
		{
			int historySize = project->get_MySurfaces()->at(k)->get_MyForceHistory()->size();

			if ((FrameNumber > 0) && (FrameNumber < project->get_MaxKeyFrame()) && (historySize > 0)) // To avoid Qt recursive repaint run-time error.
			{
				renderText((float)(historySize)*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->back()->get_Y())*10.0 / roundedYMax, 0.0f, QString::number(k));
			}
		}


	} // End of surface loop.

	  // Remove the window translation.
	glPopMatrix();
















	// Move to the correct position in the window for the z plot.
	glPushMatrix();
	glTranslatef(28.0, 1.0, 1.0);

	ylabelX = "abs(z-force)(N)";
	drawMy2DAxes(ylabelX);


	// Find the maximum value and scale the y-axis accoringly.
	yMax = 0.0f;
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{

			if (fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Z()) > yMax)
			{
				yMax = fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Z());
			}
		}

	} // End of loop over surfaces.


	  // Round up yMax to the nearest 1.
	roundedYMax = ceil(yMax);

	// Print the abscissa scale
	noOfAbscissas = roundedYMax / 10;


	// Print the abscissa scale
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.0f, 0.0f, 0.0f);

	for (int n = 0; n<10; n++)
	{
		renderText(-1.9, (float)n, 0.0, QString::number(roundedYMax*n / 10.0, 'e', 1));

		// Draw the horizontal grid lines.

		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, (float)n, 0.0);
		glVertex3f(10.0, (float)n, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);


	// Loop over surfaces, plotting the y force history.
	for (int k = 0; k<project->get_MySurfaces()->size(); k++)
	{

		glColor3f(0.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);

		// Plot the points.
		for (int n = 0; n<project->get_MySurfaces()->at(k)->get_MyForceHistory()->size(); n++)
		{
			glVertex3f((float)n*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Z())*10.0 / roundedYMax, 0.0f);
		} // End of loop over frames.

		glEnd();

		if (drawAnnotations)
		{
			int historySize = project->get_MySurfaces()->at(k)->get_MyForceHistory()->size();

			if ((FrameNumber > 0) && (FrameNumber < project->get_MaxKeyFrame()) && (historySize > 0)) // To avoid Qt recursive repaint run-time error.
			{
				renderText((float)(historySize)*10.0 / project->get_MaxKeyFrame(), fabs(project->get_MySurfaces()->at(k)->get_MyForceHistory()->back()->get_Z())*10.0 / roundedYMax, 0.0f, QString::number(k));
			}
		}


	} // End of surface loop.

	  // Remove the window translation.
	glPopMatrix();

} // End of drawForceHistories.




void MyOutputGraphicsGLWidget::drawMy2DAxesPressure(char * yLabel)
{

	// Draw X axis.
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glEnd();

	// Draw Y axis.
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 10.0f, 0.0f);
	glEnd();

	// Draw the axis titles.
	glColor3f(0.0f, 0.0f, 0.0f);
	renderText(10.0, 0.0, 0.0, QString("span"));
	renderText(0.0, 10.0, 0.0, QString(yLabel));


} // End of drawMy2DAxesPressure.



void MyOutputGraphicsGLWidget::drawSpanForceDistributions()
{

	// Move to the correct position in the window for the x plot.
	glPushMatrix();
	glTranslatef(28.0, 12.0, 1.0);

	char *ylabelX = "Spanwise force distribution";
	drawMy2DAxesPressure(ylabelX);

	// Only do the plotting if there is data.
	if (project->get_MySurfaces()->at(0)->get_MyPressureHistory()->size() > 0)
	{

		// Find the maximum spanwize slice force.
		float maxChordwiseForce = 0.0;
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{

			// Get the last entry in the pressure history.
			std::vector < std::vector <float> > *pressureVofV;
			if (IsReplay)
			{
				pressureVofV = &(project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber));
			}
			else
			{
				pressureVofV = &(project->get_MySurfaces()->at(k)->get_MyPressureHistory()->back());
			}

			for (int i = 0; i<pressureVofV->size(); i++) // Loop across span
			{

				float currentChordForce = 0.0;

				for (int j = 0; j<pressureVofV->at(0).size(); j++) // Loop over the chord
				{

					currentChordForce = currentChordForce + pressureVofV->at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Area();

				}

				if (fabs(currentChordForce) > maxChordwiseForce)
				{
					maxChordwiseForce = fabs(currentChordForce);
				}
			}
		}


		// Print the abscissa scale
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(3, 0xAAAA);
		glColor3f(0.0f, 0.0f, 0.0f);

		for (int n = 0; n<10; n++)
		{
			renderText(-1.6, (float)n, 0.0, QString::number(maxChordwiseForce*n / 10.0, 'e', 1));

			// Draw the horizontal grid lines.

			glBegin(GL_LINE_STRIP);
			glVertex3f(0.0, (float)n, 0.0);
			glVertex3f(10.0, (float)n, 0.0);
			glEnd();

		}

		glColor3f(0.0, 0.0, 0.0);
		glDisable(GL_LINE_STIPPLE);


		// Now do the drawing.
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{
			// Get the entry in the pressure history.
			std::vector < std::vector <float> > *pressureVofV;
			if (IsReplay)
			{
				pressureVofV = &(project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber));
			}
			else
			{
				pressureVofV = &(project->get_MySurfaces()->at(k)->get_MyPressureHistory()->back());
			}

			int noOfRows = pressureVofV->size();

			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_STRIP);

			float textY = 0.0;

			for (int i = 0; i<pressureVofV->size(); i++) // Loop across span
			{

				float currentChordForce = 0.0;

				for (int j = 0; j<pressureVofV->at(0).size(); j++) // Loop over th
				{

					currentChordForce = currentChordForce + pressureVofV->at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Area();

				}

				float plotX = 10.0f*i / noOfRows;
				float plotY = 10.0f*fabs(currentChordForce) / maxChordwiseForce;

				glVertex3f(plotX, plotY, 0.0f);

				textY = plotY;

			}

			glEnd();

			renderText(10.0, textY, 0.0f, QString::number(k));

		} // End of loop over surfaces.


	} // End of if history data exists.


	  // Remove the window translation.
	glPopMatrix();

} // End of drawSpanForceDistributions
