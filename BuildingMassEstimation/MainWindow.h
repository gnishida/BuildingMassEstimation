#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "GLWidget3D.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	Ui::MainWindowClass ui;
	GLWidget3D* glWidget3D;

public:
	MainWindow(QWidget *parent = 0);

public slots:
	void onLoadCGA();
	void onGenerateTrainingData();
	void onVisualizePredictedData();
	void onRenderingModeChanged();
};

#endif // MAINWINDOW_H
