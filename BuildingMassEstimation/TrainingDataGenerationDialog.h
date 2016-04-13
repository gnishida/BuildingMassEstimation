#ifndef TRAININGDATAGENERATIONDIALOG_H
#define TRAININGDATAGENERATIONDIALOG_H

#include <QDialog>
#include "ui_TrainingDataGenerationDialog.h"

class TrainingDataGenerationDialog : public QDialog
{
	Q_OBJECT

public:
	TrainingDataGenerationDialog(QWidget *parent = 0);
	~TrainingDataGenerationDialog();

public slots:
	void onCGADirectory();
	void onOutputDirectory();
	void onOK();
	void onCancel();

public:
	Ui::TrainingDataGenerationDialog ui;
};

#endif // TRAININGDATAGENERATIONDIALOG_H
