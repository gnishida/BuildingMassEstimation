/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionExit;
    QAction *actionMCMC;
    QAction *actionSaveImage;
    QAction *actionRenderingBasic;
    QAction *actionRenderingLine;
    QAction *actionRenderingContour;
    QAction *actionLoadCGA;
    QAction *actionResetCamera;
    QAction *actionMCMCAll;
    QAction *actionLoadImage;
    QAction *actionSaveContour;
    QAction *actionComputeVanishingPoints;
    QAction *actionContourVectors;
    QAction *actionGenerateTrainingDataWithoutAmbiguousViewpoints;
    QAction *actionGenerateTrainingData;
    QAction *actionVisualizePredictedData;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuTool;
    QMenu *menuRendering;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(1024, 800);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionMCMC = new QAction(MainWindowClass);
        actionMCMC->setObjectName(QStringLiteral("actionMCMC"));
        actionSaveImage = new QAction(MainWindowClass);
        actionSaveImage->setObjectName(QStringLiteral("actionSaveImage"));
        actionRenderingBasic = new QAction(MainWindowClass);
        actionRenderingBasic->setObjectName(QStringLiteral("actionRenderingBasic"));
        actionRenderingBasic->setCheckable(true);
        actionRenderingLine = new QAction(MainWindowClass);
        actionRenderingLine->setObjectName(QStringLiteral("actionRenderingLine"));
        actionRenderingLine->setCheckable(true);
        actionRenderingContour = new QAction(MainWindowClass);
        actionRenderingContour->setObjectName(QStringLiteral("actionRenderingContour"));
        actionRenderingContour->setCheckable(true);
        actionLoadCGA = new QAction(MainWindowClass);
        actionLoadCGA->setObjectName(QStringLiteral("actionLoadCGA"));
        actionResetCamera = new QAction(MainWindowClass);
        actionResetCamera->setObjectName(QStringLiteral("actionResetCamera"));
        actionMCMCAll = new QAction(MainWindowClass);
        actionMCMCAll->setObjectName(QStringLiteral("actionMCMCAll"));
        actionLoadImage = new QAction(MainWindowClass);
        actionLoadImage->setObjectName(QStringLiteral("actionLoadImage"));
        actionSaveContour = new QAction(MainWindowClass);
        actionSaveContour->setObjectName(QStringLiteral("actionSaveContour"));
        actionComputeVanishingPoints = new QAction(MainWindowClass);
        actionComputeVanishingPoints->setObjectName(QStringLiteral("actionComputeVanishingPoints"));
        actionContourVectors = new QAction(MainWindowClass);
        actionContourVectors->setObjectName(QStringLiteral("actionContourVectors"));
        actionGenerateTrainingDataWithoutAmbiguousViewpoints = new QAction(MainWindowClass);
        actionGenerateTrainingDataWithoutAmbiguousViewpoints->setObjectName(QStringLiteral("actionGenerateTrainingDataWithoutAmbiguousViewpoints"));
        actionGenerateTrainingData = new QAction(MainWindowClass);
        actionGenerateTrainingData->setObjectName(QStringLiteral("actionGenerateTrainingData"));
        actionVisualizePredictedData = new QAction(MainWindowClass);
        actionVisualizePredictedData->setObjectName(QStringLiteral("actionVisualizePredictedData"));
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        menuRendering = new QMenu(menuBar);
        menuRendering->setObjectName(QStringLiteral("menuRendering"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuBar->addAction(menuRendering->menuAction());
        menuFile->addAction(actionLoadCGA);
        menuFile->addAction(actionLoadImage);
        menuFile->addAction(actionSaveImage);
        menuFile->addAction(actionSaveContour);
        menuFile->addSeparator();
        menuFile->addAction(actionResetCamera);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuTool->addAction(actionGenerateTrainingData);
        menuTool->addAction(actionGenerateTrainingDataWithoutAmbiguousViewpoints);
        menuTool->addSeparator();
        menuTool->addAction(actionVisualizePredictedData);
        menuTool->addSeparator();
        menuTool->addAction(actionMCMC);
        menuTool->addAction(actionMCMCAll);
        menuTool->addSeparator();
        menuTool->addAction(actionComputeVanishingPoints);
        menuTool->addSeparator();
        menuTool->addAction(actionContourVectors);
        menuRendering->addAction(actionRenderingBasic);
        menuRendering->addAction(actionRenderingLine);
        menuRendering->addAction(actionRenderingContour);

        retranslateUi(MainWindowClass);

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "MainWindow", 0));
        actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0));
        actionMCMC->setText(QApplication::translate("MainWindowClass", "MCMC", 0));
        actionSaveImage->setText(QApplication::translate("MainWindowClass", "Save Image", 0));
        actionSaveImage->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+S", 0));
        actionRenderingBasic->setText(QApplication::translate("MainWindowClass", "Basic", 0));
        actionRenderingLine->setText(QApplication::translate("MainWindowClass", "Line", 0));
        actionRenderingContour->setText(QApplication::translate("MainWindowClass", "Contour", 0));
        actionLoadCGA->setText(QApplication::translate("MainWindowClass", "Load CGA", 0));
        actionLoadCGA->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+O", 0));
        actionResetCamera->setText(QApplication::translate("MainWindowClass", "Reset Camera", 0));
        actionMCMCAll->setText(QApplication::translate("MainWindowClass", "MCMC All", 0));
        actionLoadImage->setText(QApplication::translate("MainWindowClass", "Load Image", 0));
        actionSaveContour->setText(QApplication::translate("MainWindowClass", "Save Contour", 0));
        actionComputeVanishingPoints->setText(QApplication::translate("MainWindowClass", "Compute Vanishing Points", 0));
        actionContourVectors->setText(QApplication::translate("MainWindowClass", "Contour Vectors", 0));
        actionGenerateTrainingDataWithoutAmbiguousViewpoints->setText(QApplication::translate("MainWindowClass", "Generate Training Data without Ambiguous Viewpoints", 0));
        actionGenerateTrainingData->setText(QApplication::translate("MainWindowClass", "Generate Training Data", 0));
        actionVisualizePredictedData->setText(QApplication::translate("MainWindowClass", "Visualize Predicted Data", 0));
        menuFile->setTitle(QApplication::translate("MainWindowClass", "File", 0));
        menuTool->setTitle(QApplication::translate("MainWindowClass", "Tool", 0));
        menuRendering->setTitle(QApplication::translate("MainWindowClass", "Rendering", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
