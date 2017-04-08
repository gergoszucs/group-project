#include "MyXYView.h"

#include <QtOpenGL>
#include <GL/glu.h> // For gluUnproject.
#include <typeinfo> // For typeid
#include <string>

#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITPhysics.h"
#include "UtililityFunctions.h"
#include <math.h>

MyXYView::MyXYView(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	// Initialize drawing parameters.
	xRot = 0;
	yRot = 0;
	zRot = 0;

	setFocusPolicy(Qt::StrongFocus);

	lastPos = QPoint(0, 0);

	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	set_PrimedForFirstClick(false);
	set_PrimedForSecondClick(false);

	// Instanciate the control point.
	ITControlPoint *p = new ITControlPoint(0.0, 0.0, 0.0);
	set_ScratchControlPoint(p);

	get_ScratchControlPoint()->set_K(-1);
	get_ScratchControlPoint()->set_I(-1);
	get_ScratchControlPoint()->set_J(-1);
}

MyXYView::~MyXYView(void)
{
	delete _ScratchControlPoint;
}

void MyXYView::initializeGL()
{
	qglClearColor(Qt::white);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE); // Make sure both sides of QUADS are filled.
}

void MyXYView::paintGL()
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

void MyXYView::resizeGL(int width, int height)
{
	myWidth = width;
	myHeight = height;

	glViewport(0, 0, width, height);

	setViewOrtho(width, height);

	draw();
}

void MyXYView::setViewOrtho(int width, int height)
{
	float aspect = (float)width / (float)height; // Landscape is greater than 1.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Landscape.
	glOrtho(glXYPanCentreX - glXYViewHalfExtent,
		glXYPanCentreX + glXYViewHalfExtent,
		glXYPanCentreY - glXYViewHalfExtent / aspect,
		glXYPanCentreY + glXYViewHalfExtent / aspect,
		-50000.0,
		50000.0);

	glMatrixMode(GL_MODELVIEW);
}

void MyXYView::keyPressEvent(QKeyEvent * event)
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
		glXYPanCentreY = glXYPanCentreY + factor;
	}
	else if (event->key() == Qt::Key_Down)
	{
		glXYPanCentreY = glXYPanCentreY - factor;
	}
	else if (event->key() == Qt::Key_Left)
	{
		glXYPanCentreX = glXYPanCentreX - factor;
	}
	else if (event->key() == Qt::Key_Right)
	{
		glXYPanCentreX = glXYPanCentreX + factor;
	}
	else if (event->key() == Qt::Key_V)
	{
		// User is holding the V key down, so restrict dragging to vertical only.
		IsVerticalDragOnly = true;
		IsHorizontalDragOnly = false;
	}
	else if (event->key() == Qt::Key_H)
	{
		// User is holding the H key down, so restrict dragging to horizontal only.
		IsVerticalDragOnly = false;
		IsHorizontalDragOnly = true;
	}
	else if (event->key() == Qt::Key_A)
	{
		// User is holding the A key down, so remove dragging restrictions.
		IsVerticalDragOnly = false;
		IsHorizontalDragOnly = false;
	}

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Force redraw.
	update();
}

void MyXYView::draw()
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

		if (drawAnnotations)
		{
			//drawMyAnnotations();
		}

		if (drawGrids)
		{
			drawMyGrids();
		}

		if (drawDial)
		{
			drawAngleDial();
		}
		
		drawMyFocusControlPoints();
		drawMyScratchControlPoint();
	}
}

void MyXYView::drawMyScratchControlPoint()
{
	//	std::string s = typeid(get_ScratchControlPoint()).name();
	//	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Type: '%s'", s.c_str());
	//	if (strcmp(s.c_str(), "class ITControlPoint *") == 0)

	if ((get_ScratchControlPoint()->get_K() > -1) || (get_ScratchControlPoint()->get_I() > -1) || (get_ScratchControlPoint()->get_J() > -1))
	{
		//project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Type: %s", s.c_str());

		glTranslatef(get_ScratchControlPoint()->get_X(), get_ScratchControlPoint()->get_Y(), 0.0);
		float radius = glXYViewHalfExtent / 50.0;
		drawSphere(radius, 15, 15, 0.0, 1.0, 0.0);
		glTranslatef(-get_ScratchControlPoint()->get_X(), -get_ScratchControlPoint()->get_Y(), 0.0);
	}
}

void MyXYView::drawMyGrids()
{
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(3, 0xAAAA);
	glColor3f(0.5f, 0.5f, 0.5f); // grey

	// Draw horizontal lines.
	for (int i = -10; i < 11; i++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f((float)i*10.0, -100.0, 0.0);
		glVertex3f((float)i*10.0, 100.0, 0.0);
		glEnd();
	}

	// Draw vertical lines.
	for (int j = -10; j < 11; j++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(-100.0, (float)j*10.0, 0.0);
		glVertex3f(100.0, (float)j*10.0, 0.0);
		glEnd();
	}

	glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LINE_STIPPLE);

}

void MyXYView::drawMyAnnotations()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Do the plotting.
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				renderText(p->get_X(), p->get_Y(), p->get_Z(), QString("k: %1, i: %2, j: %3, \r \n x: %4, y: %5, z: %6").arg(p->get_K()).arg(p->get_I()).arg(p->get_J()).arg(p->get_X()).arg(p->get_Y()).arg(p->get_Z()));
			}
		}
	}
}

void MyXYView::drawMyAxes()
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

void MyXYView::drawAngleDial()
{
	dialSize = glXYViewHalfExtent * 0.25;

	float beginX = dialCentreX + dialSize;
	float beginY = dialCentreY;

	float endX = dialCentreX + dialSize * cosf(dialAngle);
	float endY = dialCentreY + dialSize * sinf(dialAngle);

	glBegin(GL_LINE_STRIP);

	glVertex2f(beginX, beginY);
	glVertex2f(dialCentreX, dialCentreY);
	glVertex2f(endX, endY);

	glEnd();

	float angle = 0;
	float dAngle = 0.001;

	float x = dialCentreX + dialSize * cosf(angle);
	float y = dialCentreY + dialSize * sinf(angle);

	glBegin(GL_LINE_STRIP);

	if (dialAngle > 0)
	{
		while (angle < dialAngle)
		{
			glVertex2f(x, y);

			x = dialCentreX + 0.9 * dialSize * cosf(angle);
			y = dialCentreY + 0.9 * dialSize * sinf(angle);

			angle += dAngle;
		}
	}
	else {
		dAngle = -dAngle;

		while (angle > dialAngle)
		{
			glVertex2f(x, y);

			x = dialCentreX + 0.9 * dialSize * cosf(angle);
			y = dialCentreY + 0.9 * dialSize * sinf(angle);

			angle += dAngle;
		}
	}
	

	glEnd();
	
	float textX = dialCentreX + dialSize * cosf(dialAngle/2) * 1.2;
	float textY = dialCentreY + dialSize * sinf(dialAngle/2) * 1.2;

	renderText(textX, textY, 0, QString("%1 deg").arg(dialAngle * 180 / 3.14));
}

