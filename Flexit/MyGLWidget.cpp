#include "MyGLWidget.h"
#include <QtOpenGL>
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPanel.h"
#include "ITWakePanel.h"
#include "ITVortex.h"
#include "ITBeam.h"
#include "ITBeamNode.h"
#include "ITGust.h"

MyGLWidget::MyGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;
	//setFocusPolicy(Qt::StrongFocus);
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

	// 20161215 New Stuff.
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_MULTISAMPLE);
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
		glRotatef((float)yRot, 0.0, 1.0, 0.0);
		glRotatef((float)xRot, 1.0, 0.0, 0.0); // Rotate about the X axis.
		glRotatef(0.0, 0.0, 0.0, 1.0);
	}
	else // if (drawRotateYHorizontal)
	{
		// Y horizontal. This is actually Y vertical.
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
	glOrtho(glPanCentreX - glViewHalfExtent,
		glPanCentreX + glViewHalfExtent,
		glPanCentreY - glViewHalfExtent / aspect,
		glPanCentreY + glViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Up)
	{
		glPanCentreY = glPanCentreY + 1.0;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glPanCentreY = glPanCentreY - 1.0;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glPanCentreX = glPanCentreX - 1.0;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glPanCentreX = glPanCentreX + 1.0;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	// Check for shift key press for zoom.
	if (event->modifiers() & Qt::ShiftModifier)
	{
		// The shift key was pressed, so zoom.

		// Update zoom.
		glViewHalfExtent = glViewHalfExtent + dy;

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
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void MyGLWidget::setStandardXView()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside setStandardXView.");

	xRot = 90;
	yRot = 270;
	zRot = 90;

	glViewHalfExtent = 50;
	glPanCentreX = 0.0;
	glPanCentreY = 0.0;

	// Adjust viewport view.
	setViewOrtho(1, 1);

	// Redraw everything.
	updateGL();
}

void MyGLWidget::setStandardYView()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside setStandardYView.");

	xRot = 270;
	yRot = 0;
	zRot = 90;

	glViewHalfExtent = 50;
	glPanCentreX = 0.0;
	glPanCentreY = 0.0;

	// Adjust viewport view.
	setViewOrtho(1, 1);

	// Redraw everything.
	updateGL();
}

void MyGLWidget::setStandardZView()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside setStandardZView.");

	xRot = 0;
	yRot = 0;
	zRot = 90;

	glViewHalfExtent = 50;
	glPanCentreX = 0.0;
	glPanCentreY = 0.0;

	// Adjust viewport view.
	setViewOrtho(1, 1);

	// Redraw everything.
	updateGL();
}

