#include "UtililityFunctions.h"

void getInAxesPosition(float& posX, float& posY, const int eventX, const int eventY, const int width, const int height, const float glPanCentreX, const float glPanCentreY, const float glViewHalfExtent)
{
	double orginX = width / 2;
	double orginY = height / 2;

	orginX -= glPanCentreX / glViewHalfExtent * width / 2;
	orginY += glPanCentreY / glViewHalfExtent * height / 2;

	double deltaX = eventX - orginX;
	double deltaY = eventY - orginY;

	posX = deltaX * glViewHalfExtent / width * 2;
	posY = -deltaY * glViewHalfExtent / height * 2;
}