#include "global.h"
#include "designit.h"
#include "ITIO.h"
#include "ITProject.h"
#include "ITSurface.h"
#include "ITControlPoint.h"
#include "ITPhysics.h"
#include "MyAboutDialogBox.h"
#include "ITTrajectoryCurve.h"
#include "ITTrajectoryCurveSegment.h"
#include "ITPointTrajectory.h"

#include <QMessageBox> // QMessageBox
#include <QFileDialog> // QFileDialog for open data file.
#include <QTextStream> // To display the contents of a file in the GUI.
#include <QCloseEvent> // Used in closeEvent.
#include <QClipboard>
#include <intrin.h> // To use __cpuid to get the CPU processor ID. Used in actionSend_HTTP_Log_Message_triggered().
#include <QInputDialog>

#include "qjsonmodel.h" // To display the JSON data in the QTreeView tab.
#include "qjsonitem.h"

Designit::Designit(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);

	QIcon icon = QIcon("Resources/blackDot.png");

	// Default display OpenGL axes enabled.
	//ui.actionAxes->setIcon( icon );
	ui.actionControl_points->setIcon(icon);
	ui.actionInterpolated_points->setIcon(icon);
	ui.actionNormals->setIcon(icon);
	ui.actionAnnotations->setIcon(icon);
	ui.actionGrids->setIcon(icon);

	// Set up the status table view.
	QTableWidget* my_statusTable = ui.myStatusTableWidget;
	my_statusTable->setColumnCount(2);

	my_statusTable->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Key")));
	my_statusTable->setColumnWidth(0, 40);

	my_statusTable->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("Value")));
	my_statusTable->setColumnWidth(1, 130);

	my_statusTable->verticalHeader()->setVisible(false);

	// Send HTTP log message.
	sendHTTPRequest(QString("Program"), QString("Launched"), 0.0, 0, DataFileNameWithPath);

	// Connect user text edit to enter method.
	connect(ui.myEditTextDataField, &QLineEdit::returnPressed, this, &Designit::userHasEnteredTextData);

	// Connect "finished item editing" signal of spreadsheet to "save data" slot
	connect(ui.mySpreadsheet->itemDelegate(), &QAbstractItemDelegate::commitData, this, &Designit::updateDataFromSpreadsheet);
	connect(ui.trajectorySpreadsheet->itemDelegate(), &QAbstractItemDelegate::commitData, this, &Designit::updateTrajectoryFromSpreadsheet);

	// Connect enter key pressed signal to executing command line command;
	connect(ui.commandLine, &QLineEdit::returnPressed, this, &Designit::handleCommand);
	connect(ui.commandLine, &QLineEdit::textEdited, this, &Designit::resetCommandMemory);

	// Toolbox tab chenge signal
	connect(ui.editingTools, &QToolBox::currentChanged, this, &Designit::toolBoxTabChanged);

	// Set up the curve indices for the plot window objects.
	ui.trajectoryCurveX->set_MyCurveIndex(0);
	ui.trajectoryCurveX->set_MyChar('X');

	ui.trajectoryCurveY->set_MyCurveIndex(1);
	ui.trajectoryCurveY->set_MyChar('Y');

	ui.trajectoryCurveZ->set_MyCurveIndex(2);
	ui.trajectoryCurveZ->set_MyChar('Z');

	ui.trajectoryCurveR->set_MyCurveIndex(3);
	ui.trajectoryCurveR->set_MyChar('R');

	ui.trajectoryCurveP->set_MyCurveIndex(4);
	ui.trajectoryCurveP->set_MyChar('P');

	ui.trajectoryCurveB->set_MyCurveIndex(5);
	ui.trajectoryCurveB->set_MyChar('B');

	//populate command list
	functions.emplace("testFunction", &Designit::testFunction);
	functions.emplace("movePoint", &Designit::movePoint);
	functions.emplace("setPoint", &Designit::setPoint);
	functions.emplace("rotateSurface", &Designit::rotateSurface);
	functions.emplace("rotateSurfaceCentral", &Designit::rotateSurfaceCentral);
	functions.emplace("addSurface", &Designit::addSurface);
	functions.emplace("rotatePoint", &Designit::rotatePoint);
	functions.emplace("setColumn", &Designit::setColumn);
	functions.emplace("setRow", &Designit::setRow);
	functions.emplace("setSurface", &Designit::setSurface);
	functions.emplace("moveColumn", &Designit::moveColumn);
	functions.emplace("moveRow", &Designit::moveRow);
	functions.emplace("moveSurface", &Designit::moveSurface);
	functions.emplace("rotateColumn", &Designit::rotateColumn);
	functions.emplace("rotateRow", &Designit::rotateRow);
	functions.emplace("resizeSurface", &Designit::resizeSurface);

	ui.commandLine->installEventFilter(this);
}

void Designit::on_action_Flexit_triggered() {
	QString fileName = "autosave.json";

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
	sendHTTPRequest(QString("File"), QString("Saved"), 0.0, 0, DataFileNameWithPath);

	system("Flexit.exe autosave.json");
}

void Designit::on_actionOpen_triggered()
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

void Designit::loadData(QString fileNameWithPath) // Gets called from on_actionOpen_triggered and on_jsonUpdateButton_clicked.
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

		QString text = "DesignIT: " + QString(base_filename.c_str());

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "text: %s", text.toStdString().c_str());

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
	showTrajectorySpreadsheet();

	// Set the flag.
	IsDataLoaded = true;

	// Display a satus message.
	appendStatusTableWidget(QString("File"), QString("Loaded"));

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Loaded"), 0.0, 0, DataFileNameWithPath);

	// Finally force process events.
	QApplication::processEvents();
}

