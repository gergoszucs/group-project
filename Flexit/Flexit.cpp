#include "Flexit.h"
#include <QFile>
#include <QMessageBox> // QMessageBox
#include <QFileDialog> // QFileDialog for open data file.
#include <QCloseEvent> // Used in closeEvent.
#include <Windows.h> // For retrieving MAC address for http request.
#include <iphlpapi.h> // For retrieving MAC address for http request.

#pragma comment(lib, "iphlpapi.lib")
#include "global.h"
#include "ITIO.h"
#include "ITSurface.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITGust.h"
#include "ITPhysics.h" // For the playOut methods.
#include "ITPoint.h"
#include "MyAboutDialogBox.h"
#include "qjsonmodel.h" // To display the JSON data in the QTreeView tab.
#include "qjsonitem.h"

Flexit::Flexit(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	// Disable the Close button.
	ui.actionClose->setEnabled(false);

	// Disable the Pause button.
	ui.actionPause->setEnabled(false);

	// Disable the Dry run button.
	ui.actionDry_run->setEnabled(false);

	// Disaable the Step button.
	ui.actionStep->setEnabled(false);

	// Disable the start button.
	ui.actionStart->setEnabled(false);

	// Disable the stop button.
	ui.actionStop->setEnabled(false);

	// Disable the reset button.
	ui.actionReset->setEnabled(false);

	// Disable the replay button.
	ui.actionStartReplay->setEnabled(false);

	// Disable the Trim button.
	ui.actionTrim->setEnabled(false);

	// Disable the Save As button.
	ui.actionSave_As->setEnabled(false);

	QIcon icon = QIcon("Resources/blackDot.png");

	// Default display OpenGL axes enabled.
	ui.actionAxes->setIcon(icon);
	ui.actionControl_points->setIcon(icon);
	ui.actionInterpolated_points->setIcon(icon);
	ui.actionTracks->setIcon(icon);
	ui.actionPanels->setIcon(icon);
	ui.actionNormals->setIcon(icon);
	ui.actionVorticities->setIcon(icon);
	ui.actionBound_vortices->setIcon(icon);
	ui.actionWakes->setIcon(icon);
	ui.actionAnnotations->setIcon(icon);
	ui.actionPressure_distribution->setIcon(icon);
	ui.actionDrawVelocityField->setIcon(icon);
	ui.actionBeam_Elements->setIcon(icon);
	ui.actionBeam_ghost_elements->setIcon(icon);
	ui.actionBeam_nodes->setIcon(icon);
	ui.actionDeflected_beam_nodes->setIcon(icon);
	ui.actionBeam_ghost_nodes->setIcon(icon);
	ui.actionBeam_loading->setIcon(icon);
	ui.actionGust->setIcon(icon);

	ui.actionQuiet_graphics->setIcon(QIcon());

	ui.actionTranslate_view_with_surface->setIcon(QIcon()); // Disabled.
															// Default compute modes enabled.
	ui.actionUVLM->setIcon(icon);
	ui.actionEuler_Bernoulli->setIcon(QIcon()); // EB Mode default is false on program launch.
	ui.actionVelocity_field->setIcon(icon);

	// Set up the status table view.
	QTableWidget* my_statusTable = ui.myStatusTableWidget;
	my_statusTable->setColumnCount(2);

	my_statusTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Key")));
	my_statusTable->setColumnWidth(0, 40);

	my_statusTable->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Value")));
	my_statusTable->setColumnWidth(1, 130);

	my_statusTable->verticalHeader()->setVisible(false);
}

