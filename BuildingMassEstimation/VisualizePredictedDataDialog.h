#ifndef VISUALIZEPREDICTEDDATADIALOG_H
#define VISUALIZEPREDICTEDDATADIALOG_H

#include <QDialog>
#include "ui_VisualizePredictedDataDialog.h"

class VisualizePredictedDataDialog : public QDialog
{
	Q_OBJECT

public:
	VisualizePredictedDataDialog(QWidget *parent = 0);
	~VisualizePredictedDataDialog();

	public slots:
	void onCGADirectory();
	void onDataDirectory();
	void onOK();
	void onCancel();

public:
	Ui::VisualizePredictedDataDialog ui;
};

#endif // VISUALIZEPREDICTEDDATADIALOG_H
