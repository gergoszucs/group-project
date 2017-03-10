#pragma once

#include <QGLWidget>

class MyEBWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit MyEBWidget(QWidget *parent = 0);
	~MyEBWidget(void) {};

	void plot2D();
	void drawSpanDeformationDistributions();
	void drawMy2DAxes(char *yLabel);

protected:

	void initializeGL();
	void setViewOrtho();
	void paintGL();
	void resizeGL(int width, int height);

private:

	// Drawing parameters.
	int myWidth;
	int myHeight;
};