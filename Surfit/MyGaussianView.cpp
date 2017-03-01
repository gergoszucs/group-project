#include "MyGaussianView.h"

#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.

// Reference: http://www.bogotobogo.com/Qt/Qt5_OpenGL_QGLWidget.php

// Dom's includes.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"

MyGaussianView::MyGaussianView(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 270;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

}


MyGaussianView::~MyGaussianView(void)
{
}


void MyGaussianView::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.

}


void MyGaussianView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	glRotatef((float)yRot, 0.0, 1.0, 0.0);
	glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
	glRotatef(0.0, 0.0, 0.0, 1.0);

	draw();

} // End of paintGL.

void MyGaussianView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyGaussianView::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(glGaussianPanCentreX - glGaussianViewHalfExtent, 
			glGaussianPanCentreX + glGaussianViewHalfExtent,
			glGaussianPanCentreY - glGaussianViewHalfExtent / aspect, 
			glGaussianPanCentreY + glGaussianViewHalfExtent / aspect, 
			-50000.0, 
			50000.0);

	glMatrixMode(GL_MODELVIEW);
}



void MyGaussianView::keyPressEvent( QKeyEvent * event )
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

	if( event->key() == Qt::Key_Up )
	{
		glGaussianPanCentreY = glGaussianPanCentreY + factor;
	}
	else if( event->key() == Qt::Key_Down )
	{
		glGaussianPanCentreY = glGaussianPanCentreY - factor;
	}
	else if( event->key() == Qt::Key_Left )
	{
		glGaussianPanCentreX = glGaussianPanCentreX - factor;
	}
	else if( event->key() == Qt::Key_Right )
	{
		glGaussianPanCentreX = glGaussianPanCentreX + factor;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();

} // End of keyPressEvent.


void MyGaussianView::draw()
{

	if (IsDataLoaded)
	{
		drawMyAxes();
		drawMyGaussianCurvatures();
	}
}

void MyGaussianView::drawMyGaussianCurvatures()
{
	float factor_k = 12.0;
	float factor_i = 10.0;
	float factor_j = 10.0;
	float factor_g = 4.0;	

	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{

		if (!project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->empty()) // Only draw the Gaussian curvature if the data exists.
		{
			// First the curves of constant u
			for (int i=0; i<project->get_MySurfaces()->at(k)->get_URange()->size(); i++)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_STRIP);
				for (int j=0; j<project->get_MySurfaces()->at(k)->get_VRange()->size(); j++)
				{
					glVertex3f(project->get_MySurfaces()->at(k)->get_URange()->at(i)*factor_i + k*factor_k, 
							   project->get_MySurfaces()->at(k)->get_VRange()->at(j)*factor_j, 
							   project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->at(i).at(j)*factor_g);
				}
				glEnd();
			}				

			// Next the curves of constant v
			for (int i=0; i<project->get_MySurfaces()->at(k)->get_VRange()->size(); i++) // i counts columns.
			{

				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_STRIP);
				for (int j=0; j<project->get_MySurfaces()->at(k)->get_URange()->size(); j++) // j counts rows.
				{
					glVertex3f(project->get_MySurfaces()->at(k)->get_URange()->at(j)*factor_i + k*factor_k, 
							   project->get_MySurfaces()->at(k)->get_VRange()->at(i)*factor_j, 
							   project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->at(j).at(i)*factor_g);
				}
				glEnd();
			}					

			// Write a surface id.

			renderText(project->get_MySurfaces()->at(k)->get_URange()->at(0)*factor_i + k*factor_k, 
						 project->get_MySurfaces()->at(k)->get_VRange()->at(0)*factor_j, 
						 0.0,
						 QString("k: %1").arg(k));	

		} // End if MyGaussianCurvature not empty	

	}

} // End of drawMyGaussianCurvatures.

void MyGaussianView::drawMyAxes()
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

	glColor3f(0.0f, 0.0f, 0.0f);
	renderText(30.0, 0.0, 0.0, QString("x-axis"));
	renderText(0.0, 30.0, 0.0, QString("y-axis"));
	renderText(0.0, 0.0, 30.0, QString("z-axis"));

} // End of drawMyAxes.



void MyGaussianView::mouseMoveEvent(QMouseEvent *event)
{

	float dx = (float)(event->x() - lastPos.x());
	float dy = (float)(event->y() - lastPos.y());

	if(event->modifiers() & Qt::ShiftModifier)
	{
		// The shift key was pressed, so zoom.

		// The shift key was pressed, so zoom.

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
		glGaussianViewHalfExtent = glGaussianViewHalfExtent + dy;

		lastPos = event->pos();

	}
	else
	{
		// No other keys pressed so rotate.
		if (event->buttons() & Qt::LeftButton) 
		{
			xRot = xRot + dy;
			yRot = yRot + dx;
		}
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

} // End of mouseMoveEvent.




void MyGaussianView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();


} // End of mousePressEvent.
