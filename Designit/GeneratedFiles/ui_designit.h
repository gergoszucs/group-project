/********************************************************************************
** Form generated from reading UI file 'designit.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DESIGNIT_H
#define UI_DESIGNIT_H

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
#include <QtWidgets/QToolBox>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <view2d.h>
#include "mygaussianview.h"
#include "myglgeneraltrajectorycurveview.h"
#include "myglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_DesignitClass
{
public:
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave_As;
    QAction *actionExit;
    QAction *actionDrag;
    QAction *actionNew_surface;
    QAction *actionNormals;
    QAction *actionInterpolated_points;
    QAction *actionControl_points;
    QAction *actionRotate;
    QAction *actionResize;
    QAction *actionShear;
    QAction *actionPlayout_Test;
    QAction *actionFlip;
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
    QAction *action_Flexit;
    QAction *actionUndo;
    QAction *actionRedo;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QSplitter *splitter;
    QTabWidget *tabsAdditionalData;
    QWidget *tab_statusWindow;
    QVBoxLayout *verticalLayout;
    QTableWidget *myStatusTableWidget;
    QLineEdit *commandLine;
    QWidget *tab_viewJSON;
    QVBoxLayout *verticalLayout_9;
    QTreeView *jsonTreeView;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *collapseAllButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *expandAllButton;
    QWidget *tab_dataFile;
    QVBoxLayout *verticalLayout_5;
    QTextEdit *fileTextEdit;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *myEditTextDataField;
    MyGLWidget *myGLWidget;
    QToolBox *editingTools;
    QWidget *SurfaceTools;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabsWorkSurface;
    QWidget *tab_XYView;
    QVBoxLayout *verticalLayout_4;
    View2d *myXYView;
    QWidget *tab_XZView;
    QHBoxLayout *horizontalLayout_3;
    View2d *myXZView;
    QWidget *tab_YZView;
    QVBoxLayout *verticalLayout_3;
    View2d *myYZView;
    QWidget *tab_gaussianView;
    QVBoxLayout *verticalLayout_8;
    MyGaussianView *myGaussianView;
    QWidget *tab_spreadsheet;
    QVBoxLayout *verticalLayout_7;
    QTableWidget *mySpreadsheet;
    QWidget *TrajectoryTools;
    QHBoxLayout *horizontalLayout_7;
    QTabWidget *tabsWorkTrajectory;
    QWidget *tab_trajectoryTranslations;
    QVBoxLayout *verticalLayout_6;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveX;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveY;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveZ;
    QWidget *tab_trajectoryRotations;
    QVBoxLayout *verticalLayout_19;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveR;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveP;
    MyGLGeneralTrajectoryCurveView *trajectoryCurveB;
    QWidget *tab_trajectorySpreadsheet;
    QVBoxLayout *verticalLayout_20;
    QTableWidget *trajectorySpreadsheet;
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

    void setupUi(QMainWindow *DesignitClass)
    {
        if (DesignitClass->objectName().isEmpty())
            DesignitClass->setObjectName(QStringLiteral("DesignitClass"));
        DesignitClass->resize(1129, 806);
        actionNew = new QAction(DesignitClass);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        actionOpen = new QAction(DesignitClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/icon_open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionSave_As = new QAction(DesignitClass);
        actionSave_As->setObjectName(QStringLiteral("actionSave_As"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/icon_save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_As->setIcon(icon1);
        actionExit = new QAction(DesignitClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Resources/icon_exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionDrag = new QAction(DesignitClass);
        actionDrag->setObjectName(QStringLiteral("actionDrag"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Resources/icon_drag.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDrag->setIcon(icon3);
        actionNew_surface = new QAction(DesignitClass);
        actionNew_surface->setObjectName(QStringLiteral("actionNew_surface"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Resources/icon_new_surface.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_surface->setIcon(icon4);
        actionNormals = new QAction(DesignitClass);
        actionNormals->setObjectName(QStringLiteral("actionNormals"));
        actionInterpolated_points = new QAction(DesignitClass);
        actionInterpolated_points->setObjectName(QStringLiteral("actionInterpolated_points"));
        actionControl_points = new QAction(DesignitClass);
        actionControl_points->setObjectName(QStringLiteral("actionControl_points"));
        actionRotate = new QAction(DesignitClass);
        actionRotate->setObjectName(QStringLiteral("actionRotate"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/Resources/icon_rotate_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRotate->setIcon(icon5);
        actionResize = new QAction(DesignitClass);
        actionResize->setObjectName(QStringLiteral("actionResize"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/Resources/icon_resize_all.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionResize->setIcon(icon6);
        actionShear = new QAction(DesignitClass);
        actionShear->setObjectName(QStringLiteral("actionShear"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/Resources/icon_shear.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShear->setIcon(icon7);
        actionPlayout_Test = new QAction(DesignitClass);
        actionPlayout_Test->setObjectName(QStringLiteral("actionPlayout_Test"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/Resources/icon_test.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPlayout_Test->setIcon(icon8);
        actionFlip = new QAction(DesignitClass);
        actionFlip->setObjectName(QStringLiteral("actionFlip"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/Resources/icon_flip_horizontal.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFlip->setIcon(icon9);
        actionClose = new QAction(DesignitClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/Resources/icon_close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon10);
        actionCopy_surface = new QAction(DesignitClass);
        actionCopy_surface->setObjectName(QStringLiteral("actionCopy_surface"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/Resources/icon_copy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy_surface->setIcon(icon11);
        actionDelete_surface = new QAction(DesignitClass);
        actionDelete_surface->setObjectName(QStringLiteral("actionDelete_surface"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/Resources/icon_delete_surface.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_surface->setIcon(icon12);
        actionAnnotations = new QAction(DesignitClass);
        actionAnnotations->setObjectName(QStringLiteral("actionAnnotations"));
        actionInsert_row = new QAction(DesignitClass);
        actionInsert_row->setObjectName(QStringLiteral("actionInsert_row"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/Resources/icon_insert_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionInsert_row->setIcon(icon13);
        actionDelete_row = new QAction(DesignitClass);
        actionDelete_row->setObjectName(QStringLiteral("actionDelete_row"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/Resources/icon_delete_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_row->setIcon(icon14);
        actionDuplicate_row = new QAction(DesignitClass);
        actionDuplicate_row->setObjectName(QStringLiteral("actionDuplicate_row"));
        QIcon icon15;
        icon15.addFile(QStringLiteral(":/Resources/icon_duplicate_row.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDuplicate_row->setIcon(icon15);
        actionInsert_col = new QAction(DesignitClass);
        actionInsert_col->setObjectName(QStringLiteral("actionInsert_col"));
        QIcon icon16;
        icon16.addFile(QStringLiteral(":/Resources/icon_insert_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionInsert_col->setIcon(icon16);
        actionDelete_col = new QAction(DesignitClass);
        actionDelete_col->setObjectName(QStringLiteral("actionDelete_col"));
        QIcon icon17;
        icon17.addFile(QStringLiteral(":/Resources/icon_delete_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_col->setIcon(icon17);
        actionDuplicate_col = new QAction(DesignitClass);
        actionDuplicate_col->setObjectName(QStringLiteral("actionDuplicate_col"));
        QIcon icon18;
        icon18.addFile(QStringLiteral(":/Resources/icon_duplicate_column.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDuplicate_col->setIcon(icon18);
        actionMate_points = new QAction(DesignitClass);
        actionMate_points->setObjectName(QStringLiteral("actionMate_points"));
        QIcon icon19;
        icon19.addFile(QStringLiteral(":/Resources/icon_mate_points.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMate_points->setIcon(icon19);
        actionCopy_surface_mirror = new QAction(DesignitClass);
        actionCopy_surface_mirror->setObjectName(QStringLiteral("actionCopy_surface_mirror"));
        QIcon icon20;
        icon20.addFile(QStringLiteral(":/Resources/icon_copy_mirror.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy_surface_mirror->setIcon(icon20);
        actionMerge_surfaces_by_row = new QAction(DesignitClass);
        actionMerge_surfaces_by_row->setObjectName(QStringLiteral("actionMerge_surfaces_by_row"));
        QIcon icon21;
        icon21.addFile(QStringLiteral(":/Resources/icon_merge_surfaces.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMerge_surfaces_by_row->setIcon(icon21);
        actionReset_all_views = new QAction(DesignitClass);
        actionReset_all_views->setObjectName(QStringLiteral("actionReset_all_views"));
        QIcon icon22;
        icon22.addFile(QStringLiteral(":/Resources/icon_reset_all_views.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset_all_views->setIcon(icon22);
        actionWeb_help = new QAction(DesignitClass);
        actionWeb_help->setObjectName(QStringLiteral("actionWeb_help"));
        QIcon icon23;
        icon23.addFile(QStringLiteral(":/Resources/icon_help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWeb_help->setIcon(icon23);
        actionAbout = new QAction(DesignitClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionGaussian_curvature = new QAction(DesignitClass);
        actionGaussian_curvature->setObjectName(QStringLiteral("actionGaussian_curvature"));
        QIcon icon24;
        icon24.addFile(QStringLiteral(":/Resources/icon_gaussian_curvature.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGaussian_curvature->setIcon(icon24);
        actionInterpolated_points_density_U = new QAction(DesignitClass);
        actionInterpolated_points_density_U->setObjectName(QStringLiteral("actionInterpolated_points_density_U"));
        actionInterpolated_points_density_V = new QAction(DesignitClass);
        actionInterpolated_points_density_V->setObjectName(QStringLiteral("actionInterpolated_points_density_V"));
        actionGrids = new QAction(DesignitClass);
        actionGrids->setObjectName(QStringLiteral("actionGrids"));
        actionMerge_surfaces_by_row_reverse = new QAction(DesignitClass);
        actionMerge_surfaces_by_row_reverse->setObjectName(QStringLiteral("actionMerge_surfaces_by_row_reverse"));
        QIcon icon25;
        icon25.addFile(QStringLiteral(":/Resources/icon_merge_surfaces_reverse.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMerge_surfaces_by_row_reverse->setIcon(icon25);
        actionMeasure_distance = new QAction(DesignitClass);
        actionMeasure_distance->setObjectName(QStringLiteral("actionMeasure_distance"));
        QIcon icon26;
        icon26.addFile(QStringLiteral(":/Resources/icon_measure_distance.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionMeasure_distance->setIcon(icon26);
        actionCentred_rotate = new QAction(DesignitClass);
        actionCentred_rotate->setObjectName(QStringLiteral("actionCentred_rotate"));
        QIcon icon27;
        icon27.addFile(QStringLiteral(":/Resources/icon_centred_rotate.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCentred_rotate->setIcon(icon27);
        action_Flexit = new QAction(DesignitClass);
        action_Flexit->setObjectName(QStringLiteral("action_Flexit"));
        action_Flexit->setCheckable(false);
        QIcon icon28;
        icon28.addFile(QStringLiteral(":/Resources/flexit.ico"), QSize(), QIcon::Normal, QIcon::Off);
        action_Flexit->setIcon(icon28);
        actionUndo = new QAction(DesignitClass);
        actionUndo->setObjectName(QStringLiteral("actionUndo"));
        QIcon icon29;
        icon29.addFile(QStringLiteral(":/Resources/undo_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUndo->setIcon(icon29);
        actionRedo = new QAction(DesignitClass);
        actionRedo->setObjectName(QStringLiteral("actionRedo"));
        QIcon icon30;
        icon30.addFile(QStringLiteral(":/Resources/redo_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRedo->setIcon(icon30);
        centralWidget = new QWidget(DesignitClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        tabsAdditionalData = new QTabWidget(splitter);
        tabsAdditionalData->setObjectName(QStringLiteral("tabsAdditionalData"));
        tabsAdditionalData->setMinimumSize(QSize(200, 400));
        tabsAdditionalData->setMaximumSize(QSize(16777215, 16777215));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        tabsAdditionalData->setFont(font);
        tabsAdditionalData->setTabPosition(QTabWidget::West);
        tabsAdditionalData->setTabShape(QTabWidget::Triangular);
        tab_statusWindow = new QWidget();
        tab_statusWindow->setObjectName(QStringLiteral("tab_statusWindow"));
        verticalLayout = new QVBoxLayout(tab_statusWindow);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        myStatusTableWidget = new QTableWidget(tab_statusWindow);
        myStatusTableWidget->setObjectName(QStringLiteral("myStatusTableWidget"));
        myStatusTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        myStatusTableWidget->setProperty("showDropIndicator", QVariant(false));
        myStatusTableWidget->setDragDropOverwriteMode(true);

        verticalLayout->addWidget(myStatusTableWidget);

        commandLine = new QLineEdit(tab_statusWindow);
        commandLine->setObjectName(QStringLiteral("commandLine"));

        verticalLayout->addWidget(commandLine);

        tabsAdditionalData->addTab(tab_statusWindow, QString());
        tab_viewJSON = new QWidget();
        tab_viewJSON->setObjectName(QStringLiteral("tab_viewJSON"));
        verticalLayout_9 = new QVBoxLayout(tab_viewJSON);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        jsonTreeView = new QTreeView(tab_viewJSON);
        jsonTreeView->setObjectName(QStringLiteral("jsonTreeView"));

        verticalLayout_9->addWidget(jsonTreeView);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        collapseAllButton = new QPushButton(tab_viewJSON);
        collapseAllButton->setObjectName(QStringLiteral("collapseAllButton"));

        horizontalLayout_4->addWidget(collapseAllButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        expandAllButton = new QPushButton(tab_viewJSON);
        expandAllButton->setObjectName(QStringLiteral("expandAllButton"));

        horizontalLayout_4->addWidget(expandAllButton);


        verticalLayout_9->addLayout(horizontalLayout_4);

        tabsAdditionalData->addTab(tab_viewJSON, QString());
        tab_dataFile = new QWidget();
        tab_dataFile->setObjectName(QStringLiteral("tab_dataFile"));
        verticalLayout_5 = new QVBoxLayout(tab_dataFile);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        fileTextEdit = new QTextEdit(tab_dataFile);
        fileTextEdit->setObjectName(QStringLiteral("fileTextEdit"));

        verticalLayout_5->addWidget(fileTextEdit);

        tabsAdditionalData->addTab(tab_dataFile, QString());
        splitter->addWidget(tabsAdditionalData);
        frame_2 = new QFrame(splitter);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setMinimumSize(QSize(400, 400));
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
        editingTools = new QToolBox(splitter);
        editingTools->setObjectName(QStringLiteral("editingTools"));
        SurfaceTools = new QWidget();
        SurfaceTools->setObjectName(QStringLiteral("SurfaceTools"));
        SurfaceTools->setGeometry(QRect(0, 0, 304, 659));
        horizontalLayout = new QHBoxLayout(SurfaceTools);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabsWorkSurface = new QTabWidget(SurfaceTools);
        tabsWorkSurface->setObjectName(QStringLiteral("tabsWorkSurface"));
        tabsWorkSurface->setMinimumSize(QSize(200, 400));
        tabsWorkSurface->setMaximumSize(QSize(10000000, 16777215));
        tabsWorkSurface->setFont(font);
        tabsWorkSurface->setTabPosition(QTabWidget::East);
        tabsWorkSurface->setTabShape(QTabWidget::Triangular);
        tab_XYView = new QWidget();
        tab_XYView->setObjectName(QStringLiteral("tab_XYView"));
        verticalLayout_4 = new QVBoxLayout(tab_XYView);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        myXYView = new View2d(tab_XYView);
        myXYView->setObjectName(QStringLiteral("myXYView"));

        verticalLayout_4->addWidget(myXYView);

        tabsWorkSurface->addTab(tab_XYView, QString());
        tab_XZView = new QWidget();
        tab_XZView->setObjectName(QStringLiteral("tab_XZView"));
        horizontalLayout_3 = new QHBoxLayout(tab_XZView);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        myXZView = new View2d(tab_XZView);
        myXZView->setObjectName(QStringLiteral("myXZView"));

        horizontalLayout_3->addWidget(myXZView);

        tabsWorkSurface->addTab(tab_XZView, QString());
        tab_YZView = new QWidget();
        tab_YZView->setObjectName(QStringLiteral("tab_YZView"));
        verticalLayout_3 = new QVBoxLayout(tab_YZView);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        myYZView = new View2d(tab_YZView);
        myYZView->setObjectName(QStringLiteral("myYZView"));

        verticalLayout_3->addWidget(myYZView);

        tabsWorkSurface->addTab(tab_YZView, QString());
        tab_gaussianView = new QWidget();
        tab_gaussianView->setObjectName(QStringLiteral("tab_gaussianView"));
        verticalLayout_8 = new QVBoxLayout(tab_gaussianView);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        myGaussianView = new MyGaussianView(tab_gaussianView);
        myGaussianView->setObjectName(QStringLiteral("myGaussianView"));

        verticalLayout_8->addWidget(myGaussianView);

        tabsWorkSurface->addTab(tab_gaussianView, QString());
        tab_spreadsheet = new QWidget();
        tab_spreadsheet->setObjectName(QStringLiteral("tab_spreadsheet"));
        verticalLayout_7 = new QVBoxLayout(tab_spreadsheet);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        mySpreadsheet = new QTableWidget(tab_spreadsheet);
        mySpreadsheet->setObjectName(QStringLiteral("mySpreadsheet"));

        verticalLayout_7->addWidget(mySpreadsheet);

        tabsWorkSurface->addTab(tab_spreadsheet, QString());

        horizontalLayout->addWidget(tabsWorkSurface);

        editingTools->addItem(SurfaceTools, QStringLiteral("Surface Tools"));
        TrajectoryTools = new QWidget();
        TrajectoryTools->setObjectName(QStringLiteral("TrajectoryTools"));
        TrajectoryTools->setGeometry(QRect(0, 0, 218, 418));
        horizontalLayout_7 = new QHBoxLayout(TrajectoryTools);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        tabsWorkTrajectory = new QTabWidget(TrajectoryTools);
        tabsWorkTrajectory->setObjectName(QStringLiteral("tabsWorkTrajectory"));
        tabsWorkTrajectory->setMinimumSize(QSize(200, 400));
        tabsWorkTrajectory->setMaximumSize(QSize(10000000, 16777215));
        tabsWorkTrajectory->setFont(font);
        tabsWorkTrajectory->setTabPosition(QTabWidget::East);
        tabsWorkTrajectory->setTabShape(QTabWidget::Triangular);
        tab_trajectoryTranslations = new QWidget();
        tab_trajectoryTranslations->setObjectName(QStringLiteral("tab_trajectoryTranslations"));
        verticalLayout_6 = new QVBoxLayout(tab_trajectoryTranslations);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        trajectoryCurveX = new MyGLGeneralTrajectoryCurveView(tab_trajectoryTranslations);
        trajectoryCurveX->setObjectName(QStringLiteral("trajectoryCurveX"));

        verticalLayout_6->addWidget(trajectoryCurveX);

        trajectoryCurveY = new MyGLGeneralTrajectoryCurveView(tab_trajectoryTranslations);
        trajectoryCurveY->setObjectName(QStringLiteral("trajectoryCurveY"));

        verticalLayout_6->addWidget(trajectoryCurveY);

        trajectoryCurveZ = new MyGLGeneralTrajectoryCurveView(tab_trajectoryTranslations);
        trajectoryCurveZ->setObjectName(QStringLiteral("trajectoryCurveZ"));

        verticalLayout_6->addWidget(trajectoryCurveZ);

        tabsWorkTrajectory->addTab(tab_trajectoryTranslations, QString());
        tab_trajectoryRotations = new QWidget();
        tab_trajectoryRotations->setObjectName(QStringLiteral("tab_trajectoryRotations"));
        verticalLayout_19 = new QVBoxLayout(tab_trajectoryRotations);
        verticalLayout_19->setSpacing(6);
        verticalLayout_19->setContentsMargins(11, 11, 11, 11);
        verticalLayout_19->setObjectName(QStringLiteral("verticalLayout_19"));
        trajectoryCurveR = new MyGLGeneralTrajectoryCurveView(tab_trajectoryRotations);
        trajectoryCurveR->setObjectName(QStringLiteral("trajectoryCurveR"));

        verticalLayout_19->addWidget(trajectoryCurveR);

        trajectoryCurveP = new MyGLGeneralTrajectoryCurveView(tab_trajectoryRotations);
        trajectoryCurveP->setObjectName(QStringLiteral("trajectoryCurveP"));

        verticalLayout_19->addWidget(trajectoryCurveP);

        trajectoryCurveB = new MyGLGeneralTrajectoryCurveView(tab_trajectoryRotations);
        trajectoryCurveB->setObjectName(QStringLiteral("trajectoryCurveB"));

        verticalLayout_19->addWidget(trajectoryCurveB);

        tabsWorkTrajectory->addTab(tab_trajectoryRotations, QString());
        tab_trajectorySpreadsheet = new QWidget();
        tab_trajectorySpreadsheet->setObjectName(QStringLiteral("tab_trajectorySpreadsheet"));
        verticalLayout_20 = new QVBoxLayout(tab_trajectorySpreadsheet);
        verticalLayout_20->setSpacing(6);
        verticalLayout_20->setContentsMargins(11, 11, 11, 11);
        verticalLayout_20->setObjectName(QStringLiteral("verticalLayout_20"));
        trajectorySpreadsheet = new QTableWidget(tab_trajectorySpreadsheet);
        trajectorySpreadsheet->setObjectName(QStringLiteral("trajectorySpreadsheet"));

        verticalLayout_20->addWidget(trajectorySpreadsheet);

        tabsWorkTrajectory->addTab(tab_trajectorySpreadsheet, QString());

        horizontalLayout_7->addWidget(tabsWorkTrajectory);

        editingTools->addItem(TrajectoryTools, QStringLiteral("Trajectory Tools"));
        splitter->addWidget(editingTools);

        horizontalLayout_2->addWidget(splitter);

        DesignitClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(DesignitClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1129, 21));
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
        DesignitClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(DesignitClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        DesignitClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(DesignitClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DesignitClass->setStatusBar(statusBar);

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
        menuEdit->addSeparator();
        menuEdit->addAction(actionRotate);
        menuEdit->addAction(actionResize);
        menuEdit->addSeparator();
        menuEdit->addAction(actionShear);
        menuEdit->addAction(actionFlip);
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
        mainToolBar->addAction(actionUndo);
        mainToolBar->addAction(actionRedo);
        mainToolBar->addAction(action_Flexit);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionNew_surface);
        mainToolBar->addAction(actionDelete_surface);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionDrag);
        mainToolBar->addAction(actionRotate);
        mainToolBar->addAction(actionCentred_rotate);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionShear);
        mainToolBar->addAction(actionResize);
        mainToolBar->addAction(actionFlip);
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

        retranslateUi(DesignitClass);

        tabsAdditionalData->setCurrentIndex(0);
        editingTools->setCurrentIndex(0);
        tabsWorkSurface->setCurrentIndex(0);
        tabsWorkTrajectory->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DesignitClass);
    } // setupUi

    void retranslateUi(QMainWindow *DesignitClass)
    {
        DesignitClass->setWindowTitle(QApplication::translate("DesignitClass", "Surfit", Q_NULLPTR));
        actionNew->setText(QApplication::translate("DesignitClass", "New...", Q_NULLPTR));
        actionOpen->setText(QApplication::translate("DesignitClass", "Open...", Q_NULLPTR));
        actionSave_As->setText(QApplication::translate("DesignitClass", "Save As ...", Q_NULLPTR));
        actionExit->setText(QApplication::translate("DesignitClass", "Exit", Q_NULLPTR));
        actionDrag->setText(QApplication::translate("DesignitClass", "Drag", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionDrag->setToolTip(QApplication::translate("DesignitClass", "Drag points", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionNew_surface->setText(QApplication::translate("DesignitClass", "New surface", Q_NULLPTR));
        actionNormals->setText(QApplication::translate("DesignitClass", "Normals", Q_NULLPTR));
        actionInterpolated_points->setText(QApplication::translate("DesignitClass", "Interpolated points", Q_NULLPTR));
        actionControl_points->setText(QApplication::translate("DesignitClass", "Control points", Q_NULLPTR));
        actionRotate->setText(QApplication::translate("DesignitClass", "Rotate", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionRotate->setToolTip(QApplication::translate("DesignitClass", "Rotate points", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionResize->setText(QApplication::translate("DesignitClass", "Resize", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionResize->setToolTip(QApplication::translate("DesignitClass", "Resize surfaces", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionShear->setText(QApplication::translate("DesignitClass", "Shear", Q_NULLPTR));
        actionPlayout_Test->setText(QApplication::translate("DesignitClass", "Playout Test", Q_NULLPTR));
        actionFlip->setText(QApplication::translate("DesignitClass", "Flip surface", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionFlip->setToolTip(QApplication::translate("DesignitClass", "Flip surface", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionClose->setText(QApplication::translate("DesignitClass", "Close", Q_NULLPTR));
        actionCopy_surface->setText(QApplication::translate("DesignitClass", "Copy surface", Q_NULLPTR));
        actionDelete_surface->setText(QApplication::translate("DesignitClass", "Delete surface", Q_NULLPTR));
        actionAnnotations->setText(QApplication::translate("DesignitClass", "Annotations", Q_NULLPTR));
        actionInsert_row->setText(QApplication::translate("DesignitClass", "Insert row", Q_NULLPTR));
        actionDelete_row->setText(QApplication::translate("DesignitClass", "Delete row", Q_NULLPTR));
        actionDuplicate_row->setText(QApplication::translate("DesignitClass", "Duplicate row", Q_NULLPTR));
        actionInsert_col->setText(QApplication::translate("DesignitClass", "Insert col", Q_NULLPTR));
        actionDelete_col->setText(QApplication::translate("DesignitClass", "Delete col", Q_NULLPTR));
        actionDuplicate_col->setText(QApplication::translate("DesignitClass", "Duplicate col", Q_NULLPTR));
        actionMate_points->setText(QApplication::translate("DesignitClass", "Mate points", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionMate_points->setToolTip(QApplication::translate("DesignitClass", "Mate points (master first)", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionCopy_surface_mirror->setText(QApplication::translate("DesignitClass", "Copy surface mirror", Q_NULLPTR));
        actionMerge_surfaces_by_row->setText(QApplication::translate("DesignitClass", "Merge surfaces by row", Q_NULLPTR));
        actionReset_all_views->setText(QApplication::translate("DesignitClass", "Reset all views", Q_NULLPTR));
        actionWeb_help->setText(QApplication::translate("DesignitClass", "Web help ...", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("DesignitClass", "About", Q_NULLPTR));
        actionGaussian_curvature->setText(QApplication::translate("DesignitClass", "Gaussian curvature", Q_NULLPTR));
        actionInterpolated_points_density_U->setText(QApplication::translate("DesignitClass", "Interpolated points density U", Q_NULLPTR));
        actionInterpolated_points_density_V->setText(QApplication::translate("DesignitClass", "Interpolated points density V", Q_NULLPTR));
        actionGrids->setText(QApplication::translate("DesignitClass", "Grids", Q_NULLPTR));
        actionMerge_surfaces_by_row_reverse->setText(QApplication::translate("DesignitClass", "Merge surfaces by row reverse", Q_NULLPTR));
        actionMeasure_distance->setText(QApplication::translate("DesignitClass", "Measure distance", Q_NULLPTR));
        actionCentred_rotate->setText(QApplication::translate("DesignitClass", "Centred rotate", Q_NULLPTR));
        action_Flexit->setText(QApplication::translate("DesignitClass", "action_Flexit", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        action_Flexit->setToolTip(QApplication::translate("DesignitClass", "Open the current project in Flexit", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionUndo->setText(QApplication::translate("DesignitClass", "Undo", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionUndo->setToolTip(QApplication::translate("DesignitClass", "Undo last action.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionUndo->setShortcut(QApplication::translate("DesignitClass", "Ctrl+Z", Q_NULLPTR));
        actionRedo->setText(QApplication::translate("DesignitClass", "Redo", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionRedo->setToolTip(QApplication::translate("DesignitClass", "Redo action.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionRedo->setShortcut(QApplication::translate("DesignitClass", "Ctrl+Y", Q_NULLPTR));
        tabsAdditionalData->setTabText(tabsAdditionalData->indexOf(tab_statusWindow), QApplication::translate("DesignitClass", "Status window", Q_NULLPTR));
        collapseAllButton->setText(QApplication::translate("DesignitClass", "Collapse All", Q_NULLPTR));
        expandAllButton->setText(QApplication::translate("DesignitClass", "Expand All", Q_NULLPTR));
        tabsAdditionalData->setTabText(tabsAdditionalData->indexOf(tab_viewJSON), QApplication::translate("DesignitClass", "JSON view", Q_NULLPTR));
        tabsAdditionalData->setTabText(tabsAdditionalData->indexOf(tab_dataFile), QApplication::translate("DesignitClass", "Data file", Q_NULLPTR));
        label_2->setText(QApplication::translate("DesignitClass", "3D view", Q_NULLPTR));
        tabsWorkSurface->setTabText(tabsWorkSurface->indexOf(tab_XYView), QApplication::translate("DesignitClass", "XY view", Q_NULLPTR));
        tabsWorkSurface->setTabText(tabsWorkSurface->indexOf(tab_XZView), QApplication::translate("DesignitClass", "XZ view", Q_NULLPTR));
        tabsWorkSurface->setTabText(tabsWorkSurface->indexOf(tab_YZView), QApplication::translate("DesignitClass", "YZ view", Q_NULLPTR));
        tabsWorkSurface->setTabText(tabsWorkSurface->indexOf(tab_gaussianView), QApplication::translate("DesignitClass", "Gaussian Curvature", Q_NULLPTR));
        tabsWorkSurface->setTabText(tabsWorkSurface->indexOf(tab_spreadsheet), QApplication::translate("DesignitClass", "Spreadsheet", Q_NULLPTR));
        editingTools->setItemText(editingTools->indexOf(SurfaceTools), QApplication::translate("DesignitClass", "Surface Tools", Q_NULLPTR));
        tabsWorkTrajectory->setTabText(tabsWorkTrajectory->indexOf(tab_trajectoryTranslations), QApplication::translate("DesignitClass", "Translations", Q_NULLPTR));
        tabsWorkTrajectory->setTabText(tabsWorkTrajectory->indexOf(tab_trajectoryRotations), QApplication::translate("DesignitClass", "Rotations", Q_NULLPTR));
        tabsWorkTrajectory->setTabText(tabsWorkTrajectory->indexOf(tab_trajectorySpreadsheet), QApplication::translate("DesignitClass", "Spreadsheet", Q_NULLPTR));
        editingTools->setItemText(editingTools->indexOf(TrajectoryTools), QApplication::translate("DesignitClass", "Trajectory Tools", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("DesignitClass", "File", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("DesignitClass", "View", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("DesignitClass", "Mode", Q_NULLPTR));
        menuSurface->setTitle(QApplication::translate("DesignitClass", "Surface", Q_NULLPTR));
        menuTest->setTitle(QApplication::translate("DesignitClass", "Test", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("DesignitClass", "Help", Q_NULLPTR));
        menuAnalysis->setTitle(QApplication::translate("DesignitClass", "Analysis", Q_NULLPTR));
        menuEdit_2->setTitle(QApplication::translate("DesignitClass", "Edit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DesignitClass: public Ui_DesignitClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DESIGNIT_H
