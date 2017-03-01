#pragma once

// Don's includes.
#include "ITPanel.h"

// Forward declaration.
class ITPoint;
class ITSurface;
class ITVortex;

// System includes.
#include <vector>

class ITWakePanel : public ITPanel
{

private:
	int _f; // frame number.

public:
	ITWakePanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel);
	~ITWakePanel(void);

	int get_F();
	void set_F(int f);

};