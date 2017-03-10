#pragma once

#include <QGLWidget>

class MyGLGeneralTrajectoryCurveView : public QGLWidget
{
	Q_OBJECT

public:
	explicit MyGLGeneralTrajectoryCurveView(QWidget *parent = 0);
	~MyGLGeneralTrajectoryCurveView(void) {};

	void resetMyView();

	// Accessors.
	float get_EditValue();
	void set_EditValue(float a);

	float get_MaxY();
	void set_MaxY(float y);

	float get_ViewHalfExtent();
	void set_ViewHalfExtent(float a);

	float get_PanCentreX();
	void set_PanCentreX(float a);

	float get_PanCentreY();
	void set_PanCentreY(float a);

	int get_MyCurveIndex();
	void set_MyCurveIndex(int a);

	int get_MyEndOfSegment();
	void set_MyEndOfSegment(int a);

	char get_MyChar();
	void set_MyChar(char c);

protected:

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void setViewOrtho(int width, int height);
	void keyPressEvent(QKeyEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *releaseEvent);

private:

	// Drawing parameters.
	int xRot;
	int yRot;
	int zRot;

	int myWidth;
	int myHeight;

	QPoint lastPos;

	float _EditValue;
	float _MaxY;

	float _ViewHalfExtent;
	float _PanCentreX;
	float _PanCentreY;

	int _MyCurveIndex;

	int _MyEndOfSegment; // Denotes whether the click is at the p0 or p1 point.

	char _MyChar;

	// Drawing methods.
	void draw();
	void drawMyAxes();
	void drawMyAnnotations();
	void drawMyCurve();
	void drawSphere(double r, int lats, int longs, float R, float GG, float B);
	void drawMyFocusSegments();
	void AssignFocusPoint(QMouseEvent *event);
	void findTrajectoryPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI);
	void findOrdinateRange(float *minOrdinate, float *maxOrdinate);
	void dragFocusPoints(float posX, float posY, float old_posX, float old_posY);
	void drawMyCurveHandles(int curveIndex);
	void drawMyInterpolatedCurve(int curveIndex);
};