#include "global.h"
#include "moveit.h"
#include "ITIO.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"
#include "ITPhysics.h"
#include <QMessageBox> // QMessageBox
#include <QFileDialog> // QFileDialog for open data file.
#include <QTextStream> // To display the contents of a file in the GUI.
#include <QCloseEvent> // Used in closeEvent.
#include <QClipboard>
#include <intrin.h> // To use __cpuid to get the CPU processor ID. Used in actionSend_HTTP_Log_Message_triggered().
#include "qjsonmodel.h" // To display the JSON data in the QTreeView tab.
#include "qjsonitem.h"

Moveit::Moveit(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	QIcon icon = QIcon("Resources/blackDot.png");

	// Default display OpenGL axes enabled.
	ui.actionAxes->setIcon(icon);
	ui.actionControl_points->setIcon(icon);
	ui.actionInterpolated_points->setIcon(icon);
	ui.actionNormals->setIcon(icon);
	ui.actionAnnotations->setIcon(icon);

	// Set up the status table view.
	QTableWidget* my_statusTable = ui.myStatusTableWidget;
	my_statusTable->setColumnCount(2);

	my_statusTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Key")));
	my_statusTable->setColumnWidth(0, 40);

	my_statusTable->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Value")));
	my_statusTable->setColumnWidth(1, 130);

	my_statusTable->verticalHeader()->setVisible(false);

	// Set up the curve indices for the plot window objects.
	ui.myGLGeneralTrajectoryCurveViewX->set_MyCurveIndex(0);
	ui.myGLGeneralTrajectoryCurveViewX->set_MyChar('X');

	ui.myGLGeneralTrajectoryCurveViewY->set_MyCurveIndex(1);
	ui.myGLGeneralTrajectoryCurveViewY->set_MyChar('Y');

	ui.myGLGeneralTrajectoryCurveViewZ->set_MyCurveIndex(2);
	ui.myGLGeneralTrajectoryCurveViewZ->set_MyChar('Z');

	ui.myGLGeneralTrajectoryCurveViewR->set_MyCurveIndex(3);
	ui.myGLGeneralTrajectoryCurveViewR->set_MyChar('R');

	ui.myGLGeneralTrajectoryCurveViewP->set_MyCurveIndex(4);
	ui.myGLGeneralTrajectoryCurveViewP->set_MyChar('P');

	ui.myGLGeneralTrajectoryCurveViewB->set_MyCurveIndex(5);
	ui.myGLGeneralTrajectoryCurveViewB->set_MyChar('B');

	// Disable the Dry-run button.
	ui.actionDry_run->setEnabled(false);
}

