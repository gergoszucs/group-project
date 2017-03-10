#pragma once

#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
	Q_OBJECT

public:
	explicit MyGLWidget(QWidget *parent = 0);
	~MyGLWidget(void) {};

	void setStandardXView();
	void setStandardYView();
	void setStandardZView();

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void setViewOrtho(int width, int height);
	void keyPressEvent(QKeyEvent * event);
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
	//	void drawMyDeformedControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyTracks();
	void drawMyUVLMPanels();
	void drawMyNormals();
	void drawMyVorticitiesNet();
	void drawMyBoundVortices();
	void drawMyAnnotations();
	void drawMyWakeVortices();
	void drawMyPressureDistributionNet();
	void drowMyVelocityField();
	void drawMyGust();

	// EB drawing methods.
	//	void drawMyBeamElements();
	//	void drawMyBeamGhostElements();
	//	void drawMyBeamDeflectedGhostElements();
	//	void drawMyBeamNodes();
	//	void drawMyBeamDeflectedNodes();
	//	void drawMyBeamDeflectedGhostNodes();
	//	void drawMyBeamGhostNodes();
	//	void drawMyBeamLoadVectors();

	void drawMyWakePanelHistory();
	void drawMyPressureHistory();
	void drawMyVorticityHistory();
	void drawMyBeamNodeHistory();
	void drawMyVelocityFieldHistory();

	void drawSphere(double r, int lats, int longs);
};