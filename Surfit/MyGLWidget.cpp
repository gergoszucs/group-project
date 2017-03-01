#include "MyGLWidget.h"

#include <QtOpenGL>

// Reference: http://www.bogotobogo.com/Qt/Qt5_OpenGL_QGLWidget.php

// Dom's includes.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITPanel.h"
#include "ITControlPoint.h"

MyGLWidget::MyGLWidget(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

}


MyGLWidget::~MyGLWidget(void)
{
}


void MyGLWidget::initializeGL()
{
	qglClearColor(Qt::black);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.

}


void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	// 20160510: Ordering of these rotation has been changed to give more intuitive rotation in response to mouse movement.

	if (drawRotateZVertical)
	{
		// Z vertical.  (Checked)
		glRotatef((float)xRot, 1.0, 0.0, 0.0);
		glRotatef(0.0, 0.0, 1.0, 0.0);
		glRotatef((float)yRot, 0.0, 0.0, 1.0);
	}
	else if (drawRotateXHorizontal)
	{
		// X horizontal. (Checked)
		//glRotatef((float)yRot, 0.0, 1.0, 0.0);
		//glRotatef((float)xRot, 0.0, 0.0, 1.0); // Rotate about the Z axis.
		//glRotatef(0.0, 1.0, 0.0, 0.0);

		glRotatef((float)yRot, 0.0, 1.0, 0.0);
		glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
		glRotatef(0.0, 0.0, 0.0, 1.0);
	}
	else // if (drawRotateYHorizontal)
	{
		// Y horizontal. This is actually Y vertical.
		//glRotatef((float)xRot, 1.0, 0.0, 0.0);
		//glRotatef((float)yRot, 0.0, 1.0, 0.0);
		//glRotatef(0.0, 0.0, 0.0, 1.0);


		glRotatef(0.0, 1.0, 0.0, 0.0); // Rotate about the X axis.
		glRotatef((float)yRot, 0.0, 1.0, 0.0);
		glRotatef((float)xRot, 0.0, 0.0, 1.0);

	}



	draw();

} // End of paintGL.

void MyGLWidget::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyGLWidget::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(gl3DPanCentreX - gl3DViewHalfExtent, 
			gl3DPanCentreX + gl3DViewHalfExtent,
			gl3DPanCentreY - gl3DViewHalfExtent / aspect, 
			gl3DPanCentreY + gl3DViewHalfExtent / aspect, 
			-50000.0, 
			50000.0);


	glMatrixMode(GL_MODELVIEW);
}



void MyGLWidget::keyPressEvent( QKeyEvent * event )
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
		gl3DPanCentreY = gl3DPanCentreY + factor;
	}
	else if( event->key() == Qt::Key_Down )
	{
		gl3DPanCentreY = gl3DPanCentreY - factor;
	}
	else if( event->key() == Qt::Key_Left )
	{
		gl3DPanCentreX = gl3DPanCentreX - factor;
	}
	else if( event->key() == Qt::Key_Right )
	{
		gl3DPanCentreX = gl3DPanCentreX + factor;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();

} // End of keyPressEvent.


void MyGLWidget::draw()
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
		if (drawNormals)
		{
			drawMyNormals();
			drawMyPanelNormals();
		}
		if (drawAnnotations)
		{
			drawMyAnnotations();
		}
	}
}

void MyGLWidget::drawMyAnnotations()
{

	// Draw each surface.
	glColor3f(1.0f, 1.0f, 1.0f); // White

	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{
		ITPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).at(0);
		renderText(p->get_X(), p->get_Y(), p->get_Z(), QString::number(k));
	}

}// End of drawMyAnnotations.



void MyGLWidget::drawMyInterpolatedPointsNet()
{
	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);

	// Draw each surface.
	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{

		// First the curves of constant u
		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_X(), 
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_Y(), 
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j)->get_Z());
			}
			glEnd();
		}		
		// Next the curves of constant v
		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(0).size(); i++) // i counts columns.
		{
			glBegin(GL_LINE_STRIP);
			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); j++) // j counts rows.
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_X(), 
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_Y(), 
					project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(j).at(i)->get_Z());
			}
			glEnd();
		}		

	} // End of k loop.

} // End of drawMyInterpolatedPointsNet.


void MyGLWidget::drawMyNormals()
{

	float factor = 1.0;

	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(0.1f); 

	// Draw each surface.
	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{

		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
			{
				ITPoint *p = project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j);
				ITPoint *n = project->get_MySurfaces()->at(k)->get_MyInterpolatedNormals()->at(i).at(j);

				glBegin(GL_LINE_STRIP);
					glVertex3f(p->get_X(), 
						p->get_Y(), 
						p->get_Z());

					glVertex3f(p->get_X() + factor * n->get_X(), 
						p->get_Y() + factor * n->get_Y(), 
						p->get_Z() + factor * n->get_Z());
					
				glEnd();



			}
		}
	}
} // End of drawMyNormals.


void MyGLWidget::drawMyPanelNormals()
{

	float factor = 1.0;

	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(0.1f); 

	// Draw each surface.
	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{

		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
			{
				ITPoint *p = project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).at(j);
				ITPoint *n = project->get_MySurfaces()->at(k)->get_MyInterpolatedNormals()->at(i).at(j);

				glBegin(GL_LINE_STRIP);
					glVertex3f(p->get_X(), 
						p->get_Y(), 
						p->get_Z());

					glVertex3f(p->get_X() + factor * n->get_X(), 
						p->get_Y() + factor * n->get_Y(), 
						p->get_Z() + factor * n->get_Z());
					
				glEnd();



			}
		}
	}
} // End of drawMyPanelNormals.


void MyGLWidget::drawMyAxes()
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

} // End of drawMyAxes.





void MyGLWidget::drawMyControlPointsNet()
{

	glColor3f(1.0f, 1.0f, 1.0f);

	// Curves of constant u.
	for (int k=0; k<project->get_MySurfaces()->size(); k++)
	{

		// Do the plotting.
		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);

			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{

				glVertex3f(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

			}

			glEnd();

		}

		// Curves of constant v.
		for (int i=0; i<project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size(); i++)
		{
			glBegin(GL_LINE_STRIP);

			for (int j=0; j<project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); j++)
			{

				glVertex3f(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_X(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(j).at(i)->get_Z());

			}

			glEnd();

		}

	} // End of k loop.


}  // End of drawMyControlPointsNet.


void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}






void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{

	float dx = (float)(event->x() - lastPos.x());
	float dy = (float)(event->y() - lastPos.y());

	// Check for shift key press for zoom.
	if(event->modifiers() & Qt::ShiftModifier)
	{
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
		gl3DViewHalfExtent = gl3DViewHalfExtent + dy;

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