void Moveit::on_actionOpen_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionOpen_triggered.");

	if (!IsDataLoaded)
	{
		if (!UnsavedChanges)
		{
			QString d = QDir::currentPath();
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionOpen_triggered. Current path: %s", d.toStdString().c_str());
			QString fileNameWithPath = QFileDialog::getOpenFileName(this, tr("Open File"), d.append("/Data"), tr("JSON Files (*.json *.JSON);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));

			// (re-)Set the flag.
			IsDataLoaded = false;

			if (!fileNameWithPath.isEmpty())
			{
				loadData(fileNameWithPath);
			}
		}
		else
		{
			// There are unsaved changes.
			QMessageBox::information(0, tr("Unsaved data"), tr("You have unsaved data.\nPlease save or quit before loading another project."));
		}
	}
	else
	{
		// There is already a file open.
		QMessageBox::information(0, tr("Project already loaded"), tr("You have a project loaded already.\nPlease close the existing project before loading another."));
	}
}

void Moveit::loadData(QString fileNameWithPath) // Gets called from on_actionOpen_triggered and on_jsonUpdateButton_clicked.
{
	QFile file(fileNameWithPath);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
		return;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "About to read data from the file.");

	if (MY_RUN_MODE == MYGUI)
	{
		// Only display the file if it is small.
		QByteArray latin1BAFileNameWithPathString = fileNameWithPath.toLatin1();

		// Display the file contents in the text box.
		QTextStream in(&file);
		ui.fileTextEdit->setText(in.readAll());
		QFont newFont("Courier", 8, QFont::Normal, false);
		ui.fileTextEdit->setFont(newFont);
		file.close();
		ui.fileTextEdit->setLineWrapMode(QTextEdit::NoWrap);

		// Display json as tree.
		QJsonModel * model = new QJsonModel;
		ui.jsonTreeView->setModel(model);
		ui.jsonTreeView->show();

		ui.jsonTreeView->setSelectionMode(QTreeView::ExtendedSelection);
		ui.jsonTreeView->setSelectionBehavior(QTreeView::SelectRows);

		model->load(fileNameWithPath.toStdString().c_str());

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Model loaded.");

		// Set the width of the QTreeView columns.
		ui.jsonTreeView->header()->resizeSection(0, 300);

		// Set the window title to the filename.
		std::string fileNameWithPathString = std::string(latin1BAFileNameWithPathString.data());
		std::string base_filename = fileNameWithPathString.substr(fileNameWithPathString.find_last_of("/\\") + 1);
		DataFileNameWithPath = QString(fileNameWithPathString.c_str()); // Set the global variable.
		DataFileName = QString(base_filename.c_str()); // Set the global variable.

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "base_filename: %s", base_filename.c_str());

		QString text = "Moveit: " + QString(base_filename.c_str());

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "text: %s", text.toStdString().c_str());

		// Enable the Dry-run button.
		ui.actionDry_run->setEnabled(true);

		w->setWindowTitle(text);

		w->statusBar()->showMessage(QObject::tr("File read successful"));
	}

	// Actually read the data and instanciate the C++ objects.
	QByteArray latin1BAFileNameWithPathString = fileNameWithPath.toLatin1();
	ITIO::readJSONInputFile(latin1BAFileNameWithPathString.data()); // In ITIO.cpp.

	// Compute the interpolated points and the ITPanels.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
	}

	showSpreadsheet();

	// Set the flag.
	IsDataLoaded = true;

	// Display a satus message.
	appendStatusTableWidget(QString("File"), QString("Loaded"));

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Loaded"), 0.0, 0, DataFileNameWithPath);

	// Finally force process events.
	QApplication::processEvents();
}

void Moveit::on_actionDry_run_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionDry_run_triggered");

	// Perform a dry run.
	if (IsDataLoaded)
	{
		w->statusBar()->showMessage(QObject::tr("Dry run in progress. Please wait ..."));

		// Set flags.
		IsDryRun = true;

		// Disable the Dry-run button.
		ui.actionDry_run->setEnabled(false);

		// Reset the frame number.
		FrameNumber = 0;

		// Display a status message.
		appendStatusTableWidget(QString("Dry"), QString("Started"));

		// Send the HTTP request.
		sendHTTPRequest(QString("Dry"), QString("Started"), 0, 0, DataFileNameWithPath);

		// Do the work. This call may return either at the end of the simulation or when the Pause button was pressed.
		ITPhysics::playOutDryRun();

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Dry run completed. FrameNumber: %i", FrameNumber);

		if (FrameNumber == project->get_MaxKeyFrame())
		{
			// Display a status message.
			appendStatusTableWidget(QString("Dry"), QString("Completed"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Dry"), QString("Completed"), 0, 0, DataFileNameWithPath);

			w->statusBar()->showMessage(QObject::tr("Dry run completed successfully."));

			// Enable the Dry-run button.
			ui.actionDry_run->setEnabled(true);
		}
	}
	else
	{
		QMessageBox::information(0, tr("No input data"), tr("Please open a data file before performing calculations."));
	}
}

void Moveit::on_actionControl_points_triggered()
{
	drawControlPoints = !drawControlPoints;

	if (drawControlPoints)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionControl_points->setIcon(icon);
	}
	else
	{
		ui.actionControl_points->setIcon(QIcon());
	}
}

void Moveit::on_actionInterpolated_points_triggered()
{
	drawInterpolatedPoints = !drawInterpolatedPoints;

	if (drawInterpolatedPoints)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionInterpolated_points->setIcon(icon);
	}
	else
	{
		ui.actionInterpolated_points->setIcon(QIcon());
	}
}

