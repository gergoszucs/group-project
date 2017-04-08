#include "UtililityFunctions.h"

void getInAxesPosition(float& posX, float& posY, const int eventX, const int eventY, const int width, const int height, const float glPanCentreX, const float glPanCentreY, const float glViewHalfExtent)
{
	double orginX = (float)width / 2;
	double orginY = (float)height / 2;

	float aspect = (float)width / (float)height;

	float glVHE_V = glViewHalfExtent / aspect;

	orginX -= glPanCentreX / glViewHalfExtent * (float)width / 2;
	orginY += glPanCentreY / glVHE_V * (float)height / 2;

	double deltaX = eventX - orginX;
	double deltaY = eventY - orginY;

	posX = deltaX * glViewHalfExtent / (float)width * 2;
	posY = -deltaY * glVHE_V / (float)height * 2;
}