// TODO: write ctrl+c, ctrl+v for trajectory spreadsheet
void Designit::keyPressEvent(QKeyEvent *event)
{
	// Over-ride parent class method.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "keyPressEvent");

	// Get the focus widget.
	QWidget *w = QApplication::focusWidget();

	if (w == ui.mySpreadsheet)
	{
		// We are on the spreadsheet widget.
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We are focused on the spreadsheet");

		if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
		{
			// CTRL-C was pressed, so copy data to clipboard.
			// Model of my first QTableWidget
			QItemSelectionModel *myModel = ui.mySpreadsheet->selectionModel();
			QModelIndexList list = myModel->selectedIndexes();

			if (list.size() > 0)
			{
				QString selectionString;

				// Something was selected, so perform copy.
				int currentRow = 0; // To determine when to insert newlines

				for (int t = 0; t < list.size(); t++)
				{
					QModelIndex item = list.at(t);

					QString contentsOfCurrentItem = item.data().toString();

					if (selectionString.length() == 0)
					{
						// First item
					}
					else if (item.row() != currentRow)
					{
						// New row
						selectionString += '\n';
					}
					else
					{
						// Next cell
						selectionString += '\t';
					}

					currentRow = item.row();
					selectionString += item.data().toString();

					QByteArray ba1;
					ba1 = contentsOfCurrentItem.toLatin1();
					const char *str1 = ba1.data();

					project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Row: %i, col: %i, contents: %s", item.row(), item.column(), str1);

				}

				QClipboard *clipboard = QApplication::clipboard();
				clipboard->setText(selectionString);
			}
		}
		else if (event->key() == Qt::Key_V && (event->modifiers() & Qt::ControlModifier))
		{
			// CTRL-V was pressed, so paste data from clipboard.
			QClipboard *clipboard = QApplication::clipboard();

			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Clipboard contents: %s", clipboard->text().toStdString().c_str());

			// Model of my first QTableWidget
			QItemSelectionModel *myModel = ui.mySpreadsheet->selectionModel();

			QModelIndexList list = myModel->selectedIndexes();

			if (list.size() == 1)
			{
				project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "List item found. Size: %i", list.size());

				QModelIndex item = list.at(0);

				int col = item.column();
				int row = item.row();

				QString contents = clipboard->text();
				QStringList contentsRowList = contents.split("\n");

				for (int i = 0; i < contentsRowList.size(); i++)
				{
					QStringList contentsRowColList = contentsRowList.at(i).split(QRegExp("\\s"));

					for (int j = 0; j < contentsRowColList.size(); j++)
					{
						QTableWidgetItem* new_item = new QTableWidgetItem();
						new_item->setText(QString(contentsRowColList.at(j).toStdString().c_str()));  // 
						ui.mySpreadsheet->setItem(row + i, col + j, new_item);
					}
				}
			}
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

void Designit::on_actionInterpolated_points_density_U_triggered()
{
	QStringList items;
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		items << QString::number(k);
	}

	bool ok;
	QString item = QInputDialog::getItem(this, tr("Interpolated points U direction resolution"),
		tr("Surface:"), items, 0, false, &ok);

	int kk = item.toInt();
	int nU = QInputDialog::getInt(this, "Interpolated points U direction resolution", "Number of U points:", project->get_MySurfaces()->at(kk)->get_NoOfInterpolatedPointsU(), 0, 100, 1, &ok);

	project->get_MySurfaces()->at(kk)->set_NoOfInterpolatedPointsU(nU);
	project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();

	on_actionGaussian_curvature_triggered();

	w->updateAllTabs();
}

void Designit::on_actionInterpolated_points_density_V_triggered()
{
	QStringList items;
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		items << QString::number(k);
	}

	bool ok;
	QString item = QInputDialog::getItem(this, tr("Interpolated points V direction resolution"),
		tr("Surface:"), items, 0, false, &ok);

	int kk = item.toInt();
	int nV = QInputDialog::getInt(this, "Interpolated points V direction resolution", "Number of V points:", project->get_MySurfaces()->at(kk)->get_NoOfInterpolatedPointsV(), 0, 100, 1, &ok);

	project->get_MySurfaces()->at(kk)->set_NoOfInterpolatedPointsV(nV);
	project->get_MySurfaces()->at(kk)->manageComputationOfInterpolatedPoints();

	on_actionGaussian_curvature_triggered();

	w->updateAllTabs();
}

void Designit::updateAllTabs()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 12, "Inside updateAllTabs");

	// Update the main geometry OpenGL view.
	ui.myGLWidget->repaint(); // update seems to only take effect when the used pans the opengl widget.

	// Update the output graphs.
	ui.myXYView->update();
	ui.myXZView->update();
	ui.myYZView->update();

	// Update Gaussian view.
	ui.myGaussianView->updateGL();
}

bool Designit::checkExitWithUnsavedData()
{
	if (UnsavedChanges)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Unsaved Data", "You have unsaved data.\nAre you sure you want to quit?", QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			closeProject();

			// Send the HTTP request.
			sendHTTPRequest(QString("Program"), QString("Terminated"), 0.0, 0, DataFileNameWithPath);

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
		sendHTTPRequest(QString("Program"), QString("Terminated"), 0.0, 0, DataFileNameWithPath);

		return true;
	}
}

void Designit::closeProject()
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

	// Set the main window title.
	QString text = "Designit: ";
	setWindowTitle(text);

	statusBar()->showMessage(QObject::tr("File closed successfully"));

	// Display a status message.
	appendStatusTableWidget(QString("File"), QString("Closed"));

	// Send the HTTP request.
	sendHTTPRequest(QString("File"), QString("Closed"), 0.0, 0, DataFileNameWithPath);

}

void Designit::on_actionExit_triggered()
{
	// Check for unsaved data and exit if everything is saved.
	if (checkExitWithUnsavedData())
	{
		qApp->quit();
	}

	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionExit_triggered. Just returned from checkExitWithUnsavedData.");
}

void Designit::on_actionWeb_help_triggered()
{
	system("start /max http://www.scrapeworld.com/surfit/index.shtml");
}

void Designit::on_actionAbout_triggered()
{
	MyAboutDialogBox *myAboutDialogBox = new MyAboutDialogBox();

	myAboutDialogBox->show();
}

void Designit::on_actionReset_all_views_triggered()
{
	// OpenGL view parameters.
	glXYViewHalfExtent = 50.0;
	glXYPanCentreX = 0.0;
	glXYPanCentreY = 0.0;

	glXZViewHalfExtent = 50.0;
	glXZPanCentreX = 0.0;
	glXZPanCentreY = 0.0;

	glYZViewHalfExtent = 50.0;
	glYZPanCentreX = 0.0;
	glYZPanCentreY = 0.0;

	gl3DViewHalfExtent = 50.0;
	gl3DPanCentreX = 0.0;
	gl3DPanCentreY = 0.0;


	// Drawing semaphores.
	drawRotateXHorizontal = false;
	drawRotateYHorizontal = false;
	drawRotateZVertical = true;

	// Drawing semaphores.
	drawAxes = true;
	drawControlPoints = true;
	drawInterpolatedPoints = true;
	drawAnnotations = true;
	drawNormals = true;

	QIcon icon = QIcon("Resources/blackDot.png");

	// Default display OpenGL axes enabled.
	ui.actionControl_points->setIcon(icon);
	ui.actionInterpolated_points->setIcon(icon);
	ui.actionNormals->setIcon(icon);
	ui.actionAnnotations->setIcon(icon);
	ui.actionGrids->setIcon(icon);

	updateAllTabs();
}

void Designit::on_actionClose_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionClose_triggered.");

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

	// Make sure the OpenGL windows are cleared.
	updateAllTabs();

}

// Detect user clicking the 'x' close window button.
void Designit::closeEvent(QCloseEvent *bar)
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

void Designit::on_collapseAllButton_clicked()
{
	ui.jsonTreeView->collapseAll();
}

void Designit::on_expandAllButton_clicked()
{
	ui.jsonTreeView->expandAll();
}

void Designit::on_actionSave_As_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Save As");

	if (!UnsavedChanges)
	{
		QMessageBox::information(0, tr("Save data"), tr("There is no new data to save."));
		return;
	}

	QString d = QDir::currentPath();
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionSave_As_triggered. Current path: %s", d.toStdString().c_str());

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), d.append("/Data"), tr("JSON Files (*.json *.JSON);;Text Files (*.txt);;C++ Files (*.cpp *.h)"));

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
	sendHTTPRequest(QString("File"), QString("Saved"), 0.0, 0, DataFileNameWithPath);

	// Finally set flags.
	UnsavedChanges = false;
}