void MyXYView::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();

	if (!(event->modifiers())) // Just clicking without modifiers.
	{
		set_EditValue(event->x()); // Used in perspective transformation.

		if (	(MY_EDIT_MODE == DRAG) ||
				(MY_EDIT_MODE == DRAG_ROW) ||
				(MY_EDIT_MODE == DRAG_COL) ||
				(MY_EDIT_MODE == DRAG_ALL) ||
				(MY_EDIT_MODE == ROTATE_ALL) ||
				(MY_EDIT_MODE == RESIZE_ALL) ||
				(MY_EDIT_MODE == SHEAR_ALL) ||
				(MY_EDIT_MODE == PERSPECTIVE_ALL) ||
				(MY_EDIT_MODE == FLIP_HORIZONTAL_ALL) ||
				(MY_EDIT_MODE == COPY_SURFACE) ||
				(MY_EDIT_MODE == DELETE_SURFACE) ||
				(MY_EDIT_MODE == INSERT_ROW) ||
				(MY_EDIT_MODE == DELETE_ROW) ||
				(MY_EDIT_MODE == DUPLICATE_ROW) ||
				(MY_EDIT_MODE == INSERT_COL) ||
				(MY_EDIT_MODE == DELETE_COL) ||
				(MY_EDIT_MODE == DUPLICATE_COL) ||
				(MY_EDIT_MODE == MATE_POINTS) ||
				(MY_EDIT_MODE == MERGE_SURFACES_BY_ROW) ||
				(MY_EDIT_MODE == MERGE_SURFACES_BY_ROW_REVERSE) ||
				(MY_EDIT_MODE == COPY_SURFACE_MIRROR) ||
				(MY_EDIT_MODE == MEASURE_DISTANCE) ||
				(MY_EDIT_MODE == CENTRED_ROTATE)
			)
		{
			AssignFocusPoint(event); // Get the indices of the focus point, and get ready for a mouse move.
		} 

		if (MY_EDIT_MODE == ROTATE_ALL)
		{
			dialAngle = 0;

			getInAxesPosition(dialCentreX, dialCentreY, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

			drawDial = true;
		}
	}
}

