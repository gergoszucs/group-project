// Dominique's includes
#include "global.h"
#include "ITControlSurface.h"
#include "ITPanel.h"

ITControlSurface::ITControlSurface()
{

	set_DeflectionAngle(0.0);

	_MyITPanels = new std::vector <ITPanel*>;

	_MyDeflectionHistory = new std::vector <float>;

}

ITControlSurface::~ITControlSurface(void)
{
	delete _MyITPanels;

	_MyDeflectionHistory->clear();
	delete _MyDeflectionHistory;

}


// Worker methods.



// Accessors.
float ITControlSurface::get_DeflectionAngle() { return _DeflectionAngle; }
void ITControlSurface::set_DeflectionAngle(float x) { _DeflectionAngle = x; }

std::vector <ITPanel*> *ITControlSurface::get_MyITPanels() { return _MyITPanels; }
void ITControlSurface::set_MyITPanels(std::vector <ITPanel*> *myITPanels) { _MyITPanels = myITPanels; }

std::vector <float> *ITControlSurface::get_MyDeflectionHistory() { return _MyDeflectionHistory; }
void ITControlSurface::set_MyDeflectionHistory(std::vector <float> *myDeflectionHistory) { _MyDeflectionHistory = myDeflectionHistory; }