void MyGLWidget::drawSphere(double r, int lats, int longs)
{
	// Ref: http://stackoverflow.com/questions/22058111/opengl-draw-sphere-using-glvertex3f

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

void MyGLWidget::draw()
{
	if (IsDataLoaded)
	{
		if (IsReplay)
		{
			if (drawAxes)
			{
				drawMyAxes();
			}
			if (drawControlPoints)
			{
				drawMyControlPointsNet();
				// drawMyDeformedControlPointsNet();
			}
			if (drawInterpolatedPoints)
			{
				drawMyInterpolatedPointsNet();
			}
			if (drawTracks)
			{
				drawMyTracks();
			}
			if (drawWakes)
			{
				// 20170221: There is a bug. The method drawMyWakePanelHistory only works correctly if all wake panels are present in the history. 
				if (project->get_MaxPropagationGeneration() == 0)
				{
					drawMyWakePanelHistory();
				}
			}
			if (drawPressure)
			{
				drawMyPressureHistory();
			}
			if (drawAnnotations)
			{
				drawMyAnnotations();
			}
			if (drawVorticities)
			{
				drawMyVorticityHistory();
			}
			if (drawBeamNodes)
			{
				// drawMyBeamNodeHistory();
			}
			if (drawVelocityField)
			{
				// drawMyVelocityFieldHistory();
			}
			if (drawNormals)
			{
				drawMyNormals();
			}
			if (drawPanels)
			{
				drawMyUVLMPanels();
			}
			if (drawGust)
			{
				drawMyGust();
			}
		}
		else
		{
			if (drawAxes)
			{
				drawMyAxes();
			}
			if (drawWakes)
			{
				drawMyWakeVortices();
			}
			if (drawAnnotations)
			{
				drawMyAnnotations();
			}
			if (drawControlPoints)
			{
				drawMyControlPointsNet();
				// drawMyDeformedControlPointsNet();
			}
			if (drawInterpolatedPoints)
			{
				drawMyInterpolatedPointsNet();
			}
			if (drawTracks)
			{
				drawMyTracks();
			}
			if (drawPanels)
			{
				drawMyUVLMPanels();
			}
			if (drawNormals)
			{
				drawMyNormals();
			}
			if (drawVorticities)
			{
				drawMyVorticitiesNet();
			}
			if (drawBoundVortices)
			{
				drawMyBoundVortices();
			}
			if (drawPressure)
			{
				drawMyPressureDistributionNet();
			}
			if (drawVelocityField)
			{
				drowMyVelocityField();
			}
			if (drawGust)
			{
				drawMyGust();
			}
			if (IsModeEulerBernoulli)
			{
				if (drawBeamElements)
				{
					// drawMyBeamElements();
				}
				if (drawBeamNodes)
				{
					// drawMyBeamNodes();
				}
				if (drawBeamGhostNodes)
				{
					// drawMyBeamGhostNodes();
				}
				if (drawBeamLoadVectors)
				{
					// drawMyBeamLoadVectors();
				}
				if (drawBeamGhostElements)
				{
					// drawMyBeamGhostElements();
					// drawMyBeamDeflectedGhostElements();
				}
				if (drawBeamDeflectedNodes)
				{
					// drawMyBeamDeflectedNodes();
					// drawMyBeamDeflectedGhostNodes();
				}
			}

			if ((drawTranslateViewWithSurface) && (!drawTranslationCompleteSemaphore))
			{
				// Translate view.
				glPanCentreX = glPanCentreX + project->get_MySurfaces()->at(0)->get_MyPanels()->at(0).at(0)->get_MyMidPointEarthVelocity()->get_X() / project->get_FramesPerSecond();
				glPanCentreY = glPanCentreY + project->get_MySurfaces()->at(0)->get_MyPanels()->at(0).at(0)->get_MyMidPointEarthVelocity()->get_Y() / project->get_FramesPerSecond();

				// Adjust viewport view.
				setViewOrtho(myWidth, myHeight);

				// Force redraw.
				update();

				drawTranslationCompleteSemaphore = true;
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

void MyGLWidget::drawMyInterpolatedPointsNet()
{
	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(0.1f);

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

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyUVLMPanels()
{
	// Set the colour to yellow.
	glColor3f(1.0f, 1.0f, 0.0f);

	// Draw each surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

				if (!(p->get_IgnoreForces()))
				{
					if (p->get_IsControlSurface())
					{
						// Shade in white.
						glBegin(GL_QUADS);
						glColor3d(1, 1, 1);
						glVertex3f(p->get_BottomLeftPoint()->get_X(), p->get_BottomLeftPoint()->get_Y(), p->get_BottomLeftPoint()->get_Z());
						glColor3d(1, 1, 1);
						glVertex3f(p->get_BottomRightPoint()->get_X(), p->get_BottomRightPoint()->get_Y(), p->get_BottomRightPoint()->get_Z());
						glColor3d(1, 1, 1);
						glVertex3f(p->get_TopRightPoint()->get_X(), p->get_TopRightPoint()->get_Y(), p->get_TopRightPoint()->get_Z());
						glColor3d(1, 1, 1);
						glVertex3f(p->get_TopLeftPoint()->get_X(), p->get_TopLeftPoint()->get_Y(), p->get_TopLeftPoint()->get_Z());
						glEnd();
					}
					else
					{
						glBegin(GL_QUADS);
						glColor3d(1, 0, 1);
						glVertex3f(p->get_BottomLeftPoint()->get_X(), p->get_BottomLeftPoint()->get_Y(), p->get_BottomLeftPoint()->get_Z());
						glColor3d(1, 0, 1);
						glVertex3f(p->get_BottomRightPoint()->get_X(), p->get_BottomRightPoint()->get_Y(), p->get_BottomRightPoint()->get_Z());
						glColor3d(1, 1, 0);
						glVertex3f(p->get_TopRightPoint()->get_X(), p->get_TopRightPoint()->get_Y(), p->get_TopRightPoint()->get_Z());
						glColor3d(0, 1, 1);
						glVertex3f(p->get_TopLeftPoint()->get_X(), p->get_TopLeftPoint()->get_Y(), p->get_TopLeftPoint()->get_Z());
						glEnd();
					}
				}
			}
		}
	}
}

void MyGLWidget::drawMyWakeVortices()
{
	// Actually do the drawing.
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(0.1);

	float xOffset = 0.0;
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyWakePanels()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).size(); j++)
			{
				ITWakePanel* currentWakePanel = project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(i).at(j);

				for (int t = 0; t < currentWakePanel->get_MyVortices()->size(); t++)
				{
					glBegin(GL_LINES);
					glVertex3f(currentWakePanel->get_MyVortices()->at(t)->get_StartPoint()->get_X() + xOffset,
						currentWakePanel->get_MyVortices()->at(t)->get_StartPoint()->get_Y(),
						currentWakePanel->get_MyVortices()->at(t)->get_StartPoint()->get_Z());
					glVertex3f(currentWakePanel->get_MyVortices()->at(t)->get_EndPoint()->get_X() + xOffset,
						currentWakePanel->get_MyVortices()->at(t)->get_EndPoint()->get_Y(),
						currentWakePanel->get_MyVortices()->at(t)->get_EndPoint()->get_Z());
					glEnd();
				}
			}
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyBoundVortices()
{
	// Actually do the drawing.
	glColor3f(0.0f, 1.0f, 0.5f);
	glLineWidth(0.1f);

	float xOffset = 0.0;
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				ITPanel* currentPanel = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

				for (int t = 0; t < currentPanel->get_MyVortices()->size(); t++)
				{
					glBegin(GL_LINES);
					glVertex3f(currentPanel->get_MyVortices()->at(t)->get_StartPoint()->get_X() + xOffset,
						currentPanel->get_MyVortices()->at(t)->get_StartPoint()->get_Y(),
						currentPanel->get_MyVortices()->at(t)->get_StartPoint()->get_Z());
					glVertex3f(currentPanel->get_MyVortices()->at(t)->get_EndPoint()->get_X() + xOffset,
						currentPanel->get_MyVortices()->at(t)->get_EndPoint()->get_Y(),
						currentPanel->get_MyVortices()->at(t)->get_EndPoint()->get_Z());
					glEnd();
				}
			}
		}
	}

	glLineWidth(1.0f);
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
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

				glBegin(GL_LINE_STRIP);
				glVertex3f(p->get_MidPoint()->get_X(),
					p->get_MidPoint()->get_Y(),
					p->get_MidPoint()->get_Z());

				glVertex3f(p->get_MidPoint()->get_X() + factor * p->get_Normal()->get_X(),
					p->get_MidPoint()->get_Y() + factor * p->get_Normal()->get_Y(),
					p->get_MidPoint()->get_Z() + factor * p->get_Normal()->get_Z());

				glEnd();
			}
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drowMyVelocityField()
{
	// Draw a box around the field.
	float minx = project->get_VelocityFieldMinx();
	float maxx = project->get_VelocityFieldMaxx();

	float miny = project->get_VelocityFieldMiny();
	float maxy = project->get_VelocityFieldMaxy();

	float minz = project->get_VelocityFieldMinz();
	float maxz = project->get_VelocityFieldMaxz();

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(0.1f);

	// Start with a min x square.
	glBegin(GL_LINE_LOOP);
	glVertex3f(minx, miny, minz);
	glVertex3f(minx, maxy, minz);
	glVertex3f(minx, maxy, maxz);
	glVertex3f(minx, miny, maxz);
	glEnd();

	// Now a max x square.
	glBegin(GL_LINE_LOOP);
	glVertex3f(maxx, miny, minz);
	glVertex3f(maxx, maxy, minz);
	glVertex3f(maxx, maxy, maxz);
	glVertex3f(maxx, miny, maxz);
	glEnd();

	// Now a max z square.
	glBegin(GL_LINE_LOOP);
	glVertex3f(minx, miny, maxz);
	glVertex3f(maxx, miny, maxz);
	glVertex3f(maxx, maxy, maxz);
	glVertex3f(minx, maxy, maxz);
	glEnd();

	// Now a min z square.
	glBegin(GL_LINE_LOOP);
	glVertex3f(minx, miny, minz);
	glVertex3f(maxx, miny, minz);
	glVertex3f(maxx, maxy, minz);
	glVertex3f(minx, maxy, minz);
	glEnd();

	float scaleFactor = project->get_VelocityFieldDisplayFactor();

	// Actually draw the data.
	for (int i = 0; i < project->get_VelocityFieldData()->size(); i++)
	{
		for (int j = 0; j < project->get_VelocityFieldData()->at(i).size(); j++)
		{
			// Draw a single vector (just the y and z components).

			ITPoint *v = project->get_VelocityFieldData()->at(i).at(j);
			glBegin(GL_LINES);

			glVertex3f(v->get_X(), v->get_Y(), v->get_Z());

			glVertex3f(v->get_X() + scaleFactor*v->get_VX(),
				v->get_Y() + scaleFactor*v->get_VY(),
				v->get_Z() + scaleFactor*v->get_VZ());

			glEnd();
		}
	}
	
	// Now draw colour quads.
	float sideLength = (project->get_VelocityFieldMaxy() - project->get_VelocityFieldMiny()) / project->get_VelocityFieldNy();
	float halfSideLength = sideLength / 2.0;

	for (int i = 0; i < project->get_VelocityFieldData()->size(); i++)
	{
		for (int j = 0; j < project->get_VelocityFieldData()->at(i).size(); j++)
		{
			// Draw a quad with colour proportional to magnitude.
			ITPoint *v = project->get_VelocityFieldData()->at(i).at(j);

			float upness = v->get_VZ();
			if (fabs(upness) > 0.1)
			{
				float r = 0.0;
				float b = 0.0;

				if (upness > 0.0)
				{
					r = 0.0;
					b = upness;
				}
				else
				{
					r = -upness;
					b = 0.0;
				}

				glColor4f(r, b, 0, 0.5);

				glBegin(GL_QUADS);
				glVertex3f(v->get_X(), v->get_Y() - halfSideLength, v->get_Z() - halfSideLength);
				glVertex3f(v->get_X(), v->get_Y() + halfSideLength, v->get_Z() - halfSideLength);
				glVertex3f(v->get_X(), v->get_Y() + halfSideLength, v->get_Z() + halfSideLength);
				glVertex3f(v->get_X(), v->get_Y() - halfSideLength, v->get_Z() + halfSideLength);
				glEnd();
			}

		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyGust()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyGust");

	// Set the colour to blue.
	glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(0.1f);

	// Draw a grid of gust velocities.
	float sizeFactor = 3.0;

	for (int k = -10; k < 10; k++)
	{
		for (int i = -10; i < 10; i++)
		{
			// Draw line.
			glBegin(GL_LINE_STRIP);

			for (int j = -10; j < 10; j++)
			{
				// Define the point.
				float x = project->get_MyGust()->get_X0() + i*sizeFactor;
				float y = project->get_MyGust()->get_Y0() + j*sizeFactor;
				float z = project->get_MyGust()->get_Z0() + k*sizeFactor;

				// Evaluate the gust velocity.
				float vx;
				float vy;
				float vz;

				project->get_MyGust()->computeVelocityAtPoint(x, y, z, &vx, &vy, &vz);
				// project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "z: %f, vz: %f", z, vz);
				// glVertex3f(x, y, z);

				glVertex3f(x + vx, y + vy, z + vz);
			}

			glEnd();
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyPressureDistributionNet()
{
	// Draw each surface.
	float factor = project->get_PressureDisplayFactor(); // The factor needs to be positive so that the pressure net is rendered on the leeward side of the surface.

	glColor3f(1.0f, 0.0f, 0.0f); // Red.
	glLineWidth(0.1f);

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Lines in the u direction.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}
			glEnd();
		}

		// Lines in the v direction.
		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size(); j++)
		{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_DeltaPressure()*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}
			glEnd();
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyVorticitiesNet()
{
	float factor = project->get_VorticityDisplayFactor();

	// Set the colour.
	glColor3f(1.0f, 0.0f, 0.5f);
	glLineWidth(0.1f);

	// Draw net.
	// Loop over surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

				glVertex3f(p->get_MidPoint()->get_X() + factor * p->get_MyVorticity() * p->get_Normal()->get_X(),
					p->get_MidPoint()->get_Y() + factor * p->get_MyVorticity() * p->get_Normal()->get_Y(),
					p->get_MidPoint()->get_Z() + factor * p->get_MyVorticity() * p->get_Normal()->get_Z());
			}

			glEnd();
		}

		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size(); j++)
		{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
			{
				ITPanel *p = project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j);

				glVertex3f(p->get_MidPoint()->get_X() + factor * p->get_MyVorticity() * p->get_Normal()->get_X(),
					p->get_MidPoint()->get_Y() + factor * p->get_MyVorticity() * p->get_Normal()->get_Y(),
					p->get_MidPoint()->get_Z() + factor * p->get_MyVorticity() * p->get_Normal()->get_Z());
			}

			glEnd();
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyAnnotations()
{
	// Draw each surface number.
	glColor3f(1.0f, 1.0f, 1.0f); // White

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		ITPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).at(0);
		renderText(p->get_X(), p->get_Y(), p->get_Z(), QString::number(k));
	}

	// Draw each wake generation.
	if (!IsReplay)
	{
		if (project->get_MaxPropagationGeneration() == 0)
		{
			glColor3f(1.0f, 1.0f, 1.0f); // White

			for (int k = 0; k < project->get_MySurfaces()->size(); k++)
			{
				for (int g = 0; g < project->get_MySurfaces()->at(k)->get_MyWakePanels()->size(); g++)
				{
					ITWakePanel *p = project->get_MySurfaces()->at(k)->get_MyWakePanels()->at(g).at(0);
					renderText(p->get_BottomLeftPoint()->get_X(), p->get_BottomLeftPoint()->get_Y(), p->get_BottomLeftPoint()->get_Z(), QString::number(g));
				}
			}
		}
	}
}