void MyXYView::AssignFocusPoint(QMouseEvent *event)
{
	float posX, posY, posZ;
	
	getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

	// Find the vertex that is closest.
	int i, j, k;
	findControlPointIndicesNearMouse(posX, posY, posZ, &k, &i, &j);

	// Make sure lastPos is reset.
	lastPos = event->pos();

	// Make sure edit value is reset.
	set_EditValue(0.0);
	set_EditValueX(0.0);
	set_EditValueY(0.0);

	if (MY_EDIT_MODE == CENTRED_ROTATE)
	{
		// This edit mode is unique in that the centre of the rotation need not be a control point.
		if (get_PrimedForFirstClick())
		{
			// Empty the focus vectors.
			w->emptyFocusVectors();

			// Save the centre of rotation.
			ITControlPoint *p = new ITControlPoint(posX, posY, posZ);

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Centre of rotation scratch point captured. X: %f, Y: %f, Z: %f", p->get_X(), p->get_Y(), p->get_Z());

			get_ScratchControlPoint()->set_X(posX);
			get_ScratchControlPoint()->set_Y(posY);
			get_ScratchControlPoint()->set_Z(0.0);

			get_ScratchControlPoint()->set_I(0);
			get_ScratchControlPoint()->set_J(0);
			get_ScratchControlPoint()->set_K(0);

			// Cancel primed for first click and prime for second click.
			set_PrimedForFirstClick(false);
			set_PrimedForSecondClick(true);
			set_SecondClicksFinished(false);
		}
		else if (get_PrimedForSecondClick())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Primed for second click is TRUE");

			if ((k > -1) && (i > -1) && (j > -1))
			{
				// User has clicked on a control point so push it onto the focus points.
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Focus point captured.");
			}
			else
			{
				// The user has finished clicking control points by clicking away from the surface, so do the rotation of the focus points about the scratch point.
				// Remember to delete the scratch point on mouse release.

				// Cancel primed for all clicks.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);
				set_SecondClicksFinished(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We've clicked away from the control points while primed for second click so set secondClicksFinished to TRUE.");
			}
		}
	}

	if ((k > -1) && (i > -1) && (j > -1))
	{
		if (MY_EDIT_MODE == DRAG)
		{
			// We have found a control point, so add it to the focus vector.
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
		}
		else if (MY_EDIT_MODE == DRAG_ROW)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
			} // End of j loop.
		}
		else if (MY_EDIT_MODE == INSERT_ROW)
		{
			// We have found a control point, so insert a row of control points.
			int noOfRows = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();
			if (i < noOfRows - 1) // A
			{
				// Create a new vector.
				std::vector <ITControlPoint *> dummy_v;
				std::vector <ITControlPoint *> dummy_base_v;

				// Loop over columns.
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					float x = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i + 1).at(j)->get_X());
					float y = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i + 1).at(j)->get_Y());
					float z = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i + 1).at(j)->get_Z());

					// Main surface.
					ITControlPoint *p = new ITControlPoint(x, y, z);

					p->set_U(0.0);
					p->set_V(0.0);

					// Base surface
					ITControlPoint *pb = new ITControlPoint(x, y, z);

					pb->set_U(0.0);
					pb->set_V(0.0);

					dummy_v.push_back(p);
					dummy_base_v.push_back(pb);
				}

				// Insert the row.
				std::vector < std::vector <ITControlPoint*> >::iterator it;
				it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->begin();
				project->get_MySurfaces()->at(k)->get_MyControlPoints()->insert(it + i + 1, dummy_v);

				std::vector < std::vector <ITControlPoint*> >::iterator itb;
				itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->begin();
				project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->insert(itb + i + 1, dummy_base_v);

				// Re-assign the _K, _I and _J variables.
				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
				{
					for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
					{
						ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
						p->set_K(k);
						p->set_I(i);
						p->set_J(j);

						ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
						pb->set_K(k);
						pb->set_I(i);
						pb->set_J(j);
					}
				}

				// Finally recompute the Bezier surface.
				project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

				// Mark the project as changed.
				UnsavedChanges = true;
			}
			else
			{
				// The user is trying to insert a row after the last row which is not possible, so display a warning dialogue.
				QMessageBox::StandardButton reply;
				reply = QMessageBox::warning(this, "Insert Row", "You cannot insert a row after the last row");
			}
		}
		else if (MY_EDIT_MODE == INSERT_COL)
		{
			// We have found a control point, so insert a row of control points.
			int noOfCols = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
			if (j < noOfCols - 1) // A
			{
				// Loop over the rows
				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
				{
					float x = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j + 1)->get_X());
					float y = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j + 1)->get_Y());
					float z = 0.5 * (project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z() + project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j + 1)->get_Z());

					// Main surface.
					ITControlPoint *p = new ITControlPoint(x, y, z);

					p->set_U(0.0);
					p->set_V(0.0);

					// Base surface
					ITControlPoint *pb = new ITControlPoint(x, y, z);

					pb->set_U(0.0);
					pb->set_V(0.0);

					// Insert the point.
					std::vector <ITControlPoint*>::iterator it;
					it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).insert(it + j + 1, p);

					// Insert the point.
					std::vector <ITControlPoint*>::iterator itb;
					itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
					project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).insert(itb + j + 1, p);
				}

				// Re-assign the _K, _I and _J variables.
				for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
				{
					for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
					{
						ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
						p->set_K(k);
						p->set_I(i);
						p->set_J(j);

						ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
						pb->set_K(k);
						pb->set_I(i);
						pb->set_J(j);
					}
				}

				// Finally recompute the Bezier surface.
				project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

				// Mark the project as changed.
				UnsavedChanges = true;
			}
			else
			{
				// The user is trying to insert a row after the last row which is not possible, so display a warning dialogue.
				QMessageBox::StandardButton reply;
				reply = QMessageBox::warning(this, "Insert Column", "You cannot insert a column after the last column");
			}
		}
		else if (MY_EDIT_MODE == DUPLICATE_ROW)
		{
			// We have found a control point, so insert a row of control points.
			int noOfRows = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();

			// Create a new vector.
			std::vector <ITControlPoint *> dummy_v;
			std::vector <ITControlPoint *> dummy_base_v;

			// Loop over columns.
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				float x = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X();
				float y = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y();
				float z = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z();

				// Main surface.
				ITControlPoint *p = new ITControlPoint(x, y, z);

				p->set_U(0.0);
				p->set_V(0.0);

				// Base surface
				ITControlPoint *pb = new ITControlPoint(x, y, z);

				pb->set_U(0.0);
				pb->set_V(0.0);

				dummy_v.push_back(p);
				dummy_base_v.push_back(pb);
			}

			// Insert the row.
			std::vector < std::vector <ITControlPoint*> >::iterator it;
			it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->begin();
			project->get_MySurfaces()->at(k)->get_MyControlPoints()->insert(it + i + 1, dummy_v);

			std::vector < std::vector <ITControlPoint*> >::iterator itb;
			itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->begin();
			project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->insert(itb + i + 1, dummy_base_v);

			// Re-assign the _K, _I and _J variables.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					p->set_K(k);
					p->set_I(i);
					p->set_J(j);

					ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					pb->set_K(k);
					pb->set_I(i);
					pb->set_J(j);
				}
			}

			// Finally recompute the Bezier surface.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

			// Mark the project as changed.
			UnsavedChanges = true;

		}
		else if (MY_EDIT_MODE == DUPLICATE_COL)
		{
			// We have found a control point, so insert a column of control points.
			// Loop over the rows.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				float x = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X();
				float y = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y();
				float z = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z();

				// Main surface.
				ITControlPoint *p = new ITControlPoint(x, y, z);

				p->set_U(0.0);
				p->set_V(0.0);

				// Base surface
				ITControlPoint *pb = new ITControlPoint(x, y, z);

				pb->set_U(0.0);
				pb->set_V(0.0);

				// Insert points.
				std::vector <ITControlPoint*>::iterator it;
				it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
				project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).insert(it + j + 1, p);

				std::vector <ITControlPoint*>::iterator itb;
				itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
				project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).insert(itb + j + 1, pb);
			}

			// Re-assign the _K, _I and _J variables.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					p->set_K(k);
					p->set_I(i);
					p->set_J(j);

					ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					pb->set_K(k);
					pb->set_I(i);
					pb->set_J(j);
				}
			}

			// Finally recompute the Bezier surface.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

			// Mark the project as changed.
			UnsavedChanges = true;
		}
		else if (MY_EDIT_MODE == DELETE_COL)
		{
			// In principle, the user is free to delete any column.
			// Loop over all rows, removing a point every time.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				// Remove a point.
				std::vector <ITControlPoint*>::iterator it;
				it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
				project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).erase(it + j);

				std::vector <ITControlPoint*>::iterator itb;
				itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).begin();
				project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).erase(itb + j);
			}

			// Re-assign the _K, _I and _J variables.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					p->set_K(k);
					p->set_I(i);
					p->set_J(j);

					ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					pb->set_K(k);
					pb->set_I(i);
					pb->set_J(j);
				}
			}

			// Finally recompute the Bezier surface.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

			// Mark the project as changed.
			UnsavedChanges = true;
		}
		else if (MY_EDIT_MODE == DELETE_ROW)
		{
			// In principle, the user is free to delete any row.
			std::vector < std::vector <ITControlPoint*> >::iterator it;
			it = project->get_MySurfaces()->at(k)->get_MyControlPoints()->begin();
			project->get_MySurfaces()->at(k)->get_MyControlPoints()->erase(it + i);

			std::vector < std::vector <ITControlPoint*> >::iterator itb;
			itb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->begin();
			project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->erase(itb + i);

			// Re-assign the _K, _I and _J variables.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					p->set_K(k);
					p->set_I(i);
					p->set_J(j);

					ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					pb->set_K(k);
					pb->set_I(i);
					pb->set_J(j);
				}
			}

			// Finally recompute the Bezier surface.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

			// Mark the project as changed.
			UnsavedChanges = true;
		}
		else if (MY_EDIT_MODE == DRAG_COL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
			}
		}
		else if (MY_EDIT_MODE == ROTATE_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Focusing. k: %i, i: %i, j: %i", k, i, j);

					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Number of focus points: %i", project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size());
		}
		else if (MY_EDIT_MODE == DRAG_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
		}
		else if (MY_EDIT_MODE == RESIZE_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
		}
		else if (MY_EDIT_MODE == SHEAR_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
		}
		else if (MY_EDIT_MODE == PERSPECTIVE_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}
		}
		else if (MY_EDIT_MODE == FLIP_HORIZONTAL_ALL)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}

			// Do the flip straight away, without waiting for mouse move.
			flipAllHorizontally();
		}
		else if (MY_EDIT_MODE == COPY_SURFACE)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}

			// Create the new surface.
			copyNewSurface(k);

			// Reset the buttons.
			w->resetModeButtons();

			// Empty the focus vectors.
			w->emptyFocusVectors();

			MY_EDIT_MODE = NONE;

			// Set changes flag.
			UnsavedChanges = true;
		}
		else if (MY_EDIT_MODE == COPY_SURFACE_MIRROR)
		{
			// We have found a control point, so add all control points for the k-th surface to the focus vector.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->push_back(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j));
				}
			}

			// Create the new surface.
			copyNewSurfaceMirror(k);

			// Reset the buttons.
			w->resetModeButtons();

			// Empty the focus vectors.
			w->emptyFocusVectors();

			MY_EDIT_MODE = NONE;

			// Set changes flag.
			UnsavedChanges = true;

		}
		else if (MY_EDIT_MODE == DELETE_SURFACE)
		{
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Delete Surface", "Are you sure you want to delete this surface?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes)
			{
				// We have found a control point, so delete the k-th surface.
				project->get_MySurfaces()->erase(project->get_MySurfaces()->begin() + k);
				project->get_MyBaseSurfaces()->erase(project->get_MyBaseSurfaces()->begin() + k);

				// Re-assign control point _K, _I and _J indices.
				// Re-assign the _K, _I and _J variables.
				for (int k = 0; k < project->get_MySurfaces()->size(); k++)
				{
					for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
					{
						for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
						{
							ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
							p->set_K(k);
							p->set_I(i);
							p->set_J(j);

							ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
							pb->set_K(k);
							pb->set_I(i);
							pb->set_J(j);
						}
					}
				}

				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();

				MY_EDIT_MODE = NONE;
			}
			else
			{
				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();

				MY_EDIT_MODE = NONE;
			}
		}
		else if (MY_EDIT_MODE == MATE_POINTS)
		{
			if (get_PrimedForFirstClick())
			{
				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Save the first point indices.
				get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
				get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

				get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
				get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
				get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

				// Cancel primed for first click and prime for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First point has been captured.");
			}
			else if (get_PrimedForSecondClick())
			{
				// Get second point and move it to the first one.
				ITControlPoint *firstPoint = get_ScratchControlPoint();
				ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				ITControlPoint *secondBasePoint = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				secondPoint->set_X(firstPoint->get_X());
				secondPoint->set_Y(firstPoint->get_Y());
				secondPoint->set_Z(firstPoint->get_Z());

				secondBasePoint->set_X(firstPoint->get_X());
				secondBasePoint->set_Y(firstPoint->get_Y());
				secondBasePoint->set_Z(firstPoint->get_Z());

				// Cancel primed for first click and cancel primed for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);

				// Reset the scratch point.
				get_ScratchControlPoint()->set_X(0.0);
				get_ScratchControlPoint()->set_Y(0.0);
				get_ScratchControlPoint()->set_Z(0.0);

				get_ScratchControlPoint()->set_K(-1);
				get_ScratchControlPoint()->set_I(-1);
				get_ScratchControlPoint()->set_J(-1);

				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Finally recompute all the Bezier surface.
				for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++)
				{
					project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();
				}

				// Mark the project as changed.
				UnsavedChanges = true;

				MY_EDIT_MODE = NONE;
			}
		}
		else if (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW)
		{
			if (get_PrimedForFirstClick())
			{
				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Save the first point indices.
				get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
				get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

				get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
				get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
				get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

				// Cancel primed for first click and prime for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First surface has been captured.");
			}
			else if (get_PrimedForSecondClick())
			{
				// Get second point and move it to the first one.
				ITControlPoint *firstPoint = get_ScratchControlPoint();
				ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				ITControlPoint *secondBasePoint = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				// Check that the number of columns for the two surfaces is the same.
				int k1 = firstPoint->get_K();
				int k2 = secondPoint->get_K();
				int noOfCols1 = project->get_MySurfaces()->at(k1)->get_MyControlPoints()->at(0).size();
				int noOfCols2 = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(0).size();
				if (noOfCols1 == noOfCols2)
				{
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second surface has been captured.");

					mergeSurfacesByRow(firstPoint->get_K(), secondPoint->get_K());

					// Cancel primed for first click and cancel primed for second click.
					set_PrimedForFirstClick(false);
					set_PrimedForSecondClick(false);

					// Reset the buttons.
					w->resetModeButtons();

					// Reset the scratch point.
					get_ScratchControlPoint()->set_X(0.0);
					get_ScratchControlPoint()->set_Y(0.0);
					get_ScratchControlPoint()->set_Z(0.0);

					get_ScratchControlPoint()->set_K(-1);
					get_ScratchControlPoint()->set_I(-1);
					get_ScratchControlPoint()->set_J(-1);

					// Empty the focus vectors.
					w->emptyFocusVectors();

					// Finally recompute all the Bezier surface.
					for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++)
					{
						project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Calling manageComputationOfInterpolatedPoints on surface: %i", kk);
						project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();
					}

					// Mark the project as changed.
					UnsavedChanges = true;
				}
				else
				{
					QMessageBox::StandardButton reply;
					reply = QMessageBox::warning(this, "Merge Surfaces", "You cannot merge surfaces with different numbers of control point columns.");

					// Reset the buttons.
					w->resetModeButtons();

					// Reset the edit mode.
					MY_EDIT_MODE = NONE;
				}
			} 
		}
		else if (MY_EDIT_MODE == MERGE_SURFACES_BY_ROW_REVERSE)
		{
			if (get_PrimedForFirstClick())
			{
				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Save the first point indices.
				get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
				get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

				get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
				get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
				get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

				// Cancel primed for first click and prime for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First point has been captured. k: %i", k);
			}
			else if (get_PrimedForSecondClick())
			{
				// Get second point and move it to the first one.
				ITControlPoint *firstPoint = get_ScratchControlPoint();
				ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second point has been captured. k: %i", k);

				mergeSurfacesByRowReverse(firstPoint->get_K(), secondPoint->get_K());

				// Cancel primed for first click and cancel primed for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);

				// Reset the scratch point.
				get_ScratchControlPoint()->set_X(0.0);
				get_ScratchControlPoint()->set_Y(0.0);
				get_ScratchControlPoint()->set_Z(0.0);

				get_ScratchControlPoint()->set_K(-1);
				get_ScratchControlPoint()->set_I(-1);
				get_ScratchControlPoint()->set_J(-1);

				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Finally recompute all the Bezier surface.
				for (int kk = 0; kk < project->get_MySurfaces()->size(); kk++)
				{
					project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();
				}

				// Mark the project as changed.
				UnsavedChanges = true;

				// Reset the buttons.
				w->resetModeButtons();

				// Reset the edit mode.
				MY_EDIT_MODE = NONE;
			}
		} 
		else if (MY_EDIT_MODE == MEASURE_DISTANCE)
		{
			if (get_PrimedForFirstClick())
			{
				// Empty the focus vectors.
				w->emptyFocusVectors();

				// Save the first point.
				// Save the first point indices.
				get_ScratchControlPoint()->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				get_ScratchControlPoint()->set_Y(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y());
				get_ScratchControlPoint()->set_Z(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Z());

				get_ScratchControlPoint()->set_K(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_K());
				get_ScratchControlPoint()->set_I(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_I());
				get_ScratchControlPoint()->set_J(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_J());

				// Cancel primed for first click and prime for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(true);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "First point has been captured.");
			}
			else if (get_PrimedForSecondClick())
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Second point has been captured.");

				// Get second point and move it to the first one.
				ITControlPoint *firstPoint = get_ScratchControlPoint();
				ITControlPoint *secondPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				// Find the distance.
				float dist = firstPoint->distanceFrom(secondPoint);
				float dx = secondPoint->get_X() - firstPoint->get_X();
				float dy = secondPoint->get_Y() - firstPoint->get_Y();
				float dz = secondPoint->get_Z() - firstPoint->get_Z();

				// Display it in the ui field.
				w->setMyTextDataField(QString("Distance: %1, dx: %2, dy: %3, dz: %4").arg(dist).arg(dx).arg(dy).arg(dz));

				// Cancel primed for first click and cancel primed for second click.
				set_PrimedForFirstClick(false);
				set_PrimedForSecondClick(false);

				// Reset the scratch point.
				get_ScratchControlPoint()->set_X(0.0);
				get_ScratchControlPoint()->set_Y(0.0);
				get_ScratchControlPoint()->set_Z(0.0);

				get_ScratchControlPoint()->set_K(-1);
				get_ScratchControlPoint()->set_I(-1);
				get_ScratchControlPoint()->set_J(-1);

				// Reset the buttons.
				w->resetModeButtons();

				// Empty the focus vectors.
				w->emptyFocusVectors();

				MY_EDIT_MODE = NONE;
			}
		}
	}
	// Redraw everything.
	updateGL();
}

