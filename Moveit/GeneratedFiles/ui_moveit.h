/********************************************************************************
** Form generated from reading UI file 'moveit.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOVEIT_H
#define UI_MOVEIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "myglgeneraltrajectorycurveview.h"
#include "myglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MoveitClass
{
public:
    QAction *actionOpen;
    QAction *actionAxes;
    QAction *actionInterpolated_points;
    QAction *actionControl_points;
    QAction *actionNormals;
    QAction *actionAnnotations;
    QAction *actionDrag_trajectory_point;
    QAction *actionDry_run;
    QAction *actionReset_all_views;
    QAction *actionExit;
    QAction *actionSave_As;
    QAction *actionClose;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QTableWidget *myStatusTableWidget;
    QSplitter *splitter;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_4;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QLineEdit *lineEdit;
    MyGLWidget *myGLWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout;
    QTextEdit *fileTextEdit;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_5;
    QTreeView *jsonTreeView;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_2;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewX;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewY;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewZ;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_6;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewR;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewP;
    MyGLGeneralTrajectoryCurveView *myGLGeneralTrajectoryCurveViewB;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *mySpreadsheet;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuTest;
    QMenu *menuHelp;
    QMenu *menuMode;
    QMenu *menuSimulation;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MoveitClass)
    {
        if (MoveitClass->objectName().isEmpty())
            MoveitClass->setObjectName(QStringLiteral("MoveitClass"));
        MoveitClass->resize(926, 666);
        actionOpen = new QAction(MoveitClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/icon_open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionAxes = new QAction(MoveitClass);
        actionAxes->setObjectName(QStringLiteral("actionAxes"));
        actionInterpolated_points = new QAction(MoveitClass);
        actionInterpolated_points->setObjectName(QStringLiteral("actionInterpolated_points"));
        actionControl_points = new QAction(MoveitClass);
        actionControl_points->setObjectName(QStringLiteral("actionControl_points"));
        actionNormals = new QAction(MoveitClass);
        actionNormals->setObjectName(QStringLiteral("actionNormals"));
        actionAnnotations = new QAction(MoveitClass);
        actionAnnotations->setObjectName(QStringLiteral("actionAnnotations"));
        actionDrag_trajectory_point = new QAction(MoveitClass);
        actionDrag_trajectory_point->setObjectName(QStringLiteral("actionDrag_trajectory_point"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/icon_drag_trajectory_point.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag_trajectory_point->setIcon(icon1);
        actionDry_run = new QAction(MoveitClass);
        actionDry_run->setObjectName(QStringLiteral("actionDry_run"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("Resources/icon_dryrun.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDry_run->setIcon(icon2);
        actionReset_all_views = new QAction(MoveitClass);
        actionReset_all_views->setObjectName(QStringLiteral("actionReset_all_views"));
        QIcon icon3;
        icon3.addFile(QStringLiteral("Resources/icon_reset_all_views.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset_all_views->setIcon(icon3);
        actionExit = new QAction(MoveitClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon4;
        icon4.addFile(QStringLiteral("Resources/icon_exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon4);
        actionSave_As = new QAction(MoveitClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon5;
        icon5.addFile(QStringLiteral("Resources/icon_save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon5);
        actionClose = new QAction(MoveitClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon6;
        icon6.addFile(QStringLiteral("Resources/icon_close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon6);
        centralWidget = new QWidget(MoveitClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setMinimumSize(QSize(200, 0));
        frame->setMaximumSize(QSize(200, 16777215));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_3->addWidget(label);

        myStatusTableWidget = new QTableWidget(frame);
        myStatusTableWidget->setObjectName(QStringLiteral("myStatusTableWidget"));

        verticalLayout_3->addWidget(myStatusTableWidget);


        horizontalLayout->addWidget(frame);

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        frame_2 = new QFrame(splitter);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setMinimumSize(QSize(400, 0));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frame_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        frame_4 = new QFrame(frame_2);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setMaximumSize(QSize(16777215, 40));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_4);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(frame_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label_3 = new QLabel(frame_4);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        lineEdit = new QLineEdit(frame_4);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout_4->addWidget(frame_4);

        myGLWidget = new MyGLWidget(frame_2);
        myGLWidget->setObjectName(QStringLiteral("myGLWidget"));

        verticalLayout_4->addWidget(myGLWidget);

        splitter->addWidget(frame_2);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout = new QVBoxLayout(tab);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        fileTextEdit = new QTextEdit(tab);
        fileTextEdit->setObjectName(QStringLiteral("fileTextEdit"));

        verticalLayout->addWidget(fileTextEdit);

        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_5 = new QVBoxLayout(tab_3);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        jsonTreeView = new QTreeView(tab_3);
        jsonTreeView->setObjectName(QStringLiteral("jsonTreeView"));

        verticalLayout_5->addWidget(jsonTreeView);

        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_2 = new QVBoxLayout(tab_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        myGLGeneralTrajectoryCurveViewX = new MyGLGeneralTrajectoryCurveView(tab_2);
        myGLGeneralTrajectoryCurveViewX->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewX"));

        verticalLayout_2->addWidget(myGLGeneralTrajectoryCurveViewX);

        myGLGeneralTrajectoryCurveViewY = new MyGLGeneralTrajectoryCurveView(tab_2);
        myGLGeneralTrajectoryCurveViewY->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewY"));

        verticalLayout_2->addWidget(myGLGeneralTrajectoryCurveViewY);

        myGLGeneralTrajectoryCurveViewZ = new MyGLGeneralTrajectoryCurveView(tab_2);
        myGLGeneralTrajectoryCurveViewZ->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewZ"));

        verticalLayout_2->addWidget(myGLGeneralTrajectoryCurveViewZ);

        tabWidget->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        verticalLayout_6 = new QVBoxLayout(tab_4);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        myGLGeneralTrajectoryCurveViewR = new MyGLGeneralTrajectoryCurveView(tab_4);
        myGLGeneralTrajectoryCurveViewR->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewR"));

        verticalLayout_6->addWidget(myGLGeneralTrajectoryCurveViewR);

        myGLGeneralTrajectoryCurveViewP = new MyGLGeneralTrajectoryCurveView(tab_4);
        myGLGeneralTrajectoryCurveViewP->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewP"));

        verticalLayout_6->addWidget(myGLGeneralTrajectoryCurveViewP);

        myGLGeneralTrajectoryCurveViewB = new MyGLGeneralTrajectoryCurveView(tab_4);
        myGLGeneralTrajectoryCurveViewB->setObjectName(QStringLiteral("myGLGeneralTrajectoryCurveViewB"));

        verticalLayout_6->addWidget(myGLGeneralTrajectoryCurveViewB);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        verticalLayout_7 = new QVBoxLayout(tab_5);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        mySpreadsheet = new QTableWidget(tab_5);
        mySpreadsheet->setObjectName(QStringLiteral("mySpreadsheet"));

        verticalLayout_7->addWidget(mySpreadsheet);

        tabWidget->addTab(tab_5, QString());
        splitter->addWidget(tabWidget);

        horizontalLayout->addWidget(splitter);

        MoveitClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MoveitClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 926, 18));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuTest = new QMenu(menuBar);
        menuTest->setObjectName(QStringLiteral("menuTest"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuMode = new QMenu(menuBar);
        menuMode->setObjectName(QStringLiteral("menuMode"));
        menuSimulation = new QMenu(menuBar);
        menuSimulation->setObjectName(QStringLiteral("menuSimulation"));
        MoveitClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MoveitClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MoveitClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MoveitClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MoveitClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTest->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuBar->addAction(menuMode->menuAction());
        menuBar->addAction(menuSimulation->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionExit);
        menuFile->addAction(actionSave_As);
        menuFile->addAction(actionClose);
        menuTest->addAction(actionAxes);
        menuTest->addAction(actionControl_points);
        menuTest->addAction(actionInterpolated_points);
        menuTest->addAction(actionNormals);
        menuTest->addAction(actionAnnotations);
        menuTest->addSeparator();
        menuTest->addAction(actionReset_all_views);
        menuMode->addAction(actionDrag_trajectory_point);
        menuSimulation->addAction(actionDry_run);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSave_As);
        mainToolBar->addAction(actionClose);
        mainToolBar->addAction(actionExit);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDry_run);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDrag_trajectory_point);
        mainToolBar->addAction(actionReset_all_views);

        retranslateUi(MoveitClass);

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(MoveitClass);
    } // setupUi

    void retranslateUi(QMainWindow *MoveitClass)
    {
        MoveitClass->setWindowTitle(QApplication::translate("MoveitClass", "Moveit", 0));
        actionOpen->setText(QApplication::translate("MoveitClass", "Open", 0));
        actionAxes->setText(QApplication::translate("MoveitClass", "Axes", 0));
        actionInterpolated_points->setText(QApplication::translate("MoveitClass", "Interpolated points", 0));
        actionControl_points->setText(QApplication::translate("MoveitClass", "Control points", 0));
        actionNormals->setText(QApplication::translate("MoveitClass", "Normals", 0));
        actionAnnotations->setText(QApplication::translate("MoveitClass", "Annotations", 0));
        actionDrag_trajectory_point->setText(QApplication::translate("MoveitClass", "Drag trajectory point", 0));
        actionDry_run->setText(QApplication::translate("MoveitClass", "Dry run", 0));
        actionReset_all_views->setText(QApplication::translate("MoveitClass", "Reset all views", 0));
        actionExit->setText(QApplication::translate("MoveitClass", "Exit", 0));
        actionSave_As->setText(QApplication::translate("MoveitClass", "Save As ...", 0));
        actionClose->setText(QApplication::translate("MoveitClass", "Close", 0));
        label->setText(QApplication::translate("MoveitClass", "Status window", 0));
        label_2->setText(QApplication::translate("MoveitClass", "3D view", 0));
        label_3->setText(QApplication::translate("MoveitClass", "Data:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MoveitClass", "Data file", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MoveitClass", "JSON view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MoveitClass", "Translations", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MoveitClass", "Rotations", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MoveitClass", "Spreadsheet", 0));
        menuFile->setTitle(QApplication::translate("MoveitClass", "File", 0));
        menuTest->setTitle(QApplication::translate("MoveitClass", "View", 0));
        menuHelp->setTitle(QApplication::translate("MoveitClass", "Help", 0));
        menuMode->setTitle(QApplication::translate("MoveitClass", "Mode", 0));
        menuSimulation->setTitle(QApplication::translate("MoveitClass", "Simulation", 0));
    } // retranslateUi

};

namespace Ui {
    class MoveitClass: public Ui_MoveitClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOVEIT_H
