#include <QtWidgets>
#include <QtOpenGL>
#include "MatrixViewGLWidget.h"
#include "global.h"
#include "ITProject.h"

MatrixViewGLWidget::MatrixViewGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{}


void MatrixViewGLWidget::freeDisplayMatrix()
{
	// Delete displayMatrix.
	if (displayMatrix) free(displayMatrix);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside freeDisplayMatrix. displayMatrix has been freed.");
}

void MatrixViewGLWidget::initializeGL()
{
	qglClearColor(Qt::black);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
}

void MatrixViewGLWidget::paintGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	draw();
}

void MatrixViewGLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	float L = 1.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-L, +L, -L, +L, -1.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

void MatrixViewGLWidget::draw()
{
	drawMyAxes();

	if (IsDataLoaded)
	{
		if (FrameNumber > 0)
		{
			updateDisplayMatrix();
		}

		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, texName);

		// Render a textured quad with row 1, col 1 f matrix displayed in top left corner.
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f, 1.0, 0.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, -1.0f, 0.0f);
		glEnd();

		glFlush();
		glDisable(GL_TEXTURE_2D);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Texture has been drawn");
	}
}

void MatrixViewGLWidget::drawMyAxes()
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
}

void MatrixViewGLWidget::updateMyDisplay()
{
	update();
}

void MatrixViewGLWidget::updateDisplayMatrix()
{
	// Find the maximum value.
	int noOfPanels = project->get_A()->size();

	float maxA = 0.0;

	for (int i = 0; i < noOfPanels; i++)
	{
		for (int j = 0; j < noOfPanels; j++)
		{
			if (project->get_A()->at(i).at(j) > maxA)
			{
				maxA = project->get_A()->at(i).at(j);
			}
		}
	}

	for (int i = 0; i < noOfPanels; i++)
	{
		for (int j = 0; j < noOfPanels; j++)
		{
			float currentA = fabs(project->get_A()->at(i).at(j));
			float currentValue = 255 * currentA / maxA;

			// Let's assume currentValue is between 0 and 10.
			displayMatrix[i*noOfPanels * 4 + j * 4 + 0] = (GLubyte)(currentValue); // R
			displayMatrix[i*noOfPanels * 4 + j * 4 + 1] = (GLubyte)(currentValue); // G
			displayMatrix[i*noOfPanels * 4 + j * 4 + 2] = (GLubyte)(currentValue); // B
			displayMatrix[i*noOfPanels * 4 + j * 4 + 3] = (GLubyte)255; // Opacity.
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "update displayMatrix[0]: %i,get_A()->at(0).at(0): %f, FrameNumber: %i", displayMatrix[0], (float)(project->get_A()->at(0).at(0)), FrameNumber);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//           target        level  intFormat                             border  format    type               pixels.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, noOfPanels, noOfPanels, 0, GL_RGBA, GL_UNSIGNED_BYTE, displayMatrix);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Texture has been bound.");

	// Replace the texture with the data from the latest display matrix and redisplay.
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, noOfPanels, noOfPanels, GL_RGBA, GL_UNSIGNED_BYTE, displayMatrix);
}

void MatrixViewGLWidget::populateDisplayMatrix(int noOfPanels)
{
	// This gets called after data is loaded.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside populateDisplayMatrix");

	// Allocate memory for displayMatrix.
	displayMatrix = (GLubyte*)malloc(noOfPanels * noOfPanels * 4 * sizeof(GLubyte));

	for (int i = 0; i < noOfPanels; i++)
	{
		for (int j = 0; j < noOfPanels; j++)
		{
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;

			displayMatrix[i*noOfPanels * 4 + j * 4 + 0] = (GLubyte)c;
			displayMatrix[i*noOfPanels * 4 + j * 4 + 1] = (GLubyte)c;
			displayMatrix[i*noOfPanels * 4 + j * 4 + 2] = (GLubyte)c;
			displayMatrix[i*noOfPanels * 4 + j * 4 + 3] = (GLubyte)255;
		}
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Populate displayMatrix[0]: %i", displayMatrix[0]);
}