void MyXYView::findControlPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI, int *targetJ)
{
	float threshold = 0.2;
	float minDistance = 10000.0;
	*targetI = -1;
	*targetJ = -1;
	*targetK = -1;

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *currentPoint = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				float deltaX = posX - currentPoint->get_X();
				float deltaY = posY - currentPoint->get_Y();
				float deltaZ = posZ - currentPoint->get_Z();

				float distanceSquared = deltaX*deltaX + deltaY*deltaY;

				if ((distanceSquared < threshold) && (distanceSquared < minDistance))
				{
					minDistance = distanceSquared;
					*targetK = k;
					*targetI = i;
					*targetJ = j;
				}
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Indices of nearest control point. k: %i, i: %i, j: %i", *targetK, *targetI, *targetJ);
}

void MyXYView::mergeSurfacesByRowReverse(int k1, int k2)
{
}

void MyXYView::mergeSurfacesByRow(int k1, int k2)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow");

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. About to appended to k1. k1 i: %i", project->get_MySurfaces()->at(k1)->get_MyControlPoints()->size());

	// The k1 surface forms the start of the new surface.
	// Go through the second surface and add its points to the end of the first.
	for (int i = 0; i < project->get_MySurfaces()->at(k2)->get_MyControlPoints()->size(); i++)
	{
		// Create new vectors that will contain the data for a row to be appended to k1.
		std::vector <ITControlPoint *> dummy_v;
		std::vector <ITControlPoint *> dummy_base_v;

		// Loop over columns of the current row of k2.
		for (int j = 0; j < project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).size(); j++)
		{
			float x = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_X();
			float y = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_Y();
			float z = project->get_MySurfaces()->at(k2)->get_MyControlPoints()->at(i).at(j)->get_Z();

			ITControlPoint *p = new ITControlPoint(x, y, z);

			ITControlPoint *pb = new ITControlPoint(x, y, z);

			dummy_v.push_back(p);
			dummy_base_v.push_back(pb);
		}

		project->get_MySurfaces()->at(k1)->get_MyControlPoints()->push_back(dummy_v);
		project->get_MyBaseSurfaces()->at(k1)->get_MyControlPoints()->push_back(dummy_base_v);
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. Data for k2 has been appended to k1. k1 i: %i", project->get_MySurfaces()->at(k1)->get_MyControlPoints()->size());

	// Erase the second surface.
	project->get_MySurfaces()->erase(project->get_MySurfaces()->begin() + k2);
	project->get_MyBaseSurfaces()->erase(project->get_MyBaseSurfaces()->begin() + k2);

	// Re-assign control point _K, _I and _J indices.
	// Re-assign the _K, _I and _J variables.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				p->set_K(k);
				p->set_I(i);
				p->set_J(j);

				ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				pb->set_K(k);
				pb->set_I(i);
				pb->set_J(j);
			}
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mergeSurfacesByRow. All data has been merged, and indices have been reassigned.");
}

