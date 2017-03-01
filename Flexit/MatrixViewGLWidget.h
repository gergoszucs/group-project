#pragma once
#include "qgl.h"
#include <QGLWidget>

class MatrixViewGLWidget :
	public QGLWidget
{
	Q_OBJECT

private:

	GLubyte *displayMatrix;
	GLuint texName;

	void draw();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void drawMyAxes();

public:
	explicit MatrixViewGLWidget(QWidget *parent = 0);
	~MatrixViewGLWidget(void);

	void updateMyDisplay();
	void updateDisplayMatrix();
	void populateDisplayMatrix(int noOfPanels);
	void freeDisplayMatrix();

};

