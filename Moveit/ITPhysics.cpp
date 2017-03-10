#include <QTime> // Used in the delay function.
#include "ITPhysics.h"
#include "global.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITPoint.h"
#include "ITControlPoint.h"
#include "ITTrajectoryCurve.h" // Used in playOutTest.
#include "ITTrajectoryCurveSegment.h" // Used in playOutTest.
#include "ITPointTrajectory.h" // Used in playoutTest.

void ITPhysics::playOutDryRun()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside playOutDryRun");

    // At the start of the dry run, we move points back to base.
    if (FrameNumber == 0)
    {
        for (int k=0; k<project->get_MySurfaces()->size(); k++)
        {
			project->get_MySurfaces()->at(k)->moveMeBackToBase(k);
        } // k
    }

	while ((FrameNumber < project->get_MaxKeyFrame()) && (IsDryRun)) // Note that we do NOT require !IsStep here. This allows us to dry run after stepping.
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside playOutDryRun. FrameNumber: %i", FrameNumber);
		
		if (MY_RUN_MODE == MYGUI)
		{
			w->statusBar()->showMessage(QString("Dry run in Progress. Frame: %1 of %2").arg(FrameNumber).arg(project->get_MaxKeyFrame()));
		}

		ITPhysics::PropagateSurfaceGeometry(FrameNumber);

		// Update the main widget.
		if (MY_RUN_MODE == MYGUI)
		{
			w->updateAllTabs();
			ITPhysics::delay(30); // 30 msecs for rendering the current Frame.
		}

		FrameNumber++;
	}
}

void ITPhysics::delay( int millisecondsToWait )
{
	QTime dieTime= QTime::currentTime().addMSecs( millisecondsToWait  );
	while (QTime::currentTime() < dieTime)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
}

void ITPhysics::PropagateSurfaceGeometry(int FrameNumber)
{
    // Propagate the surface geometries.
    for (int k=0; k<project->get_MySurfaces()->size(); k++)
    {
        //============================================================================================
        // Compute new surface, panel and bound vortex segment geometry for this frame.
        // The vorticities must be saved from old panels and copied to new panels.
        //============================================================================================
        project->get_MySurfaces()->at(k)->propagateGeometry(k);
    }
}