void Designit::on_actionDelete_surface_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionDelete_surface_triggered.");

	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != DELETE_SURFACE)
	{
		// Update the ENUM
		MY_EDIT_MODE = DELETE_SURFACE;

		/// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_delete_surface_highlight.png");
		this->ui.actionDelete_surface->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_delete_surface.png");
		this->ui.actionDelete_surface->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionNew_surface_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionNew_surface_triggered.");

	ITSurface *s = new ITSurface();
	project->get_MySurfaces()->push_back(s);

	s->set_NoOfInterpolatedPointsU(20);
	s->set_NoOfInterpolatedPointsV(20);

	ITSurface *sb = new ITSurface();
	project->get_MyBaseSurfaces()->push_back(sb);

	sb->set_NoOfInterpolatedPointsU(20);
	sb->set_NoOfInterpolatedPointsV(20);

	int k = project->get_MySurfaces()->size() - 1;

	// Create control points for asymmetric NACA LPQXX aerofoil with 1m chord.
	// Ref: https://en.wikipedia.org/wiki/NACA_airfoil
	// Ref: http://www.pdas.com/naca456mean3.html

	float p = 0.20;
	float m = 0.29;
	float k1 = 6.643;
	float c = 1.0;

	for (int i = 0; i < 6; i++) // Step along span (5 metre span).
	{
		std::vector <ITControlPoint *> v_dummy;
		std::vector <ITControlPoint *> v_base_dummy;

		for (int j = 0; j < 7; j++) // j steps along chord.
		{
			float x = (float)j / 6.0; // Distance along chord.

			// The camber line offset (z coordinate).
			float yc = 0.0;
			if ((0.0 <= x) && (x <= p))
			{
				yc = (k1 / 6.0) * (x*x*x - 3 * m*x*x + m*m*(3.0 - m)*x);
			}
			else
			{
				yc = (k1 / 6.0)*m*m*m*(1 - x);
			}

			// Span is parallel to y-axis.
			ITControlPoint *p = new ITControlPoint(-x, (float)i, yc);

			p->set_K(k);
			p->set_I(i);
			p->set_J(j);

			v_dummy.push_back(p);

			ITControlPoint *pb = new ITControlPoint(-x, (float)i, yc);

			pb->set_K(k);
			pb->set_I(i);
			pb->set_J(j);

			v_base_dummy.push_back(pb);

		}

		project->get_MySurfaces()->at(k)->get_MyControlPoints()->push_back(v_dummy);
		project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->push_back(v_base_dummy);

	}

	IsDataLoaded = true;
	UnsavedChanges = true;

	// Show status log entry.
	appendStatusTableWidget(QString("Design"), QString("New surface"));

	project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();

	project->createNewTrajectoryCurve(k);

	updateSpreadsheet();
	updateTrajectorySpreadsheet();
	updateAllTabs();
}

