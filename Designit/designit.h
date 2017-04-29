#ifndef DESIGNIT_H
#define DESIGNIT_H

#include <QtWidgets/QMainWindow>
#include "ui_designit.h"
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <unordered_map>
#include <functional>
#include "Enums.h"
#include "undoRedoSystem.h"

class Designit : public QMainWindow
{
	Q_OBJECT

public:
	Designit(QWidget *parent = 0);
	~Designit() {};

	void appendStatusTableWidget(const QString key, const QString val);
	void updateAllTabs();
	void setMyTextDataField(QString str);
	void resetModeButtons();
	void emptyFocusVectors();
	void keyPressEvent(QKeyEvent *event);

	void sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath);

	UndoRedoSystem undoRedo;

public slots:

	void on_actionOpen_triggered();
	void on_actionExit_triggered();
	void on_actionSave_As_triggered();
	void on_actionClose_triggered();
	void on_actionReset_all_views_triggered();
	void on_actionWeb_help_triggered();
	void on_actionAbout_triggered();
	void on_action_Flexit_triggered();

	void on_actionNew_surface_triggered();
	void on_actionDelete_surface_triggered();

	void on_actionDrag_triggered();
	void on_actionDrag_row_triggered();
	void on_actionDrag_col_triggered();
	void on_actionDrag_all_triggered();
	void on_actionRotate_all_triggered();
	void on_actionResize_all_triggered();
	void on_actionShear_triggered();
	void on_actionPerspective_triggered();
	void on_actionFlip_horizontal_triggered();
	void on_actionPlayout_Test_triggered();
	void on_actionCopy_surface_triggered();
	void on_actionInsert_row_triggered();
	void on_actionDelete_row_triggered();
	void on_actionDuplicate_row_triggered();
	void on_actionInsert_col_triggered();
	void on_actionDelete_col_triggered();
	void on_actionDuplicate_col_triggered();
	void on_actionMate_points_triggered();
	void on_actionCopy_surface_mirror_triggered();
	void on_actionMerge_surfaces_by_row_triggered();
	void on_actionMerge_surfaces_by_row_reverse_triggered();
	void on_actionMeasure_distance_triggered();
	void on_actionCentred_rotate_triggered();

	void on_actionControl_points_triggered();
	void on_actionInterpolated_points_triggered();
	void on_actionNormals_triggered();
	void on_actionAnnotations_triggered();
	void on_actionGrids_triggered();

	void on_actionGaussian_curvature_triggered();

	void on_actionInterpolated_points_density_U_triggered();
	void on_actionInterpolated_points_density_V_triggered();

	void on_collapseAllButton_clicked();
	void on_expandAllButton_clicked();

	void on_actionUndo_triggered();
	void on_actionRedo_triggered();

	void userHasEnteredTextData();

	void toolBoxTabChanged(int index);

	void handleCommand();

	void executeCommand(QString message, QStringList arguments, const bool reg);

	/**
	* @fn	static bool Designit::parsePlane(QString str, PLANE & p);
	*
	* @brief	Parses plane string.
	* 
	* @param str	String to be parsed.
	* @param p		Variable parsed plane will be hold in.
	*
	* @return		Bool status of operations (true if success).
	*
	* @author	Pawel Zybura
	* @date		28.04.2017
	*/
	static bool parsePlane(QString str, PLANE & p);

	void showSpreadsheet();
	void updateSpreadsheet();

	/**
	* @fn	void Designit::updateDataFromSpreadsheet();
	*
	* @brief	Handles updating mesh data when spreadsheet is modified.
	*
	* @author	Pawel Zybura
	* @date	14.03.2017
	*/
	void updateDataFromSpreadsheet();

	void showTrajectorySpreadsheet();
	void updateTrajectorySpreadsheet();

	/**
	* @fn	void Designit::updateTrajectoryFromSpreadsheet();
	*
	* @brief	Handles updating trajectory data when spreadsheet is modified.
	*
	* @author	Pawel Zybura
	* @date	07.04.2017
	*/
	void updateTrajectoryFromSpreadsheet();

	bool eventFilter(QObject *obj, QEvent *event);

	void resetCommandMemory();

	QString getCommandList();

private:
	Ui::DesignitClass ui;

	void loadData(QString fileNameWithPath);
	bool checkExitWithUnsavedData(); // Returns true if user wants to exit.
	void closeProject();
	void closeEvent(QCloseEvent *bar);

	static int testFunction(const QStringList & arguments, const bool reg);
	
	static int setPoint(const QStringList & arguments, const bool reg);
	static int movePoint(const QStringList & arguments, const bool reg);
	static int rotatePoint(const QStringList & arguments, const bool reg);

	static int setColumn(const QStringList & arguments, const bool reg);
	static int moveColumn(const QStringList & arguments, const bool reg);
	static int rotateColumn(const QStringList & arguments, const bool reg);

	static int setRow(const QStringList & arguments, const bool reg);
	static int moveRow(const QStringList & arguments, const bool reg);
	static int rotateRow(const QStringList & arguments, const bool reg);

	static int setSurface(const QStringList & arguments, const bool reg);
	static int moveSurface(const QStringList & arguments, const bool reg);
	static int rotateSurface(const QStringList & arguments, const bool reg);
	
	static int rotateSurfaceCentral(const QStringList & arguments, const bool reg);
	static int resizeSurface(const QStringList & arguments, const bool reg);
	static int addSurface(const QStringList & arguments, const bool reg);
	static int deleteSurface(const QStringList & arguments, const bool reg);

	static int sheer(const QStringList & arguments, const bool reg);
	static int sheerD(const QStringList & arguments, const bool reg);

	static int flipSurface(const QStringList & arguments, const bool reg);
	static int flipSurfacePoint(const QStringList & arguments, const bool reg);
	static int flipSurfaceCentral(const QStringList & arguments, const bool reg);

	static int copySurface(const QStringList & arguments, const bool reg);
	static int copySurfaceMirror(const QStringList & arguments, const bool reg);

	static int insertRow(const QStringList & arguments, const bool reg);
	static int duplicateRow(const QStringList & arguments, const bool reg);
	static int deleteRow(const QStringList & arguments, const bool reg);
	static int insertColumn(const QStringList & arguments, const bool reg);
	static int duplicateColumn(const QStringList & arguments, const bool reg);
	static int deleteColumn(const QStringList & arguments, const bool reg);

	static int matePoints(const QStringList & arguments, const bool reg);

	static int help(const QStringList & arguments, const bool reg);

	static int redoSurfaceDelete(const QStringList & arguments, const bool reg);
	static int redoRowDelete(const QStringList & arguments, const bool reg);
	static int redoColumnDelete(const QStringList & arguments, const bool reg);

public:
	std::unordered_map<std::string, std::function<int(const QStringList & arguments, const bool reg)>> functions;
	std::unordered_map<std::string, std::function<int(const QStringList & arguments, const bool reg)>> systemFunctions;
	std::vector< QString > commandMemory;
	int commandPointer = -1;
};

#endif // DESIGNIT_H