void MyXYView::copyNewSurface(int k)
{
	ITSurface *s = new ITSurface();
	project->get_MySurfaces()->push_back(s);

	ITSurface *sb = new ITSurface();
	project->get_MyBaseSurfaces()->push_back(sb);

	s->set_NoOfInterpolatedPointsU(20);
	s->set_NoOfInterpolatedPointsV(20);

	sb->set_NoOfInterpolatedPointsU(20);
	sb->set_NoOfInterpolatedPointsV(20);

	int kNew = project->get_MySurfaces()->size() - 1;

	for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++) // Step along span (5 metre span).
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> v_base_dummy;

		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++) // j steps along chord.
		{
			ITControlPoint *sourcePt = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
			float x = sourcePt->get_X() + 1.0;
			float y = sourcePt->get_Y() + 1.0;
			float z = sourcePt->get_Z() + 1.0;

			ITControlPoint *p = new ITControlPoint(x, y, z);

			p->set_K(kNew);
			p->set_I(i);
			p->set_J(j);

			v_dummy.push_back(p);


			ITControlPoint *pb = new ITControlPoint(x, y, z);

			pb->set_K(kNew);
			pb->set_I(i);
			pb->set_J(j);

			v_base_dummy.push_back(pb);
		}

		project->get_MySurfaces()->at(kNew)->get_MyControlPoints()->push_back(v_dummy);
		project->get_MyBaseSurfaces()->at(kNew)->get_MyControlPoints()->push_back(v_base_dummy);
	}

	IsDataLoaded = true;
	UnsavedChanges = true;

	// Show status log entry.
	w->appendStatusTableWidget(QString("Design"), QString("New surface"));

	project->get_MySurfaces()->at(kNew)->manageComputationOfInterpolatedPoints();

	w->updateAllTabs();

}