void MyGLWidget::drawMyTracks()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyTracks.");

	glColor3f(1.0f, 0.0f, 0.0f); // Red.
	glLineWidth(0.1f);

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		glBegin(GL_LINE_STRIP);

		// Loop through the frames for the first corner control point.
		for (int n = 0; n < FrameNumber; n++)
		{
			ITPoint *translationPoint = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *rotationPoint = new ITPoint(0.0, 0.0, 0.0);

			project->get_MySurfaces()->at(k)->computeNetTrajectoryPointsAtFrame(n, k, translationPoint, rotationPoint);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "drawing translationPoint x: %f, y: %f, z: %f", translationPoint->get_X(), translationPoint->get_Y(), translationPoint->get_Z());

			ITPoint *currentPoint = new ITPoint(0.0, 0.0, 0.0);

			currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_X());
			currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_Y());
			currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(0).at(0)->get_Z());

			ITPoint *cp = project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint();
			currentPoint->propagateMeWithMorph(cp, rotationPoint, translationPoint, project->get_MySurfaces()->at(k)->get_IsMorph(), project->get_MySurfaces()->at(k)->get_MorphStartFrame(), project->get_MySurfaces()->at(k)->get_MorphEndFrame(), project->get_MySurfaces()->at(k)->get_MorphType());

			glVertex3f(currentPoint->get_X(), currentPoint->get_Y(), currentPoint->get_Z());

			delete translationPoint;
			delete rotationPoint;
			delete currentPoint;
		}

		glEnd();
		glBegin(GL_LINE_STRIP);

		// Loop through the frames for the second corner control point.
		for (int n = 0; n < FrameNumber; n++)
		{
			ITPoint *translationPoint = new ITPoint(0.0, 0.0, 0.0);
			ITPoint *rotationPoint = new ITPoint(0.0, 0.0, 0.0);

			project->get_MySurfaces()->at(k)->computeNetTrajectoryPointsAtFrame(n, k, translationPoint, rotationPoint);

			ITPoint *currentPoint = new ITPoint(0.0, 0.0, 0.0);

			currentPoint->set_X(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_X());
			currentPoint->set_Y(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_Y());
			currentPoint->set_Z(project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->back().at(0)->get_Z());

			ITPoint *cp = project->get_MyBaseSurfaces()->at(k)->get_MyCentreOfRotationPoint();
			currentPoint->propagateMeWithMorph(cp, rotationPoint, translationPoint, project->get_MySurfaces()->at(k)->get_IsMorph(), project->get_MySurfaces()->at(k)->get_MorphStartFrame(), project->get_MySurfaces()->at(k)->get_MorphEndFrame(), project->get_MySurfaces()->at(k)->get_MorphType());

			glVertex3f(currentPoint->get_X(), currentPoint->get_Y(), currentPoint->get_Z());

			delete translationPoint;
			delete rotationPoint;
			delete currentPoint;
		}

		glEnd();
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyWakePanelHistory()
{
	glColor3f(1.0f, 1.0f, 0.0f);
	glLineWidth(0.1f);

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(FrameNumber).size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(FrameNumber).at(i).size(); j++)
			{
				ITWakePanel* currentWakePanel = project->get_MySurfaces()->at(k)->get_MyWakePanelHistory()->at(FrameNumber).at(i).at(j);

				glBegin(GL_LINES);

				glVertex3f(currentWakePanel->get_BottomLeftPoint()->get_X(),
					currentWakePanel->get_BottomLeftPoint()->get_Y(),
					currentWakePanel->get_BottomLeftPoint()->get_Z());
				glVertex3f(currentWakePanel->get_BottomRightPoint()->get_X(),
					currentWakePanel->get_BottomRightPoint()->get_Y(),
					currentWakePanel->get_BottomRightPoint()->get_Z());

				glVertex3f(currentWakePanel->get_BottomRightPoint()->get_X(),
					currentWakePanel->get_BottomRightPoint()->get_Y(),
					currentWakePanel->get_BottomRightPoint()->get_Z());
				glVertex3f(currentWakePanel->get_TopRightPoint()->get_X(),
					currentWakePanel->get_TopRightPoint()->get_Y(),
					currentWakePanel->get_TopRightPoint()->get_Z());

				glVertex3f(currentWakePanel->get_TopRightPoint()->get_X(),
					currentWakePanel->get_TopRightPoint()->get_Y(),
					currentWakePanel->get_TopRightPoint()->get_Z());
				glVertex3f(currentWakePanel->get_TopLeftPoint()->get_X(),
					currentWakePanel->get_TopLeftPoint()->get_Y(),
					currentWakePanel->get_TopLeftPoint()->get_Z());

				glVertex3f(currentWakePanel->get_TopLeftPoint()->get_X(),
					currentWakePanel->get_TopLeftPoint()->get_Y(),
					currentWakePanel->get_TopLeftPoint()->get_Z());
				glVertex3f(currentWakePanel->get_BottomLeftPoint()->get_X(),
					currentWakePanel->get_BottomLeftPoint()->get_Y(),
					currentWakePanel->get_BottomLeftPoint()->get_Z());

				glEnd();
			}
		}
	}
	
	// Reset line width.
	glLineWidth(1.0f);
}

