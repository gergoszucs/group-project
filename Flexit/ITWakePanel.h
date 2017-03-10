#pragma once

#include "ITPanel.h"
#include <vector>

class ITPoint;
class ITSurface;
class ITVortex;

class ITWakePanel : public ITPanel
{

private:
	int _f; // frame number.

public:
	ITWakePanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel);
	~ITWakePanel(void) {};

	int get_F();
	void set_F(int f);
};