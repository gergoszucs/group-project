/********************************************************************************
** Form generated from reading UI file 'surfit.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SURFIT_H
#define UI_SURFIT_H

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
#include <QtWidgets/QPushButton>
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
#include <myxzview.h>
#include "mygaussianview.h"
#include "myglwidget.h"
#include "myxyview.h"
#include "myyzview.h"

QT_BEGIN_NAMESPACE

class Ui_SurfitClass
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave_As;
    QAction *actionExit;
    QAction *actionDrag;
    QAction *actionNew_surface;
    QAction *actionDrag_all;
    QAction *actionNormals;
    QAction *actionDrag_row;
    QAction *actionDrag_col;
    QAction *actionInterpolated_points;
    QAction *actionControl_points;
    QAction *actionRotate_all;
    QAction *actionResize_all;
    QAction *actionShear;
    QAction *actionPerspective;
    QAction *actionPlayout_Test;
    QAction *actionFlip_horizontal;
    QAction *actionClose;
    QAction *actionCopy_surface;
    QAction *actionDelete_surface;
    QAction *actionAnnotations;
    QAction *actionInsert_row;
    QAction *actionDelete_row;
    QAction *actionDuplicate_row;
    QAction *actionInsert_col;
    QAction *actionDelete_col;
    QAction *actionDuplicate_col;
    QAction *actionMate_points;
    QAction *actionCopy_surface_mirror;
    QAction *actionMerge_surfaces_by_row;
    QAction *actionReset_all_views;
    QAction *actionWeb_help;
    QAction *actionAbout;
    QAction *actionGaussian_curvature;
    QAction *actionInterpolated_points_density_U;
    QAction *actionInterpolated_points_density_V;
    QAction *actionGrids;
    QAction *actionMerge_surfaces_by_row_reverse;
    QAction *actionMeasure_distance;
    QAction *actionCentred_rotate;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTableWidget *myStatusTableWidget;
    QSplitter *splitter;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *myEditTextDataField;
    MyGLWidget *myGLWidget;
    QTabWidget *tabWidget;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_5;
    QTextEdit *fileTextEdit;
    QWidget *tab_5;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_6;
    QTreeView *jsonTreeView;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *collapseAllButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *expandAllButton;
    QWidget *tab;
    QVBoxLayout *verticalLayout_4;
    MyXYView *myXYView;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_3;
    MyXZView *myXZView;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_3;
    MyYZView *myYZView;
    QWidget *tab_7;
    QVBoxLayout *verticalLayout_8;
    MyGaussianView *myGaussianView;
    QWidget *tab_6;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *mySpreadsheet;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuEdit;
    QMenu *menuSurface;
    QMenu *menuTest;
    QMenu *menuHelp;
    QMenu *menuAnalysis;
    QMenu *menuEdit_2;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SurfitClass)
    {
        if (SurfitClass->objectName().isEmpty())
            SurfitClass->setObjectName(QStringLiteral("SurfitClass"));
        SurfitClass->resize(1129, 806);
        actionNew = new QAction(SurfitClass);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        actionOpen = new QAction(SurfitClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral("Resources/icon_open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionSave_As = new QAction(SurfitClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon1;
        icon1.addFile(QStringLiteral("Resources/icon_save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon1);
        actionExit = new QAction(SurfitClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral("Resources/icon_exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionDrag = new QAction(SurfitClass);
        actionDrag->setObjectName(QStringLiteral("actionDrag"));
        QIcon icon3;
        icon3.addFile(QStringLiteral("Resources/icon_drag.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag->setIcon(icon3);
        actionNew_surface = new QAction(SurfitClass);
        actionNew_surface->setObjectName(QStringLiteral("actionNew_surface"));
        QIcon icon4;
        icon4.addFile(QStringLiteral("Resources/icon_new_surface.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_surface->setIcon(icon4);
        actionDrag_all = new QAction(SurfitClass);
        actionDrag_all->setObjectName(QStringLiteral("actionDrag_all"));
        QIcon icon5;
        icon5.addFile(QStringLiteral("Resources/icon_drag_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag_all->setIcon(icon5);
        actionNormals = new QAction(SurfitClass);
        actionNormals->setObjectName(QStringLiteral("actionNormals"));
        actionDrag_row = new QAction(SurfitClass);
        actionDrag_row->setObjectName(QStringLiteral("actionDrag_row"));
        QIcon icon6;
        icon6.addFile(QStringLiteral("Resources/icon_drag_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag_row->setIcon(icon6);
        actionDrag_col = new QAction(SurfitClass);
        actionDrag_col->setObjectName(QStringLiteral("actionDrag_col"));
        QIcon icon7;
        icon7.addFile(QStringLiteral("Resources/icon_drag_col.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag_col->setIcon(icon7);
        actionInterpolated_points = new QAction(SurfitClass);
        actionInterpolated_points->setObjectName(QStringLiteral("actionInterpolated_points"));
        actionControl_points = new QAction(SurfitClass);
        actionControl_points->setObjectName(QStringLiteral("actionControl_points"));
        actionRotate_all = new QAction(SurfitClass);
        actionRotate_all->setObjectName(QStringLiteral("actionRotate_all"));
        QIcon icon8;
        icon8.addFile(QStringLiteral("Resources/icon_rotate_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRotate_all->setIcon(icon8);
        actionResize_all = new QAction(SurfitClass);
        actionResize_all->setObjectName(QStringLiteral("actionResize_all"));
        QIcon icon9;
        icon9.addFile(QStringLiteral("Resources/icon_resize_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionResize_all->setIcon(icon9);
        actionShear = new QAction(SurfitClass);
        actionShear->setObjectName(QStringLiteral("actionShear"));
        QIcon icon10;
        icon10.addFile(QStringLiteral("Resources/icon_shear.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShear->setIcon(icon10);
        actionPerspective = new QAction(SurfitClass);
        actionPerspective->setObjectName(QStringLiteral("actionPerspective"));
        QIcon icon11;
        icon11.addFile(QStringLiteral("Resources/icon_perspective_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPerspective->setIcon(icon11);
        actionPlayout_Test = new QAction(SurfitClass);
        actionPlayout_Test->setObjectName(QStringLiteral("actionPlayout_Test"));
        QIcon icon12;
        icon12.addFile(QStringLiteral("Resources/icon_test.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPlayout_Test->setIcon(icon12);
        actionFlip_horizontal = new QAction(SurfitClass);
        actionFlip_horizontal->setObjectName(QStringLiteral("actionFlip_horizontal"));
        QIcon icon13;
        icon13.addFile(QStringLiteral("Resources/icon_flip_horizontal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFlip_horizontal->setIcon(icon13);
        actionClose = new QAction(SurfitClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon14;
        icon14.addFile(QStringLiteral("Resources/icon_close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon14);
        actionCopy_surface = new QAction(SurfitClass);
        actionCopy_surface->setObjectName(QStringLiteral("actionCopy_surface"));
        QIcon icon15;
        icon15.addFile(QStringLiteral("Resources/icon_copy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy_surface->setIcon(icon15);
        actionDelete_surface = new QAction(SurfitClass);
        actionDelete_surface->setObjectName(QStringLiteral("actionDelete_surface"));
        QIcon icon16;
        icon16.addFile(QStringLiteral("Resources/icon_delete_surface.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_surface->setIcon(icon16);
        actionAnnotations = new QAction(SurfitClass);
        actionAnnotations->setObjectName(QStringLiteral("actionAnnotations"));
        actionInsert_row = new QAction(SurfitClass);
        actionInsert_row->setObjectName(QStringLiteral("actionInsert_row"));
        QIcon icon17;
        icon17.addFile(QStringLiteral("Resources/icon_insert_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionInsert_row->setIcon(icon17);
        actionDelete_row = new QAction(SurfitClass);
        actionDelete_row->setObjectName(QStringLiteral("actionDelete_row"));
        QIcon icon18;
        icon18.addFile(QStringLiteral("Resources/icon_delete_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_row->setIcon(icon18);
        actionDuplicate_row = new QAction(SurfitClass);
        actionDuplicate_row->setObjectName(QStringLiteral("actionDuplicate_row"));
        QIcon icon19;
        icon19.addFile(QStringLiteral("Resources/icon_duplicate_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDuplicate_row->setIcon(icon19);
        actionInsert_col = new QAction(SurfitClass);
        actionInsert_col->setObjectName(QStringLiteral("actionInsert_col"));
        QIcon icon20;
        icon20.addFile(QStringLiteral("Resources/icon_insert_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionInsert_col->setIcon(icon20);
        actionDelete_col = new QAction(SurfitClass);
        actionDelete_col->setObjectName(QStringLiteral("actionDelete_col"));
        QIcon icon21;
        icon21.addFile(QStringLiteral("Resources/icon_delete_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_col->setIcon(icon21);
        actionDuplicate_col = new QAction(SurfitClass);
        actionDuplicate_col->setObjectName(QStringLiteral("actionDuplicate_col"));
        QIcon icon22;
        icon22.addFile(QStringLiteral("Resources/icon_duplicate_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDuplicate_col->setIcon(icon22);
        actionMate_points = new QAction(SurfitClass);
        actionMate_points->setObjectName(QStringLiteral("actionMate_points"));
        QIcon icon23;
        icon23.addFile(QStringLiteral("Resources/icon_mate_points.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMate_points->setIcon(icon23);
        actionCopy_surface_mirror = new QAction(SurfitClass);
        actionCopy_surface_mirror->setObjectName(QStringLiteral("actionCopy_surface_mirror"));
        QIcon icon24;
        icon24.addFile(QStringLiteral("Resources/icon_copy_mirror.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy_surface_mirror->setIcon(icon24);
        actionMerge_surfaces_by_row = new QAction(SurfitClass);
        actionMerge_surfaces_by_row->setObjectName(QStringLiteral("actionMerge_surfaces_by_row"));
        QIcon icon25;
        icon25.addFile(QStringLiteral("Resources/icon_merge_surfaces.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMerge_surfaces_by_row->setIcon(icon25);
        actionReset_all_views = new QAction(SurfitClass);
        actionReset_all_views->setObjectName(QStringLiteral("actionReset_all_views"));
        QIcon icon26;
        icon26.addFile(QStringLiteral("Resources/icon_reset_all_views.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset_all_views->setIcon(icon26);
        actionWeb_help = new QAction(SurfitClass);
        actionWeb_help->setObjectName(QStringLiteral("actionWeb_help"));
        QIcon icon27;
        icon27.addFile(QStringLiteral("Resources/icon_help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWeb_help->setIcon(icon27);
        actionAbout = new QAction(SurfitClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionGaussian_curvature = new QAction(SurfitClass);
        actionGaussian_curvature->setObjectName(QStringLiteral("actionGaussian_curvature"));
        QIcon icon28;
        icon28.addFile(QStringLiteral("Resources/icon_gaussian_curvature.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGaussian_curvature->setIcon(icon28);
        actionInterpolated_points_density_U = new QAction(SurfitClass);
        actionInterpolated_points_density_U->setObjectName(QStringLiteral("actionInterpolated_points_density_U"));
        actionInterpolated_points_density_V = new QAction(SurfitClass);
        actionInterpolated_points_density_V->setObjectName(QStringLiteral("actionInterpolated_points_density_V"));
        actionGrids = new QAction(SurfitClass);
        actionGrids->setObjectName(QStringLiteral("actionGrids"));
        actionMerge_surfaces_by_row_reverse = new QAction(SurfitClass);
        actionMerge_surfaces_by_row_reverse->setObjectName(QStringLiteral("actionMerge_surfaces_by_row_reverse"));
        QIcon icon29;
        icon29.addFile(QStringLiteral("Resources/icon_merge_surfaces_reverse.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMerge_surfaces_by_row_reverse->setIcon(icon29);
        actionMeasure_distance = new QAction(SurfitClass);
        actionMeasure_distance->setObjectName(QStringLiteral("actionMeasure_distance"));
        QIcon icon30;
        icon30.addFile(QStringLiteral("Resources/icon_measure_distance.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMeasure_distance->setIcon(icon30);
        actionCentred_rotate = new QAction(SurfitClass);
        actionCentred_rotate->setObjectName(QStringLiteral("actionCentred_rotate"));
        QIcon icon31;
        icon31.addFile(QStringLiteral("Resources/icon_centred_rotate.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCentred_rotate->setIcon(icon31);
        centralWidget = new QWidget(SurfitClass);
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

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        frame_2 = new QFrame(splitter);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setMinimumSize(QSize(400, 0));
        frame_2->setMaximumSize(QSize(100000, 16777215));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setMaximumSize(QSize(16777215, 40));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_3);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(frame_3);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(16777215, 20));

        horizontalLayout_5->addWidget(label_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        myEditTextDataField = new QLineEdit(frame_3);
        myEditTextDataField->setObjectName(QStringLiteral("myEditTextDataField"));
        myEditTextDataField->setMinimumSize(QSize(400, 20));

        horizontalLayout_5->addWidget(myEditTextDataField);


        verticalLayout_2->addWidget(frame_3);

        myGLWidget = new MyGLWidget(frame_2);
        myGLWidget->setObjectName(QStringLiteral("myGLWidget"));

        verticalLayout_2->addWidget(myGLWidget);

        splitter->addWidget(frame_2);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMaximumSize(QSize(10000000, 16777215));
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        verticalLayout_5 = new QVBoxLayout(tab_4);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        fileTextEdit = new QTextEdit(tab_4);
        fileTextEdit->setObjectName(QStringLiteral("fileTextEdit"));

        verticalLayout_5->addWidget(fileTextEdit);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        horizontalLayout_2 = new QHBoxLayout(tab_5);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        jsonTreeView = new QTreeView(tab_5);
        jsonTreeView->setObjectName(QStringLiteral("jsonTreeView"));

        verticalLayout_6->addWidget(jsonTreeView);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        collapseAllButton = new QPushButton(tab_5);
        collapseAllButton->setObjectName(QStringLiteral("collapseAllButton"));

        horizontalLayout_4->addWidget(collapseAllButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        expandAllButton = new QPushButton(tab_5);
        expandAllButton->setObjectName(QStringLiteral("expandAllButton"));

        horizontalLayout_4->addWidget(expandAllButton);


        verticalLayout_6->addLayout(horizontalLayout_4);


        horizontalLayout_2->addLayout(verticalLayout_6);

        tabWidget->addTab(tab_5, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_4 = new QVBoxLayout(tab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        myXYView = new MyXYView(tab);
        myXYView->setObjectName(QStringLiteral("myXYView"));

        verticalLayout_4->addWidget(myXYView);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        horizontalLayout_3 = new QHBoxLayout(tab_2);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        myXZView = new MyXZView(tab_2);
        myXZView->setObjectName(QStringLiteral("myXZView"));

        horizontalLayout_3->addWidget(myXZView);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_3 = new QVBoxLayout(tab_3);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        myYZView = new MyYZView(tab_3);
        myYZView->setObjectName(QStringLiteral("myYZView"));

        verticalLayout_3->addWidget(myYZView);

        tabWidget->addTab(tab_3, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QStringLiteral("tab_7"));
        verticalLayout_8 = new QVBoxLayout(tab_7);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        myGaussianView = new MyGaussianView(tab_7);
        myGaussianView->setObjectName(QStringLiteral("myGaussianView"));

        verticalLayout_8->addWidget(myGaussianView);

        tabWidget->addTab(tab_7, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        verticalLayout_7 = new QVBoxLayout(tab_6);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        mySpreadsheet = new QTableWidget(tab_6);
        mySpreadsheet->setObjectName(QStringLiteral("mySpreadsheet"));

        verticalLayout_7->addWidget(mySpreadsheet);

        tabWidget->addTab(tab_6, QString());
        splitter->addWidget(tabWidget);

        horizontalLayout->addWidget(splitter);

        SurfitClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(SurfitClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1129, 18));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuSurface = new QMenu(menuBar);
        menuSurface->setObjectName(QStringLiteral("menuSurface"));
        menuTest = new QMenu(menuBar);
        menuTest->setObjectName(QStringLiteral("menuTest"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuAnalysis = new QMenu(menuBar);
        menuAnalysis->setObjectName(QStringLiteral("menuAnalysis"));
        menuEdit_2 = new QMenu(menuBar);
        menuEdit_2->setObjectName(QStringLiteral("menuEdit_2"));
        SurfitClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SurfitClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        SurfitClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(SurfitClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        SurfitClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit_2->menuAction());
        menuBar->addAction(menuSurface->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuTest->menuAction());
        menuBar->addAction(menuAnalysis->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave_As);
        menuFile->addAction(actionExit);
        menuFile->addAction(actionClose);
        menuView->addAction(actionNormals);
        menuView->addAction(actionInterpolated_points);
        menuView->addAction(actionControl_points);
        menuView->addAction(actionAnnotations);
        menuView->addAction(actionGrids);
        menuView->addAction(actionReset_all_views);
        menuEdit->addAction(actionDrag);
        menuEdit->addAction(actionCentred_rotate);
        menuEdit->addAction(actionDrag_row);
        menuEdit->addAction(actionDrag_col);
        menuEdit->addAction(actionDrag_all);
        menuEdit->addSeparator();
        menuEdit->addAction(actionRotate_all);
        menuEdit->addAction(actionResize_all);
        menuEdit->addSeparator();
        menuEdit->addAction(actionShear);
        menuEdit->addAction(actionPerspective);
        menuEdit->addAction(actionFlip_horizontal);
        menuEdit->addAction(actionCopy_surface);
        menuEdit->addAction(actionCopy_surface_mirror);
        menuEdit->addSeparator();
        menuEdit->addAction(actionInsert_row);
        menuEdit->addAction(actionDelete_row);
        menuEdit->addAction(actionDuplicate_row);
        menuEdit->addSeparator();
        menuEdit->addAction(actionInsert_col);
        menuEdit->addAction(actionDelete_col);
        menuEdit->addAction(actionDuplicate_col);
        menuEdit->addAction(actionMate_points);
        menuEdit->addAction(actionMerge_surfaces_by_row);
        menuEdit->addAction(actionMerge_surfaces_by_row_reverse);
        menuEdit->addAction(actionMeasure_distance);
        menuSurface->addAction(actionNew_surface);
        menuSurface->addAction(actionDelete_surface);
        menuTest->addAction(actionPlayout_Test);
        menuHelp->addAction(actionWeb_help);
        menuHelp->addAction(actionAbout);
        menuAnalysis->addAction(actionGaussian_curvature);
        menuEdit_2->addAction(actionInterpolated_points_density_U);
        menuEdit_2->addAction(actionInterpolated_points_density_V);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionSave_As);
        mainToolBar->addAction(actionClose);
        mainToolBar->addAction(actionExit);
        mainToolBar->addAction(actionReset_all_views);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionNew_surface);
        mainToolBar->addAction(actionDelete_surface);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDrag);
        mainToolBar->addAction(actionCentred_rotate);
        mainToolBar->addAction(actionDrag_row);
        mainToolBar->addAction(actionDrag_col);
        mainToolBar->addAction(actionDrag_all);
        mainToolBar->addAction(actionResize_all);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionRotate_all);
        mainToolBar->addAction(actionShear);
        mainToolBar->addAction(actionPerspective);
        mainToolBar->addAction(actionFlip_horizontal);
        mainToolBar->addAction(actionCopy_surface);
        mainToolBar->addAction(actionCopy_surface_mirror);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionInsert_row);
        mainToolBar->addAction(actionDelete_row);
        mainToolBar->addAction(actionDuplicate_row);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionInsert_col);
        mainToolBar->addAction(actionDelete_col);
        mainToolBar->addAction(actionDuplicate_col);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionMate_points);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionGaussian_curvature);
        mainToolBar->addAction(actionMerge_surfaces_by_row);
        mainToolBar->addAction(actionMerge_surfaces_by_row_reverse);
        mainToolBar->addAction(actionMeasure_distance);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionPlayout_Test);
        mainToolBar->addAction(actionWeb_help);

        retranslateUi(SurfitClass);

        tabWidget->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(SurfitClass);
    } // setupUi

    void retranslateUi(QMainWindow *SurfitClass)
    {
        SurfitClass->setWindowTitle(QApplication::translate("SurfitClass", "Surfit", 0));
        actionNew->setText(QApplication::translate("SurfitClass", "New...", 0));
        actionOpen->setText(QApplication::translate("SurfitClass", "Open...", 0));
        actionSave_As->setText(QApplication::translate("SurfitClass", "Save As ...", 0));
        actionExit->setText(QApplication::translate("SurfitClass", "Exit", 0));
        actionDrag->setText(QApplication::translate("SurfitClass", "Drag", 0));
        actionNew_surface->setText(QApplication::translate("SurfitClass", "New surface", 0));
        actionDrag_all->setText(QApplication::translate("SurfitClass", "Drag all", 0));
        actionNormals->setText(QApplication::translate("SurfitClass", "Normals", 0));
        actionDrag_row->setText(QApplication::translate("SurfitClass", "Drag row", 0));
        actionDrag_col->setText(QApplication::translate("SurfitClass", "Drag col", 0));
        actionInterpolated_points->setText(QApplication::translate("SurfitClass", "Interpolated points", 0));
        actionControl_points->setText(QApplication::translate("SurfitClass", "Control points", 0));
        actionRotate_all->setText(QApplication::translate("SurfitClass", "Rotate all", 0));
        actionResize_all->setText(QApplication::translate("SurfitClass", "Resize all", 0));
        actionShear->setText(QApplication::translate("SurfitClass", "Shear", 0));
        actionPerspective->setText(QApplication::translate("SurfitClass", "Perspective", 0));
        actionPlayout_Test->setText(QApplication::translate("SurfitClass", "Playout Test", 0));
        actionFlip_horizontal->setText(QApplication::translate("SurfitClass", "Flip all horizontal", 0));
        actionClose->setText(QApplication::translate("SurfitClass", "Close", 0));
        actionCopy_surface->setText(QApplication::translate("SurfitClass", "Copy surface", 0));
        actionDelete_surface->setText(QApplication::translate("SurfitClass", "Delete surface", 0));
        actionAnnotations->setText(QApplication::translate("SurfitClass", "Annotations", 0));
        actionInsert_row->setText(QApplication::translate("SurfitClass", "Insert row", 0));
        actionDelete_row->setText(QApplication::translate("SurfitClass", "Delete row", 0));
        actionDuplicate_row->setText(QApplication::translate("SurfitClass", "Duplicate row", 0));
        actionInsert_col->setText(QApplication::translate("SurfitClass", "Insert col", 0));
        actionDelete_col->setText(QApplication::translate("SurfitClass", "Delete col", 0));
        actionDuplicate_col->setText(QApplication::translate("SurfitClass", "Duplicate col", 0));
        actionMate_points->setText(QApplication::translate("SurfitClass", "Mate points", 0));
#ifndef QT_NO_TOOLTIP
        actionMate_points->setToolTip(QApplication::translate("SurfitClass", "Mate points (master first)", 0));
#endif // QT_NO_TOOLTIP
        actionCopy_surface_mirror->setText(QApplication::translate("SurfitClass", "Copy surface mirror", 0));
        actionMerge_surfaces_by_row->setText(QApplication::translate("SurfitClass", "Merge surfaces by row", 0));
        actionReset_all_views->setText(QApplication::translate("SurfitClass", "Reset all views", 0));
        actionWeb_help->setText(QApplication::translate("SurfitClass", "Web help ...", 0));
        actionAbout->setText(QApplication::translate("SurfitClass", "About", 0));
        actionGaussian_curvature->setText(QApplication::translate("SurfitClass", "Gaussian curvature", 0));
        actionInterpolated_points_density_U->setText(QApplication::translate("SurfitClass", "Interpolated points density U", 0));
        actionInterpolated_points_density_V->setText(QApplication::translate("SurfitClass", "Interpolated points density V", 0));
        actionGrids->setText(QApplication::translate("SurfitClass", "Grids", 0));
        actionMerge_surfaces_by_row_reverse->setText(QApplication::translate("SurfitClass", "Merge surfaces by row reverse", 0));
        actionMeasure_distance->setText(QApplication::translate("SurfitClass", "Measure distance", 0));
        actionCentred_rotate->setText(QApplication::translate("SurfitClass", "Centred rotate", 0));
        label->setText(QApplication::translate("SurfitClass", "Status window", 0));
        label_2->setText(QApplication::translate("SurfitClass", "3D view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("SurfitClass", "Data file", 0));
        collapseAllButton->setText(QApplication::translate("SurfitClass", "Collapse All", 0));
        expandAllButton->setText(QApplication::translate("SurfitClass", "Expand All", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("SurfitClass", "JSON view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SurfitClass", "XY view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SurfitClass", "XZ view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("SurfitClass", "YZ view", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QApplication::translate("SurfitClass", "Gaussian Curvature", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("SurfitClass", "Spreadsheet", 0));
        menuFile->setTitle(QApplication::translate("SurfitClass", "File", 0));
        menuView->setTitle(QApplication::translate("SurfitClass", "View", 0));
        menuEdit->setTitle(QApplication::translate("SurfitClass", "Mode", 0));
        menuSurface->setTitle(QApplication::translate("SurfitClass", "Surface", 0));
        menuTest->setTitle(QApplication::translate("SurfitClass", "Test", 0));
        menuHelp->setTitle(QApplication::translate("SurfitClass", "Help", 0));
        menuAnalysis->setTitle(QApplication::translate("SurfitClass", "Analysis", 0));
        menuEdit_2->setTitle(QApplication::translate("SurfitClass", "Edit", 0));
    } // retranslateUi

};

namespace Ui {
    class SurfitClass: public Ui_SurfitClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SURFIT_H
