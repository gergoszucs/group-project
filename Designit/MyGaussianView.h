#pragma once

#include <QGLWidget>

class MyGaussianView : public QGLWidget
{
	Q_OBJECT

public:
	explicit MyGaussianView(QWidget *parent = 0);
	~MyGaussianView(void) {};

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void setViewOrtho(int width, int height);
	void keyPressEvent(QKeyEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

private:

	// Drawing parameters.
	int xRot;
	int yRot;
	int zRot;

	int myWidth;
	int myHeight;

	QPoint lastPos;

	// Drawing methods.
	void drawMyAxes();
	void draw();
	void drawMyGaussianCurvatures();
};