void Designit::on_actionCopy_surface_triggered()
{
	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != COPY_SURFACE)
	{
		// Update the ENUM
		MY_EDIT_MODE = COPY_SURFACE;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_copy_highlight.png");
		this->ui.actionCopy_surface->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_copy.png");
		this->ui.actionCopy_surface->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionDrag_triggered()
{
	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != DRAG)
	{
		// Update the ENUM
		MY_EDIT_MODE = DRAG;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_drag_highlight.png");
		this->ui.actionDrag->setIcon(icon);

		IsHorizontalDragOnly = false;
		IsVerticalDragOnly = false;
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_drag.png");
		this->ui.actionDrag->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionDrag_row_triggered()
{
	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != DRAG_ROW)
	{
		// Update the ENUM
		MY_EDIT_MODE = DRAG_ROW;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_drag_row_highlight.png");
		this->ui.actionDrag_row->setIcon(icon);

		IsHorizontalDragOnly = false;
		IsVerticalDragOnly = false;
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_drag_row.png");
		this->ui.actionDrag_row->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionDrag_col_triggered()
{
	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != DRAG_COL)
	{
		// Update the ENUM
		MY_EDIT_MODE = DRAG_COL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_drag_col_highlight.png");
		this->ui.actionDrag_col->setIcon(icon);

		IsHorizontalDragOnly = false;
		IsVerticalDragOnly = false;
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_drag_col.png");
		this->ui.actionDrag_col->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionDrag_all_triggered()
{
	// Set all the buttons to "normal" icons.
	resetModeButtons();

	if (MY_EDIT_MODE != DRAG_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = DRAG_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_drag_all_highlight.png");
		this->ui.actionDrag_all->setIcon(icon);

		IsHorizontalDragOnly = false;
		IsVerticalDragOnly = false;
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_drag_all.png");
		this->ui.actionDrag_all->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::resetModeButtons()
{
	// Set all the buttons to "normal" icons.
	QIcon icon1 = QIcon("Resources/icon_drag.png");
	this->ui.actionDrag->setIcon(icon1);

	QIcon icon2 = QIcon("Resources/icon_drag_row.png");
	this->ui.actionDrag_row->setIcon(icon2);

	QIcon icon3 = QIcon("Resources/icon_drag_col.png");
	this->ui.actionDrag_col->setIcon(icon3);

	QIcon icon4 = QIcon("Resources/icon_drag_all.png");
	this->ui.actionDrag_all->setIcon(icon4);

	QIcon icon5 = QIcon("Resources/icon_rotate_all.png");
	this->ui.actionRotate_all->setIcon(icon5);

	QIcon icon6 = QIcon("Resources/icon_resize_all.png");
	this->ui.actionResize_all->setIcon(icon6);

	QIcon icon7 = QIcon("Resources/icon_shear.png");
	this->ui.actionShear->setIcon(icon7);

	QIcon icon8 = QIcon("Resources/icon_perspective_all.png");
	this->ui.actionPerspective->setIcon(icon8);

	QIcon icon9 = QIcon("Resources/icon_flip_horizontal.png");
	this->ui.actionFlip_horizontal->setIcon(icon9);

	QIcon icon10 = QIcon("Resources/icon_copy.png");
	this->ui.actionCopy_surface->setIcon(icon10);

	QIcon icon11 = QIcon("Resources/icon_delete_surface.png");
	this->ui.actionDelete_surface->setIcon(icon11);

	QIcon icon12 = QIcon("Resources/icon_insert_row.png");
	this->ui.actionInsert_row->setIcon(icon12);

	QIcon icon13 = QIcon("Resources/icon_delete_row.png");
	this->ui.actionDelete_row->setIcon(icon13);

	QIcon icon14 = QIcon("Resources/icon_duplicate_row.png");
	this->ui.actionDuplicate_row->setIcon(icon14);

	QIcon icon15 = QIcon("Resources/icon_insert_column.png");
	this->ui.actionInsert_col->setIcon(icon15);

	QIcon icon16 = QIcon("Resources/icon_delete_column.png");
	this->ui.actionDelete_col->setIcon(icon16);

	QIcon icon17 = QIcon("Resources/icon_duplicate_column.png");
	this->ui.actionDuplicate_col->setIcon(icon17);

	QIcon icon18 = QIcon("Resources/icon_mate_points.png");
	this->ui.actionMate_points->setIcon(icon18);

	QIcon icon19 = QIcon("Resources/icon_copy_mirror.png");
	this->ui.actionCopy_surface_mirror->setIcon(icon19);

	QIcon icon20 = QIcon("Resources/icon_merge_surfaces.png");
	this->ui.actionMerge_surfaces_by_row->setIcon(icon20);

	QIcon icon21 = QIcon("Resources/icon_merge_surfaces_reverse.png");
	this->ui.actionMerge_surfaces_by_row_reverse->setIcon(icon21);

	QIcon icon22 = QIcon("Resources/icon_measure_distance.png");
	this->ui.actionMeasure_distance->setIcon(icon22);

	QIcon icon23 = QIcon("Resources/icon_centred_rotate.png");
	this->ui.actionCentred_rotate->setIcon(icon23);
}

void Designit::on_actionCopy_surface_mirror_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != COPY_SURFACE_MIRROR)
	{
		// Update the ENUM
		MY_EDIT_MODE = COPY_SURFACE_MIRROR;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_copy_mirror_highlight.png");
		this->ui.actionCopy_surface_mirror->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_copy_mirror.png");
		this->ui.actionCopy_surface_mirror->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Delete scratch point.
		delete ui.myXYView->get_ScratchControlPoint();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionRotate_all_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != ROTATE_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = ROTATE_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_rotate_all_highlight.png");
		this->ui.actionRotate_all->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_rotate_all.png");
		this->ui.actionRotate_all->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionInsert_row_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != INSERT_ROW)
	{
		// Update the ENUM
		MY_EDIT_MODE = INSERT_ROW;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_insert_row_highlight.png");
		this->ui.actionInsert_row->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_insert_row.png");
		this->ui.actionInsert_row->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionMerge_surfaces_by_row_reverse_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != MERGE_SURFACES_BY_ROW_REVERSE)
	{
		// Update the ENUM
		MY_EDIT_MODE = MERGE_SURFACES_BY_ROW_REVERSE;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_merge_surfaces_reverse_highlight.png");
		this->ui.actionMerge_surfaces_by_row_reverse->setIcon(icon);

		ui.myXYView->set_PrimedForFirstClick(true);
		ui.myXYView->set_PrimedForSecondClick(false);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_merge_surfaces_reverse.png");
		this->ui.actionMerge_surfaces_by_row_reverse->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionMerge_surfaces_by_row_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != MERGE_SURFACES_BY_ROW)
	{
		// Update the ENUM
		MY_EDIT_MODE = MERGE_SURFACES_BY_ROW;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_merge_surfaces_highlight.png");
		this->ui.actionMerge_surfaces_by_row->setIcon(icon);

		ui.myXYView->set_PrimedForFirstClick(true);
		ui.myXYView->set_PrimedForSecondClick(false);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_merge_surfaces.png");
		this->ui.actionMerge_surfaces_by_row->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionMeasure_distance_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionMeasure_distance_triggered");

	resetModeButtons();

	if (MY_EDIT_MODE != MEASURE_DISTANCE)
	{
		// Update the ENUM
		MY_EDIT_MODE = MEASURE_DISTANCE;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_measure_distance_highlight.png");
		this->ui.actionMeasure_distance->setIcon(icon);

		ui.myXYView->set_PrimedForFirstClick(true);
		ui.myXYView->set_PrimedForSecondClick(false);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_measure_distance.png");
		this->ui.actionMeasure_distance->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionGaussian_curvature_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionGaussian_curvature_triggered");

	// Empty pre-existing data.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		if (!project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->empty())
		{
			project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->clear();
		}

	}

	// Now do the calculations.
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		// Create a convenient pointer.
		ITSurface *s = project->get_MySurfaces()->at(k);

		// Construct matrix of control point coordinate values.
		int rows = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();
		int n = rows - 1;

		int cols = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
		int m = cols - 1;

		for (int i = 0; i < project->get_MySurfaces()->at(k)->get_URange()->size(); i++)
		{
			float seedU = project->get_MySurfaces()->at(k)->get_URange()->at(i);

			// Create a dummy vector to hold Gaussian Curvatures.
			std::vector <float> *dummy_K = new std::vector <float>;

			for (int j = 0; j < project->get_MySurfaces()->at(k)->get_VRange()->size(); j++)
			{
				// Allocate the memory for each of the matrices of coordinates of control points.
				Eigen::MatrixXd PX = Eigen::MatrixXd::Constant(rows, cols, 0.0);
				Eigen::MatrixXd PY = Eigen::MatrixXd::Constant(rows, cols, 0.0);
				Eigen::MatrixXd PZ = Eigen::MatrixXd::Constant(rows, cols, 0.0);

				// Fill the control point matrix elements with values.
				for (int i = 0; i < rows; i++)
				{
					for (int j = 0; j < cols; j++)
					{
						PX(i, j) = s->get_MyControlPoints()->at(i).at(j)->get_X();
						PY(i, j) = s->get_MyControlPoints()->at(i).at(j)->get_Y();
						PZ(i, j) = s->get_MyControlPoints()->at(i).at(j)->get_Z();
					}
				}

				float seedV = project->get_MySurfaces()->at(k)->get_VRange()->at(j);

				// Initialize an Eigen vector of that will contain powers of U.
				Eigen::VectorXd up_vector = Eigen::VectorXd::Constant(rows, 1.0); // The vector of powers of the U parameter seedU.				

				for (int i = 0; i < rows; i++)
				{
					up_vector(i) = pow(seedU, i);
				}

				up_vector.reverseInPlace(); // Reverse the order of the elements so u^(r-1) is first and u^0 is last.
//				std::cout << "up_vector =\n" << up_vector << std::endl; // This agrees with legacy matlab code.				

				// Initialize an Eigen vector of that will contain powers of V.
				Eigen::VectorXd vp_vector = Eigen::VectorXd::Constant(cols, 1.0); // The vector of powers of the V parameter seedV.				

				for (int i = 0; i < cols; i++) // Columns
				{
					vp_vector(i) = pow(seedV, i);
				}

				vp_vector.reverseInPlace(); // Reverse the order of the elements so v^(r-1) is first and v^0 is last.

				// Now find the first derivatives of the elements of the U and V power vectors.
				Eigen::VectorXd coeffu = Eigen::VectorXd::LinSpaced(rows, 0, n);
				coeffu.reverseInPlace();
				Eigen::VectorXd coeffv = Eigen::VectorXd::LinSpaced(cols, 0, m);
				coeffv.reverseInPlace();

				Eigen::VectorXd Uu = coeffu.cwiseProduct(up_vector) / seedU; // Uu = (3u^2, 2u, 1, 0).
				Eigen::VectorXd Vv = coeffv.cwiseProduct(vp_vector) / seedV; // Vv = (3v^2, 3v, 1, 0).				

				// Now find the second derivative of the elements of the U and V power vectors
				Eigen::VectorXd coeffuu = Eigen::VectorXd::Zero(rows);
				Eigen::VectorXd coeffvv = Eigen::VectorXd::Zero(cols);

				for (int i = 2; i < rows; i++)
				{
					coeffuu(i) = i - 1;
				}
				coeffuu.reverseInPlace(); // The large coefficients are at the beginning of the vector.

				for (int i = 2; i < cols; i++)
				{
					coeffvv(i) = i - 1;
				}
				coeffvv.reverseInPlace();

				Eigen::VectorXd Uuu = coeffuu.cwiseProduct(Uu) / seedU;
				Eigen::VectorXd Vvv = coeffvv.cwiseProduct(Vv) / seedV;

				// Compute tangent plane basis (e1 and e2)
				Eigen::VectorXd dxdu = Uu.transpose()*s->get_MU()*PX*s->get_MV()*vp_vector;
				Eigen::VectorXd dydu = Uu.transpose()*s->get_MU()*PY*s->get_MV()*vp_vector;
				Eigen::VectorXd dzdu = Uu.transpose()*s->get_MU()*PZ*s->get_MV()*vp_vector;

				Eigen::VectorXd dxdv = up_vector.transpose()*s->get_MU()*PX*s->get_MV()*Vv;
				Eigen::VectorXd dydv = up_vector.transpose()*s->get_MU()*PY*s->get_MV()*Vv;
				Eigen::VectorXd dzdv = up_vector.transpose()*s->get_MU()*PZ*s->get_MV()*Vv;

				// Convert the first derivatives to scalars, instanciate two ITPoints and take the cross product.
				ITPoint *sigma_u = new ITPoint((float)dxdu(0), (float)dydu(0), (float)dzdu(0));
				ITPoint *sigma_v = new ITPoint((float)dxdv(0), (float)dydv(0), (float)dzdv(0));

				ITPoint *surfaceNormal = sigma_u->cross(sigma_v);

				surfaceNormal->normalize();

				// Compute second derivatives
				Eigen::VectorXd d2xdu2 = Uuu.transpose()*s->get_MU()*PX*s->get_MV()*vp_vector;
				Eigen::VectorXd d2ydu2 = Uuu.transpose()*s->get_MU()*PY*s->get_MV()*vp_vector;
				Eigen::VectorXd d2zdu2 = Uuu.transpose()*s->get_MU()*PZ*s->get_MV()*vp_vector;
				Eigen::VectorXd d2xdudv = Uu.transpose()*s->get_MU()*PX*s->get_MV()*Vv;
				Eigen::VectorXd d2ydudv = Uu.transpose()*s->get_MU()*PY*s->get_MV()*Vv;
				Eigen::VectorXd d2zdudv = Uu.transpose()*s->get_MU()*PZ*s->get_MV()*Vv;

				Eigen::VectorXd d2xdv2 = up_vector.transpose()*s->get_MU()*PX*s->get_MV()*Vvv;
				Eigen::VectorXd d2ydv2 = up_vector.transpose()*s->get_MU()*PY*s->get_MV()*Vvv;
				Eigen::VectorXd d2zdv2 = up_vector.transpose()*s->get_MU()*PZ*s->get_MV()*Vvv;

				ITPoint *sigma_uu = new ITPoint((float)d2xdu2(0), (float)d2ydu2(0), (float)d2zdu2(0));
				ITPoint *sigma_uv = new ITPoint((float)d2xdudv(0), (float)d2ydudv(0), (float)d2zdudv(0));
				ITPoint *sigma_vv = new ITPoint((float)d2xdv2(0), (float)d2ydv2(0), (float)d2zdv2(0));

				//Compute EFG and LMN
				float E = sigma_u->dot(sigma_u);
				float F = sigma_u->dot(sigma_v);
				float GG = sigma_v->dot(sigma_v);

				float L = sigma_uu->dot(surfaceNormal);
				float M = sigma_uv->dot(surfaceNormal);
				float N = sigma_vv->dot(surfaceNormal);

				// Determine the Gaussian Curvature K
				float K = (L*N - M*M) / (E*GG - F*F);
				dummy_K->push_back(K);

				// Manage memory.
				delete sigma_u;
				delete sigma_v;
				delete surfaceNormal;
				delete sigma_uu;
				delete sigma_uv;
				delete sigma_vv;

			}
			project->get_MySurfaces()->at(k)->get_MyGaussianCurvature()->push_back(*dummy_K);
		}
	}
	ui.myGaussianView->updateGL();
}

void Designit::on_actionMate_points_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != MATE_POINTS)
	{
		// Update the ENUM
		MY_EDIT_MODE = MATE_POINTS;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_mate_points_highlight.png");
		this->ui.actionMate_points->setIcon(icon);

		ui.myXYView->set_PrimedForFirstClick(true);
		ui.myXYView->set_PrimedForSecondClick(false);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_mate_points.png");
		this->ui.actionMate_points->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

} // End of void on_actionMate_points_triggered().

void Designit::on_actionInsert_col_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != INSERT_COL)
	{
		// Update the ENUM
		MY_EDIT_MODE = INSERT_COL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_insert_column_highlight.png");
		this->ui.actionInsert_col->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_insert_column.png");
		this->ui.actionInsert_col->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionDelete_row_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != DELETE_ROW)
	{
		// Update the ENUM
		MY_EDIT_MODE = DELETE_ROW;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_delete_row_highlight.png");
		this->ui.actionDelete_row->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_delete_row.png");
		this->ui.actionDelete_row->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionDelete_col_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != DELETE_COL)
	{
		// Update the ENUM
		MY_EDIT_MODE = DELETE_COL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_delete_column_highlight.png");
		this->ui.actionDelete_col->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_delete_column.png");
		this->ui.actionDelete_col->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionDuplicate_row_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != DUPLICATE_ROW)
	{
		// Update the ENUM
		MY_EDIT_MODE = DUPLICATE_ROW;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_duplicate_row_highlight.png");
		this->ui.actionDuplicate_row->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_duplicate_row.png");
		this->ui.actionDuplicate_row->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionDuplicate_col_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != DUPLICATE_COL)
	{
		// Update the ENUM
		MY_EDIT_MODE = DUPLICATE_COL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_duplicate_column_highlight.png");
		this->ui.actionDuplicate_col->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_duplicate_column.png");
		this->ui.actionDuplicate_col->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres.
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::emptyFocusVectors()
{
	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int N = project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->size();
		for (int n = 0; n < N; n++)
		{
			project->get_MySurfaces()->at(k)->get_MyFocusControlPoints()->pop_back();
		}
	}
}

void Designit::on_actionResize_all_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != RESIZE_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = RESIZE_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_resize_all_highlight.png");
		this->ui.actionResize_all->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_Resize_all.png");
		this->ui.actionResize_all->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionShear_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != SHEAR_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = SHEAR_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_shear_highlight.png");
		this->ui.actionShear->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_shear.png");
		this->ui.actionShear->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionPerspective_triggered()
{
	resetModeButtons();

	if (MY_EDIT_MODE != PERSPECTIVE_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = PERSPECTIVE_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_perspective_all_highlight.png");
		this->ui.actionPerspective->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_perspective_all.png");
		this->ui.actionPerspective->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionFlip_horizontal_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionFlip_horizontal_triggered");

	resetModeButtons();

	if (MY_EDIT_MODE != FLIP_HORIZONTAL_ALL)
	{
		// Update the ENUM
		MY_EDIT_MODE = FLIP_HORIZONTAL_ALL;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_flip_horizontal_highlight.png");
		this->ui.actionFlip_horizontal->setIcon(icon);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_flip_horizontal.png");
		this->ui.actionFlip_horizontal->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();
}

void Designit::on_actionCentred_rotate_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_actionCentred_rotate_triggered");

	resetModeButtons();

	if (MY_EDIT_MODE != CENTRED_ROTATE)
	{
		// Update the ENUM
		MY_EDIT_MODE = CENTRED_ROTATE;

		// Highlight the icon.
		QIcon icon = QIcon("Resources/icon_centred_rotate_highlight.png");
		this->ui.actionCentred_rotate->setIcon(icon);

		ui.myXYView->set_PrimedForFirstClick(true);
		ui.myXYView->set_PrimedForSecondClick(false);
		ui.myXYView->set_SecondClicksFinished(false);

		ui.myXZView->set_PrimedForFirstClick(true);
		ui.myXZView->set_PrimedForSecondClick(false);
		ui.myXZView->set_SecondClicksFinished(false);
	}
	else
	{
		// Update the ENUM
		MY_EDIT_MODE = NONE;

		// Reset the icon.
		QIcon icon = QIcon("Resources/icon_centred_rotate.png");
		this->ui.actionCentred_rotate->setIcon(icon);

		// Empty the focus vectors.
		emptyFocusVectors();

		// Redraw everything (to get rid of any spheres).
		w->updateAllTabs();
	}

	// Finally force process events.
	QApplication::processEvents();

}

void Designit::on_actionControl_points_triggered()
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

	// Redraw other views.
	updateAllTabs();

}

void Designit::on_actionInterpolated_points_triggered()
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

	// Redraw other views.
	updateAllTabs();

}

void Designit::on_actionGrids_triggered()
{
	drawGrids = !drawGrids;

	if (drawGrids)
	{
		QIcon icon = QIcon("Resources/blackDot.png");
		ui.actionGrids->setIcon(icon);
	}
	else
	{
		ui.actionGrids->setIcon(QIcon());
	}

	// Redraw other views.
	updateAllTabs();

}

void Designit::on_actionNormals_triggered()
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

	// Redraw other views.
	updateAllTabs();
}

