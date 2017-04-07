#include "MyGaussianView.h"
#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "UtililityFunctions.h"

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

}

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

void MyGaussianView::keyPressEvent(QKeyEvent * event)
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
		glGaussianPanCentreY = glGaussianPanCentreY + factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glGaussianPanCentreY = glGaussianPanCentreY - factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glGaussianPanCentreX = glGaussianPanCentreX - factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glGaussianPanCentreX = glGaussianPanCentreX + factor;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
}

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

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (!project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->empty()) // Only draw the Gaussian curvature if the data exists.
		{
			// First the curves of constant u
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_URange()->size(); i++)
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_VRange()->size(); j++)
				{
					glVertex3f(project->get_MySurfaces()->at(k)->get_URange()->at(i)*factor_i + k*factor_k,
						project->get_MySurfaces()->at(k)->get_VRange()->at(j)*factor_j,
						project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->at(i).at(j)*factor_g);
				}
				glEnd();
			}

			// Next the curves of constant v
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_VRange()->size(); i++) // i counts columns.
			{
				glColor3f(0.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_STRIP);
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_URange()->size(); j++) // j counts rows.
				{
					glVertex3f(project->get_MySurfaces()->at(k)->get_URange()->at(j)*factor_i + k*factor_k,
						project->get_MySurfaces()->at(k)->get_VRange()->at(i)*factor_j,
						project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->at(j).at(i)*factor_g);
				}
				glEnd();
			}

			renderText(project->get_MySurfaces()->at(k)->get_URange()->at(0)*factor_i + k*factor_k,
				project->get_MySurfaces()->at(k)->get_VRange()->at(0)*factor_j,
				0.0,
				QString("k: %1").arg(k));
		} // End if MyGaussianCurvature not empty	
	}
}

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
}

void MyGaussianView::mouseMoveEvent(QMouseEvent *event)
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
			float tmp = glGaussianViewHalfExtent + factor * dy;

			if (tmp >= 0.0) glGaussianViewHalfExtent = tmp;
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
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		glGaussianPanCentreX -= factor * dx;
		glGaussianPanCentreY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyGaussianView::wheelEvent(QWheelEvent *event)
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glGaussianPanCentreX, glGaussianPanCentreY, glGaussianViewHalfExtent);

	float tmp = glGaussianViewHalfExtent - factor * dy;

	if (tmp >= 0.0) glGaussianViewHalfExtent = tmp;

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), glGaussianPanCentreX, glGaussianPanCentreY, glGaussianViewHalfExtent);

	glGaussianPanCentreX += tmpAx - tmpBx;
	glGaussianPanCentreY += tmpAy - tmpBy;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyGaussianView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}