void MyGLWidget::drawMyPressureHistory()
{
	glColor3f(1.0f, 0.0f, 0.0f); // Red.
	glLineWidth(0.1f);

	float factor = project->get_PressureDisplayFactor(); // The factor needs to be positive so that the pressure net is rendered on the leeward side of the surface.

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Lines in the u direction.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}

			glEnd();
		}

		// Lines in the v direction.
		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size(); j++)
		{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->get_MyPressureHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}
			glEnd();
		}
	}

	glLineWidth(1.0f);
}

void MyGLWidget::drawMyVorticityHistory()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyVorticityHistory.");

	// Set the colour.
	glColor3f(1.0f, 0.0f, 0.5f);

	float factor = project->get_VorticityDisplayFactor(); // The factor needs to be positive so that the vorticity net is rendered on the leeward side of the surface.

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Lines in the u direction.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->
					at(k)->get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->
					get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->
					get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}

			glEnd();
		}

		// Lines in the v direction.
		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size(); j++)
		{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
			{
				glVertex3f(project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_X() + factor*project->get_MySurfaces()->at(k)->
					get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_X(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Y() + factor*project->get_MySurfaces()->at(k)->
					get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Y(),
					project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_MidPoint()->get_Z() + factor*project->get_MySurfaces()->at(k)->
					get_MyVorticityHistory()->at(FrameNumber).at(i).at(j)*project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).at(j)->get_Normal()->get_Z());
			}

			glEnd();
		}
	}
}