void Designit::on_actionAnnotations_triggered()
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

	// Redraw other views.
	updateAllTabs();
}

void Designit::setMyTextDataField(QString str)
{
	ui.myEditTextDataField->setText(str);

}

void Designit::userHasEnteredTextData()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Return");

	if (MY_EDIT_MODE == ROTATE_ALL)
	{
		float netAngleReds = ui.myEditTextDataField->text().toFloat() * PI / 180.0 - ui.myXYView->get_EditValue();
		ui.myXYView->set_EditValue(netAngleReds);
		ui.myXYView->rotateFocusPoints(netAngleReds);

		// Redraw other views.
		updateAllTabs();
	}
	else if (MY_EDIT_MODE == SHEAR_ALL)
	{
		float netShearDistance = ui.myEditTextDataField->text().toFloat();
		ui.myXYView->set_EditValue(netShearDistance);
		ui.myXYView->shearFocusPoints(netShearDistance);

		// Redraw other views.
		updateAllTabs();
	}
}

void Designit::handleCommand()
{
	QString input = ui.commandLine->text();

	commandPointer = -1;
	commandMemory.insert(commandMemory.begin(), input);

	ui.commandLine->setText("");

	QRegExp rx("(\\ |\\t+|\\s+)"); //RegEx for ' ' or '\t'
	QStringList arguments = input.split(rx);
	
	if (functions.find(arguments[0].toUtf8().constData()) != functions.end())
	{
		int returnCode = functions[arguments[0].toUtf8().constData()](arguments);

		switch (returnCode)
		{
		case 0:
			appendStatusTableWidget(QString(arguments[0].toUtf8().constData()), QString("DONE"));
			break;
		case 1:
			appendStatusTableWidget(QString(arguments[0].toUtf8().constData()), QString("Wrong number of arguments"));
			break;
		case 2:
			appendStatusTableWidget(QString(arguments[0].toUtf8().constData()), QString("Wrong arguments of function"));
			break;
		case 99:
			appendStatusTableWidget(QString(arguments[0].toUtf8().constData()), QString("Unspecified error"));
			break;
		default:
			break;
		}
	}
	else {
		w->statusBar()->showMessage(QObject::tr("Unknown command"));
	}
}

