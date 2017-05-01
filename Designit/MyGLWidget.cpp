#include "MyGLWidget.h"
#include <QtOpenGL>
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITPanel.h"
#include "ITControlPoint.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"
#include "UtililityFunctions.h"
#include <gl\GLU.h>

MyGLWidget::MyGLWidget(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;

	glViewHalfExtent = 50.0;
	glPanCentreX = 0.0;
	glPanCentreY = 0.0;

	setFocusPolicy(Qt::StrongFocus);

	centerX = 0.0;
	centerY = 0.0;
	centerZ = 0.0;
	azimuth = M_PI + M_PI_2;
	polar = M_PI_2;
	radius = 20;

	eyeX = 0.0;
	eyeY = 0.0;
	eyeZoom = 1.0;
}

void MyGLWidget::setDrawParameters(float glPanCenterX, float glPanCenterY, float glViewHalfExtent)
{
	this->glPanCentreX = glPanCenterX;
	this->glPanCentreY = glPanCenterY;
	this->glViewHalfExtent = glViewHalfExtent;
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

	//<from moveit>
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_MULTISAMPLE);
	//</from moveit>
}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-eyeX*eyeZoom, -eyeY*eyeZoom, 0.0); //
	glScalef(eyeZoom, eyeZoom, eyeZoom);
	//gluLookAt(centerX + cameraX, centerY + cameraY, centerZ + cameraZ, centerX, centerY, centerZ, 0, 0, 1);

	//glTranslatef(0.0, 0.0, -10.0);

	//// 20160510: Ordering of these rotation has been changed to give more intuitive rotation in response to mouse movement.
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
}

void MyGLWidget::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	//glViewport(0, 0, width, height);

	//this->setViewOrtho(width, height);

	glViewport(0, 0, width, height);

	float aspect = (float)width / (float)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0, +50.0, -50.0 /aspect, +50.0 /aspect, -50000, 50000.0);
	/*glOrtho(glPanCentreX - glViewHalfExtent,
		glPanCentreX + glViewHalfExtent,
		glPanCentreY - glViewHalfExtent / aspect,
		glPanCentreY + glViewHalfExtent / aspect,
		-50000.0,
		50000.0);*/

	glMatrixMode(GL_MODELVIEW);

	draw();
}

void MyGLWidget::setViewOrtho(int width, int height)
{
	//float aspect = (float)width / (float)height; // Landscape is greater than 1.

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

	//// Landscape.

	//glOrtho(glPanCentreX - glViewHalfExtent,
	//	glPanCentreX + glViewHalfExtent,
	//	glPanCentreY - glViewHalfExtent / aspect,
	//	glPanCentreY + glViewHalfExtent / aspect,
	//	-50000.0,
	//	50000.0);
	//glMatrixMode(GL_MODELVIEW);
}

//drawing functions

void MyGLWidget::draw()
{
	glTranslatef(centerX, centerY, centerZ);
	drawSphere(1, 15, 15, 0.0, 1.0, 0.0);
	glTranslatef(-centerX, -centerY, -centerZ);

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

		if ( trajectoryMode && ( project->get_MySurfaces()->size() > 0 ) )
		{
			int n = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size() - 1;
			int frame = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(n)->get_EndKeyFrame();

			drawMyTracks( frame );
		}
		
	}
}

void MyGLWidget::drawMyAnnotations()
{
	// Draw each surface.
	glColor3f(1.0f, 1.0f, 1.0f); // White

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		ITPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).at(0);
		renderText(p->get_X(), p->get_Y(), p->get_Z(), QString::number(k));
	}
}

void MyGLWidget::drawMyInterpolatedPointsNet()
{
	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);

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

void MyGLWidget::drawMyNormals()
{
	float factor = 1.0;

	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(0.1f);

	// Draw each surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
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
}

void MyGLWidget::drawMyPanelNormals()
{
	float factor = 1.0;

	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(0.1f);

	// Draw each surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyInterpolatedPoints()->at(i).size(); j++)
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
}

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
}

void MyGLWidget::drawMyControlPointsNet()
{
	glColor3f(1.0f, 1.0f, 1.0f);

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
}