void MyGLWidget::drawMyBeamNodeHistory()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyBeamNodeHistory.");

	glColor3f(1.0f, 1.0f, 1.0f); // White.

	// Check if there is data to plot.
	// Note that if IsModeEulerBernoulli was false during the simulation, then the node history data will contain unpropagated and relaxed geometry.
	// Draw spheres and text.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->size(); i++)
		{
			ITBeamNode* n = project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber).at(i);

			glTranslatef(n->get_X(), n->get_Y(), n->get_Z());

			float diameter = 40.0*n->get_Mass();

			if (diameter > 0.2)
			{
				diameter = 0.2;
			}

			drawSphere(diameter, 15, 15);
			glTranslatef(-n->get_X(), -n->get_Y(), -n->get_Z());
			renderText(n->get_X(), n->get_Y(), n->get_Z(), QString::number(i));
		}
	}

	// Draw relaxed elements.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Loop around the nodes.
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->size(); i++)
		{
			ITBeamNode* n = project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber).at(i);
			glVertex3f(n->get_X(), n->get_Y(), n->get_Z());
		}

		glEnd();
	}

	// Draw deformed nodes.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->size(); i++)
		{
			ITBeamNode* n = project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber).at(i);

			glTranslatef(n->get_W()->get_X(), n->get_W()->get_Y(), n->get_W()->get_Z());

			float diameter = 40.0*n->get_Mass();
			if (diameter > 0.2)
			{
				diameter = 0.2;
			}

			drawSphere(diameter, 15, 15);
			glTranslatef(-n->get_W()->get_X(), -n->get_W()->get_Y(), -n->get_W()->get_Z());
		}
	}

	// Draw deformed elements.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Loop around the nodes.
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyBeamNodes()->size(); i++)
		{
			ITBeamNode* n = project->get_MySurfaces()->at(k)->get_MyBeam()->get_MyNodeHistory()->at(FrameNumber).at(i);

			glVertex3f(n->get_X() + n->get_W()->get_X(),
				n->get_Y() + n->get_W()->get_Y(),
				n->get_Z() + n->get_W()->get_Z());
		}

		glEnd();
	}
}