void MyXYView::copyNewSurfaceMirror(int k)
{
	ITSurface *s = new ITSurface();
	project->get_MySurfaces()->push_back(s);

	ITSurface *sb = new ITSurface();
	project->get_MyBaseSurfaces()->push_back(sb);

	s->set_NoOfInterpolatedPointsU(20);
	s->set_NoOfInterpolatedPointsV(20);

	sb->set_NoOfInterpolatedPointsU(20);
	sb->set_NoOfInterpolatedPointsV(20);

	int kNew = project->get_MySurfaces()->size() - 1;

	for (int i = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size() - 1; i >= 0; i--) // Step along span (5 metre span).
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> v_base_dummy;

		for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++) // j steps along chord.
		{
			ITControlPoint *sourcePt = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
			float x = sourcePt->get_X() + 1.0;
			float y = sourcePt->get_Y() + 1.0;
			float z = sourcePt->get_Z() + 1.0;

			ITControlPoint *p = new ITControlPoint(x, y, z);

			v_dummy.push_back(p);

			ITControlPoint *pb = new ITControlPoint(x, y, z);

			v_base_dummy.push_back(pb);
		}

		project->get_MySurfaces()->at(kNew)->get_MyControlPoints()->push_back(v_dummy);
		project->get_MyBaseSurfaces()->at(kNew)->get_MyControlPoints()->push_back(v_base_dummy);

	} 

	// Re-assign the _K, _I and _J variables.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint* p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				p->set_K(k);
				p->set_I(i);
				p->set_J(j);

				ITControlPoint* pb = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
				pb->set_K(k);
				pb->set_I(i);
				pb->set_J(j);
			}
		}
	}

	IsDataLoaded = true;
	UnsavedChanges = true;

	// Show status log entry.
	w->appendStatusTableWidget(QString("Design"), QString("New surface"));

	project->get_MySurfaces()->at(kNew)->manageComputationOfInterpolatedPoints();

	// Delete all focus points.
	w->updateAllTabs();
}

void MyXYView::mouseMoveEvent(QMouseEvent *event)
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
			float tmp = glXYViewHalfExtent + factor * dy;

			if (tmp >= 0.0) glXYViewHalfExtent = tmp;
		}
		else if (!(event->modifiers())) // Just clicking without modifiers.
		{

			float posX, posY, posZ, old_posX, old_posY, old_posZ;

			getInAxesPosition(posX, posY, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);
			getInAxesPosition(old_posX, old_posY, lastPos.x(), lastPos.y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);

			const int x = event->x();
			const int y = viewport[3] - event->y();

			const int xold = lastPos.x();
			const int yold = viewport[3] - lastPos.y();

			w->statusBar()->showMessage(QString("X: %1, Y: %2").arg(posX).arg(posY));

			if (((MY_EDIT_MODE == DRAG) || (MY_EDIT_MODE == DRAG_ROW) || (MY_EDIT_MODE == DRAG_COL) || (MY_EDIT_MODE == DRAG_ALL)) && (project->get_MySurfaces()->at(0)->get_MyFocusControlPoints()->size() > 0))
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Here: %i, %i", x, y);

				// Drag the Focus points
				dragFocusPoints(posX, posY, old_posX, old_posY);

				lastPos = event->pos();

				UnsavedChanges = true;

				// Display the distance moved.
				set_EditValueX(get_EditValueX() + (posX - old_posX));
				set_EditValueY(get_EditValueY() + (posY - old_posY));
				w->setMyTextDataField(QString("Distance dragged x: %1, y: %2").arg(get_EditValueX()).arg(get_EditValueY()));
			}
			else if (MY_EDIT_MODE == ROTATE_ALL)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. ROTATE_ALL");

				float thetaOld = atan2(old_posY - dialCentreY, old_posX - dialCentreX);
				float thetaNew = atan2(posY - dialCentreY, posX - dialCentreX);
				float dTheta = thetaNew - thetaOld;

				dialAngle += dTheta;

				set_EditValue(get_EditValue() + dTheta*180.0 / PI);

				// Rotate the Focus points around the first point in the focus vector. 
				rotateFocusPoints(dTheta);
				lastPos = event->pos();

				UnsavedChanges = true;
				w->setMyTextDataField(QString::number(get_EditValue()));
			}
			else if (MY_EDIT_MODE == RESIZE_ALL)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. RESIZE_ALL");

				// Rotate the Focus points around the first point in the focus vector. 
				resizeFocusPoints(y, yold);
				lastPos = event->pos();

				UnsavedChanges = true;
				w->setMyTextDataField(QString::number(get_EditValue()));
			}
			else if (MY_EDIT_MODE == SHEAR_ALL)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside mouseMoveEvent. SHEAR_ALL");

				float dx = 0.001*(x - xold);

				set_EditValue(get_EditValue() + dx);
				shearFocusPoints(dx);

				lastPos = event->pos();
				UnsavedChanges = true;
				w->setMyTextDataField(QString::number(get_EditValue()));

			}
			else if (MY_EDIT_MODE == PERSPECTIVE_ALL)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside mouseMoveEvent. PERSPECTIVE_ALL");
				perspectiveFocusPoints(x, y, xold, yold);

				lastPos = event->pos();
				UnsavedChanges = true;
				w->setMyTextDataField(QString::number(get_EditValue()));

				float delta = posX - old_posX;
				if (delta > 0.0)
				{
					set_Sense(1);
				}
				else
				{
					set_Sense(-1);
				}
			}
			else if (MY_EDIT_MODE == CENTRED_ROTATE)
			{
				if (get_SecondClicksFinished())
				{
					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside mouseMoveEvent. CENTRED_ROTATE");

					float thetaOld = atan2(old_posY - get_ScratchControlPoint()->get_Y(), old_posX - get_ScratchControlPoint()->get_X());
					float thetaNew = atan2(posY - get_ScratchControlPoint()->get_Y(), posX - get_ScratchControlPoint()->get_X());
					float dTheta = thetaNew - thetaOld;

					set_EditValue(get_EditValue() + dTheta*180.0 / PI);

					// Rotate the Focus points around the first point in the focus vector. 
					centredRotateFocusPoints(dTheta);
					lastPos = event->pos();

					UnsavedChanges = true;
					w->setMyTextDataField(QString::number(get_EditValue()));
				}
			}
		}
	}
	else if (event->buttons() & Qt::RightButton)
	{
		if (factor == 1.0) factor = 0.5;
		glXYPanCentreX -= factor * dx;
		glXYPanCentreY += factor * dy;
	}

	lastPos = event->pos();

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();

	// Redraw other views.
	w->updateAllTabs();
}

void MyXYView::wheelEvent(QWheelEvent *event)
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

	getInAxesPosition(tmpAx, tmpAy, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

	float tmp = glXYViewHalfExtent - factor * dy;

	if (tmp >= 0.0) glXYViewHalfExtent = tmp;

	getInAxesPosition(tmpBx, tmpBy, event->x(), event->y(), this->width(), this->height(), glXYPanCentreX, glXYPanCentreY, glXYViewHalfExtent);

	glXYPanCentreX += tmpAx - tmpBx;
	glXYPanCentreY += tmpAy - tmpBy;

	// Adjust viewport view.
	setViewOrtho(myWidth, myHeight);

	// Redraw everything.
	updateGL();
}

