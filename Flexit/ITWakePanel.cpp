// Dom's includes.
#include "global.h"
#include "ITWakePanel.h"
#include "ITPoint.h"
#include "ITVortex.h"
#include "ITSurface.h"

ITWakePanel::ITWakePanel(ITPoint* bottomLeft, ITPoint* bottomRight, ITPoint* topRight, ITPoint* topLeft, ITSurface* s, int typeOfPanel)
	: ITPanel(bottomLeft, bottomRight, topRight, topLeft, s, typeOfPanel)
{
	// The call to the super class constructor takes care of setting up the instance variables.
	// Note that the vertices are ITPoint objects created in the superclass.
	// If typeOfPanel is 0 then we are dealing with a bound panel and 1 when dealing with a wake panel.
}

ITWakePanel::~ITWakePanel(void)
{


}


// Accessors.
int ITWakePanel::get_F() { return _f; }
void ITWakePanel::set_F(int f) { _f = f; }
