#ifndef MOVEIT_H
#define MOVEIT_H

#include <QtWidgets/QMainWindow>
#include "ui_moveit.h"
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

class Moveit : public QMainWindow
{
	Q_OBJECT

public:
	Moveit(QWidget *parent = 0);
	~Moveit() {};

	void appendStatusTableWidget(const QString key, const QString val);
	void resetModeButtons();
	void updateAllTabs();
	void updateSpreadsheet();
	void keyPressEvent(QKeyEvent *event);

	void sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath);

	public slots:

	void on_actionOpen_triggered();
	void on_actionAnnotations_triggered();
	void on_actionNormals_triggered();
	void on_actionControl_points_triggered();
	void on_actionInterpolated_points_triggered();
	void on_actionAxes_triggered();
	void on_actionDrag_trajectory_point_triggered();
	void on_actionDry_run_triggered();
	void on_actionReset_all_views_triggered();
	void on_actionExit_triggered();
	void on_actionSave_As_triggered();
	void on_actionClose_triggered();

private:
	Ui::MoveitClass ui;
	void loadData(QString fileNameWithPath);
	void showSpreadsheet();

	void closeProject();
	bool checkExitWithUnsavedData(); // Returns true if user wants to exit.
	void closeEvent(QCloseEvent *bar);

};

#endif // MOVEIT_H