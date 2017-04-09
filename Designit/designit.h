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

	void userHasEnteredTextData();

	void toolBoxTabChanged(int index);

	void handleCommand();

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

private:
	Ui::DesignitClass ui;

	void loadData(QString fileNameWithPath);
	bool checkExitWithUnsavedData(); // Returns true if user wants to exit.
	void closeProject();
	void closeEvent(QCloseEvent *bar);

	static int testFunction(const QStringList & arguments);
	static int setPoint(const QStringList & arguments);
	static int movePoint(const QStringList & arguments);

	void createNewTrajectoryCurve( const int k );

public:
	//std::unordered_map<QString, std::function<void(const QStringList & arguments)>> functions;
	std::unordered_map<std::string, std::function<int(const QStringList & arguments)>> functions;
	std::vector< QString > commandMemory;
	int commandPointer = -1;
};

#endif // DESIGNIT_H