void Designit::resetCommandMemory()
{
	commandPointer = -1;
}

// TODO: import spreadsheet functions for moveIT spreadsheet
void Designit::showSpreadsheet()
{
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "showSpreadsheet");

		// Compute the number of columns and display column headers.============================================================================

		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.mySpreadsheet;

		int noOfRows = 0;
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			noOfRows = noOfRows + project->get_MySurfaces()->at(k)->get_MyControlPoints()->size() * project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
		}

		my_table->setRowCount(noOfRows);

		int columnCount = 6; // The column count for the FrameNumber, CL, CD, Lift and Drag for each surface.

		// Actually set the column count of the table.
		my_table->setColumnCount(columnCount);

		// Now display the column headers.
		my_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Sur index"));
		my_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Row index"));
		my_table->setHorizontalHeaderItem(2, new QTableWidgetItem("Col index"));
		my_table->setHorizontalHeaderItem(3, new QTableWidgetItem("x"));
		my_table->setHorizontalHeaderItem(4, new QTableWidgetItem("y"));
		my_table->setHorizontalHeaderItem(5, new QTableWidgetItem("z"));

		// Display data for each surface.======================================================================================
		int rowIndex = 0;
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			// Do the plotting.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					int columnIndex = 0;

					QTableWidgetItem* new_itemK = new QTableWidgetItem();
					new_itemK->setText(QString::number(k));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemK);
					columnIndex++;

					QTableWidgetItem* new_itemI = new QTableWidgetItem();
					new_itemI->setText(QString::number(i));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemI);
					columnIndex++;

					QTableWidgetItem* new_itemJ = new QTableWidgetItem();
					new_itemJ->setText(QString::number(j));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemJ);
					columnIndex++;

					ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

					QTableWidgetItem* new_itemX = new QTableWidgetItem();
					new_itemX->setText(QString::number(p->get_X()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemX);
					columnIndex++;

					QTableWidgetItem* new_itemY = new QTableWidgetItem();
					new_itemY->setText(QString::number(p->get_Y()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemY);
					columnIndex++;

					QTableWidgetItem* new_itemZ = new QTableWidgetItem();
					new_itemZ->setText(QString::number(p->get_Z()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemZ);
					columnIndex++;

					rowIndex++;
				}
			}
		}
	}
}

void Designit::showTrajectorySpreadsheet()
{
	// Populate the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "showTrajectorySpreadsheet");
		// Compute the number of columns and display column headers.============================================================================
		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.trajectorySpreadsheet;

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

void Designit::updateSpreadsheet()
{
	// Update the output table tab.
	// Populate the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "updateSpreadsheet");

		// Compute the number of columns and display column headers.============================================================================
		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.mySpreadsheet;

		int noOfRows = 0;
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			noOfRows = noOfRows + project->get_MySurfaces()->at(k)->get_MyControlPoints()->size() * project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
		}

		my_table->setRowCount(noOfRows);

		int columnCount = 6; // The column count for the FrameNumber, CL, CD, Lift and Drag for each surface.

		// Actually set the column count of the table.
		my_table->setColumnCount(columnCount);

		// Now display the column headers.
		my_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Sur index"));
		my_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Row index"));
		my_table->setHorizontalHeaderItem(2, new QTableWidgetItem("Col index"));
		my_table->setHorizontalHeaderItem(3, new QTableWidgetItem("x"));
		my_table->setHorizontalHeaderItem(4, new QTableWidgetItem("y"));
		my_table->setHorizontalHeaderItem(5, new QTableWidgetItem("z"));

		// Display data for each surface.======================================================================================
		int rowIndex = 0;
		for (int k = 0; k < project->get_MySurfaces()->size(); k++)
		{
			// Do the plotting.
			for (int i = 0; i < project->get_MySurfaces()->at(k)->get_MyControlPoints()->size(); i++)
			{
				for (int j = 0; j < project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).size(); j++)
				{
					int columnIndex = 0;

					QTableWidgetItem* new_itemK = new QTableWidgetItem();
					new_itemK->setText(QString::number(k));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemK);
					columnIndex++;

					QTableWidgetItem* new_itemI = new QTableWidgetItem();
					new_itemI->setText(QString::number(i));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemI);
					columnIndex++;

					QTableWidgetItem* new_itemJ = new QTableWidgetItem();
					new_itemJ->setText(QString::number(j));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemJ);
					columnIndex++;

					ITControlPoint *p = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j);

					QTableWidgetItem* new_itemX = new QTableWidgetItem();
					new_itemX->setText(QString::number(p->get_X()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemX);
					columnIndex++;

					QTableWidgetItem* new_itemY = new QTableWidgetItem();
					new_itemY->setText(QString::number(p->get_Y()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemY);
					columnIndex++;

					QTableWidgetItem* new_itemZ = new QTableWidgetItem();
					new_itemZ->setText(QString::number(p->get_Z()));  // 
					my_table->setItem(rowIndex, columnIndex, new_itemZ);
					columnIndex++;

					rowIndex++;
				}
			}
		}
	}
}

