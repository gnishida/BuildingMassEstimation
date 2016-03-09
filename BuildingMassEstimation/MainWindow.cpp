#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionGenerateTrainingData, SIGNAL(triggered()), this, SLOT(onGenerateTrainingData()));
	connect(ui.actionMCMC, SIGNAL(triggered()), this, SLOT(onMCMC()));

	glWidget3D = new GLWidget3D();
	this->setCentralWidget(glWidget3D);
}

void MainWindow::onGenerateTrainingData() {
	glWidget3D->generateTrainingData(256, 200);
}

void MainWindow::onMCMC() {
	glWidget3D->runMCMC();
}