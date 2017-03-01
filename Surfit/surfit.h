#ifndef SURFIT_H
#define SURFIT_H

#include <QtWidgets/QMainWindow>
#include "ui_surfit.h"

// Stuff for http requests.
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

class Surfit : public QMainWindow
{
	Q_OBJECT

public:
	Surfit(QWidget *parent = 0);
	~Surfit();

	void appendStatusTableWidget(const QString key, const QString val);
	void sendHTTPRequest(QString actionKey, QString actionValue, float elapsedTimeSecs, int totalProblemSize, QString fileNameWithPath);
	void updateAllTabs();
	void setMyTextDataField(QString str);
	void resetModeButtons();
	void emptyFocusVectors();
	void updateSpreadsheet();
	void keyPressEvent(QKeyEvent *event);

public slots:

	void on_actionOpen_triggered();
	void on_actionExit_triggered();
	void on_actionSave_As_triggered();
	void on_actionClose_triggered();
	void on_actionReset_all_views_triggered();
	void on_actionWeb_help_triggered();
	void on_actionAbout_triggered();

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

private:
	Ui::SurfitClass ui;

	void loadData(QString fileNameWithPath);
	bool checkExitWithUnsavedData(); // Returns true if user wants to exit.
	void closeProject();
	void closeEvent(QCloseEvent *bar);
	void showSpreadsheet();
};

#endif // SURFIT_H
