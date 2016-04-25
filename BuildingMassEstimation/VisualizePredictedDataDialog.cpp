#include "VisualizePredictedDataDialog.h"
#include <QFileDialog>

VisualizePredictedDataDialog::VisualizePredictedDataDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.lineEditCGADirectory->setText("..\\cga\\mass_20160329\\");
	ui.lineEditDataDirectory->setText("C:\\Anaconda\\caffe\\data\\contours_regression\\contours\\");
	ui.radioButtonCameraTypeStreetView->setChecked(true);
	ui.lineEditXrotMin->setText("0");
	ui.lineEditXrotMax->setText("40");
	ui.lineEditYrotMin->setText("15");
	ui.lineEditYrotMax->setText("75");
	ui.lineEditFovMin->setText("0");
	ui.lineEditFovMax->setText("60");

	connect(ui.pushButtonCGADirectory, SIGNAL(clicked()), this, SLOT(onCGADirectory()));
	connect(ui.pushButtonDataDirectory, SIGNAL(clicked()), this, SLOT(onDataDirectory()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

}

VisualizePredictedDataDialog::~VisualizePredictedDataDialog() {
}

void VisualizePredictedDataDialog::onCGADirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditCGADirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditCGADirectory->setText(dir);
	}
}

void VisualizePredictedDataDialog::onDataDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditDataDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditDataDirectory->setText(dir);
	}

}

void VisualizePredictedDataDialog::onOK() {
	accept();
}

void VisualizePredictedDataDialog::onCancel() {
	reject();
}