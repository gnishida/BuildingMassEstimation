/********************************************************************************
** Form generated from reading UI file 'TrainingDataGenerationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRAININGDATAGENERATIONDIALOG_H
#define UI_TRAININGDATAGENERATIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_TrainingDataGenerationDialog
{
public:
    QLabel *label;
    QLineEdit *lineEditCGADirectory;
    QPushButton *pushButtonCGADirectory;
    QLabel *label_2;
    QLineEdit *lineEditOutputDirectory;
    QPushButton *pushButtonOutputDirectory;
    QLabel *label_3;
    QLineEdit *lineEditNumSamples;
    QLabel *label_4;
    QLineEdit *lineEditImageWidth;
    QLabel *label_5;
    QLineEdit *lineEditImageHeight;
    QCheckBox *checkBoxGrayscale;
    QLabel *label_6;
    QLineEdit *lineEditXrotMin;
    QLineEdit *lineEditXrotMax;
    QLabel *label_8;
    QLineEdit *lineEditYrotMin;
    QLineEdit *lineEditYrotMax;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_10;
    QLineEdit *lineEditFovMin;
    QLineEdit *lineEditFovMax;
    QLabel *label_11;
    QLabel *label_12;
    QRadioButton *radioButtonCameraTypeStreetView;
    QRadioButton *radioButtonCameraTypeAerialView;
    QCheckBox *checkBoxCentering;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_7;
    QLineEdit *lineEditCameraDistance;
    QCheckBox *checkBoxCentering3D;
    QGroupBox *groupBox;
    QLabel *label_9;
    QLineEdit *lineEditLineWidthMin;
    QCheckBox *checkBoxEdgeNoise;
    QCheckBox *checkBoxModifyImage;
    QLineEdit *lineEditLineWidthMax;
    QLabel *label_15;
    QLineEdit *lineEditEdgeNoiseMax;
    QLabel *label_16;
    QCheckBox *checkBoxMeanGeneration;
    QCheckBox *checkBoxCenteringNoise;
    QLabel *label_17;
    QLineEdit *lineEditCenteringNoiseMax;

    void setupUi(QDialog *TrainingDataGenerationDialog)
    {
        if (TrainingDataGenerationDialog->objectName().isEmpty())
            TrainingDataGenerationDialog->setObjectName(QStringLiteral("TrainingDataGenerationDialog"));
        TrainingDataGenerationDialog->resize(537, 502);
        label = new QLabel(TrainingDataGenerationDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 10, 81, 16));
        lineEditCGADirectory = new QLineEdit(TrainingDataGenerationDialog);
        lineEditCGADirectory->setObjectName(QStringLiteral("lineEditCGADirectory"));
        lineEditCGADirectory->setGeometry(QRect(110, 10, 381, 20));
        pushButtonCGADirectory = new QPushButton(TrainingDataGenerationDialog);
        pushButtonCGADirectory->setObjectName(QStringLiteral("pushButtonCGADirectory"));
        pushButtonCGADirectory->setGeometry(QRect(495, 9, 31, 23));
        label_2 = new QLabel(TrainingDataGenerationDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 40, 91, 16));
        lineEditOutputDirectory = new QLineEdit(TrainingDataGenerationDialog);
        lineEditOutputDirectory->setObjectName(QStringLiteral("lineEditOutputDirectory"));
        lineEditOutputDirectory->setGeometry(QRect(110, 40, 381, 20));
        pushButtonOutputDirectory = new QPushButton(TrainingDataGenerationDialog);
        pushButtonOutputDirectory->setObjectName(QStringLiteral("pushButtonOutputDirectory"));
        pushButtonOutputDirectory->setGeometry(QRect(495, 39, 31, 23));
        label_3 = new QLabel(TrainingDataGenerationDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 70, 111, 16));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditNumSamples = new QLineEdit(TrainingDataGenerationDialog);
        lineEditNumSamples->setObjectName(QStringLiteral("lineEditNumSamples"));
        lineEditNumSamples->setGeometry(QRect(140, 70, 61, 20));
        label_4 = new QLabel(TrainingDataGenerationDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(60, 100, 71, 16));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditImageWidth = new QLineEdit(TrainingDataGenerationDialog);
        lineEditImageWidth->setObjectName(QStringLiteral("lineEditImageWidth"));
        lineEditImageWidth->setGeometry(QRect(140, 100, 61, 20));
        label_5 = new QLabel(TrainingDataGenerationDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(220, 100, 71, 16));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditImageHeight = new QLineEdit(TrainingDataGenerationDialog);
        lineEditImageHeight->setObjectName(QStringLiteral("lineEditImageHeight"));
        lineEditImageHeight->setGeometry(QRect(300, 100, 61, 20));
        checkBoxGrayscale = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxGrayscale->setObjectName(QStringLiteral("checkBoxGrayscale"));
        checkBoxGrayscale->setGeometry(QRect(380, 100, 70, 17));
        label_6 = new QLabel(TrainingDataGenerationDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(60, 250, 71, 16));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrotMin = new QLineEdit(TrainingDataGenerationDialog);
        lineEditXrotMin->setObjectName(QStringLiteral("lineEditXrotMin"));
        lineEditXrotMin->setGeometry(QRect(140, 250, 61, 20));
        lineEditXrotMax = new QLineEdit(TrainingDataGenerationDialog);
        lineEditXrotMax->setObjectName(QStringLiteral("lineEditXrotMax"));
        lineEditXrotMax->setGeometry(QRect(230, 250, 61, 20));
        label_8 = new QLabel(TrainingDataGenerationDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(60, 280, 71, 16));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditYrotMin = new QLineEdit(TrainingDataGenerationDialog);
        lineEditYrotMin->setObjectName(QStringLiteral("lineEditYrotMin"));
        lineEditYrotMin->setGeometry(QRect(140, 280, 61, 20));
        lineEditYrotMax = new QLineEdit(TrainingDataGenerationDialog);
        lineEditYrotMax->setObjectName(QStringLiteral("lineEditYrotMax"));
        lineEditYrotMax->setGeometry(QRect(230, 280, 61, 20));
        pushButtonOK = new QPushButton(TrainingDataGenerationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(330, 460, 91, 31));
        pushButtonCancel = new QPushButton(TrainingDataGenerationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(430, 460, 91, 31));
        label_10 = new QLabel(TrainingDataGenerationDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(60, 310, 71, 16));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFovMin = new QLineEdit(TrainingDataGenerationDialog);
        lineEditFovMin->setObjectName(QStringLiteral("lineEditFovMin"));
        lineEditFovMin->setGeometry(QRect(140, 310, 61, 20));
        lineEditFovMax = new QLineEdit(TrainingDataGenerationDialog);
        lineEditFovMax->setObjectName(QStringLiteral("lineEditFovMax"));
        lineEditFovMax->setGeometry(QRect(230, 310, 61, 20));
        label_11 = new QLabel(TrainingDataGenerationDialog);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(203, 310, 16, 20));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_12 = new QLabel(TrainingDataGenerationDialog);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(60, 190, 71, 16));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radioButtonCameraTypeStreetView = new QRadioButton(TrainingDataGenerationDialog);
        radioButtonCameraTypeStreetView->setObjectName(QStringLiteral("radioButtonCameraTypeStreetView"));
        radioButtonCameraTypeStreetView->setGeometry(QRect(140, 190, 82, 17));
        radioButtonCameraTypeAerialView = new QRadioButton(TrainingDataGenerationDialog);
        radioButtonCameraTypeAerialView->setObjectName(QStringLiteral("radioButtonCameraTypeAerialView"));
        radioButtonCameraTypeAerialView->setGeometry(QRect(240, 190, 82, 17));
        checkBoxCentering = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxCentering->setObjectName(QStringLiteral("checkBoxCentering"));
        checkBoxCentering->setGeometry(QRect(140, 160, 70, 17));
        label_13 = new QLabel(TrainingDataGenerationDialog);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(203, 250, 16, 20));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_14 = new QLabel(TrainingDataGenerationDialog);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(203, 280, 16, 20));
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_7 = new QLabel(TrainingDataGenerationDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(40, 220, 91, 20));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditCameraDistance = new QLineEdit(TrainingDataGenerationDialog);
        lineEditCameraDistance->setObjectName(QStringLiteral("lineEditCameraDistance"));
        lineEditCameraDistance->setGeometry(QRect(140, 220, 61, 20));
        checkBoxCentering3D = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxCentering3D->setObjectName(QStringLiteral("checkBoxCentering3D"));
        checkBoxCentering3D->setGeometry(QRect(140, 130, 91, 17));
        groupBox = new QGroupBox(TrainingDataGenerationDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(40, 340, 471, 111));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(30, 50, 61, 20));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditLineWidthMin = new QLineEdit(groupBox);
        lineEditLineWidthMin->setObjectName(QStringLiteral("lineEditLineWidthMin"));
        lineEditLineWidthMin->setGeometry(QRect(100, 50, 61, 20));
        checkBoxEdgeNoise = new QCheckBox(groupBox);
        checkBoxEdgeNoise->setObjectName(QStringLiteral("checkBoxEdgeNoise"));
        checkBoxEdgeNoise->setGeometry(QRect(40, 80, 70, 17));
        checkBoxModifyImage = new QCheckBox(groupBox);
        checkBoxModifyImage->setObjectName(QStringLiteral("checkBoxModifyImage"));
        checkBoxModifyImage->setGeometry(QRect(40, 20, 151, 17));
        lineEditLineWidthMax = new QLineEdit(groupBox);
        lineEditLineWidthMax->setObjectName(QStringLiteral("lineEditLineWidthMax"));
        lineEditLineWidthMax->setGeometry(QRect(190, 50, 61, 20));
        label_15 = new QLabel(groupBox);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(164, 50, 16, 20));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditEdgeNoiseMax = new QLineEdit(groupBox);
        lineEditEdgeNoiseMax->setObjectName(QStringLiteral("lineEditEdgeNoiseMax"));
        lineEditEdgeNoiseMax->setGeometry(QRect(140, 80, 61, 20));
        label_16 = new QLabel(groupBox);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(206, 80, 21, 20));
        label_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        checkBoxMeanGeneration = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxMeanGeneration->setObjectName(QStringLiteral("checkBoxMeanGeneration"));
        checkBoxMeanGeneration->setGeometry(QRect(340, 130, 141, 17));
        checkBoxCenteringNoise = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxCenteringNoise->setObjectName(QStringLiteral("checkBoxCenteringNoise"));
        checkBoxCenteringNoise->setGeometry(QRect(230, 160, 70, 17));
        label_17 = new QLabel(TrainingDataGenerationDialog);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(385, 160, 20, 20));
        label_17->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        lineEditCenteringNoiseMax = new QLineEdit(TrainingDataGenerationDialog);
        lineEditCenteringNoiseMax->setObjectName(QStringLiteral("lineEditCenteringNoiseMax"));
        lineEditCenteringNoiseMax->setGeometry(QRect(320, 160, 61, 20));
        QWidget::setTabOrder(lineEditCGADirectory, pushButtonCGADirectory);
        QWidget::setTabOrder(pushButtonCGADirectory, lineEditOutputDirectory);
        QWidget::setTabOrder(lineEditOutputDirectory, pushButtonOutputDirectory);
        QWidget::setTabOrder(pushButtonOutputDirectory, lineEditNumSamples);
        QWidget::setTabOrder(lineEditNumSamples, lineEditImageWidth);
        QWidget::setTabOrder(lineEditImageWidth, lineEditImageHeight);
        QWidget::setTabOrder(lineEditImageHeight, checkBoxGrayscale);
        QWidget::setTabOrder(checkBoxGrayscale, checkBoxCentering3D);
        QWidget::setTabOrder(checkBoxCentering3D, checkBoxMeanGeneration);
        QWidget::setTabOrder(checkBoxMeanGeneration, checkBoxCentering);
        QWidget::setTabOrder(checkBoxCentering, checkBoxCenteringNoise);
        QWidget::setTabOrder(checkBoxCenteringNoise, lineEditCenteringNoiseMax);
        QWidget::setTabOrder(lineEditCenteringNoiseMax, radioButtonCameraTypeStreetView);
        QWidget::setTabOrder(radioButtonCameraTypeStreetView, radioButtonCameraTypeAerialView);
        QWidget::setTabOrder(radioButtonCameraTypeAerialView, lineEditCameraDistance);
        QWidget::setTabOrder(lineEditCameraDistance, lineEditXrotMin);
        QWidget::setTabOrder(lineEditXrotMin, lineEditXrotMax);
        QWidget::setTabOrder(lineEditXrotMax, lineEditYrotMin);
        QWidget::setTabOrder(lineEditYrotMin, lineEditYrotMax);
        QWidget::setTabOrder(lineEditYrotMax, lineEditFovMin);
        QWidget::setTabOrder(lineEditFovMin, lineEditFovMax);
        QWidget::setTabOrder(lineEditFovMax, checkBoxModifyImage);
        QWidget::setTabOrder(checkBoxModifyImage, lineEditLineWidthMin);
        QWidget::setTabOrder(lineEditLineWidthMin, lineEditLineWidthMax);
        QWidget::setTabOrder(lineEditLineWidthMax, checkBoxEdgeNoise);
        QWidget::setTabOrder(checkBoxEdgeNoise, lineEditEdgeNoiseMax);
        QWidget::setTabOrder(lineEditEdgeNoiseMax, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(TrainingDataGenerationDialog);

        QMetaObject::connectSlotsByName(TrainingDataGenerationDialog);
    } // setupUi

    void retranslateUi(QDialog *TrainingDataGenerationDialog)
    {
        TrainingDataGenerationDialog->setWindowTitle(QApplication::translate("TrainingDataGenerationDialog", "Training Data Generation", 0));
        label->setText(QApplication::translate("TrainingDataGenerationDialog", "CGA directory:", 0));
        pushButtonCGADirectory->setText(QApplication::translate("TrainingDataGenerationDialog", "...", 0));
        label_2->setText(QApplication::translate("TrainingDataGenerationDialog", "Output directory:", 0));
        pushButtonOutputDirectory->setText(QApplication::translate("TrainingDataGenerationDialog", "...", 0));
        label_3->setText(QApplication::translate("TrainingDataGenerationDialog", "#Samples / viewpoint:", 0));
        label_4->setText(QApplication::translate("TrainingDataGenerationDialog", "Image width:", 0));
        label_5->setText(QApplication::translate("TrainingDataGenerationDialog", "Image height:", 0));
        checkBoxGrayscale->setText(QApplication::translate("TrainingDataGenerationDialog", "Grayscale", 0));
        label_6->setText(QApplication::translate("TrainingDataGenerationDialog", "Xrot:", 0));
        label_8->setText(QApplication::translate("TrainingDataGenerationDialog", "Yrot:", 0));
        pushButtonOK->setText(QApplication::translate("TrainingDataGenerationDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("TrainingDataGenerationDialog", "Cancel", 0));
        label_10->setText(QApplication::translate("TrainingDataGenerationDialog", "Field of view:", 0));
        label_11->setText(QApplication::translate("TrainingDataGenerationDialog", "~", 0));
        label_12->setText(QApplication::translate("TrainingDataGenerationDialog", "Camera type:", 0));
        radioButtonCameraTypeStreetView->setText(QApplication::translate("TrainingDataGenerationDialog", "Street view", 0));
        radioButtonCameraTypeAerialView->setText(QApplication::translate("TrainingDataGenerationDialog", "Aerial view", 0));
        checkBoxCentering->setText(QApplication::translate("TrainingDataGenerationDialog", "Centering", 0));
        label_13->setText(QApplication::translate("TrainingDataGenerationDialog", "~", 0));
        label_14->setText(QApplication::translate("TrainingDataGenerationDialog", "~", 0));
        label_7->setText(QApplication::translate("TrainingDataGenerationDialog", "Camera distance:", 0));
        checkBoxCentering3D->setText(QApplication::translate("TrainingDataGenerationDialog", "3D Centering", 0));
        groupBox->setTitle(QApplication::translate("TrainingDataGenerationDialog", "Image Modification", 0));
        label_9->setText(QApplication::translate("TrainingDataGenerationDialog", "Line width:", 0));
        checkBoxEdgeNoise->setText(QApplication::translate("TrainingDataGenerationDialog", "Edge noise", 0));
        checkBoxModifyImage->setText(QApplication::translate("TrainingDataGenerationDialog", "Modify rendered image", 0));
        label_15->setText(QApplication::translate("TrainingDataGenerationDialog", "~", 0));
        label_16->setText(QApplication::translate("TrainingDataGenerationDialog", "%", 0));
        checkBoxMeanGeneration->setText(QApplication::translate("TrainingDataGenerationDialog", "Mean Generation", 0));
        checkBoxCenteringNoise->setText(QApplication::translate("TrainingDataGenerationDialog", "Add noise", 0));
        label_17->setText(QApplication::translate("TrainingDataGenerationDialog", "%", 0));
    } // retranslateUi

};

namespace Ui {
    class TrainingDataGenerationDialog: public Ui_TrainingDataGenerationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAININGDATAGENERATIONDIALOG_H
