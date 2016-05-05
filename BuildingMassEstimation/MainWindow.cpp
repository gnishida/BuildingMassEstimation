#include "MainWindow.h"
#include <QFileDialog>
#include "MCMCConfigDialog.h"
#include "TrainingDataGenerationDialog.h"
#include "VisualizePredictedDataDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	ui.actionRenderingLine->setChecked(true);
	QActionGroup* groupRenderingMode = new QActionGroup(this);
	groupRenderingMode->addAction(ui.actionRenderingBasic);
	groupRenderingMode->addAction(ui.actionRenderingLine);
	groupRenderingMode->addAction(ui.actionRenderingContour);

	connect(ui.actionLoadCGA, SIGNAL(triggered()), this, SLOT(onLoadCGA()));
	connect(ui.actionLoadImage, SIGNAL(triggered()), this, SLOT(onLoadImage()));
	connect(ui.actionSaveImage, SIGNAL(triggered()), this, SLOT(onSaveImage()));
	connect(ui.actionSaveContour, SIGNAL(triggered()), this, SLOT(onSaveContour()));
	connect(ui.actionResetCamera, SIGNAL(triggered()), this, SLOT(onResetCamera()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionGenerateTrainingData, SIGNAL(triggered()), this, SLOT(onGenerateTrainingData()));
	connect(ui.actionGenerateTrainingDataWithoutAmbiguousViewpoints, SIGNAL(triggered()), this, SLOT(onGenerateTrainingDataWithoutAmgiousViewpoints()));
	connect(ui.actionVisualizePredictedData, SIGNAL(triggered()), this, SLOT(onVisualizePredictedData()));
	connect(ui.actionMCMC, SIGNAL(triggered()), this, SLOT(onMCMC()));
	connect(ui.actionMCMCAll, SIGNAL(triggered()), this, SLOT(onMCMCAll()));
	connect(ui.actionComputeVanishingPoints, SIGNAL(triggered()), this, SLOT(onComputeVanishingPoints()));
	connect(ui.actionRenderingBasic, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingLine, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));
	connect(ui.actionRenderingContour, SIGNAL(triggered()), this, SLOT(onRenderingModeChanged()));

	glWidget3D = new GLWidget3D(this);
	this->setCentralWidget(glWidget3D);
}

void MainWindow::onLoadCGA() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load CGA file..."), "", tr("CGA Files (*.xml)"));
	if (filename.isEmpty()) return;

	glWidget3D->loadCGA(filename.toUtf8().constData());
}

void MainWindow::onLoadImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Image file..."), "", tr("Image Files (*.png)"));
	if (filename.isEmpty()) return;

	glWidget3D->loadImage(filename.toUtf8().constData());
}

void MainWindow::onSaveImage() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image file..."), "", tr("Image Files (*.png)"));
	if (filename.isEmpty()) return;

	glWidget3D->grabFrameBuffer().save(filename.toUtf8().constData());
}

void MainWindow::onSaveContour() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Contour Image file..."), "", tr("Contour Image Files (*.png)"));
	if (filename.isEmpty()) return;

	glWidget3D->saveContour(filename.toUtf8().constData());

}

void MainWindow::onResetCamera() {
	glWidget3D->fixCamera();
	glWidget3D->update();
}

void MainWindow::onGenerateTrainingData() {
	TrainingDataGenerationDialog dlg;
	if (dlg.exec() && !dlg.ui.lineEditCGADirectory->text().isEmpty() && !dlg.ui.lineEditOutputDirectory->text().isEmpty()) {
		int numSamples = dlg.ui.lineEditNumSamples->text().toInt();
		int imageWidth = dlg.ui.lineEditImageWidth->text().toInt();
		int imageHeight = dlg.ui.lineEditImageHeight->text().toInt();
		bool grayscale = dlg.ui.checkBoxGrayscale->isChecked();
		bool centering3D = dlg.ui.checkBoxCentering3D->isChecked();
		bool meanGeneration = dlg.ui.checkBoxMeanGeneration->isChecked();
		bool centering = dlg.ui.checkBoxCentering->isChecked();
		bool centeringNoise = dlg.ui.checkBoxCenteringNoise->isChecked();
		float centeringNoiseMax = dlg.ui.lineEditCenteringNoiseMax->text().toFloat();
		int cameraType = 0;
		if (dlg.ui.radioButtonCameraTypeAerialView->isChecked()) {
			cameraType = 1;
		}
		float cameraDistanceBase = dlg.ui.lineEditCameraDistance->text().toFloat();
		int xrotMin = dlg.ui.lineEditXrotMin->text().toInt();
		int xrotMax = dlg.ui.lineEditXrotMax->text().toInt();
		int yrotMin = dlg.ui.lineEditYrotMin->text().toInt();
		int yrotMax = dlg.ui.lineEditYrotMax->text().toInt();
		int fovMin = dlg.ui.lineEditFovMin->text().toInt();
		int fovMax = dlg.ui.lineEditFovMax->text().toInt();
		bool modifyImage = dlg.ui.checkBoxModifyImage->isChecked();
		int lineWidthMin = dlg.ui.lineEditLineWidthMin->text().toInt();
		int lineWidthMax = dlg.ui.lineEditLineWidthMax->text().toInt();
		bool edgeNoise = dlg.ui.checkBoxEdgeNoise->isChecked();
		float edgeNoiseMax = dlg.ui.lineEditEdgeNoiseMax->text().toFloat();

		glWidget3D->generateTrainingData(dlg.ui.lineEditCGADirectory->text(), dlg.ui.lineEditOutputDirectory->text(), numSamples, imageWidth, imageHeight, grayscale, centering3D, centering, centeringNoise, centeringNoiseMax, meanGeneration, cameraType, cameraDistanceBase, 0.0f, xrotMin, xrotMax, yrotMin, yrotMax, fovMin, fovMax, modifyImage, lineWidthMin, lineWidthMax, edgeNoise, edgeNoiseMax);
	}
}

