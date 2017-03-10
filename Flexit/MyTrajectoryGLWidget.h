#pragma once

#include <QGLWidget>

class MyTrajectoryGLWidget : public QGLWidget
{
	Q_OBJECT

public:

	explicit MyTrajectoryGLWidget(QWidget *parent = 0);
	~MyTrajectoryGLWidget(void) {};

	void plot2D();
	void drawMy2DAxes(char *yLabel);
	void drawMyCurves(int curveIndex);
	void drawMyCurveHandles(int curveIndex);
	void drawMyInterpolatedCurves(int curveIndex);

protected:

	void initializeGL();
	void setViewOrtho();
	void paintGL();
	void resizeGL(int width, int height);
};