void Moveit::on_actionNormals_triggered()
{
	drawNormals = !drawNormals;

	if (drawNormals)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionNormals->setIcon(icon);
	}
	else
	{
		ui.actionNormals->setIcon(QIcon());
	}
}

void Moveit::on_actionAnnotations_triggered()
{
	drawAnnotations = !drawAnnotations;

	if (drawAnnotations)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionAnnotations->setIcon(icon);
	}
	else
	{
		ui.actionAnnotations->setIcon(QIcon());
	}
}

void Moveit::on_actionAxes_triggered()
{
	drawAxes = !drawAxes;

	if (drawAxes)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionAxes->setIcon(icon);
	}
	else
	{
		ui.actionAxes->setIcon(QIcon());
	}
}

void Moveit::on_actionReset_all_views_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionDrag_trajectory_point_triggered");

	ui.myGLGeneralTrajectoryCurveViewX->resetMyView();
	ui.myGLGeneralTrajectoryCurveViewY->resetMyView();
	ui.myGLGeneralTrajectoryCurveViewZ->resetMyView();
	ui.myGLGeneralTrajectoryCurveViewR->resetMyView();
	ui.myGLGeneralTrajectoryCurveViewP->resetMyView();
	ui.myGLGeneralTrajectoryCurveViewB->resetMyView();

	// Redraw everything (to get rid of any spheres).
	w->updateAllTabs();
}

void Moveit::on_actionDrag_trajectory_point_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionDrag_trajectory_point_triggered");

	resetModeButtons();

	if (MY_EDIT_MODE != DRAG_TRAJECTORY_POINT)
	{
		// Update the ENUM
		MY_EDIT_MODE = DRAG_TRAJECTORY_POINT;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_Drag_trajectory_point_highlight.png");
		this->ui.actionDrag_trajectory_point->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_Drag_trajectory_point.png");
		this->ui.actionDrag_trajectory_point->setIcon(icon);

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Moveit::updateAllTabs()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside updateAllTabs");

	// Update the main geometry OpenGL view.
	ui.myGLWidget->update();

	// Update the output graphs.
	ui.myGLGeneralTrajectoryCurveViewX->update();
	ui.myGLGeneralTrajectoryCurveViewY->update();
	ui.myGLGeneralTrajectoryCurveViewZ->update();
	ui.myGLGeneralTrajectoryCurveViewR->update();
	ui.myGLGeneralTrajectoryCurveViewP->update();
	ui.myGLGeneralTrajectoryCurveViewB->update();
}

