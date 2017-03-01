#pragma once

#include <QGLWidget>

#include "ITControlPoint.h"

class MyYZView : public QGLWidget
{
	    Q_OBJECT

public:
	explicit MyYZView(QWidget *parent = 0);
	~MyYZView(void);

	// Accessors.
	float get_EditValue();
	void set_EditValue(float a);

	float get_EditValueX();
	void set_EditValueX(float a);

	float get_EditValueY();
	void set_EditValueY(float a);

	bool get_PrimedForFirstClick();
	void set_PrimedForFirstClick(bool p);

	bool get_PrimedForSecondClick();
	void set_PrimedForSecondClick(bool p);

	ITControlPoint *get_ScratchControlPoint();
	void set_ScratchControlPoint(ITControlPoint *p);

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

	float _EditValue;
	float _EditValueX;
	float _EditValueY;

	bool _PrimedForFirstClick;
	bool _PrimedForSecondClick;

	ITControlPoint *_ScratchControlPoint; // Used for MATCH_POINT mode.

	// Drawing methods.
	void drawMyAxes();
	void draw();
	void drawMyControlPointsNet();
	void drawMyInterpolatedPointsNet();
	void drawMyFocusControlPoints();
	void findControlPointIndicesNearMouse(double posX, double posY, double posZ, int *targetK, int *targetI, int *targetJ);
	void drawSphere(double r, int lats, int longs, float R, float GG, float B);
	void AssignFocusPoint(QMouseEvent *event);
	void drawMyGrids();
	void drawMyAnnotations();
	void dragFocusPoints(float posY, float posZ, float old_posY, float old_posZ);

};