void MyGLWidget::drawMyVelocityFieldHistory()
{
	if (project->get_IsDoVelocityField())
	{
		// Draw a box around the field.
		float minx = project->get_VelocityFieldMinx();
		float maxx = project->get_VelocityFieldMaxx();

		float miny = project->get_VelocityFieldMiny();
		float maxy = project->get_VelocityFieldMaxy();

		float minz = project->get_VelocityFieldMinz();
		float maxz = project->get_VelocityFieldMaxz();

		glColor3f(1.0, 1.0, 1.0);

		// Start with a min x square.
		glBegin(GL_LINE_LOOP);
		glVertex3f(minx, miny, minz);
		glVertex3f(minx, maxy, minz);
		glVertex3f(minx, maxy, maxz);
		glVertex3f(minx, miny, maxz);
		glEnd();

		// Now a max x square.
		glBegin(GL_LINE_LOOP);
		glVertex3f(maxx, miny, minz);
		glVertex3f(maxx, maxy, minz);
		glVertex3f(maxx, maxy, maxz);
		glVertex3f(maxx, miny, maxz);
		glEnd();

		// Now a max z square.
		glBegin(GL_LINE_LOOP);
		glVertex3f(minx, miny, maxz);
		glVertex3f(maxx, miny, maxz);
		glVertex3f(maxx, maxy, maxz);
		glVertex3f(minx, maxy, maxz);
		glEnd();

		// Now a min z square.
		glBegin(GL_LINE_LOOP);
		glVertex3f(minx, miny, minz);
		glVertex3f(maxx, miny, minz);
		glVertex3f(maxx, maxy, minz);
		glVertex3f(minx, maxy, minz);
		glEnd();

		float factor = project->get_VelocityFieldDisplayFactor();
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyVelocityFieldHistory i: %i", project->get_VelocityFieldDataHistory()->at(FrameNumber).size());
		glColor3f(1.0f, 1.0f, 1.0f);

		for (int i = 0; i < project->get_VelocityFieldDataHistory()->at(FrameNumber).size(); i++)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyVelocityFieldHistory i: %i,   j: %i", i, project->get_VelocityFieldDataHistory()->at(FrameNumber).at(i).size());

			for (int j = 0; j < project->get_VelocityFieldDataHistory()->at(FrameNumber).at(i).size(); j++)
			{
				ITPoint *v = project->get_VelocityFieldDataHistory()->at(FrameNumber).at(i).at(j);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside drawMyVelocityFieldHistory   x: %f, y: %f, z:%f, ... vy: %f, vz: %f", v->get_X(), v->get_Y(), v->get_Z(), v->get_VY(), v->get_VZ());

				glBegin(GL_LINES);
				glVertex3f(v->get_X(), v->get_Y(), v->get_Z());
				glVertex3f(v->get_X() + factor * v->get_VX(),
					v->get_Y() + factor * v->get_VY(),
					v->get_Z() + factor * v->get_VZ());
				glEnd();
			}
		}

		// Now draw colour quads.
		float sideLength = (project->get_VelocityFieldMaxy() - project->get_VelocityFieldMiny()) / project->get_VelocityFieldNy();
		float halfSideLength = sideLength / 2.0;

		for (int i = 0; i < project->get_VelocityFieldDataHistory()->at(FrameNumber).size(); i++)
		{
			for (int j = 0; j < project->get_VelocityFieldDataHistory()->at(FrameNumber).at(i).size(); j++)
			{
				// Draw a quad with colour proportional to magnitude.
				ITPoint *v = project->get_VelocityFieldDataHistory()->at(FrameNumber).at(i).at(j);

				float upness = v->get_VZ();

				//project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "upness: %f", upness);

				if (fabs(upness) > 0.1)
				{
					float r = 0.0;
					float b = 0.0;

					if (upness > 0.0)
					{
						r = upness;
						b = 0.0;
					}
					else
					{
						r = 0.0;
						b = -upness;
					}

					glColor4f(r, b, 0, 0.5);

					glBegin(GL_QUADS);
					glVertex3f(v->get_X(), v->get_Y() - halfSideLength, v->get_Z() - halfSideLength);
					glVertex3f(v->get_X(), v->get_Y() + halfSideLength, v->get_Z() - halfSideLength);
					glVertex3f(v->get_X(), v->get_Y() + halfSideLength, v->get_Z() + halfSideLength);
					glVertex3f(v->get_X(), v->get_Y() - halfSideLength, v->get_Z() + halfSideLength);
					glEnd();
				}
			}
		}
	} 
}