void Moveit::showSpreadsheet()
{
	// Populate the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "showSpreadsheet");
		// Compute the number of columns and display column headers.============================================================================
		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.mySpreadsheet;

		my_table->setRowCount(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size() + 1);

		int columnCount = 2 + 6 * noOfSurfaces; // The column count for the FrameNumber, CL, CD, Lift and Drag for each surface.

		// Actually set the column count of the table.
		my_table->setColumnCount(columnCount);

		// Now display the column headers.
		my_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Index"));
		my_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Frame Number"));

		int columnIndex = 2;
		for (int k = 0; k < noOfSurfaces; k++)
		{
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("X %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Y %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Z %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;

			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Roll %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Pitch %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Yaw %1").arg(k)));
			my_table->setColumnWidth(columnIndex, 50);
			columnIndex++;
		}

		// Display first row (p0 of the first segments)
		columnIndex = 0;

		QTableWidgetItem* new_itemF = new QTableWidgetItem();
		new_itemF->setText(QString::number(0));  // 
		my_table->setItem(0, columnIndex, new_itemF);
		columnIndex++;

		int currentFrame = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(0)->get_StartKeyFrame();
		QTableWidgetItem* new_itemT = new QTableWidgetItem();
		new_itemT->setText(QString::number(currentFrame));  // 
		my_table->setItem(0, columnIndex, new_itemT);
		columnIndex++;

		for (int k = 0; k < noOfSurfaces; k++)
		{
			// X
			ITPointTrajectory *pX = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemX = new QTableWidgetItem();
			new_itemX->setText(QString::number(pX->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemX);
			columnIndex++;
			// Y
			ITPointTrajectory *pY = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemY = new QTableWidgetItem();
			new_itemY->setText(QString::number(pY->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemY);
			columnIndex++;
			// Z
			ITPointTrajectory *pZ = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemZ = new QTableWidgetItem();
			new_itemZ->setText(QString::number(pZ->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemZ);
			columnIndex++;
			// Roll
			ITPointTrajectory *pR = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemR = new QTableWidgetItem();
			new_itemR->setText(QString::number(pR->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemR);
			columnIndex++;
			// Pitch
			ITPointTrajectory *pP = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemP = new QTableWidgetItem();
			new_itemP->setText(QString::number(pP->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemP);
			columnIndex++;
			// Yaw
			ITPointTrajectory *pB = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemB = new QTableWidgetItem();
			new_itemB->setText(QString::number(pB->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemB);
			columnIndex++;
		} // End of k loop.

		// Display data for each surface.======================================================================================
		for (int n = 0; n < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); n++)
		{
			int columnIndex = 0;
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showSpreadsheet. n = %i", n);

			QTableWidgetItem* new_itemF = new QTableWidgetItem();
			new_itemF->setText(QString::number(n + 1));  // 
			my_table->setItem(n + 1, columnIndex, new_itemF);
			columnIndex++;

			int currentFrame = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(n)->get_EndKeyFrame();
			QTableWidgetItem* new_itemT = new QTableWidgetItem();
			new_itemT->setText(QString::number(currentFrame));  // 
			my_table->setItem(n + 1, columnIndex, new_itemT);
			columnIndex++;

			for (int k = 0; k < noOfSurfaces; k++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showSpreadsheet 3");

				// X
				ITPointTrajectory *pX = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemX = new QTableWidgetItem();
				new_itemX->setText(QString::number(pX->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemX);
				columnIndex++;
				// Y
				ITPointTrajectory *pY = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemY = new QTableWidgetItem();
				new_itemY->setText(QString::number(pY->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemY);
				columnIndex++;
				// Z
				ITPointTrajectory *pZ = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemZ = new QTableWidgetItem();
				new_itemZ->setText(QString::number(pZ->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemZ);
				columnIndex++;
				// Roll
				ITPointTrajectory *pR = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemR = new QTableWidgetItem();
				new_itemR->setText(QString::number(pR->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemR);
				columnIndex++;
				// Pitch
				ITPointTrajectory *pP = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemP = new QTableWidgetItem();
				new_itemP->setText(QString::number(pP->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemP);
				columnIndex++;
				// Yaw
				ITPointTrajectory *pB = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemB = new QTableWidgetItem();
				new_itemB->setText(QString::number(pB->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemB);
				columnIndex++;
			}
		}
	}
}

void Moveit::updateSpreadsheet()
{
	// Update the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "updateSpreadsheet");

		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.mySpreadsheet;

		my_table->setRowCount(project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size() + 1);

		// Display first row (p0 of the first segments)
		int columnIndex = 0;

		QTableWidgetItem* new_itemF = new QTableWidgetItem();
		new_itemF->setText(QString::number(0));  // 
		my_table->setItem(0, columnIndex, new_itemF);
		columnIndex++;

		int currentFrame = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(0)->get_StartKeyFrame();
		QTableWidgetItem* new_itemT = new QTableWidgetItem();
		new_itemT->setText(QString::number(currentFrame));  // 
		my_table->setItem(0, columnIndex, new_itemT);
		columnIndex++;

		for (int k = 0; k < noOfSurfaces; k++)
		{
			// X
			ITPointTrajectory *pX = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemX = new QTableWidgetItem();
			new_itemX->setText(QString::number(pX->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemX);
			columnIndex++;
			// Y
			ITPointTrajectory *pY = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemY = new QTableWidgetItem();
			new_itemY->setText(QString::number(pY->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemY);
			columnIndex++;
			// Z
			ITPointTrajectory *pZ = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemZ = new QTableWidgetItem();
			new_itemZ->setText(QString::number(pZ->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemZ);
			columnIndex++;
			// Roll
			ITPointTrajectory *pR = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemR = new QTableWidgetItem();
			new_itemR->setText(QString::number(pR->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemR);
			columnIndex++;
			// Pitch
			ITPointTrajectory *pP = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemP = new QTableWidgetItem();
			new_itemP->setText(QString::number(pP->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemP);
			columnIndex++;
			// Yaw
			ITPointTrajectory *pB = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p();
			QTableWidgetItem* new_itemB = new QTableWidgetItem();
			new_itemB->setText(QString::number(pB->get_X()));  // 
			my_table->setItem(0, columnIndex, new_itemB);
			columnIndex++;
		}

		// Display data for each segment (after the first).======================================================================================
		for (int n = 0; n < project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->size(); n++)
		{
			int columnIndex = 0;

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "updateSpreadsheet. n = %i", n);

			QTableWidgetItem* new_itemF = new QTableWidgetItem();
			new_itemF->setText(QString::number(n + 1));  // 
			my_table->setItem(n + 1, columnIndex, new_itemF);
			columnIndex++;

			int currentFrame = project->get_MySurfaces()->at(0)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(n)->get_EndKeyFrame();
			QTableWidgetItem* new_itemT = new QTableWidgetItem();
			new_itemT->setText(QString::number(currentFrame));  // 
			my_table->setItem(n + 1, columnIndex, new_itemT);
			columnIndex++;

			// For the current segment, loop over each surface.
			for (int k = 0; k < noOfSurfaces; k++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "updateSpreadsheet 3");

				// X
				ITPointTrajectory *pX = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(0)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemX = new QTableWidgetItem();
				new_itemX->setText(QString::number(pX->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemX);
				columnIndex++;
				// Y
				ITPointTrajectory *pY = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(1)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemY = new QTableWidgetItem();
				new_itemY->setText(QString::number(pY->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemY);
				columnIndex++;
				// Z
				ITPointTrajectory *pZ = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(2)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemZ = new QTableWidgetItem();
				new_itemZ->setText(QString::number(pZ->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemZ);
				columnIndex++;
				// Roll
				ITPointTrajectory *pR = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(3)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemR = new QTableWidgetItem();
				new_itemR->setText(QString::number(pR->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemR);
				columnIndex++;
				// Pitch
				ITPointTrajectory *pP = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(4)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemP = new QTableWidgetItem();
				new_itemP->setText(QString::number(pP->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemP);
				columnIndex++;
				// Yaw (Bearing)
				ITPointTrajectory *pB = project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(5)->get_MyTrajectoryCurveSegments()->at(n)->get_P1_p();
				QTableWidgetItem* new_itemB = new QTableWidgetItem();
				new_itemB->setText(QString::number(pB->get_X()));  // 
				my_table->setItem(n + 1, columnIndex, new_itemB);
				columnIndex++;
			}
		}
	}
}

void Moveit::on_actionExit_triggered()
{
	// Check for unsaved data and exit if everything is saved.
	if (checkExitWithUnsavedData())
	{
		qApp->quit();
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionExit_triggered. Just returned from checkExitWithUnsavedData.");
}

bool Moveit::checkExitWithUnsavedData()
{
	if (UnsavedChanges)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Unsaved Data", "You have unsaved data.\nAre you sure you want to quit?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			closeProject();

			// Send the HTTP request.
			sendHTTPRequest(QString("Program"), QString("Terminated"), 0, 0, DataFileNameWithPath);

			return true;
		}
		else
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Cancelled quit");
			return false;
		}
	}
	else
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "No unsaved data. Simply exiting.");

		if (IsDataLoaded)
		{
			closeProject();
		}

		// Send the HTTP request.
		sendHTTPRequest(QString("Program"), QString("Terminated"), 0, 0, DataFileNameWithPath);

		return true;
	}
}

void Moveit::closeProject()
{
	// Clear the spreadsheet window.
	ui.mySpreadsheet->clear();

	// Clear the json tree view.
	QItemSelectionModel *m = ui.jsonTreeView->selectionModel();
	ui.jsonTreeView->setModel(NULL);
	delete m;

	// Delete the ITProject object.
	delete project;

	// Instanciate a new ITProject.
	project = new ITProject();
	project->set_DebugLevel(5);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Debug level: %i", project->get_DebugLevel());

	// (re-)Set the flags.
	IsDataLoaded = false;
	UnsavedChanges = false;

	IsDryRun = false;

	// Set the main window title.
	QString text = "Flexit: ";
	setWindowTitle(text);

	statusBar()->showMessage(QObject::tr("File closed successfully"));

	// Disable the Dry run button.
	ui.actionDry_run->setEnabled(false);

	// Force a redraw.
	ui.myGLWidget->repaint();

	// Force a redraw.
	ui.myGLGeneralTrajectoryCurveViewX->repaint();
	ui.myGLGeneralTrajectoryCurveViewY->repaint();
	ui.myGLGeneralTrajectoryCurveViewZ->repaint();
	ui.myGLGeneralTrajectoryCurveViewR->repaint();
	ui.myGLGeneralTrajectoryCurveViewP->repaint();
	ui.myGLGeneralTrajectoryCurveViewB->repaint();

	// Display a status message.
	appendStatusTableWidget(QString("File"), QString("Closed"));


	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Closed"), 0, 0, DataFileNameWithPath);
}

void Moveit::on_actionSave_As_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Save As");

	if (!UnsavedChanges)
	{
		QMessageBox::information(0, tr("Save data"), tr("There is no new data to save."));
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), QString(), tr("JSON Files (*.json *.JSON);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
		return;
	}

	// Close the file.
	file.close();

	// OK, the file is there, so let's start writing to the file.
	// Get a char * from the file name.
	QByteArray latin1BAFilenameString = fileName.toLatin1();

	// Write the project to the file.
	ITIO::writeMyProjectToFile(latin1BAFilenameString.data());

	// Set status message.
	QString str1 = QString("File %1 saved successfully.").arg(fileName);
	w->statusBar()->showMessage(str1);

	// Show status log entry
	appendStatusTableWidget(QString("File"), QString("Saved"));

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Saved"), 0, 0, DataFileNameWithPath);

	// Finally set flags.
	UnsavedChanges = false;

}

void Moveit::on_actionClose_triggered()
{
	// Check for data already loaded.
	if (IsDataLoaded)
	{
		// Check for unsaved data.
		if (UnsavedChanges)
		{
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, "Unsaved Data", "You have unsaved data.\nAre you sure you want to close without saving?", QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes)
			{
				closeProject();
			}
			else
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Cancelled close");
			}
		}
		else
		{
			closeProject();
		}
	}
}

// Detect user clicking the 'x' close window button.
void Moveit::closeEvent(QCloseEvent *bar)
{
	if (checkExitWithUnsavedData())
	{
		bar->accept();
	}
	else
	{
		bar->ignore();
	}
}

void Moveit::resetModeButtons()
{
	// Reset the icon.
	QIcon icon = QIcon("Resources/icon_Drag_trajectory_point.png");
	this->ui.actionDrag_trajectory_point->setIcon(icon);

}

void Moveit::sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath)
{
	if (IsScrutiny)
	{
		// Ref: https://karanbalkar.com/2014/02/sending-a-http-request-using-qt-5-framework/
		// create custom temporary event loop on stack
		QEventLoop eventLoop;

		// "quit()" the event-loop, when the network request "finished()"
		QNetworkAccessManager mgr;
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

		// Get idArray.
		int idArray[4] = { 0 };
		unsigned    nIds;
		__cpuid(idArray, 0);

		// Put idArray into string.
		char str[15];
		sprintf(str, "%x", idArray[1]);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside sendHTTPRequest. str: %s", str);

		// the HTTP request.
	    //QNetworkRequest req = QNetworkRequest( QUrl( QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Moveit&UserID=1&WorkgroupID=1&PlatformID=%1").arg(QString::number(*idArray)) ) );
		QNetworkRequest req = QNetworkRequest(QUrl(QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Moveit&UserID=1&WorkgroupID=1&PlatformID=%1&ActionKey=%2&ActionValue=%3&ElapsedTimeSecs=%4&ProgramVersion=%5&DataFileNameWithPath=%6&TotalProblemSize=%7&MaxKeyFrame=%8").arg(str).arg(actionKey).arg(actionValue).arg(elapsedTimeSecs).arg(PROGRAM_VERSION).arg(fileNameWithPath).arg(totalProblemSize).arg(project->get_MaxKeyFrame())));
		QNetworkReply *reply = mgr.get(req);

		eventLoop.exec(); // blocks stack until "finished()" has been called

		if (reply->error() == QNetworkReply::NoError)
		{
			//success
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Success: %s", reply->readAll());

			delete reply;
		}
		else
		{
			//failure
			qDebug() << "Failure" << reply->errorString();
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Failure: %s", reply->readAll());

			delete reply;
		}
	}
}

void Moveit::keyPressEvent(QKeyEvent *event)
{
	// Over-ride parent class method.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "keyPressEvent");

	// Get the focus widget.
	QWidget *w = QApplication::focusWidget();

	if (w == ui.mySpreadsheet)
	{
		// We are on the spreadsheet widget.
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We are focused on the spreadsheet");

		// Model of my first QTableWidget
		QItemSelectionModel *myModel = ui.mySpreadsheet->selectionModel();

		QModelIndexList list = myModel->selectedIndexes();

		if (list.size() > 0)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "List item found. Size: %i", list.size());

			for (int t = 0; t < list.size(); t++)
			{
				QModelIndex item = list.at(t);

				QString contentsOfCurrentItem = item.data().toString();

				int col = item.column();
				int row = item.row();

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Row: %i, Column: %i, Contents: %s", row, col, contentsOfCurrentItem.toStdString().c_str());

				int baseCol = col - 2;
				int surfaceIndex = baseCol / 6.0;
				int curveIndex = baseCol % 6;
				int segIndex = row - 1;

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Surface Index: %i, curveIndex: %i, segment index: %i", surfaceIndex, curveIndex, segIndex);

				// Update the value.
				if (segIndex > -1)
				{
					project->get_MySurfaces()->at(surfaceIndex)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(segIndex)->get_P1_p()->set_X(contentsOfCurrentItem.toFloat());
				}
				else
				{
					// Treat the first row differently.
					project->get_MySurfaces()->at(surfaceIndex)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(0)->get_P0_p()->set_X(contentsOfCurrentItem.toFloat());
				}

				// See if we need to update the start point of the next curve.
				int noOfSegments = project->get_MySurfaces()->at(surfaceIndex)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->size();

				if ((segIndex < noOfSegments - 1) && (segIndex > -1))
				{
					project->get_MySurfaces()->at(surfaceIndex)->get_MyTrajectoryCurves()->at(curveIndex)->get_MyTrajectoryCurveSegments()->at(segIndex + 1)->get_P0_p()->set_X(contentsOfCurrentItem.toFloat());
				}

				// Recompute tangents etc.
				project->get_MySurfaces()->at(surfaceIndex)->get_MyTrajectoryCurves()->at(curveIndex)->computeMySegmentEndTangentVectors();

				// Set flag.
				UnsavedChanges = true;
			}

			// Update the spreadsheet.
			updateSpreadsheet();

			// Update the plots.
			updateAllTabs();
		}
	}
	else
	{
		// We are on another widget.
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We are focused on another widget");
	}

	// Call the base class method.
	QMainWindow::keyPressEvent(event);
}

void Moveit::appendStatusTableWidget(const QString key, const QString val)
{
	if (MY_RUN_MODE == MYGUI)
	{
		QFont fnt;
		fnt.setPointSize(7);
		fnt.setFamily("Arial");

		QTableWidget* my_statusTable = ui.myStatusTableWidget;
		int insertRow = my_statusTable->rowCount();
		my_statusTable->insertRow(insertRow);

		QTableWidgetItem* new_itemKey = new QTableWidgetItem();
		new_itemKey->setText(key);  // 
		new_itemKey->setFont(fnt);
		my_statusTable->setItem(insertRow, 0, new_itemKey);

		QTableWidgetItem* new_itemVal = new QTableWidgetItem();
		new_itemVal->setText(val);
		new_itemVal->setFont(fnt);
		my_statusTable->setItem(insertRow, 1, new_itemVal);

		// Try to change the default row height.
		my_statusTable->resizeRowsToContents();

		my_statusTable->scrollToBottom();

		my_statusTable->update();
	}
}