void MyXYView::centredRotateFocusPoints(float dTheta)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "centredRotateFocusPoints. Number of surfaces: %i", project->get_MySurfaces()->size());

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() == 0)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "No Focus points for surface: %i", k);
			continue;
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rotateFocusPoints. Focus points found for surface: %i", k);

		ITPoint *basePoint = new ITPoint(get_ScratchControlPoint()->get_X(), get_ScratchControlPoint()->get_Y(), get_ScratchControlPoint()->get_Z());

		// Loop over focus points vector and rotate each point.
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
		{
			ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);
			float pxOld = p->get_X() - basePoint->get_X();
			float pyOld = p->get_Y() - basePoint->get_Y();
			float pzOld = p->get_Z() - basePoint->get_Z();

			float pxNew = pxOld*cos(dTheta) - pyOld*sin(dTheta);
			float pyNew = pxOld*sin(dTheta) + pyOld*cos(dTheta);

			pxNew = pxNew + basePoint->get_X();
			pyNew = pyNew + basePoint->get_Y();

			p->set_X(pxNew);
			p->set_Y(pyNew);
		}

		delete basePoint;

		// Update interpolated points.
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}
}

void MyXYView::synchronizeBaseSurfaceCoordinates()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n > 0)
		{
			// Update Base points and recalculate.
			for (int i = 0; i < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint *p = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

					p->set_X(project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X());
				}
			}
		}

		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}
}

void MyXYView::flipAllHorizontally()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n == 0)
		{
			continue;
		}

		ITControlPoint *basep = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0);

		for (int t = 0; t < n; t++)
		{
			ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t);

			ITControlPoint *tempPoint = new ITControlPoint(p->get_X(), p->get_Y(), p->get_Z());

			// Translate back to base.
			tempPoint->set_X(tempPoint->get_X() - basep->get_X());

			// Flip X
			tempPoint->set_X(-tempPoint->get_X());

			// Translate back.
			tempPoint->set_X(tempPoint->get_X() + basep->get_X());

			// Reassign control point variable.
			p->set_X(tempPoint->get_X());

			// Delete tempPoint.
			delete tempPoint;
		}

		// Update interpolated points.
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}
}

void MyXYView::perspectiveFocusPointsOld(float posX, float posY, float old_posX, float old_posY)
{
	// This method only works on the whole surface.

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n == 0)
		{
			continue;
		}

		float maxX = -100000.0;
		float minX = 100000.0;
		float maxY = -100000.0;
		float minY = 100000.0;

		for (int t = 0; t < n; t++)
		{
			// Check minX
			if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_X() < minX)
			{
				minX = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_X();
			}

			// Check minY
			if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_Y() < minY)
			{
				minY = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_Y();
			}

			// Check maxX
			if (maxX < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_X())
			{
				maxX = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_X();
			}

			// Check maxY
			if (maxY < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_Y())
			{
				maxY = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t)->get_Y();
			}
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "minx: %f, maxx: %f, miny: %f, maxy: %f", minX, maxX, minY, maxY);

		float deltaX = maxX - minX;
		float d = (posX - lastPos.x()) / deltaX;

		// Now adjust the focus points.
		for (int t = 0; t < n; t++)
		{
			ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t);
			float dx = d  * ((p->get_X() - minX) / (maxX - minX));
			float dc = dx * ((p->get_Y() - minY) / (maxY - minY));
			p->set_X(p->get_X() + dc);
		}
	}
}

void MyXYView::perspectiveFocusPoints(float posX, float posY, float old_posX, float old_posY)
{
	// This method only works on the whole surface.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n > 0)
		{
			// We have found the focal surface.
			// Find the enclosing box.
			float maxX = -100000.0;
			float minX = 100000.0;
			float maxY = -100000.0;
			float minY = 100000.0;

			for (int i = 0; i < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint *p = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);
					// Check minX
					if (p->get_X() < minX)
					{
						minX = p->get_X();
					}

					// Check minY
					if (p->get_Y() < minY)
					{
						minY = p->get_Y();
					}

					// Check maxX
					if (maxX < p->get_X())
					{
						maxX = p->get_X();
					}

					// Check maxY
					if (maxY < p->get_Y())
					{
						maxY = p->get_Y();
					}
				}
			}

			float deltaX = maxX - minX;
			float deltaY = maxY - minY;

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "minx: %f, maxx: %f, miny: %f, maxy: %f", minX, maxX, minY, maxY);

			// Compute K (capital K).
			float d = posX - get_EditValue();

			float K = 0.0;
			float dd = 0.0;
			float theta = 0.0;
			float factor = 10.0;

			if (d < 0.0)
			{
				dd = -d / deltaX;
				theta = atan2(dd, 1.0);
				K = factor / tan(theta);
			}
			else
			{
				dd = d / deltaX;
				theta = atan2(dd, 1.0);
				K = -factor / tan(theta);
			}

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "K: %f, theta: %f", K, theta);

			// Actualy adjust the control point positions.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					ITControlPoint *p = project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

					float x = p->get_X();
					x = x - minX;
					x = x / deltaX;

					float y = p->get_Y();
					y = y - minY;
					y = y / deltaY;

					// now x,y is a point in the unit square.
					// Map x.
					float x_n = x * (1.0 - y / K);

					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "i: %i, j: %i, x: %f, y: %f, x_n: %f", i, j, x, y, x_n);

					// Expand x.
					x_n = x_n*deltaX;
					x_n = x_n + minX;

					// Now assign the new control point x.
					project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_X(x_n);
				}
			}
		}
	}
}

void MyXYView::mouseReleaseEvent(QMouseEvent *event)
{
	if (MY_EDIT_MODE == PERSPECTIVE_ALL)
	{
		synchronizeBaseSurfaceCoordinates();
	} // End of if PERSPECTIVE_ALL.

	if (MY_EDIT_MODE == CENTRED_ROTATE)
	{
		if (get_SecondClicksFinished())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "_SecondClicksFinished is true so disabling priming flags.");

			synchronizeBaseSurfaceCoordinates();

			set_PrimedForFirstClick(false);
			set_PrimedForSecondClick(false);
			set_SecondClicksFinished(false);

			get_ScratchControlPoint()->set_I(-1);
			get_ScratchControlPoint()->set_J(-1);
			get_ScratchControlPoint()->set_K(-1);

			w->emptyFocusVectors();
		}

	} // End of CENTRED_ROTATE

	if (MY_EDIT_MODE == ROTATE_ALL)
	{
		drawDial = false;
	}

	set_EditValue(0.0);

	w->updateAllTabs();

	// Update the spreadsheet.
	w->updateSpreadsheet();
}

