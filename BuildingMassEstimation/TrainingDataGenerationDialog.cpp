#include "TrainingDataGenerationDialog.h"
#include <QFileDialog>

TrainingDataGenerationDialog::TrainingDataGenerationDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.lineEditCGADirectory->setText("..\\cga\\mass_20160329\\");
	ui.lineEditOutputDirectory->setText("C:\\Anaconda\\caffe\\data\\contours_regression\\");
	ui.lineEditNumSamples->setText("100");
	ui.lineEditImageWidth->setText("128");
	ui.lineEditImageHeight->setText("128");
	ui.checkBoxGrayscale->setChecked(true);
	ui.lineEditXrot->setText("20");
	ui.lineEditXrotRange->setText("20");
	ui.lineEditYrot->setText("30");
	ui.lineEditYrotRange->setText("20");

	connect(ui.pushButtonCGADirectory, SIGNAL(clicked()), this, SLOT(onCGADirectory()));
	connect(ui.pushButtonOutputDirectory, SIGNAL(clicked()), this, SLOT(onOutputDirectory()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

TrainingDataGenerationDialog::~TrainingDataGenerationDialog() {
}

void TrainingDataGenerationDialog::onCGADirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditCGADirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditCGADirectory->setText(dir);
	}
}

void TrainingDataGenerationDialog::onOutputDirectory() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), ui.lineEditOutputDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui.lineEditOutputDirectory->setText(dir);
	}

}

void TrainingDataGenerationDialog::onOK() {
	accept();
}

void TrainingDataGenerationDialog::onCancel() {
	reject();
}