void MyGLWidget::drawMyTracks( int frame )
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyTracks.");

	glColor3f(1.0f, 0.0f, 0.0f); // Red.

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		glBegin(GL_LINE_STRIP);

		// Loop through the frames for the first corner control point.
		for (int n = 0; n < frame; n++)
		{
			ITPoint *translationPoint = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *rotationPoint = new ITPoint(0.0, 0.0, 0.0);

			project->get_MySurfaces()->at(k)->computeTrajectoryPointsAtFrame(n, k, translationPoint, rotationPoint);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "drawing translationPoint x: %f, y: %f, z: %f", translationPoint->get_X(), translationPoint->get_Y(), translationPoint->get_Z());

			ITPoint *currentPoint = new ITPoint(0.0, 0.0, 0.0);

			currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_X());
			currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_Y());
			currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_Z());

			ITPoint *cp = project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint();
			currentPoint->propagateMe(cp, rotationPoint, translationPoint);

			glVertex3f(currentPoint->get_X(), currentPoint->get_Y(), currentPoint->get_Z());

			delete translationPoint;
			delete rotationPoint;
			delete currentPoint;
		}

		glEnd();
		glBegin(GL_LINE_STRIP);

		// Loop through the frames for the second corner control point.
		for (int n = 0; n < frame; n++)
		{
			ITPoint *translationPoint = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *rotationPoint = new ITPoint(0.0, 0.0, 0.0);

			project->get_MySurfaces()->at(k)->computeTrajectoryPointsAtFrame(n, k, translationPoint, rotationPoint);

			ITPoint *currentPoint = new ITPoint(0.0, 0.0, 0.0);

			currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_X());
			currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_Y());
			currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_Z());

			ITPoint *cp = project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint();
			currentPoint->propagateMe(cp, rotationPoint, translationPoint);

			glVertex3f(currentPoint->get_X(), currentPoint->get_Y(), currentPoint->get_Z());

			delete translationPoint;
			delete rotationPoint;
			delete currentPoint;
		}

		glEnd();
	}
}

void MyGLWidget::drawSphere(double r, int lats, int longs, float R, float GG, float B)
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

void MyGLWidget::modPolar(float p)
{
	float tmp = polar + DEG_TO_RAD(p);

	if ((tmp < M_PI) && (tmp > 0)) polar = tmp;
}

void MyGLWidget::modAzimuth(float a)
{
	azimuth += DEG_TO_RAD(a);
}

//Control slots

void MyGLWidget::keyPressEvent(QKeyEvent * event)
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
		glPanCentreY += factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glPanCentreY -= factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glPanCentreX -= factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glPanCentreX += factor;
	}

	// Adjust viewport view.
	//this->setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();

}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
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
			//float tmp = glViewHalfExtent + factor * dy;

			//if (tmp >= 0.0) glViewHalfExtent = tmp;
		}
		else // No other keys pressed so rotate.
		{
			xRot = xRot + dy;
			yRot = yRot + dx;
			//modAzimuth(-dx);
			//modPolar(dy);
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		//glPanCentreX -= factor * dx;
		//glPanCentreY += factor * dy;
		//centerX += dx * cos(azimuth);
		//centerY += dy * sin(azimuth);
		//centerZ += dy * sin(azimuth) * sin(polar);
		//float cameraX = radius * sin(polar) * cos(azimuth);
		//float cameraY = radius * sin(polar) * sin(azimuth);
		//float cameraZ = radius * cos(polar);
		eyeX -= factor * dx;
		eyeY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	//this->setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyGLWidget::wheelEvent(QWheelEvent *event)
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

	/*float tmpAx, tmpAy, tmpBx, tmpBy;

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glPanCentreX, glPanCentreY, glViewHalfExtent);
	*/
	float tmp = eyeZoom + 0.01 * factor * dy;
	
	if (tmp >= 0.0) eyeZoom = tmp;
	/*
	getinaxesposition(tmpbx, tmpby, event->x(), event->y(), this->width(), this->height(), glpancentrex, glpancentrey, glviewhalfextent);
	
	glPanCentreX += tmpAx - tmpBx;
	glPanCentreY += tmpAy - tmpBy;*/

	// Adjust viewport view.
	//this->setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	update();
}