void MyXYView::dragFocusPoints(float posX, float posY, float old_posX, float old_posY)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int n = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();

		if (n == 0)
		{
			continue;
		}

		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() > 0)
		{
			// Loop over focus points vector and add dx and dy
			for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);

				if (IsVerticalDragOnly)
				{
					p->set_Y(p->get_Y() + (posY - old_posY));
				}
				else if (IsHorizontalDragOnly)
				{
					p->set_X(p->get_X() + (posX - old_posX));
				}
				else
				{
					p->set_X(p->get_X() + (posX - old_posX));
					p->set_Y(p->get_Y() + (posY - old_posY));
				}
			}

			// Update interpolated points.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		}
	}
}

void MyXYView::shearFocusPoints(float dx)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() > 0)
		{
			ITControlPoint* basePoint = new ITControlPoint(project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_X(),
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Y(),
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Z());

			//float dy = 0.001*(y - yold);

			for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);

				// Translate surface back to origin.
				p->set_X(p->get_X() - basePoint->get_X());
				p->set_Y(p->get_Y() - basePoint->get_Y());
				p->set_Z(p->get_Z() - basePoint->get_Z());

				// Shear the x.
				p->set_X(p->get_X() + p->get_Y()*dx);

				// Squeeze the x.
				// p->set_X( p->get_X() + (p->get_X() * p->get_Y())*dy );

				// Translate surface back again.
				p->set_X(p->get_X() + basePoint->get_X());
				p->set_Y(p->get_Y() + basePoint->get_Y());
				p->set_Z(p->get_Z() + basePoint->get_Z());
			}

			delete basePoint;

			// Update interpolated points.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		}
	}
}

void MyXYView::resizeFocusPoints(float y, float yold)
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() > 0)
		{
			ITControlPoint* basePoint = new ITControlPoint(project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_X(),
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Y(),
				project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Z());

			float d = (y - basePoint->get_Y()) / (yold - basePoint->get_Y());

			for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);

				// Translate surface back to origin.
				p->set_X(p->get_X() - basePoint->get_X());
				p->set_Y(p->get_Y() - basePoint->get_Y());
				p->set_Z(p->get_Z() - basePoint->get_Z());

				// Scale the focus points.
				p->set_X(p->get_X()*d);
				p->set_Y(p->get_Y()*d);
				p->set_Z(p->get_Z()*d);

				// Translate back.
				p->set_X(p->get_X() + basePoint->get_X());
				p->set_Y(p->get_Y() + basePoint->get_Y());
				p->set_Z(p->get_Z() + basePoint->get_Z());

			}

			delete basePoint;

			// Update interpolated points.
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		}
	}
}

void MyXYView::rotateFocusPoints(float dTheta)
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "rotateFocusPoints. Number of surfaces: %i", project->get_MySurfaces()->size());

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size() == 0)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "No Focus points for surface: %i", k);
			continue;
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "rotateFocusPoints. Focus points found for surface: %i", k);

		ITPoint *basePoint = new ITPoint(project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_X(),
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Y(),
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(0)->get_Z());

		// Loop over focus points vector and rotate each point.
		for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); n++)
		{
			ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(n);
			float pxOld = p->get_X() - basePoint->get_X();
			float pyOld = p->get_Y() - basePoint->get_Y();
			float pzOld = p->get_Z() - basePoint->get_Z();

			float pxNew = pxOld*cos(dTheta) - pyOld*sin(dTheta);
			float pyNew = pxOld*sin(dTheta) + pyOld*cos(dTheta);

			pxNew = pxNew + basePoint->get_X();
			pyNew = pyNew + basePoint->get_Y();

			p->set_X(pxNew);
			p->set_Y(pyNew);

		}

		delete basePoint;

		// Update interpolated points.
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

	}
}

void MyXYView::drawMyControlPointsNet()
{
	glColor3f(0.0f, 0.0f, 0.0f); // black

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

	// Now draw spheres.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
			{
				ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

				glTranslatef(p->get_X(), p->get_Y(), 0.0);

				float radius = glXYViewHalfExtent / 100.0;
				drawSphere(radius, 15, 15, 0.0, 0.0, 0.0);

				glTranslatef(-p->get_X(), -p->get_Y(), 0.0);
			}
		}
	}
}

void MyXYView::drawMyInterpolatedPointsNet()
{
	// Set the colour to blue.
	glColor3f(0.0f, 0.0f, 1.0f);

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

void MyXYView::drawMyFocusControlPoints()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size(); t++)
		{
			ITControlPoint *fp = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->at(t);

			glTranslatef(fp->get_X(), fp->get_Y(), 0.0);

			float radius = glXYViewHalfExtent / 50.0;
			drawSphere(radius, 15, 15, 1.0, 0.0, 0.0);

			if (t == 0) // Only dsraw indices for the first point.
			{
				glColor3f(0.0, 0.0, 0.0);
				renderText(radius, 0, radius, QString("k: %1, i: %2, j: %3, x: %4, y: %5, z: %6").arg(fp->get_K()).arg(fp->get_I()).arg(fp->get_J()).arg(fp->get_X()).arg(fp->get_Y()).arg(fp->get_Z()));
			}

			glTranslatef(-fp->get_X(), -fp->get_Y(), 0.0);
		}
	}
}

void MyXYView::drawSphere(double r, int lats, int longs, float R, float GG, float B)
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

// Accessors.
float MyXYView::get_EditValue() { return _EditValue; }
void MyXYView::set_EditValue(float a) { _EditValue = a; }

float MyXYView::get_EditValueX() { return _EditValueX; }
void MyXYView::set_EditValueX(float a) { _EditValueX = a; }

float MyXYView::get_EditValueY() { return _EditValueY; }
void MyXYView::set_EditValueY(float a) { _EditValueY = a; }

int MyXYView::get_Sense() { return _Sense; }
void MyXYView::set_Sense(int a) { _Sense = a; }

bool MyXYView::get_PrimedForFirstClick() { return _PrimedForFirstClick; }
void MyXYView::set_PrimedForFirstClick(bool p) { _PrimedForFirstClick = p; }

bool MyXYView::get_PrimedForSecondClick() { return _PrimedForSecondClick; }
void MyXYView::set_PrimedForSecondClick(bool p) { _PrimedForSecondClick = p; }

bool MyXYView::get_SecondClicksFinished() { return _SecondClicksFinished; }
void MyXYView::set_SecondClicksFinished(bool p) { _SecondClicksFinished = p; }

ITControlPoint *MyXYView::get_ScratchControlPoint() { return _ScratchControlPoint; }
void MyXYView::set_ScratchControlPoint(ITControlPoint *p) { _ScratchControlPoint = p; }