void Designit::updateTrajectorySpreadsheet()
{
	// Update the output table tab.
	if (MY_RUN_MODE == MYGUI)
	{
		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "updateSpreadsheet");

		int noOfSurfaces = project->get_MySurfaces()->size();
		QTableWidget* my_table = ui.trajectorySpreadsheet;

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

void Designit::appendStatusTableWidget(const QString key, const QString val)
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

void Designit::sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath)
{
	if (IsScrutiny)
	{
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

		// QString sss = QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Surfit&UserID=1&WorkgroupID=1&PlatformID=%1&ActionKey=%2&ActionValue=%3&ElapsedTimeSecs=%4&ProgramVersion=%5&DataFileNameWithPath=%6&TotalProblemSize=%7&MaxKeyFrame=0").arg(str).arg(actionKey).arg(actionValue).arg(elapsedTimeSecs).arg(PROGRAM_VERSION).arg(fileNameWithPath).arg(totalProblemSize);
		// project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Request: %s", sss.toStdString().c_str());
		// the HTTP request.
		//QNetworkRequest req = QNetworkRequest( QUrl( QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Flexit&UserID=1&WorkgroupID=1&PlatformID=%1").arg(QString::number(*idArray)) ) );
		QNetworkRequest req = QNetworkRequest(QUrl(QString("http://www.scrapeworld.com/cgi-bin/surfit/receiveLogFromProgram.pl?ProgramName=Surfit&UserID=1&WorkgroupID=1&PlatformID=%1&ActionKey=%2&ActionValue=%3&ElapsedTimeSecs=%4&ProgramVersion=%5&DataFileNameWithPath=%6&TotalProblemSize=%7&MaxKeyFrame=0").arg(str).arg(actionKey).arg(actionValue).arg(elapsedTimeSecs).arg(PROGRAM_VERSION).arg(fileNameWithPath).arg(totalProblemSize)));
		QNetworkReply *reply = mgr.get(req);

		eventLoop.exec(); // blocks stack until "finished()" has been called

		if (reply->error() == QNetworkReply::NoError)
		{
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Success: %s", reply->readAll());
			delete reply;
		}
		else
		{
			qDebug() << "Failure" << reply->errorString();
			project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Failure: %s", reply->readAll());

			delete reply;
		}
	}
}

void Designit::on_actionPlayout_Test_triggered()
{
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "Inside on_Playout_Test_triggered");

	for (int k = 0; k < project->get_MySurfaces()->size(); k++)
	{
		int nr = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();
		int nc = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
		int noOfElements = nr * nc * sizeof(float);

		float *h_x = (float *)malloc(noOfElements); // Allocate the host object-point x-coordinate.
		float *h_y = (float *)malloc(noOfElements); // Allocate the host object-point y-coordinate.
		float *h_yx = (float *)malloc(noOfElements); // Allocate the host object-point z-coordinate.

		int noOfRows = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();
		int noOfCols = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();

		// Create x and y data.
		// Create the x matrix in memory.
		int t = 0;
		for (int i = 0; i < noOfRows; i++)
		{
			for (int j = 0; j < noOfCols; j++)
			{
				h_x[t] = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_X();
				t++;
			}
		}

		// Create the x matrix in memory.
		t = 0;
		for (int j = 0; j < noOfCols; j++)
		{
			for (int i = 0; i < noOfRows; i++)
			{
				h_y[t] = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->get_Y();
				t++;
			}
		}

		// Print the matrices.
		ITPhysics::printDebugMatrix(noOfRows, noOfCols, 'R', h_x, "X", 2);
		ITPhysics::printDebugMatrix(noOfCols, noOfRows, 'R', h_y, "Y", 2);

		// Free memory.
		free(h_x);
		free(h_y);
		free(h_yx);
	}
}

void Designit::updateDataFromSpreadsheet()
{
	// User is editing the cells.
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

			int rowOffset = 0;
			int lastRowOffset = 0;
			for (int k = 0; k < project->get_MySurfaces()->size(); k++)
			{
				int noOfRows = project->get_MySurfaces()->at(k)->get_MyControlPoints()->size();
				int noOfCols = project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(0).size();
				rowOffset = rowOffset + noOfRows * noOfCols;

				if ((lastRowOffset <= row) && (row < rowOffset))
				{
					// We are editing the current surface.
					int innerRow = row - lastRowOffset;
					int i = innerRow / noOfCols;
					int j = innerRow%noOfCols;

					if (col == 3)
					{
						// We are editing an x value.
						project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_X(contentsOfCurrentItem.toFloat());
						project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_X(contentsOfCurrentItem.toFloat());
					}
					else if (col == 4)
					{
						// We are editing a y value.
						project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_Y(contentsOfCurrentItem.toFloat());
						project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_Y(contentsOfCurrentItem.toFloat());
					}
					else if (col == 5)
					{
						// We are editing a z value.
						project->get_MySurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_Z(contentsOfCurrentItem.toFloat());
						project->get_MyBaseSurfaces()->at(k)->get_MyControlPoints()->at(i).at(j)->set_Z(contentsOfCurrentItem.toFloat());
					}

					// Update the Bezier data.
					project->get_MySurfaces()->at(k)->manageComputationOfInterpolatedPoints();
				}
				lastRowOffset = rowOffset;
			}
		}

		// Update the spreadsheet.
		updateSpreadsheet();

		// Update the plots.
		updateAllTabs();

		// The user has changed something, so there is now unsaved data.
		UnsavedChanges = true;
	}
}

void Designit::updateTrajectoryFromSpreadsheet()
{
	// We are on the spreadsheet widget.
	project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, "We are focused on the spreadsheet");

	// Model of my first QTableWidget
	QItemSelectionModel *myModel = ui.trajectorySpreadsheet->selectionModel();

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
		updateTrajectorySpreadsheet();

		// Update the plots.
		updateAllTabs();
	}
}

void Designit::toolBoxTabChanged( int index )
{
	if (index == 1)
	{
		trajectoryMode = true;
	}
	else {
		trajectoryMode = false;
	}

	updateAllTabs();
}

bool Designit::eventFilter(QObject *obj, QEvent *event)
{
	if ((event->type() == QEvent::KeyPress) && (obj == ui.commandLine))
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		switch (keyEvent->key())
		{
		case Qt::Key_Up:

			if (commandPointer < 0)
			{
				commandPointer = 0;
			} 
			else if (commandPointer >= commandMemory.size() - 1)  \
			{
				commandPointer = commandMemory.size() - 1;
			}
			else
			{
				commandPointer++;
			}

			ui.commandLine->setText(commandMemory[commandPointer]);

			return true;

			break;

		case Qt::Key_Down:

			if (commandPointer <= 0)
			{
				commandPointer = 0;
			}
			else if (commandPointer > commandMemory.size() - 1)  \
			{
				commandPointer = commandMemory.size() - 1;
			}
			else
			{
				commandPointer--;
			}

			ui.commandLine->setText(commandMemory[commandPointer]);
			
			return true;

			break;

		default:
			return QMainWindow::eventFilter(obj, event);
			break;
		}
	}
	else {
		// pass the event on to the parent class
		return QMainWindow::eventFilter(obj, event);
	}
}

int Designit::testFunction(const QStringList & arguments)
{
	for (int i = 0; i < arguments.size(); i++)
	{
		QByteArray ba = arguments[i].toLatin1();
		const char *c_str2 = ba.data();

		project->printDebug(__FILE__, __LINE__, __FUNCTION__, 2, c_str2);
	}

	return 0;
}