void Flexit::on_actionOpen_triggered()
{
	if (!IsDataLoaded)
	{
		if (!UnsavedChanges)
		{
			QString d = QDir::currentPath();
			QString fileNameWithPath = QFileDialog::getOpenFileName(this, tr("Open File"), d.append("/Data"), tr("JSON Files (*.json *.JSON);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));

			// (re-)Set the flag.
			IsDataLoaded = false;

			if (!fileNameWithPath.isEmpty())
			{
				loadData(fileNameWithPath);
			} // End if filename is valid.
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

void Flexit::loadData(QString fileNameWithPath)
{
	QFile file(fileNameWithPath);
	if (!file.open(QIODevice::ReadOnly))
	{
		if (MY_RUN_MODE == MYGUI)
		{
			QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
		}
		return;
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "About to read data from the file.");

	if (MY_RUN_MODE == MYGUI)
	{
		// Only display the file if it is small.
		QByteArray latin1BAFileNameWithPathString = fileNameWithPath.toLatin1();
	}

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

		QString text = "Flexit: " + QString(base_filename.c_str());
		w->setWindowTitle(text);

		w->statusBar()->showMessage(QObject::tr("File read successful"));

		// Enable the Start button.
		ui.actionStart->setEnabled(true);

		// Enable the Step button.
		ui.actionStep->setEnabled(true);

		// Enable the Close button.
		ui.actionClose->setEnabled(true);

		// Enable the Step button.
		ui.actionStep->setEnabled(true);

		// Enable the Dry run button.
		ui.actionDry_run->setEnabled(true);

		// Disable the Replay button.
		ui.actionStartReplay->setEnabled(false);

		// Disable the Pause button.
		ui.actionPause->setEnabled(false);

		// Disable the Stop button.
		ui.actionStop->setEnabled(false);

		//Enable the Trim button.
		ui.actionTrim->setEnabled(true);

		// Disable the Reset button.
		ui.actionReset->setEnabled(false);

		// Enable the Save As button.
		ui.actionSave_As->setEnabled(true);
	}

	// Actually read the data and instanciate the C++ objects.
	QByteArray latin1BAFileNameWithPathString = fileNameWithPath.toLatin1();
	ITIO::readJSONInputFile(latin1BAFileNameWithPathString.data()); // In ITIO.cpp.

	if (MY_RUN_MODE == MYGUI)
	{
		if (project->get_IsDoVelocityField())
		{
			// Set ui on.
			QIcon icon = QIcon("Resources/blackDot.png");
			ui.actionVelocity_field->setIcon(icon);
		}
		else
		{
			ui.actionVelocity_field->setIcon(QIcon());
		}
	}

	// Organise the gust.
	if (project->get_IsGust())
	{
		ITGust *g = new ITGust(20.0, 0.0, 0.0, 10.0, 1640.0);
		project->set_MyGust(g);
	}
	else
	{
		ITGust *g = new ITGust(0.0, 0.0, 0.0, 0.0, 0.0);
		project->set_MyGust(g);
	}

	// Try to read the Velocity Field History data.
	ITIO::readMyVelocityFieldHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Pressure History data.
	ITIO::readMyPressureHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Vorticity History data.
	ITIO::readMyVorticityHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Wake Panel History data.
	ITIO::readMyWakePanelHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Beam Node History data.
	ITIO::readMyBeamNodeHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Output Data History data.
	ITIO::readMyOutputDataHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Try to read the Control Point Deflection History data.
	ITIO::readMyControlPointDeflectionHistoryFromFile(latin1BAFileNameWithPathString.data());

	// Read the beam node data from the JSON file.
	ITIO::readJSONBeamNodes(latin1BAFileNameWithPathString.data()); // In ITIO.cpp.

	// Compute the interpolated points and the ITPanels.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		project->get_MySurfaces()->at(k)->instanciateMyPanels(k);

		project->get_MyBaseSurfaces()->at(k)->manageComputationOfInterpolatedPoints();
		project->get_MyBaseSurfaces()->at(k)->instanciateMyPanels(k);

		if (IsModeEulerBernoulli)
		{
			// project->get_MySurfaces()->at(k)->get_MyBeam()->createBeamElements();
			// project->get_MySurfaces()->at(k)->get_MyBeam()->createGhostBeamElements();
			// project->get_MySurfaces()->at(k)->get_MyBeam()->createBeamNodes();
			// project->get_MySurfaces()->at(k)->get_MyBeam()->createGhostBeamNodes();
		}
	}

	// Read the control surfaces from the JSON file.
	ITIO::readJSONControlSurfaces(latin1BAFileNameWithPathString.data()); // In ITIO.cpp.

	// Compute the data for trajectory curves.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int t = 0; t < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->size(); t++)
		{
			for (int n = 0; n < project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(t)->get_MyTrajectoryCurveSegments()->size(); n++)
			{
				project->get_MySurfaces()->at(k)->get_MyTrajectoryCurves()->at(t)->get_MyTrajectoryCurveSegments()->at(n)->calculateData();
			}
		}
	}

	// Set the flag.
	IsDataLoaded = true;

	// Reset the FrameNumber.
	FrameNumber = 0;

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Loaded"), cummulativeElapsedTimeSeconds, 0, fileNameWithPath);

	if (MY_RUN_MODE == MYGUI)
	{
		// Enable the close button.
		ui.actionClose->setEnabled(true);

		// Enable the Step button.
		ui.actionStep->setEnabled(true);

		// Enable the Dry run button.
		ui.actionDry_run->setEnabled(true);

		if (project->get_MySurfaces()->at(0)->get_MyWakePanelHistory()->empty())
		{
			// Disable the History Replay button.
			ui.actionStartReplay->setEnabled(false);
		}
		else
		{
			// Enable the History Replay button.
			ui.actionStartReplay->setEnabled(true);
		}

		// Display a satus message.
		appendStatusTableWidget(QString("File"), QString("Loaded"));

		// Populate the display matrix.
		managePopulationOfDisplayMatrix();

		// Finally force process events.
		QApplication::processEvents();
	}
}

void Flexit::sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath)
{
	if (IsScrutiny)
	{
		// create custom temporary event loop on stack
		QEventLoop eventLoop;

		// "quit()" the event-loop, when the network request "finished()"
		QNetworkAccessManager mgr;
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

		// Get idArray.
		int idArray[4] = { 0, 0, 0, 0 };
		unsigned    nIds;
		__cpuid(idArray, 1);

		// Put idArray into string.
		char str0[15];
		sprintf(str0, "%x", idArray[0]);
		char str1[15];
		sprintf(str1, "%x", idArray[1]);
		char str2[15];
		sprintf(str2, "%x", idArray[2]);
		char str3[15];
		sprintf(str3, "%x", idArray[3]);

		std::string string0(str0);
		std::string string1(str1);
		std::string string2(str2);
		std::string string3(str3);

		std::string stringFinal = string0 + string1 + string2 + string3;

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. str0: %s", str0);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. str1: %s", str1);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. str2: %s", str2);
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. str3: %s", str3);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. stringFinal: %s", stringFinal);

		// ========================================================================
		// MAC Address. http://stackoverflow.com/questions/13646621/how-to-get-mac-address-in-windows-with-c
		// ========================================================================

		PIP_ADAPTER_INFO AdapterInfo;
		DWORD dwBufLen = sizeof(AdapterInfo);
		char *mac_addr = (char*)malloc(17);
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

		// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
		{
			AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
			if (AdapterInfo == NULL)
			{
				printf("Error allocating memory needed to call GetAdaptersinfo\n");
			}
		}

		if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
		{
			PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
			do {
				sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
					pAdapterInfo->Address[0], pAdapterInfo->Address[1],
					pAdapterInfo->Address[2], pAdapterInfo->Address[3],
					pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside sendHTTPRequest. Address: %s, mac: %s", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);

				pAdapterInfo = pAdapterInfo->Next;

			} while (pAdapterInfo);
		}
		free(AdapterInfo);

		std::string macString(mac_addr);

		// ========================================================================
		// End of MAC Address. http://stackoverflow.com/questions/13646621/how-to-get-mac-address-in-windows-with-c
		// ========================================================================

		// the HTTP request.
		//QNetworkRequest req = QNetworkRequest( QUrl( QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Flexit&UserID=1&WorkgroupID=1&PlatformID=%1").arg(QString::number(*idArray)) ) );
		// QNetworkRequest req = QNetworkRequest(QUrl(QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Flexit&UserID=1&WorkgroupID=1&PlatformID=%1&ActionKey=%2&ActionValue=%3&ElapsedTimeSecs=%4&ProgramVersion=%5&DataFileNameWithPath=%6&TotalProblemSize=%7&MaxKeyFrame=%8").arg(str).arg(actionKey).arg(actionValue).arg(elapsedTimeSecs).arg(PROGRAM_VERSION).arg(fileNameWithPath).arg(totalProblemSize).arg(project->get_MaxKeyFrame())));
		QNetworkRequest req = QNetworkRequest(QUrl(QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Flexit&UserID=1&WorkgroupID=1&PlatformID=%1&ActionKey=%2&ActionValue=%3&ElapsedTimeSecs=%4&ProgramVersion=%5&DataFileNameWithPath=%6&TotalProblemSize=%7&MaxKeyFrame=%8&MACAddress=%9&GPUName=%10&CPUName=%11").arg(QString::fromStdString(macString)).arg(actionKey).arg(actionValue).arg(elapsedTimeSecs).arg(PROGRAM_VERSION).arg(fileNameWithPath).arg(totalProblemSize).arg(project->get_MaxKeyFrame()).arg(QString::fromStdString(macString)).arg(GPUName).arg(QString::fromStdString(stringFinal))));
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

void Flexit::appendStatusTableWidget(const QString key, const QString val)
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

void Flexit::managePopulationOfDisplayMatrix()
{
	// Called from loadData.
	// Compute the number of panels.
	int noOfPanels = 0;
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyPanels()->size(); i++)
		{
			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyPanels()->at(i).size(); j++)
			{
				noOfPanels++;
			}
		}
	}

	// Update the matrix for the OpenGL view.
	ui.myMatrixViewGLWidget->populateDisplayMatrix(noOfPanels);
}