void MainWindow::onGenerateTrainingDataWithoutAmgiousViewpoints() {
	TrainingDataGenerationDialog dlg;
	if (dlg.exec() && !dlg.ui.lineEditCGADirectory->text().isEmpty() && !dlg.ui.lineEditOutputDirectory->text().isEmpty()) {
		int numSamples = dlg.ui.lineEditNumSamples->text().toInt();
		int imageWidth = dlg.ui.lineEditImageWidth->text().toInt();
		int imageHeight = dlg.ui.lineEditImageHeight->text().toInt();
		bool grayscale = false;
		if (dlg.ui.checkBoxGrayscale->isChecked()) {
			grayscale = true;
		}
		bool centering = false;
		if (dlg.ui.checkBoxCentering->isChecked()) {
			centering = true;
		}
		int xrotMin = dlg.ui.lineEditXrotMin->text().toInt();
		int xrotMax = dlg.ui.lineEditXrotMax->text().toInt();
		int yrotMin = dlg.ui.lineEditYrotMin->text().toInt();
		int yrotMax = dlg.ui.lineEditYrotMax->text().toInt();

		glWidget3D->generateTrainingDataWithoutAmgiousViewpoints(dlg.ui.lineEditCGADirectory->text(), dlg.ui.lineEditOutputDirectory->text(), numSamples, imageWidth, imageHeight, grayscale, centering, xrotMin, xrotMax, yrotMin, yrotMax);
	}
}

void MainWindow::onVisualizePredictedData() {
	VisualizePredictedDataDialog dlg;
	if (dlg.exec() && !dlg.ui.lineEditCGADirectory->text().isEmpty() && !dlg.ui.lineEditDataDirectory->text().isEmpty()) {
		bool centering3D = false;
		if (dlg.ui.checkBoxCentering3D->isChecked()) {
			centering3D = true;
		}
		int cameraType = 0;
		if (dlg.ui.radioButtonCameraTypeAerialView->isChecked()) {
			cameraType = 1;
		}
		float cameraDistanceBase = dlg.ui.lineEditCameraDistance->text().toFloat();
		int xrotMin = dlg.ui.lineEditXrotMin->text().toInt();
		int xrotMax = dlg.ui.lineEditXrotMax->text().toInt();
		int yrotMin = dlg.ui.lineEditYrotMin->text().toInt();
		int yrotMax = dlg.ui.lineEditYrotMax->text().toInt();
		int fovMin = dlg.ui.lineEditFovMin->text().toInt();
		int fovMax = dlg.ui.lineEditFovMax->text().toInt();

		glWidget3D->visualizePredictedData(dlg.ui.lineEditCGADirectory->text(), dlg.ui.lineEditDataDirectory->text(), centering3D, cameraType, cameraDistanceBase, 0, xrotMin, xrotMax, yrotMin, yrotMax, fovMin, fovMax);
	}
}

void MainWindow::onMCMC() {
	MCMCConfigDialog dlg;
	if (dlg.exec() && !dlg.ui.lineEditCGAFilename->text().isEmpty() && !dlg.ui.lineEditTargetFilename->text().isEmpty()) {
		glWidget3D->runMCMC(dlg.ui.lineEditCGAFilename->text().toUtf8().constData(), dlg.ui.lineEditTargetFilename->text().toUtf8().constData(), dlg.ui.lineEditIterations->text().toInt());
	}
}

void MainWindow::onMCMCAll() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),	".", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty()) return;

	glWidget3D->runMCMCAll(dir.toUtf8().constData(), 300);
}

void MainWindow::onComputeVanishingPoints() {
	glWidget3D->extractCameraParameters();
}

void MainWindow::onRenderingModeChanged() {
	if (ui.actionRenderingBasic->isChecked()) {
		glWidget3D->renderManager.renderingMode = RenderManager::RENDERING_MODE_BASIC;
	}
	else if (ui.actionRenderingLine->isChecked()) {
		glWidget3D->renderManager.renderingMode = RenderManager::RENDERING_MODE_LINE;
	}
	else if (ui.actionRenderingContour->isChecked()) {
		glWidget3D->renderManager.renderingMode = RenderManager::RENDERING_MODE_CONTOUR;
	}
	glWidget3D->update();
}