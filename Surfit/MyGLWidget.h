#pragma once

#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
	    Q_OBJECT

public:
	explicit MyGLWidget(QWidget *parent = 0);
	~MyGLWidget(void);

signals:

public slots:

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void setViewOrtho(int width, int height);
	void keyPressEvent( QKeyEvent * event );
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

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
	void drawMyControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyNormals();
	void drawMyPanelNormals();
	void drawMyAnnotations();
};

