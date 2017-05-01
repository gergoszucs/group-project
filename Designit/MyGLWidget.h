#pragma once

#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit MyGLWidget(QWidget *parent = 0);
	~MyGLWidget(void) {};

	void setDrawParameters(float glPanCenterX, float glPanCenterY, float glViewHalfExtent);

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

	float glViewHalfExtent;
	float glPanCentreX;
	float glPanCentreY;

	float centerX;
	float centerY;
	float centerZ;
	float azimuth;
	float polar;
	float radius;
	float eyeX;
	float eyeY;
	float eyeZoom;

	// Drawing methods.
	void drawMyAxes();
	void draw();
	void drawMyControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyNormals();
	void drawMyPanelNormals();
	void drawMyAnnotations();
	void drawMyTracks( int frame );

	void drawSphere(double r, int lats, int longs, float R, float GG, float B);

	void modPolar(float p);
	void modAzimuth(float a);
};