void Flexit::manageUpdateOfDisplayMatrix(int noOfPanels)
{
	// Update the matrix for the OpenGL view.
	ui.myMatrixViewGLWidget->updateDisplayMatrix();
}

void Flexit::setHardwareReportTextEdit(const QString str)
{
	ui.hardwareReportTextEdit->setText(str);
}

void Flexit::updateAllTabsForCurrentFrameNumber()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside updateAllTabsForCurrentFrameNumber");

	// Update the main geometry OpenGL view.
	ui.myGLWidget->update();

	// Update the output graphs.
	ui.myOutputGraphicsGLWidget->update();

	// Update the Euler-Bernoulli OpenGL view.
	ui.myEBWidget->update();

	// Update the Matrix of Influence coefficients OpenGL texture view.
	ui.myMatrixViewGLWidget->update();
}

void Flexit::on_collapseAllButton_clicked()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "on_collapseAllButton_clicked");
	ui.jsonTreeView->collapseAll();
}

void Flexit::on_expandAllButton_clicked()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "on_expandAllButton_clicked");
	ui.jsonTreeView->expandAll();
}

void Flexit::appendDataViewTextEdit(const QString str)
{
	ui.myDataViewTextEdit->append(str);
}

void Flexit::on_actionEuler_Bernoulli_triggered()
{
	IsModeEulerBernoulli = !IsModeEulerBernoulli;

	if (MY_RUN_MODE == MYGUI)
	{
		if (IsModeEulerBernoulli)
		{
			QIcon icon = QIcon("Resources/blackDot.png");
			ui.actionEuler_Bernoulli->setIcon(icon);

			// Display a satus message.
			appendStatusTableWidget(QString("Sim"), QString("EB Enabled"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Sim"), QString("EB Enabled"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
		else
		{
			ui.actionEuler_Bernoulli->setIcon(QIcon());

			// Display a satus message.
			appendStatusTableWidget(QString("Sim"), QString("EB Disabled"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Sim"), QString("EB Disabled"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
	}
}

void Flexit::on_actionPanels_triggered()
{
	drawPanels = !drawPanels;

	if (drawPanels)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionPanels->setIcon(icon);
	}
	else
	{
		ui.actionPanels->setIcon(QIcon());
	}
}

void Flexit::on_actionControl_points_triggered()
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

void Flexit::on_actionInterpolated_points_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "on_actionInterpolated_points_triggered");

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

void Flexit::on_actionVorticities_triggered()
{
	drawVorticities = !drawVorticities;

	if (drawVorticities)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionVorticities->setIcon(icon);
	}
	else
	{
		ui.actionVorticities->setIcon(QIcon());
	}
}

void Flexit::on_actionWakes_triggered()
{
	drawWakes = !drawWakes;

	if (drawWakes)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionWakes->setIcon(icon);
	}
	else
	{
		ui.actionWakes->setIcon(QIcon());
	}
}

void Flexit::on_actionBound_vortices_triggered()
{
	drawBoundVortices = !drawBoundVortices;

	if (drawBoundVortices)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionBound_vortices->setIcon(icon);
	}
	else
	{
		ui.actionBound_vortices->setIcon(QIcon());
	}
}

void Flexit::on_actionNormals_triggered()
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

void Flexit::on_actionVelocity_field_triggered()
{
	if (IsDataLoaded)
	{
		// Fiddle with project instance variables.
		if (project->get_IsDoVelocityField())
		{
			// IsDoVelocityField is currently enabled, so nmow disable it.
			project->set_IsDoVelocityField(false);

			// Set ui off.
			ui.actionVelocity_field->setIcon(QIcon());

			// Display a status message.
			appendStatusTableWidget(QString("Sim"), QString("Vel Field Disabled"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Sim"), QString("Vel Field Disabled"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
		else
		{
			// IsDoVelocityField is currently disabled, so now enable it.
			project->set_IsDoVelocityField(true);

			// Set ui on.
			QIcon icon = QIcon("Resources/blackDot.png");
			ui.actionVelocity_field->setIcon(icon);

			// Display a status message.
			appendStatusTableWidget(QString("Sim"), QString("Vel Field Enabled"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Sim"), QString("Vel Field Enabled"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
	}
	else
	{
		// Display warning message.
		QMessageBox::information(0, tr("No input data"), tr("Please open a data file before changing this mode."));
	}
}

void Flexit::on_actionDrawVelocityField_triggered()
{
	drawVelocityField = !drawVelocityField;

	if (drawVelocityField)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionDrawVelocityField->setIcon(icon);
	}
	else
	{
		ui.actionDrawVelocityField->setIcon(QIcon());
	}
}

void Flexit::on_actionGust_triggered()
{
	drawGust = !drawGust;

	if (drawGust)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionGust->setIcon(icon);
	}
	else
	{
		ui.actionGust->setIcon(QIcon());
	}
}

void Flexit::on_actionPressure_distribution_triggered()
{
	drawPressure = !drawPressure;

	if (drawPressure)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionPressure_distribution->setIcon(icon);
	}
	else
	{
		ui.actionPressure_distribution->setIcon(QIcon());
	}
}

void Flexit::on_actionAnnotations_triggered()
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

void Flexit::on_actionTracks_triggered()
{
	drawTracks = !drawTracks;

	if (drawTracks)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionTracks->setIcon(icon);
	}
	else
	{
		ui.actionTracks->setIcon(QIcon());
	}
}

void Flexit::on_actionDry_run_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Dry run");

	// Perform a dry run.
	if (IsDataLoaded)
	{
		w->statusBar()->showMessage(QObject::tr("Dry run in progress. Please wait ..."));

		// Set flags.
		IsPaused = false;
		IsSimulating = false;
		IsReplay = false;
		IsDryRun = true;

		// Disable the Dry-run button.
		ui.actionDry_run->setEnabled(false);

		//Disable the Trim button.
		ui.actionTrim->setEnabled(false);

		// Disable the Step button.
		ui.actionStep->setEnabled(false);

		// Disable the Start button.
		ui.actionStart->setEnabled(false);

		// Disable the Replay button.
		ui.actionStartReplay->setEnabled(false);

		// Enable the Pause button.
		ui.actionPause->setEnabled(true);

		// Disable the Close button.
		ui.actionClose->setEnabled(false);

		// Disable the reset button.
		ui.actionReset->setEnabled(false);

		// Reset the frame number.
		FrameNumber = 0;

		// Display a status message.
		appendStatusTableWidget(QString("Dry"), QString("Started"));

		// Send the HTTP request.
		sendHTTPRequest(QString("Dry"), QString("Started"), cummulativeElapsedTimeSeconds, 0, DataFileNameWithPath);

		// Do the work. This call may return either at the end of the simulation or when the Pause button was pressed.
		ITPhysics::playOutDryRun();

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Dry run completed. FrameNumber: %i", FrameNumber);

		if (FrameNumber == project->get_MaxKeyFrame())
		{
			// The Dry run has completed without a pause, so enable the Start button.
			ui.actionStart->setEnabled(true);

			// Enable the Step button.
			ui.actionStep->setEnabled(true);

			// Enable the Start button.
			ui.actionStart->setEnabled(true);

			// Enable the reset button.
			ui.actionReset->setEnabled(true);

			// Enable the Trim button.
			ui.actionTrim->setEnabled(true);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);

			// Display a status message.
			appendStatusTableWidget(QString("Dry"), QString("Completed"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Dry"), QString("Completed"), cummulativeElapsedTimeSeconds, 0, DataFileNameWithPath);

			w->statusBar()->showMessage(QObject::tr("Dry run completed successfully."));
		}

		// Enable the Dry-run button.
		ui.actionDry_run->setEnabled(true);

		// Disable the Pause button.
		ui.actionPause->setEnabled(false);
	}
	else
	{
		QMessageBox::information(0, tr("No input data"), tr("Please open a data file before performing calculations."));
	}
}

void Flexit::on_actionStartReplay_triggered()
{
	// Enable the Pause button.
	ui.actionPause->setEnabled(true);

	// Disable the Start button.
	ui.actionStart->setEnabled(false);

	// Disable the Step button.
	ui.actionStep->setEnabled(false);

	// Disable the Dry Run button.
	ui.actionDry_run->setEnabled(false);

	// Disable the Trim button.
	ui.actionTrim->setEnabled(false);

	// Disable the Close button.
	ui.actionClose->setEnabled(false);

	// Disable the reset button.
	ui.actionReset->setEnabled(false);

	// Perform a replay.
	if (IsDataLoaded)
	{
		// Set flag.
		IsSimulating = false;
		IsPaused = false;
		IsReplay = true;

		// Reset frame number.
		FrameNumber = 0;

		// Enable the Pause button.
		ui.actionPause->setEnabled(true);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside on_actionStartReplay_triggered. IsPaused: %i, IsReplay: %i.", IsPaused, IsReplay);

		// Display a satus message.
		appendStatusTableWidget(QString("Replay"), QString("Started"));

		// Send the HTTP request.
		sendHTTPRequest(QString("Replay"), QString("Started"), 0.0, project->get_TotalProblemSize(), DataFileNameWithPath);

		// Actually do the computations.
		ITPhysics::playOutReplay();

		// If we have reached the end of the replay and we want to restart the replay, then reset the frame number.
		if (FrameNumber == project->get_MaxKeyFrame())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside on_actionStartReplay_triggered. We are at the end of the replay, so resetting FrameNumber.");

			w->statusBar()->showMessage(QObject::tr("Replay completed successfully."));

			IsSimulating = false;
			IsPaused = false;
			IsReplay = false;

			// Disable the Pause button.
			ui.actionPause->setEnabled(false);

			// Enable the Start button.
			ui.actionStart->setEnabled(true);

			// Enable the Step button.
			ui.actionStep->setEnabled(true);

			// Enable the Dry Run button.
			ui.actionDry_run->setEnabled(true);

			// Enable the Trim button.
			ui.actionTrim->setEnabled(true);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);

			// Enable the reset button.
			ui.actionReset->setEnabled(true);

			// Display a satus message.
			appendStatusTableWidget(QString("Replay"), QString("Completed"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Replay"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
	}
}

void Flexit::on_actionStep_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionStep_triggered.");

	// Perform a single step in the simulation.
	if (IsDataLoaded)
	{
		w->statusBar()->showMessage(QObject::tr("Single step in progress. Please wait ..."));

		// Set flags.
		IsPaused = false;
		IsSimulating = false;
		IsStep = true;
		IsReplay = false;
		IsDryRun = false;

		// Disable the Start button.
		ui.actionStart->setEnabled(false);

		// Disable the Start Replay button.
		ui.actionStartReplay->setEnabled(false);

		// Disable the Step button.
		ui.actionStep->setEnabled(false);

		// Enable the Pause button.
		ui.actionPause->setEnabled(true);

		// Enable the Stop button.
		ui.actionStop->setEnabled(true);

		// Disable the Trim button.
		ui.actionTrim->setEnabled(false);

		// Disable the Close button.
		ui.actionClose->setEnabled(false);

		// Display a status message.
		appendStatusTableWidget(QString("Sim"), QString("Step"));

		// ==============================================================================================================
		// Do the work. This call only returns here.
		// ==============================================================================================================
		// Check if the simulation finished.
		if (FrameNumber < project->get_MaxKeyFrame())
		{
			// Do nothing to FrameNumber.
		}
		else
		{
			// FrameNumber == MaxKeyFrame, so reset the frame number to zero.
			FrameNumber = 0;
		}

		cummulativeElapsedTimeSeconds = 0;

		ITPhysics::playOutStep();

		w->statusBar()->showMessage(QObject::tr("Step completed successfully."));

		// Send HTTP log message.
		sendHTTPRequest(QString("Sim"), QString("Step"), cummulativeElapsedTimeSeconds, 0, DataFileNameWithPath);

		// Display the contents of the influence coefficient matrix.
		displayInfluenceCoefficientTable();

		// Reset the elapsed time.
		cummulativeElapsedTimeSeconds = 0.0;

		// Leave the flags.
		// Disable the Pause button.
		ui.actionPause->setEnabled(false);

		// Disable the Stop button.
		ui.actionStop->setEnabled(false);

		// Enable the reset button.
		ui.actionReset->setEnabled(true);

		// Enable the Start button.
		ui.actionStart->setEnabled(true);

		// Enable the Step button.
		ui.actionStep->setEnabled(true);

		// Enable the Dry-run button.
		ui.actionDry_run->setEnabled(true);

		// Enable the Trim button.
		ui.actionTrim->setEnabled(true);

		// Enable the Close button.
		ui.actionClose->setEnabled(true);
	}
	else
	{
		QMessageBox::information(0, tr("No input data"), tr("Please open a data file before performing calculations."));
	}
}

void Flexit::on_actionStart_triggered()
{
	// Disable the Start button.
	ui.actionStart->setEnabled(false);

	// Disable the Dry Run button.
	ui.actionDry_run->setEnabled(false);

	// Disable the Replay button.
	ui.actionStartReplay->setEnabled(false);

	// Disable the Step button.
	ui.actionStep->setEnabled(false);

	// Disable the Trim button.
	ui.actionTrim->setEnabled(false);

	// Enable the Pause button.
	ui.actionPause->setEnabled(true);

	// Disable the Stop button.
	ui.actionStop->setEnabled(false);

	// Disable the reset button.
	ui.actionReset->setEnabled(false);

	// Disable the Close button.
	ui.actionClose->setEnabled(false);

	// Perform a simulation.
	if (IsDataLoaded)
	{
		w->statusBar()->showMessage(QObject::tr("Calculation in progress. Please wait ..."));

		// Display a satus message.
		appendStatusTableWidget(QString("Sim"), QString("Started"));

		// Send HTTP log message.
		sendHTTPRequest(QString("Sim"), QString("Started"), 0.0, 0, DataFileNameWithPath);

		if (IsStep)
		{
			// Don't need to reset the FrameNumber.
		}
		else
		{
			// Reset the frame number.
			FrameNumber = 0;
		}

		// Set flags.
		IsPaused = false;
		IsSimulating = true;
		IsStep = false;
		IsReplay = false;
		IsDryRun = false;

		// Enable the Pause button.
		ui.actionPause->setEnabled(true);

		// Enable the Stop button.
		ui.actionStop->setEnabled(true);

		// ==============================================================================================================
		// Do the work. This call may return either at the end of the simulation or when the Pause button was pressed.
		// ==============================================================================================================
		ITPhysics::playOutFlexit();

		// Check if the simulation finished (without having paused).
		if (FrameNumber == project->get_MaxKeyFrame())
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside on_actionStart_triggered. FrameNumber == maxKeyFrame.");

			w->statusBar()->showMessage(QObject::tr("Simulation completed successfully."));

			// Disable the Pause button.
			ui.actionPause->setEnabled(false);

			// Disable the Stop button.
			ui.actionStop->setEnabled(false);

			// Enable the reset button.
			ui.actionReset->setEnabled(true);

			// Enable the Start button.
			ui.actionStart->setEnabled(true);

			// Enable the Step button.
			ui.actionStep->setEnabled(true);

			// Enable the Replay button.
			ui.actionStartReplay->setEnabled(true);

			// Enable the Dry-run button.
			ui.actionDry_run->setEnabled(true);

			// Enable the Trim button.
			ui.actionTrim->setEnabled(true);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);

			// Display a status message.
			appendStatusTableWidget(QString("Sim"), QString("Completed"));

			// Send log message to server.
			sendHTTPRequest(QString("Sim"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

			// Display the output data in the output table.
			bool showOutputDataTable = true;
			if (showOutputDataTable)
			{
				showMyOutputTable();
			}

			// Display the contents of the influence coefficient matrix.
			displayInfluenceCoefficientTable();

			// Reset the frame number.
			FrameNumber = 0;
		}
		else
		{
			// The simulation is still active, so make sure the Pause and Stop buttons are active.
			// Enable the Pause button.
			ui.actionPause->setEnabled(true);

			// Enable the Stop button.
			ui.actionStop->setEnabled(true);
		}
	}
	else
	{
		QMessageBox::information(0, tr("No input data"), tr("Please open a data file before performing calculations."));
	}
}

void Flexit::on_actionPause_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionPause_clicked.");

	IsPaused = !IsPaused;

	if (IsPaused)
	{
		// The user has paused the calculation or the dry run or the replay.
		if (IsSimulating)
		{
			// The user has paused the simulation.
			QString str2 = QString("Calculation is paused. FrameNumber: %1").arg(QString::number(FrameNumber));
			statusBar()->showMessage(str2);

			IsSimulating = true;

			// Enable the Step button.
			// ui.actionStep->setEnabled(true);
			// Display a satus message.
			appendStatusTableWidget(QString("Sim"), QString("Paused"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Sim"), QString("Paused"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);
		}
		else if (IsDryRun)
		{
			QString str2 = QString("Dry-run is paused. FrameNumber: %1").arg(QString::number(FrameNumber));
			statusBar()->showMessage(str2);

			IsDryRun = true;

			// Enable the pause button (so the user can unpause the dry run).
			ui.actionPause->setEnabled(true);

			bool isEnabled = ui.actionPause->isEnabled();

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "actionPause isEnabled %i.", isEnabled);

			// Enable the stop button (so the user can stop the dry run).
			ui.actionStop->setEnabled(true);

			// Disable the Step button.
			ui.actionStep->setEnabled(false);

			// Disable the start button (so the user can't start a simulation part-way through a dry run).
			ui.actionStart->setEnabled(false);

			// Disable the dry run button (so the user can't start a dry run part-way through a dry run).
			ui.actionDry_run->setEnabled(false);

			// Disable the replay button (so the user can't start a replay part-way through a dry run).
			ui.actionStartReplay->setEnabled(false);

			// Disable the Trim button.
			ui.actionTrim->setEnabled(false);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);

			// Display a satus message.
			appendStatusTableWidget(QString("Dry"), QString("Paused"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Dry"), QString("Paused"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
		else if (IsStep)
		{
			QString str2 = QString("Step is paused. FrameNumber: %1").arg(QString::number(FrameNumber));
			statusBar()->showMessage(str2);

			IsStep = true;

			// Display a satus message.
			appendStatusTableWidget(QString("Step"), QString("Finished"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Step"), QString("Finished"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
		else if (IsReplay)
		{
			QString str2 = QString("Replay is paused. FrameNumber: %1").arg(QString::number(FrameNumber));
			statusBar()->showMessage(str2);

			IsReplay = true;

			// Enable the stop button (so the user can stop the replay).
			ui.actionStop->setEnabled(true);

			// Disable the Step button.
			ui.actionStep->setEnabled(false);

			// Disable the start button (so the user can't start a simulation part-way through a replay).
			ui.actionStart->setEnabled(false);

			// Disable the dry run button (so the user can't start a dry run part-way through a replay).
			ui.actionDry_run->setEnabled(false);

			// Disable the replay button (so the user can't start a replay part-way through a replay).
			ui.actionStartReplay->setEnabled(false);

			// Disable the Trim button.
			ui.actionTrim->setEnabled(false);

			// Enable the Close button.
			ui.actionClose->setEnabled(true);

			// Display a status message.
			appendStatusTableWidget(QString("Replay"), QString("Paused"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Replay"), QString("Paused"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
		}
		else
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionPause_clicked. Something went wrong.");
		}
	}
	else
	{
		// The user has toggled the Pause button off, so he/she wants to resume calculating or dry-running or replaying.
		if (IsSimulating)
		{
			// Disable the Step button.
			//ui.actionStep->setEnabled(false);
			if (FrameNumber < project->get_MaxKeyFrame())
			{
				// Display a status message.
				appendStatusTableWidget(QString("Sim"), QString("Resumed"));

				// Send the HTTP request.
				sendHTTPRequest(QString("Sim"), QString("Resumed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

				// Disable the Close button.
				ui.actionClose->setEnabled(false);

				// Resume the simulation.
				ITPhysics::playOutFlexit();

				if (FrameNumber == project->get_MaxKeyFrame())
				{
					// The simulation has completed after at least one temporary pause, so reset the buttons and the flags.
					// Set flags.
					IsPaused = false;
					IsSimulating = false;
					IsStep = false;
					IsReplay = false;
					IsDryRun = false;

					// Reset the buttons.
					// Disable the Pause button.
					ui.actionPause->setEnabled(false);

					// Disable the Stop button.
					ui.actionStop->setEnabled(false);

					// Enable the reset button.
					ui.actionReset->setEnabled(true);

					// Enable the Dry run button.
					ui.actionDry_run->setEnabled(true);

					// Enable the Start button.
					ui.actionStart->setEnabled(true);

					// Enable the Step button.
					ui.actionStep->setEnabled(true);

					// Enable the Replay button.
					ui.actionStartReplay->setEnabled(true);

					// Enable the Trim button.
					ui.actionTrim->setEnabled(true);

					// Enable the Close button.
					ui.actionClose->setEnabled(true);

					// Enable the Reset button.
					//ui.actionReset->setEnabled(true);

					// Display a status message.
					w->statusBar()->showMessage(QObject::tr("Simulation completed successfully."));

					// Display a status message.
					appendStatusTableWidget(QString("Sim"), QString("Completed"));

					// Send the HTTP request.
					sendHTTPRequest(QString("Sim"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

					// Reset elapsed time.
					cummulativeElapsedTimeSeconds = 0.0;

					// Display influence coefficient table.
					displayInfluenceCoefficientTable();
				}
			}
			else if (FrameNumber == project->get_MaxKeyFrame())
			{
				// The simulation has completed after at least one temporary pause, so reset the buttons and the flags.
				// Set flags.
				IsPaused = false;
				IsSimulating = false;
				IsStep = false;
				IsReplay = false;
				IsDryRun = false;

				// Reset the buttons.
				// Disable the Pause button.
				ui.actionPause->setEnabled(false);

				// Disable the Stop button.
				ui.actionStop->setEnabled(false);

				// Enable the Dry run button.
				ui.actionDry_run->setEnabled(true);

				// Enable the Start button.
				ui.actionStart->setEnabled(true);

				// Enable the Step button.
				ui.actionStep->setEnabled(true);

				// Enable the Replay button.
				ui.actionStartReplay->setEnabled(true);

				// Enable the Trim button.
				ui.actionTrim->setEnabled(true);

				// Enable the Close button.
				ui.actionClose->setEnabled(true);

				// Enable the Reset button.
				ui.actionReset->setEnabled(true);

				// Display a status message.
				w->statusBar()->showMessage(QObject::tr("Simulation completed successfully."));

				// Display a status message.
				appendStatusTableWidget(QString("Sim"), QString("Completed"));

				// Send the HTTP request.
				sendHTTPRequest(QString("Sim"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

				// Reset elapsed time.
				cummulativeElapsedTimeSeconds = 0.0;

				// Display influence coefficient table.
				displayInfluenceCoefficientTable();
			}
		}
		else if (IsDryRun)
		{
			// Display a status message.
			appendStatusTableWidget(QString("Dry"), QString("Resumed"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Dry"), QString("Resumed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

			// Disable the Close button.
			ui.actionClose->setEnabled(false);

			// Resume the dry run.
			ITPhysics::playOutDryRun();

			if (FrameNumber == project->get_MaxKeyFrame())
			{
				// The dry run has completed after at least one temporary pause, so reset the buttons and the flags.
				// Set flags.
				IsPaused = false;
				IsSimulating = false;
				IsStep = false;
				IsReplay = false;
				IsDryRun = false;

				// Reset the buttons.
				// Disable the Pause button.
				ui.actionPause->setEnabled(false);

				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "In code that resumes a dry run after a pause.");

				// Disable the Stop button.
				ui.actionStop->setEnabled(false);

				// Enable the Dry run button.
				ui.actionDry_run->setEnabled(true);

				// Enable the Start button.
				ui.actionStart->setEnabled(true);

				// Enable the Step button.
				ui.actionStep->setEnabled(true);

				// Enable the Replay button.
				ui.actionStartReplay->setEnabled(true);

				// Enable the Trim button.
				ui.actionTrim->setEnabled(true);

				// Enable the Reset button.
				ui.actionReset->setEnabled(true);

				// Enable the Close button.
				ui.actionClose->setEnabled(true);

				// Display a status message.
				w->statusBar()->showMessage(QObject::tr("Dry run completed successfully."));

				// Display a status message.
				appendStatusTableWidget(QString("Dry"), QString("Completed"));

				// Send the HTTP request.
				sendHTTPRequest(QString("Dry"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

				// Reset elapsed time.
				cummulativeElapsedTimeSeconds = 0.0;
			}
		}
		else if (IsReplay)
		{
			// Display a status message.
			appendStatusTableWidget(QString("Replay"), QString("Resumed"));

			// Send the HTTP request.
			sendHTTPRequest(QString("Replay"), QString("Resumed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

			// Resume the replay.
			ITPhysics::playOutReplay();

			if (FrameNumber == project->get_MaxKeyFrame())
			{
				// The dry run has completed after at least one temporary pause, so reset the buttons and the flags.
				// Set flags.
				IsPaused = false;
				IsSimulating = false;
				IsStep = false;
				IsReplay = true; // Leave IsReplay true here so drawReplay continues to render.
				IsDryRun = false;

				// Reset the buttons.
				// Disable the Pause button.
				//ui.actionPause->setEnabled(false);

				// Disable the Stop button.
				ui.actionStop->setEnabled(false);

				// Enable the Dry run button.
				ui.actionDry_run->setEnabled(true);

				// Enable the Start button.
				ui.actionStart->setEnabled(true);

				// Enable the Step button.
				ui.actionStep->setEnabled(true);

				// Enable the Replay button.
				ui.actionStartReplay->setEnabled(true);

				// Enable the Trim button.
				ui.actionTrim->setEnabled(true);

				// Enable the Reset button.
				ui.actionReset->setEnabled(true);

				// Enable the Close button.
				ui.actionClose->setEnabled(true);

				// Display a status message.
				w->statusBar()->showMessage(QObject::tr("Replay completed successfully."));

				// Display a status message.
				appendStatusTableWidget(QString("Replay"), QString("Completed"));

				// Send the HTTP request.
				sendHTTPRequest(QString("Replay"), QString("Completed"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

				// Reset elapsed time.
				cummulativeElapsedTimeSeconds = 0.0;
			}
			else
			{
				// Display a status message.
				appendStatusTableWidget(QString("Replay"), QString("Paused"));

				// Send the HTTP request.
				sendHTTPRequest(QString("Replay"), QString("Paused"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);
			}
		}
		else
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionPause_clicked. Something went wrong.");
		}
	}
}

void Flexit::on_actionStop_triggered()
{
	if (IsDryRun)
	{
		// Set flags.
		IsPaused = false;
		IsSimulating = false;
		IsStep = false;
		IsReplay = false;
		IsDryRun = false;

		// Reset the FrameNumber.
		FrameNumber = 0;

		QString str2 = QString("Dry-run stopped.");
		statusBar()->showMessage(str2);

		// Show status log entry
		appendStatusTableWidget(QString("Dry"), QString("Stopped"));

		// Send the HTTP request.
		sendHTTPRequest(QString("Dry"), QString("Stopped"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

		// Reset elapsed time.
		cummulativeElapsedTimeSeconds = 0.0;
	}
	else if (IsSimulating)
	{
		// Set flags.
		IsPaused = false;
		IsSimulating = false;
		IsStep = false;
		IsReplay = false;
		IsDryRun = false;

		// Reset the FrameNumber.
		FrameNumber = 0;


		QString str2 = QString("Simulation stopped.");
		statusBar()->showMessage(str2);

		// Show status log entry
		appendStatusTableWidget(QString("Sim"), QString("Stopped"));

		// Send the HTTP request.
		sendHTTPRequest(QString("Sim"), QString("Stopped"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

		// Reset elapsed time.
		cummulativeElapsedTimeSeconds = 0.0;

		ui.actionStop->setEnabled(false);
	}

	// Reset the buttons.
	// Disable the Pause button.
	ui.actionPause->setEnabled(false);

	// Disable the Stop button.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Disabling Stop action.");
	ui.actionStop->setEnabled(false);
	ui.actionStop->setEnabled(false);
	ui.actionStop->setEnabled(false);
	ui.actionStop->setEnabled(false);
	ui.actionStop->setEnabled(false);
	ui.actionStop->setEnabled(false);
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Stop action disabled.");

	// Enable the Dry run button.
	ui.actionDry_run->setEnabled(true);

	// Enable the Start button.
	ui.actionStart->setEnabled(true);

	// Enable the Step button.
	ui.actionStep->setEnabled(true);

	// Enable the Trim button.
	ui.actionTrim->setEnabled(true);

	// Enable the Close button.
	ui.actionClose->setEnabled(true);

	// Enable the reset button.
	ui.actionReset->setEnabled(true);

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "End of on_actionStop_clicked.");
}

void Flexit::on_actionReset_triggered()
{
	// Note that this method deletes the histories, we ask the user to confirm.
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Reset Simulation", 
		"You are about to reset the simulation.\nThe results of computations will be lost.\nAre you sure you want to continue to reset?", QMessageBox::Yes | QMessageBox::No);
	
	if (reply == QMessageBox::Yes)
	{
		// Disable the Replay button.
		ui.actionStartReplay->setEnabled(false);

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionReset_triggered.");

		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			project->get_MySurfaces()->at(k)->moveMeBackToBase(k);
			project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

			// Move panels back to base.
			project->get_MySurfaces()->at(k)->instanciateMyPanels(k);

			project->get_MySurfaces()->at(k)->prepareForSimulation();
		}

		// Reset velocity field.
		for (int i = 0; i < project->get_VelocityFieldData()->size(); i++)
		{
			for (int j = 0; j < project->get_VelocityFieldData()->at(i).size(); j++)
			{
				project->get_VelocityFieldData()->at(i).at(j)->set_VX(0.0);
				project->get_VelocityFieldData()->at(i).at(j)->set_VY(0.0);
				project->get_VelocityFieldData()->at(i).at(j)->set_VZ(0.0);
			}
		}

		// Reset the frame number.
		FrameNumber = 0;

		// Reset elapsed time.
		cummulativeElapsedTimeSeconds = 0.0;

		// Reset the mode.
		IsSimulating = false;
		IsReplay = false;
		IsPaused = false;
	}

	// Update all the tabs.
	updateAllTabsForCurrentFrameNumber();
}

void Flexit::on_actionTest_triggered()
{
	// Show status log entry
	appendStatusTableWidget(QString("Test"), QString("Test run"));

	// Send the HTTP request.
	sendHTTPRequest(QString("Test"), QString("Test run"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

	// Reset elapsed time.
	cummulativeElapsedTimeSeconds = 0.0;

	ITPhysics::playOutTest();
}

void Flexit::on_actionClose_triggered()
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

void Flexit::closeProject()
{
	// Clear the text edit window.
	ui.fileTextEdit->clear();

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

	IsReplay = false;
	IsSimulating = false;
	IsPaused = false;
	IsDryRun = false;

	// Set the main window title.
	QString text = "Flexit: ";
	setWindowTitle(text);

	statusBar()->showMessage(QObject::tr("File closed successfully"));

	// Free the display matrix.
	ui.myMatrixViewGLWidget->freeDisplayMatrix();

	// Disable the close button.
	ui.actionClose->setEnabled(false);

	// Disable the Step button.
	ui.actionStep->setEnabled(false);

	// Disable the start button.
	ui.actionStart->setEnabled(false);

	// Disable the pause button.
	ui.actionPause->setEnabled(false);

	// Disable the stop button.
	ui.actionStop->setEnabled(false);

	// Disable the reset button.
	ui.actionReset->setEnabled(false);

	// Disable the Dry run button.
	ui.actionDry_run->setEnabled(false);

	// Disable the replay button.
	ui.actionStartReplay->setEnabled(false);

	//Disable the Trim button.
	ui.actionTrim->setEnabled(false);

	// Clear the output table.
	ui.myOutputTableWidget->clear();

	// Clear the Influence coefficient table.
	ui.myInfluenceCoeffTableWidget->clear();

	// Clear the data view text edit tab.
	ui.myDataViewTextEdit->clear();

	// Force a redraw.
	ui.myGLWidget->repaint();

	// Force a redraw.
	ui.myTrajectoryGLWidget->repaint();

	// Force a redraw.
	ui.myMatrixViewGLWidget->repaint();

	// Force a redraw.
	ui.myOutputGraphicsGLWidget->repaint();

	// Force a redraw.
	ui.myEBWidget->repaint();

	// Display a status message.
	appendStatusTableWidget(QString("File"), QString("Closed"));

	// Reset elapsed time.
	cummulativeElapsedTimeSeconds = 0.0;

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Closed"), cummulativeElapsedTimeSeconds, 0, DataFileNameWithPath);
}

void Flexit::on_actionAbout_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "on_actionAbout_triggered");

	MyAboutDialogBox *myAboutDialogBox = new MyAboutDialogBox();

	myAboutDialogBox->show();
}

void Flexit::on_actionWeb_help_triggered()
{
	system("start /max http://www.scrapeworld.com/surfit/index.shtml");
}

void Flexit::on_actionSave_As_triggered()
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
	sendHTTPRequest(QString("File"), QString("Saved"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

	// Finally set flags.
	UnsavedChanges = false;
}

void Flexit::on_actionExit_triggered()
{
	// Check for unsaved data and exit if everything is saved.
	if (checkExitWithUnsavedData())
	{
		qApp->quit();
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionExit_triggered. Just returned from checkExitWithUnsavedData.");
}

bool Flexit::checkExitWithUnsavedData()
{
	if (UnsavedChanges)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Unsaved Data", "You have unsaved data.\nAre you sure you want to quit?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			closeProject();

			// Send the HTTP request.
			sendHTTPRequest(QString("Program"), QString("Terminated"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

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
		sendHTTPRequest(QString("Program"), QString("Terminated"), cummulativeElapsedTimeSeconds, project->get_TotalProblemSize(), DataFileNameWithPath);

		return true;
	}
}

// Detect user clicking the 'x' close window button.
void Flexit::closeEvent(QCloseEvent *bar)
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

void Flexit::displayInfluenceCoefficientTable()
{
	// Display the contents of the influence coefficient matrix.
	bool showInfluenceCoefficientMatrix = true;
	if (showInfluenceCoefficientMatrix)
	{
		if (MY_RUN_MODE == MYGUI)
		{
			int noOfPanels = 0; // Initialize noOfPanels.
			for (int k = 0; k < project->get_MySurfaces()->size(); k++) // Loop through all the surfaces, adding the number of panels to r.
			{
				noOfPanels = noOfPanels + (project->get_MySurfaces()->at(k)->get_MyPanels()->size())*(project->get_MySurfaces()->at(k)->get_MyPanels()->at(0).size());
			}

			QTableWidget* my_table = ui.myInfluenceCoeffTableWidget;
			my_table->setRowCount(noOfPanels);
			my_table->setColumnCount(noOfPanels);

			for (int r = 0; r < noOfPanels; r++)
			{
				for (int c = 0; c < noOfPanels; c++)
				{
					QTableWidgetItem* new_item = new QTableWidgetItem();
					new_item->setText(QString::number(project->get_A()->at(r).at(c)));  // 
					my_table->setItem(r, c, new_item);
				}
			}
		}
	}
}

void Flexit::showMyOutputTable()
{
	// Populate the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showOutputDataTable");

		// Compute the number of columns and display column headers.============================================================================
		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.myOutputTableWidget;
		my_table->setRowCount(FrameNumber);

		int columnCount = 2 + 4 * noOfSurfaces; // The column count for the FrameNumber, CL, CD, Lift and Drag for each surface.

		// Actually set the column count of the table.
		my_table->setColumnCount(columnCount);

		// Now display the column headers.
		my_table->setHorizontalHeaderItem(0, new QTableWidgetItem("FrameNumber"));
		my_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Time (secs.)"));

		int columnIndex = 2;
		for (int k = 0; k < noOfSurfaces; k++)
		{
			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Sur. %1 CL (K&P)").arg(k)));
			my_table->setColumnWidth(columnIndex, 100);
			columnIndex++;

			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Sur. %1 CD (K&P)").arg(k)));
			my_table->setColumnWidth(columnIndex, 100);
			columnIndex++;

			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Sur. %1 Lift").arg(k)));
			my_table->setColumnWidth(columnIndex, 100);
			columnIndex++;

			my_table->setHorizontalHeaderItem(columnIndex, new QTableWidgetItem(QString("Sur. %1 Drag").arg(k)));
			my_table->setColumnWidth(columnIndex, 100);
			columnIndex++;
		}

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showOutputDataTable 2");

		// Display CL, CD, Lift and Drag histories for each surface.======================================================================================
		for (int n = 0; n < project->get_MySurfaces()->at(0)->get_MyCLHistory()->size(); n++)
		{
			int columnIndex = 0;

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showOutputDataTable. n = %i", n);

			QTableWidgetItem* new_itemF = new QTableWidgetItem();
			new_itemF->setText(QString::number(n));  // 
			my_table->setItem(n, columnIndex, new_itemF);
			columnIndex++;

			float timeSecs = (float)n / (float)project->get_FramesPerSecond();
			QTableWidgetItem* new_itemT = new QTableWidgetItem();
			new_itemT->setText(QString::number(timeSecs));  // 
			my_table->setItem(n, columnIndex, new_itemT);
			columnIndex++;

			for (int k = 0; k < noOfSurfaces; k++)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "showOutputDataTable 3");

				// Coefficient lift and drag.
				float CL = 0.0;
				float CD = 0.0;
				if (n > 1)
				{
					CL = project->get_MySurfaces()->at(k)->get_MyCLHistory()->at(n);
					CD = project->get_MySurfaces()->at(k)->get_MyCDHistory()->at(n);
				}
				else
				{
					CL = 0.0;
					CD = 0.0;
				}

				// CL k and p
				QTableWidgetItem* new_itemCLkp = new QTableWidgetItem();
				new_itemCLkp->setText(QString::number(CL));  // 
				my_table->setItem(n, columnIndex, new_itemCLkp);
				// new_itemCLkp->setBackground(Qt::magenta);
				columnIndex++;

				// CD k and p
				QTableWidgetItem* new_itemCDkp = new QTableWidgetItem();
				new_itemCDkp->setText(QString::number(CD));  // 
				my_table->setItem(n, columnIndex, new_itemCDkp);
				columnIndex++;

				float Lift = project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_Z();

				// Lift
				QTableWidgetItem* new_itemLift = new QTableWidgetItem();
				new_itemLift->setText(QString::number(Lift));  // 
				my_table->setItem(n, columnIndex, new_itemLift);
				columnIndex++;

				float Drag = project->get_MySurfaces()->at(k)->get_MyForceHistory()->at(n)->get_X();

				// Drag
				QTableWidgetItem* new_itemDrag = new QTableWidgetItem();
				new_itemDrag->setText(QString::number(Drag));  // 
				my_table->setItem(n, columnIndex, new_itemDrag);
				columnIndex++;
			}
		}
	}
}