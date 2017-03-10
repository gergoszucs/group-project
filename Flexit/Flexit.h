#pragma once

#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QtWidgets/QMainWindow>
#include "ui_Flexit.h"

class Flexit : public QMainWindow
{
	Q_OBJECT

public:
	Flexit(QWidget *parent = Q_NULLPTR);
	void loadData(QString fileNameWithPath);
	void sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeMSecs, int totalProblemSize, QString fileNameWithPath);
	void appendStatusTableWidget(const QString key, const QString val);
	void managePopulationOfDisplayMatrix();
	void setHardwareReportTextEdit(const QString str);
	void updateAllTabsForCurrentFrameNumber();
	void manageUpdateOfDisplayMatrix(int noOfPanels);
	void appendDataViewTextEdit(const QString str);

	public slots:

	void on_actionOpen_triggered();
	void on_collapseAllButton_clicked();
	void on_expandAllButton_clicked();
	void on_actionDry_run_triggered();
	void on_actionStep_triggered();
	void on_actionPanels_triggered();
	void on_actionControl_points_triggered();
	void on_actionInterpolated_points_triggered();
	void on_actionVorticities_triggered();
	void on_actionWakes_triggered();
	void on_actionBound_vortices_triggered();
	void on_actionNormals_triggered();
	void on_actionVelocity_field_triggered();
	void on_actionDrawVelocityField_triggered();
	void on_actionGust_triggered();
	void on_actionPressure_distribution_triggered();
	void on_actionAnnotations_triggered();
	void on_actionTracks_triggered();

	void on_actionStart_triggered();
	void on_actionPause_triggered();
	void on_actionStop_triggered();
	void on_actionReset_triggered();
	void on_actionClose_triggered();
	void on_actionStartReplay_triggered();
	void on_actionTest_triggered();

	void on_actionEuler_Bernoulli_triggered();

	void on_actionAbout_triggered();
	void on_actionWeb_help_triggered();
	void on_actionSave_As_triggered();
	void on_actionExit_triggered();

	void closeEvent(QCloseEvent *bar);

private:
	Ui::FlexitClass ui;

	void displayInfluenceCoefficientTable();
	void showMyOutputTable();
	void closeProject();
	bool checkExitWithUnsavedData();

};