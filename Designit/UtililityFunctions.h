#pragma once

#include <QGLWidget>

void getInAxesPosition(float& posX, float& posY, const int eventX, const int eventY, const int width, const int height, const float glPanCentreX, const float glPanCentreY, const float glViewHalfExtent);