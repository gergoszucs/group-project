#ifndef MYOUTPUTGRAPHICSGLWIDGET_H
#define MYOUTPUTGRAPHICSGLWIDGET_H

#include <QGLWidget>

class MyOutputGraphicsGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	MyOutputGraphicsGLWidget(QWidget *parent);
	~MyOutputGraphicsGLWidget() {};

	void drawSpanForceDistributions();
	void plot2D();
	void drawMy2DAxesPressure(char *yLabel);
	void drawForceHistories();
	void drawCoefficientHistories();
	void drawMy2DAxes(char *yLabel);

protected:

	void initializeGL();
	void setViewOrtho();
	void paintGL();
	void resizeGL(int width, int height);
};

#endif // MYOUTPUTGRAPHICSGLWIDGET_H