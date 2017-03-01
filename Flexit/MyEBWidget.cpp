#include "MyEBWidget.h"


// Dom's includes.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITBeam.h"
#include "ITBeamNode.h"

MyEBWidget::MyEBWidget(QWidget *parent)
	: QGLWidget(parent)
{

}

MyEBWidget::~MyEBWidget()
{

}

void MyEBWidget::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyEBWidget::setViewOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 40, 0, 23, -150.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}

void MyEBWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);
	plot2D();
}

void MyEBWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport(0, 0, width, height);

	setViewOrtho();

	plot2D();

}

void MyEBWidget::plot2D()
{

	if (IsDataLoaded)
	{

		drawSpanDeformationDistributions();


	} // End of if dataIsLoaded.

} // End of plot2D



void MyEBWidget::drawSpanDeformationDistributions()
{

	// Move to the correct position in the window for the x plot.
	glPushMatrix();
	glTranslatef(28.0, 12.0, 1.0);

	char *ylabelX = "Spanwise deformation distribution";
	drawMy2DAxes(ylabelX);



	// Only do the plotting if there is data.
	if (project->get_MySurfaces()->at(0)->get_MyBeam()->get_MyNodeHistory()->size() > 0)
	{

		// Find the maximum spanwize deflection.
		float maxDeflection = -10000.0;
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{
			// Get the entry in the deflection history.
			std::vector <ITBeamNode*> *deflectionV;
			if (IsReplay)
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber));
			}
			else
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->back());
			}

			for (int i = 0; i<deflectionV->size(); i++) // Loop across span
			{

				if (deflectionV->at(i)->get_W()->get_Z() > maxDeflection)
				{
					maxDeflection = deflectionV->at(i)->get_W()->get_Z();
				}
			}
		}



		// Find the minimum spanwize deflection.
		float minDeflection = 10000.0;
		for (int k = 0; k<project->get_MySurfaces()->size(); k++)
		{
			// Get the entry in the deflection history.
			std::vector <ITBeamNode*> *deflectionV;
			if (IsReplay)
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber));
			}
			else
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->back());
			}

			for (int i = 0; i<deflectionV->size(); i++) // Loop across span
			{

				if (deflectionV->at(i)->get_W()->get_Z() < minDeflection)
				{
					minDeflection = deflectionV->at(i)->get_W()->get_Z();
				}
			}
		}

		// Print the abscissa scale
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(3, 0xAAAA);
		glColor3f(0.0f, 0.0f, 0.0f);

		for (int n = 0; n<10; n++)
		{
			renderText(-2.0, (float)n, 0.0, QString::number(minDeflection + (maxDeflection - minDeflection)*n / 10.0, 'e', 1));

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
			std::vector <ITBeamNode*> *deflectionV;
			if (IsReplay)
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber));
			}
			else
			{
				deflectionV = &(project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->back());
			}

			int noOfNodes = deflectionV->size();

			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_LINE_STRIP);

			float textY = 0.0;



			for (int i = 0; i<deflectionV->size(); i++) // Loop across beam
			{

				float plotX = 10.0f*i / noOfNodes;
				float plotY = 10.0f*(deflectionV->at(i)->get_W()->get_Z() - minDeflection) / (maxDeflection - minDeflection);

				glVertex3f(plotX, plotY, 0.0f);

				textY = plotY;

			}

			glEnd();

			renderText(10.0, textY, 0.0f, QString::number(k));

		} // End of loop over surfaces.






	} // End of if there is data.










	  // Remove the window translation.
	glPopMatrix();

} // End of drawSpanDeformationDistributions



void MyEBWidget::drawMy2DAxes(char * yLabel)
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
