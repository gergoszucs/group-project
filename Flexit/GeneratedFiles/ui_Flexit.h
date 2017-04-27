/********************************************************************************
** Form generated from reading UI file 'Flexit.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLEXIT_H
#define UI_FLEXIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <myglwidget.h>
#include "matrixviewglwidget.h"
#include "myebwidget.h"
#include "myoutputgraphicsglwidget.h"
#include "mytrajectoryglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_FlexitClass
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionExit;
    QAction *actionWeb_help;
    QAction *actionAxes;
    QAction *actionControl_points;
    QAction *actionInterpolated_points;
    QAction *actionDry_run;
    QAction *actionPause;
    QAction *actionTracks;
    QAction *actionPanels;
    QAction *actionNormals;
    QAction *actionStart;
    QAction *actionStop;
    QAction *actionVorticities;
    QAction *actionWakes;
    QAction *actionBound_vortices;
    QAction *actionStartReplay;
    QAction *actionTranslate_view_with_surface;
    QAction *actionAnnotations;
    QAction *actionPressure_distribution;
    QAction *actionSave_As;
    QAction *actionBeam_Elements;
    QAction *actionBeam_nodes;
    QAction *actionBeam_loading;
    QAction *actionBeam_ghost_nodes;
    QAction *actionBeam_ghost_elements;
    QAction *actionDeflected_beam_nodes;
    QAction *actionUVLM;
    QAction *actionEuler_Bernoulli;
    QAction *actionVelocity_field;
    QAction *actionQuiet_graphics;
    QAction *actionStep;
    QAction *actionTest;
    QAction *actionDrawVelocityField;
    QAction *actionTrim;
    QAction *actionZ_reset;
    QAction *actionY_reset;
    QAction *actionX_reset;
    QAction *actionAbout;
    QAction *actionSend_email;
    QAction *actionSend_HTTP_log_message;
    QAction *actionReset;
    QAction *actionGust;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTableWidget *myStatusTableWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_2;
    QTextEdit *hardwareReportTextEdit;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_5;
    QTextEdit *fileTextEdit;
    QWidget *tab_3;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QTreeView *jsonTreeView;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *expandAllButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *collapseAllButton;
    QWidget *tab_4;
    QHBoxLayout *horizontalLayout_6;
    MyGLWidget *myGLWidget;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_3;
    MyTrajectoryGLWidget *myTrajectoryGLWidget;
    QWidget *tab_6;
    QHBoxLayout *horizontalLayout_7;
    MatrixViewGLWidget *myMatrixViewGLWidget;
    QWidget *tab_7;
    QHBoxLayout *horizontalLayout_8;
    QTableWidget *myInfluenceCoeffTableWidget;
    QWidget *tab_9;
    QHBoxLayout *horizontalLayout_10;
    QTextEdit *myDataViewTextEdit;
    QWidget *tab_8;
    QHBoxLayout *horizontalLayout_9;
    QTableWidget *myOutputTableWidget;
    QWidget *tab_10;
    QHBoxLayout *horizontalLayout_11;
    MyOutputGraphicsGLWidget *myOutputGraphicsGLWidget;
    QWidget *tab_11;
    QHBoxLayout *horizontalLayout_12;
    MyEBWidget *myEBWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuView;
    QMenu *menuDry;
    QMenu *menuCompute;
    QMenu *menuReplay;
    QMenu *menuModes;
    QMenu *menuTest;
    QMenu *menuPrepocessing;
    QMenu *menuAccount;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *FlexitClass)
    {
        if (FlexitClass->objectName().isEmpty())
            FlexitClass->setObjectName(QStringLiteral("FlexitClass"));
        FlexitClass->resize(1113, 598);
        actionOpen = new QAction(FlexitClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/icon_open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionClose = new QAction(FlexitClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/icon_close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon1);
        actionExit = new QAction(FlexitClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("Resources/icon_exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionWeb_help = new QAction(FlexitClass);
        actionWeb_help->setObjectName(QStringLiteral("actionWeb_help"));
        QIcon icon3;
        icon3.addFile(QStringLiteral("Resources/icon_help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWeb_help->setIcon(icon3);
        actionAxes = new QAction(FlexitClass);
        actionAxes->setObjectName(QStringLiteral("actionAxes"));
        actionControl_points = new QAction(FlexitClass);
        actionControl_points->setObjectName(QStringLiteral("actionControl_points"));
        actionInterpolated_points = new QAction(FlexitClass);
        actionInterpolated_points->setObjectName(QStringLiteral("actionInterpolated_points"));
        actionDry_run = new QAction(FlexitClass);
        actionDry_run->setObjectName(QStringLiteral("actionDry_run"));
        QIcon icon4;
        icon4.addFile(QStringLiteral("Resources/icon_dryrun.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDry_run->setIcon(icon4);
        actionPause = new QAction(FlexitClass);
        actionPause->setObjectName(QStringLiteral("actionPause"));
        QIcon icon5;
        icon5.addFile(QStringLiteral("Resources/icon_pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPause->setIcon(icon5);
        actionTracks = new QAction(FlexitClass);
        actionTracks->setObjectName(QStringLiteral("actionTracks"));
        actionPanels = new QAction(FlexitClass);
        actionPanels->setObjectName(QStringLiteral("actionPanels"));
        actionNormals = new QAction(FlexitClass);
        actionNormals->setObjectName(QStringLiteral("actionNormals"));
        actionStart = new QAction(FlexitClass);
        actionStart->setObjectName(QStringLiteral("actionStart"));
        QIcon icon6;
        icon6.addFile(QStringLiteral("Resources/icon_cuda.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStart->setIcon(icon6);
        actionStop = new QAction(FlexitClass);
        actionStop->setObjectName(QStringLiteral("actionStop"));
        QIcon icon7;
        icon7.addFile(QStringLiteral("Resources/icon_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStop->setIcon(icon7);
        actionVorticities = new QAction(FlexitClass);
        actionVorticities->setObjectName(QStringLiteral("actionVorticities"));
        actionWakes = new QAction(FlexitClass);
        actionWakes->setObjectName(QStringLiteral("actionWakes"));
        actionBound_vortices = new QAction(FlexitClass);
        actionBound_vortices->setObjectName(QStringLiteral("actionBound_vortices"));
        actionStartReplay = new QAction(FlexitClass);
        actionStartReplay->setObjectName(QStringLiteral("actionStartReplay"));
        QIcon icon8;
        icon8.addFile(QStringLiteral("Resources/icon_replay.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStartReplay->setIcon(icon8);
        actionTranslate_view_with_surface = new QAction(FlexitClass);
        actionTranslate_view_with_surface->setObjectName(QStringLiteral("actionTranslate_view_with_surface"));
        actionAnnotations = new QAction(FlexitClass);
        actionAnnotations->setObjectName(QStringLiteral("actionAnnotations"));
        actionPressure_distribution = new QAction(FlexitClass);
        actionPressure_distribution->setObjectName(QStringLiteral("actionPressure_distribution"));
        actionSave_As = new QAction(FlexitClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon9;
        icon9.addFile(QStringLiteral("Resources/icon_save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon9);
        actionBeam_Elements = new QAction(FlexitClass);
        actionBeam_Elements->setObjectName(QStringLiteral("actionBeam_Elements"));
        actionBeam_nodes = new QAction(FlexitClass);
        actionBeam_nodes->setObjectName(QStringLiteral("actionBeam_nodes"));
        actionBeam_loading = new QAction(FlexitClass);
        actionBeam_loading->setObjectName(QStringLiteral("actionBeam_loading"));
        actionBeam_ghost_nodes = new QAction(FlexitClass);
        actionBeam_ghost_nodes->setObjectName(QStringLiteral("actionBeam_ghost_nodes"));
        actionBeam_ghost_elements = new QAction(FlexitClass);
        actionBeam_ghost_elements->setObjectName(QStringLiteral("actionBeam_ghost_elements"));
        actionDeflected_beam_nodes = new QAction(FlexitClass);
        actionDeflected_beam_nodes->setObjectName(QStringLiteral("actionDeflected_beam_nodes"));
        actionUVLM = new QAction(FlexitClass);
        actionUVLM->setObjectName(QStringLiteral("actionUVLM"));
        actionEuler_Bernoulli = new QAction(FlexitClass);
        actionEuler_Bernoulli->setObjectName(QStringLiteral("actionEuler_Bernoulli"));
        actionVelocity_field = new QAction(FlexitClass);
        actionVelocity_field->setObjectName(QStringLiteral("actionVelocity_field"));
        actionQuiet_graphics = new QAction(FlexitClass);
        actionQuiet_graphics->setObjectName(QStringLiteral("actionQuiet_graphics"));
        actionStep = new QAction(FlexitClass);
        actionStep->setObjectName(QStringLiteral("actionStep"));
        QIcon icon10;
        icon10.addFile(QStringLiteral("Resources/icon_step.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionStep->setIcon(icon10);
        actionTest = new QAction(FlexitClass);
        actionTest->setObjectName(QStringLiteral("actionTest"));
        QIcon icon11;
        icon11.addFile(QStringLiteral("Resources/icon_test.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionTest->setIcon(icon11);
        actionDrawVelocityField = new QAction(FlexitClass);
        actionDrawVelocityField->setObjectName(QStringLiteral("actionDrawVelocityField"));
        actionTrim = new QAction(FlexitClass);
        actionTrim->setObjectName(QStringLiteral("actionTrim"));
        QIcon icon12;
        icon12.addFile(QStringLiteral("Resources/icon_trim.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionTrim->setIcon(icon12);
        actionZ_reset = new QAction(FlexitClass);
        actionZ_reset->setObjectName(QStringLiteral("actionZ_reset"));
        actionY_reset = new QAction(FlexitClass);
        actionY_reset->setObjectName(QStringLiteral("actionY_reset"));
        actionX_reset = new QAction(FlexitClass);
        actionX_reset->setObjectName(QStringLiteral("actionX_reset"));
        actionAbout = new QAction(FlexitClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionSend_email = new QAction(FlexitClass);
        actionSend_email->setObjectName(QStringLiteral("actionSend_email"));
        QIcon icon13;
        icon13.addFile(QStringLiteral("Resources/icon_email.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSend_email->setIcon(icon13);
        actionSend_HTTP_log_message = new QAction(FlexitClass);
        actionSend_HTTP_log_message->setObjectName(QStringLiteral("actionSend_HTTP_log_message"));
        QIcon icon14;
        icon14.addFile(QStringLiteral("Resources/icon_web.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSend_HTTP_log_message->setIcon(icon14);
        actionReset = new QAction(FlexitClass);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        QIcon icon15;
        icon15.addFile(QStringLiteral("Resources/icon_reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset->setIcon(icon15);
        actionGust = new QAction(FlexitClass);
        actionGust->setObjectName(QStringLiteral("actionGust"));
        centralWidget = new QWidget(FlexitClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setMaximumSize(QSize(200, 16777215));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        myStatusTableWidget = new QTableWidget(frame);
        myStatusTableWidget->setObjectName(QStringLiteral("myStatusTableWidget"));

        verticalLayout->addWidget(myStatusTableWidget);


        horizontalLayout->addWidget(frame);

        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        horizontalLayout_2 = new QHBoxLayout(tab);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        hardwareReportTextEdit = new QTextEdit(tab);
        hardwareReportTextEdit->setObjectName(QStringLiteral("hardwareReportTextEdit"));

        horizontalLayout_2->addWidget(hardwareReportTextEdit);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        horizontalLayout_5 = new QHBoxLayout(tab_2);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        fileTextEdit = new QTextEdit(tab_2);
        fileTextEdit->setObjectName(QStringLiteral("fileTextEdit"));

        horizontalLayout_5->addWidget(fileTextEdit);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        horizontalLayout_4 = new QHBoxLayout(tab_3);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        jsonTreeView = new QTreeView(tab_3);
        jsonTreeView->setObjectName(QStringLiteral("jsonTreeView"));
        jsonTreeView->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_2->addWidget(jsonTreeView);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        expandAllButton = new QPushButton(tab_3);
        expandAllButton->setObjectName(QStringLiteral("expandAllButton"));

        horizontalLayout_3->addWidget(expandAllButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        collapseAllButton = new QPushButton(tab_3);
        collapseAllButton->setObjectName(QStringLiteral("collapseAllButton"));

        horizontalLayout_3->addWidget(collapseAllButton);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout_4->addLayout(verticalLayout_2);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        horizontalLayout_6 = new QHBoxLayout(tab_4);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        myGLWidget = new MyGLWidget(tab_4);
        myGLWidget->setObjectName(QStringLiteral("myGLWidget"));

        horizontalLayout_6->addWidget(myGLWidget);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        verticalLayout_3 = new QVBoxLayout(tab_5);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        myTrajectoryGLWidget = new MyTrajectoryGLWidget(tab_5);
        myTrajectoryGLWidget->setObjectName(QStringLiteral("myTrajectoryGLWidget"));

        verticalLayout_3->addWidget(myTrajectoryGLWidget);

        tabWidget->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        horizontalLayout_7 = new QHBoxLayout(tab_6);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        myMatrixViewGLWidget = new MatrixViewGLWidget(tab_6);
        myMatrixViewGLWidget->setObjectName(QStringLiteral("myMatrixViewGLWidget"));

        horizontalLayout_7->addWidget(myMatrixViewGLWidget);

        tabWidget->addTab(tab_6, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QStringLiteral("tab_7"));
        horizontalLayout_8 = new QHBoxLayout(tab_7);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        myInfluenceCoeffTableWidget = new QTableWidget(tab_7);
        myInfluenceCoeffTableWidget->setObjectName(QStringLiteral("myInfluenceCoeffTableWidget"));

        horizontalLayout_8->addWidget(myInfluenceCoeffTableWidget);

        tabWidget->addTab(tab_7, QString());
        tab_9 = new QWidget();
        tab_9->setObjectName(QStringLiteral("tab_9"));
        horizontalLayout_10 = new QHBoxLayout(tab_9);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        myDataViewTextEdit = new QTextEdit(tab_9);
        myDataViewTextEdit->setObjectName(QStringLiteral("myDataViewTextEdit"));

        horizontalLayout_10->addWidget(myDataViewTextEdit);

        tabWidget->addTab(tab_9, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName(QStringLiteral("tab_8"));
        horizontalLayout_9 = new QHBoxLayout(tab_8);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        myOutputTableWidget = new QTableWidget(tab_8);
        myOutputTableWidget->setObjectName(QStringLiteral("myOutputTableWidget"));

        horizontalLayout_9->addWidget(myOutputTableWidget);

        tabWidget->addTab(tab_8, QString());
        tab_10 = new QWidget();
        tab_10->setObjectName(QStringLiteral("tab_10"));
        horizontalLayout_11 = new QHBoxLayout(tab_10);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        myOutputGraphicsGLWidget = new MyOutputGraphicsGLWidget(tab_10);
        myOutputGraphicsGLWidget->setObjectName(QStringLiteral("myOutputGraphicsGLWidget"));

        horizontalLayout_11->addWidget(myOutputGraphicsGLWidget);

        tabWidget->addTab(tab_10, QString());
        tab_11 = new QWidget();
        tab_11->setObjectName(QStringLiteral("tab_11"));
        horizontalLayout_12 = new QHBoxLayout(tab_11);
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        myEBWidget = new MyEBWidget(tab_11);
        myEBWidget->setObjectName(QStringLiteral("myEBWidget"));

        horizontalLayout_12->addWidget(myEBWidget);

        tabWidget->addTab(tab_11, QString());

        horizontalLayout->addWidget(tabWidget);

        FlexitClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(FlexitClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1113, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuDry = new QMenu(menuBar);
        menuDry->setObjectName(QStringLiteral("menuDry"));
        menuCompute = new QMenu(menuBar);
        menuCompute->setObjectName(QStringLiteral("menuCompute"));
        menuReplay = new QMenu(menuBar);
        menuReplay->setObjectName(QStringLiteral("menuReplay"));
        menuModes = new QMenu(menuBar);
        menuModes->setObjectName(QStringLiteral("menuModes"));
        menuTest = new QMenu(menuBar);
        menuTest->setObjectName(QStringLiteral("menuTest"));
        menuPrepocessing = new QMenu(menuBar);
        menuPrepocessing->setObjectName(QStringLiteral("menuPrepocessing"));
        menuAccount = new QMenu(menuBar);
        menuAccount->setObjectName(QStringLiteral("menuAccount"));
        FlexitClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FlexitClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        FlexitClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(FlexitClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        FlexitClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuModes->menuAction());
        menuBar->addAction(menuDry->menuAction());
        menuBar->addAction(menuPrepocessing->menuAction());
        menuBar->addAction(menuCompute->menuAction());
        menuBar->addAction(menuReplay->menuAction());
        menuBar->addAction(menuAccount->menuAction());
        menuBar->addAction(menuTest->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionClose);
        menuFile->addAction(actionExit);
        menuFile->addAction(actionSave_As);
        menuHelp->addAction(actionWeb_help);
        menuHelp->addAction(actionAbout);
        menuView->addAction(actionQuiet_graphics);
        menuView->addSeparator();
        menuView->addAction(actionAxes);
        menuView->addAction(actionControl_points);
        menuView->addAction(actionInterpolated_points);
        menuView->addAction(actionTracks);
        menuView->addAction(actionPanels);
        menuView->addAction(actionNormals);
        menuView->addAction(actionVorticities);
        menuView->addAction(actionBound_vortices);
        menuView->addAction(actionWakes);
        menuView->addAction(actionAnnotations);
        menuView->addAction(actionPressure_distribution);
        menuView->addAction(actionDrawVelocityField);
        menuView->addAction(actionGust);
        menuView->addSeparator();
        menuView->addAction(actionBeam_Elements);
        menuView->addAction(actionBeam_ghost_elements);
        menuView->addAction(actionBeam_nodes);
        menuView->addAction(actionBeam_ghost_nodes);
        menuView->addAction(actionBeam_loading);
        menuView->addSeparator();
        menuView->addAction(actionDeflected_beam_nodes);
        menuView->addSeparator();
        menuView->addAction(actionTranslate_view_with_surface);
        menuView->addSeparator();
        menuView->addAction(actionX_reset);
        menuView->addAction(actionY_reset);
        menuView->addAction(actionZ_reset);
        menuDry->addAction(actionDry_run);
        menuDry->addAction(actionPause);
        menuCompute->addAction(actionStep);
        menuCompute->addAction(actionStart);
        menuCompute->addAction(actionStop);
        menuCompute->addAction(actionReset);
        menuReplay->addAction(actionStartReplay);
        menuModes->addAction(actionUVLM);
        menuModes->addAction(actionEuler_Bernoulli);
        menuModes->addAction(actionVelocity_field);
        menuTest->addAction(actionTest);
        menuPrepocessing->addAction(actionTrim);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDry_run);
        mainToolBar->addAction(actionTrim);
        mainToolBar->addAction(actionStep);
        mainToolBar->addAction(actionStart);
        mainToolBar->addAction(actionPause);
        mainToolBar->addAction(actionReset);
        mainToolBar->addAction(actionStartReplay);
        mainToolBar->addAction(actionStop);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSave_As);
        mainToolBar->addAction(actionClose);
        mainToolBar->addAction(actionExit);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionTest);
        mainToolBar->addAction(actionWeb_help);

        retranslateUi(FlexitClass);

        tabWidget->setCurrentIndex(10);


        QMetaObject::connectSlotsByName(FlexitClass);
    } // setupUi

    void retranslateUi(QMainWindow *FlexitClass)
    {
        FlexitClass->setWindowTitle(QApplication::translate("FlexitClass", "Flexit", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("FlexitClass", "Open...", Q_NULLPTR));
        actionClose->setText(QApplication::translate("FlexitClass", "Close", Q_NULLPTR));
        actionExit->setText(QApplication::translate("FlexitClass", "Exit", Q_NULLPTR));
        actionWeb_help->setText(QApplication::translate("FlexitClass", "Web help", Q_NULLPTR));
        actionAxes->setText(QApplication::translate("FlexitClass", "Axes", Q_NULLPTR));
        actionControl_points->setText(QApplication::translate("FlexitClass", "Control points", Q_NULLPTR));
        actionInterpolated_points->setText(QApplication::translate("FlexitClass", "Interpolated points", Q_NULLPTR));
        actionDry_run->setText(QApplication::translate("FlexitClass", "Dry run", Q_NULLPTR));
        actionPause->setText(QApplication::translate("FlexitClass", "Pause", Q_NULLPTR));
        actionTracks->setText(QApplication::translate("FlexitClass", "Tracks", Q_NULLPTR));
        actionPanels->setText(QApplication::translate("FlexitClass", "Panels", Q_NULLPTR));
        actionNormals->setText(QApplication::translate("FlexitClass", "Normals", Q_NULLPTR));
        actionStart->setText(QApplication::translate("FlexitClass", "Start", Q_NULLPTR));
        actionStop->setText(QApplication::translate("FlexitClass", "Stop", Q_NULLPTR));
        actionVorticities->setText(QApplication::translate("FlexitClass", "Vorticities", Q_NULLPTR));
        actionWakes->setText(QApplication::translate("FlexitClass", "Wakes", Q_NULLPTR));
        actionBound_vortices->setText(QApplication::translate("FlexitClass", "Bound vortices", Q_NULLPTR));
        actionStartReplay->setText(QApplication::translate("FlexitClass", "Start replay", Q_NULLPTR));
        actionTranslate_view_with_surface->setText(QApplication::translate("FlexitClass", "Translate view with surface", Q_NULLPTR));
        actionAnnotations->setText(QApplication::translate("FlexitClass", "Annotations", Q_NULLPTR));
        actionPressure_distribution->setText(QApplication::translate("FlexitClass", "Pressure distribution", Q_NULLPTR));
        actionSave_As->setText(QApplication::translate("FlexitClass", "Save As ...", Q_NULLPTR));
        actionBeam_Elements->setText(QApplication::translate("FlexitClass", "Beam Elements", Q_NULLPTR));
        actionBeam_nodes->setText(QApplication::translate("FlexitClass", "Beam nodes", Q_NULLPTR));
        actionBeam_loading->setText(QApplication::translate("FlexitClass", "Beam loading", Q_NULLPTR));
        actionBeam_ghost_nodes->setText(QApplication::translate("FlexitClass", "Beam ghost nodes", Q_NULLPTR));
        actionBeam_ghost_elements->setText(QApplication::translate("FlexitClass", "Beam ghost elements", Q_NULLPTR));
        actionDeflected_beam_nodes->setText(QApplication::translate("FlexitClass", "Deflected beam nodes", Q_NULLPTR));
        actionUVLM->setText(QApplication::translate("FlexitClass", "UVLM", Q_NULLPTR));
        actionEuler_Bernoulli->setText(QApplication::translate("FlexitClass", "Euler Bernoulli", Q_NULLPTR));
        actionVelocity_field->setText(QApplication::translate("FlexitClass", "Velocity field", Q_NULLPTR));
        actionQuiet_graphics->setText(QApplication::translate("FlexitClass", "Quiet graphics", Q_NULLPTR));
        actionStep->setText(QApplication::translate("FlexitClass", "Step", Q_NULLPTR));
        actionTest->setText(QApplication::translate("FlexitClass", "Test", Q_NULLPTR));
        actionDrawVelocityField->setText(QApplication::translate("FlexitClass", "Velocity field", Q_NULLPTR));
        actionTrim->setText(QApplication::translate("FlexitClass", "Trim node masses", Q_NULLPTR));
        actionZ_reset->setText(QApplication::translate("FlexitClass", "Z reset", Q_NULLPTR));
        actionY_reset->setText(QApplication::translate("FlexitClass", "Y reset", Q_NULLPTR));
        actionX_reset->setText(QApplication::translate("FlexitClass", "X reset", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("FlexitClass", "About ...", Q_NULLPTR));
        actionSend_email->setText(QApplication::translate("FlexitClass", "Send email", Q_NULLPTR));
        actionSend_HTTP_log_message->setText(QApplication::translate("FlexitClass", "Send HTTP log message", Q_NULLPTR));
        actionReset->setText(QApplication::translate("FlexitClass", "Reset", Q_NULLPTR));
        actionGust->setText(QApplication::translate("FlexitClass", "Gust", Q_NULLPTR));
        label->setText(QApplication::translate("FlexitClass", "Status window", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("FlexitClass", "Hardware", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("FlexitClass", "Data file", Q_NULLPTR));
        expandAllButton->setText(QApplication::translate("FlexitClass", "Expand All", Q_NULLPTR));
        collapseAllButton->setText(QApplication::translate("FlexitClass", "Collapse All", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("FlexitClass", "JSON tree", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("FlexitClass", "Graphics", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("FlexitClass", "Trajectories", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("FlexitClass", "B matrix", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QApplication::translate("FlexitClass", "Influence matrix", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_9), QApplication::translate("FlexitClass", "Data view", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_8), QApplication::translate("FlexitClass", "Output table", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_10), QApplication::translate("FlexitClass", "Output graphics", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_11), QApplication::translate("FlexitClass", "Euler-Bernoulli", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("FlexitClass", "File", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("FlexitClass", "Help", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("FlexitClass", "View", Q_NULLPTR));
        menuDry->setTitle(QApplication::translate("FlexitClass", "Dry", Q_NULLPTR));
        menuCompute->setTitle(QApplication::translate("FlexitClass", "Compute", Q_NULLPTR));
        menuReplay->setTitle(QApplication::translate("FlexitClass", "Replay", Q_NULLPTR));
        menuModes->setTitle(QApplication::translate("FlexitClass", "Modes", Q_NULLPTR));
        menuTest->setTitle(QApplication::translate("FlexitClass", "Testing", Q_NULLPTR));
        menuPrepocessing->setTitle(QApplication::translate("FlexitClass", "Prepocessing", Q_NULLPTR));
        menuAccount->setTitle(QApplication::translate("FlexitClass", "Account", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FlexitClass: public Ui_FlexitClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLEXIT_H