int Designit::setPoint(const QStringList & arguments)
{
	if (arguments.size() != 7)
	{
		return 1;
	}

	int i, j, k;
	float posX, posY, posZ;
	bool status;

	i = arguments[1].toInt(&status);
	if (!status) return 2;
	j = arguments[2].toInt(&status);
	if (!status) return 2;
	k = arguments[3].toInt(&status);
	if (!status) return 2;
	posX = arguments[4].toFloat(&status);
	if (!status) return 2;
	posY = arguments[5].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[6].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->setPoint(i,j,k,posX,posY,posZ, true);
	}
	catch (std::exception& e) {
		return 99;
	}
	
	return 0;
}

int Designit::movePoint(const QStringList & arguments)
{
	if (arguments.size() != 7)
	{
		return 1;
	}

	int i, j, k;
	float posX, posY, posZ;
	bool status;

	i = arguments[1].toInt(&status);
	if (!status) return 2;
	j = arguments[2].toInt(&status);
	if (!status) return 2;
	k = arguments[3].toInt(&status);
	if (!status) return 2;
	posX = arguments[4].toFloat(&status);
	if (!status) return 2;
	posY = arguments[5].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[6].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->movePoint(i, j, k, posX, posY, posZ, true);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::rotatePoint(const QStringList & arguments)
{
	if (arguments.size() != 9)
	{
		return 1;
	}

	int surfaceID, i, j;
	float posX, posY, posZ, angle;
	PLANE p;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	i = arguments[2].toInt(&status);
	if (!status) return 2;
	j = arguments[3].toInt(&status);
	if (!status) return 2;
	posX = arguments[4].toFloat(&status);
	if (!status) return 2;
	posY = arguments[5].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[6].toFloat(&status);
	if (!status) return 2;

	angle = arguments[7].toFloat(&status);
	if (!status) return 2;

	angle = DEG_TO_RAD(angle);

	QString tmpStr = arguments[8].toLower();

	if (tmpStr == "xy")
	{
		p = XY;
	}
	else if (tmpStr == "xz")
	{
		p = XZ;
	}
	else if (tmpStr == "yz")
	{
		p = YZ;
	}
	else
	{
		return 2;
	}

	try
	{
		project->rotatePoint(surfaceID, i, j, posX, posY, posZ, angle, p, true);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::setColumn(const QStringList & arguments)
{
	if (arguments.size() != 6)
	{
		return 1;
	}

	int surfaceID, j;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	j = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->setColumn(surfaceID, j, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::moveColumn(const QStringList & arguments)
{
	if (arguments.size() != 6)
	{
		return 1;
	}

	int surfaceID, j;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	j = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->moveColumn(surfaceID, j, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::rotateColumn(const QStringList & arguments)
{
	if (arguments.size() != 8)
	{
		return 1;
	}

	int surfaceID, j;
	float posX, posY, posZ, angle;
	PLANE p;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	j = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	angle = arguments[6].toFloat(&status);
	if (!status) return 2;

	angle = DEG_TO_RAD(angle);

	QString tmpStr = arguments[7].toLower();

	if (tmpStr == "xy")
	{
		p = XY;
	}
	else if (tmpStr == "xz")
	{
		p = XZ;
	}
	else if (tmpStr == "yz")
	{
		p = YZ;
	}
	else
	{
		return 2;
	}

	try
	{
		project->rotateColumn(surfaceID, j, posX, posY, posZ, angle, p);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::setRow(const QStringList & arguments)
{
	if (arguments.size() != 6)
	{
		return 1;
	}

	int surfaceID, i;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	i = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->setRow(surfaceID, i, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::moveRow(const QStringList & arguments)
{
	if (arguments.size() != 6)
	{
		return 1;
	}

	int surfaceID, i;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	i = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->moveRow(surfaceID, i, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::rotateRow(const QStringList & arguments)
{
	if (arguments.size() != 8)
	{
		return 1;
	}

	int surfaceID, i;
	float posX, posY, posZ, angle;
	PLANE p;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	i = arguments[2].toInt(&status);
	if (!status) return 2;
	posX = arguments[3].toFloat(&status);
	if (!status) return 2;
	posY = arguments[4].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[5].toFloat(&status);
	if (!status) return 2;

	angle = arguments[6].toFloat(&status);
	if (!status) return 2;

	angle = DEG_TO_RAD(angle);

	QString tmpStr = arguments[7].toLower();

	if (tmpStr == "xy")
	{
		p = XY;
	}
	else if (tmpStr == "xz")
	{
		p = XZ;
	}
	else if (tmpStr == "yz")
	{
		p = YZ;
	}
	else
	{
		return 2;
	}

	try
	{
		project->rotateRow(surfaceID, i, posX, posY, posZ, angle, p);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::setSurface(const QStringList & arguments)
{
	if (arguments.size() != 5)
	{
		return 1;
	}

	int surfaceID;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	posX = arguments[2].toFloat(&status);
	if (!status) return 2;
	posY = arguments[3].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[4].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->setSurface(surfaceID, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::moveSurface(const QStringList & arguments)
{
	if (arguments.size() != 5)
	{
		return 1;
	}

	int surfaceID;
	float posX, posY, posZ;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	posX = arguments[2].toFloat(&status);
	if (!status) return 2;
	posY = arguments[3].toFloat(&status);
	if (!status) return 2;
	posZ = arguments[4].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->moveSurface(surfaceID, posX, posY, posZ);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::rotateSurface(const QStringList & arguments)
{
	if (arguments.size() != 7)
	{
		return 1;
	}

	int surfaceID;
	float x, y, z, angle;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	x = arguments[2].toFloat(&status);
	if (!status) return 2;
	y = arguments[3].toFloat(&status);
	if (!status) return 2;
	z = arguments[4].toFloat(&status);
	if (!status) return 2;
	angle = arguments[5].toFloat(&status);
	if (!status) return 2;

	angle = DEG_TO_RAD(angle);

	QString tmpStr = arguments[6].toLower();
	PLANE p;

	if (tmpStr == "xy")
	{
		p = XY;
	}
	else if (tmpStr == "xz")
	{
		p = XZ;
	}
	else if (tmpStr == "yz")
	{
		p = YZ;
	}
	else
	{
		return 2;
	}

	try
	{
		project->rotateSurface(surfaceID, x, y, z, angle, p);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::rotateSurfaceCentral(const QStringList & arguments)
{
	if (arguments.size() != 4)
	{
		return 1;
	}

	int surfaceID;
	float angle;
	bool status;
	PLANE p;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	angle = arguments[2].toFloat(&status);
	if (!status) return 2;

	angle = DEG_TO_RAD(angle);

	QString tmpStr = arguments[6].toLower();

	if (tmpStr == "xy")
	{
		p = XY;
	}
	else if (tmpStr == "xz")
	{
		p = XZ;
	}
	else if (tmpStr == "yz")
	{
		p = YZ;
	}
	else
	{
		return 2;
	}

	try
	{
		project->rotateSurfaceCentral(surfaceID, angle, p);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::resizeSurface(const QStringList & arguments)
{
	if (arguments.size() != 3)
	{
		return 1;
	}

	int surfaceID;
	float factor;
	bool status;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;
	factor = arguments[2].toFloat(&status);
	if (!status) return 2;

	try
	{
		project->resizeSurface(surfaceID, factor);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::addSurface(const QStringList & arguments)
{
	if (arguments.size() != 1)
	{
		return 1;
	}

	try
	{
		project->addSurface();
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}

int Designit::deleteSurface(const QStringList & arguments)
{
	if (arguments.size() != 2)
	{
		return 1;
	}

	int surfaceID;
	float angle;
	bool status;
	PLANE p;

	surfaceID = arguments[1].toInt(&status);
	if (!status) return 2;

	try
	{
		project->deleteSurface(surfaceID);
	}
	catch (std::exception& e) {
		return 99;
	}

	return 0;
}
