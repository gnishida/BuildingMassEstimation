/********************************************************************************
** Form generated from reading UI file 'TrainingDataGenerationDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
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
    QLineEdit *lineEditXrot;
    QLabel *label_7;
    QLineEdit *lineEditXrotRange;
    QLabel *label_8;
    QLineEdit *lineEditYrot;
    QLabel *label_9;
    QLineEdit *lineEditYrotRange;
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

    void setupUi(QDialog *TrainingDataGenerationDialog)
    {
        if (TrainingDataGenerationDialog->objectName().isEmpty())
            TrainingDataGenerationDialog->setObjectName(QStringLiteral("TrainingDataGenerationDialog"));
        TrainingDataGenerationDialog->resize(537, 292);
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
        label_6->setGeometry(QRect(60, 160, 71, 16));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrot = new QLineEdit(TrainingDataGenerationDialog);
        lineEditXrot->setObjectName(QStringLiteral("lineEditXrot"));
        lineEditXrot->setGeometry(QRect(140, 160, 61, 20));
        label_7 = new QLabel(TrainingDataGenerationDialog);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(208, 160, 16, 20));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrotRange = new QLineEdit(TrainingDataGenerationDialog);
        lineEditXrotRange->setObjectName(QStringLiteral("lineEditXrotRange"));
        lineEditXrotRange->setGeometry(QRect(230, 160, 61, 20));
        label_8 = new QLabel(TrainingDataGenerationDialog);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(60, 190, 71, 16));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditYrot = new QLineEdit(TrainingDataGenerationDialog);
        lineEditYrot->setObjectName(QStringLiteral("lineEditYrot"));
        lineEditYrot->setGeometry(QRect(140, 190, 61, 20));
        label_9 = new QLabel(TrainingDataGenerationDialog);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(208, 190, 16, 20));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditYrotRange = new QLineEdit(TrainingDataGenerationDialog);
        lineEditYrotRange->setObjectName(QStringLiteral("lineEditYrotRange"));
        lineEditYrotRange->setGeometry(QRect(230, 190, 61, 20));
        pushButtonOK = new QPushButton(TrainingDataGenerationDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(330, 250, 91, 31));
        pushButtonCancel = new QPushButton(TrainingDataGenerationDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(430, 250, 91, 31));
        label_10 = new QLabel(TrainingDataGenerationDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(60, 220, 71, 16));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFovMin = new QLineEdit(TrainingDataGenerationDialog);
        lineEditFovMin->setObjectName(QStringLiteral("lineEditFovMin"));
        lineEditFovMin->setGeometry(QRect(140, 220, 61, 20));
        lineEditFovMax = new QLineEdit(TrainingDataGenerationDialog);
        lineEditFovMax->setObjectName(QStringLiteral("lineEditFovMax"));
        lineEditFovMax->setGeometry(QRect(230, 220, 61, 20));
        label_11 = new QLabel(TrainingDataGenerationDialog);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(203, 220, 16, 20));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_12 = new QLabel(TrainingDataGenerationDialog);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(60, 130, 71, 16));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radioButtonCameraTypeStreetView = new QRadioButton(TrainingDataGenerationDialog);
        radioButtonCameraTypeStreetView->setObjectName(QStringLiteral("radioButtonCameraTypeStreetView"));
        radioButtonCameraTypeStreetView->setGeometry(QRect(140, 130, 82, 17));
        radioButtonCameraTypeAerialView = new QRadioButton(TrainingDataGenerationDialog);
        radioButtonCameraTypeAerialView->setObjectName(QStringLiteral("radioButtonCameraTypeAerialView"));
        radioButtonCameraTypeAerialView->setGeometry(QRect(240, 130, 82, 17));
        checkBoxCentering = new QCheckBox(TrainingDataGenerationDialog);
        checkBoxCentering->setObjectName(QStringLiteral("checkBoxCentering"));
        checkBoxCentering->setGeometry(QRect(460, 100, 70, 17));
        QWidget::setTabOrder(lineEditCGADirectory, pushButtonCGADirectory);
        QWidget::setTabOrder(pushButtonCGADirectory, lineEditOutputDirectory);
        QWidget::setTabOrder(lineEditOutputDirectory, pushButtonOutputDirectory);
        QWidget::setTabOrder(pushButtonOutputDirectory, lineEditNumSamples);
        QWidget::setTabOrder(lineEditNumSamples, lineEditImageWidth);
        QWidget::setTabOrder(lineEditImageWidth, lineEditImageHeight);
        QWidget::setTabOrder(lineEditImageHeight, checkBoxGrayscale);
        QWidget::setTabOrder(checkBoxGrayscale, checkBoxCentering);
        QWidget::setTabOrder(checkBoxCentering, radioButtonCameraTypeStreetView);
        QWidget::setTabOrder(radioButtonCameraTypeStreetView, radioButtonCameraTypeAerialView);
        QWidget::setTabOrder(radioButtonCameraTypeAerialView, lineEditXrot);
        QWidget::setTabOrder(lineEditXrot, lineEditXrotRange);
        QWidget::setTabOrder(lineEditXrotRange, lineEditYrot);
        QWidget::setTabOrder(lineEditYrot, lineEditYrotRange);
        QWidget::setTabOrder(lineEditYrotRange, lineEditFovMin);
        QWidget::setTabOrder(lineEditFovMin, lineEditFovMax);
        QWidget::setTabOrder(lineEditFovMax, pushButtonOK);
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
        label_7->setText(QApplication::translate("TrainingDataGenerationDialog", "+/-", 0));
        label_8->setText(QApplication::translate("TrainingDataGenerationDialog", "Yrot:", 0));
        label_9->setText(QApplication::translate("TrainingDataGenerationDialog", "+/-", 0));
        pushButtonOK->setText(QApplication::translate("TrainingDataGenerationDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("TrainingDataGenerationDialog", "Cancel", 0));
        label_10->setText(QApplication::translate("TrainingDataGenerationDialog", "Field of view:", 0));
        label_11->setText(QApplication::translate("TrainingDataGenerationDialog", "~", 0));
        label_12->setText(QApplication::translate("TrainingDataGenerationDialog", "Camera type:", 0));
        radioButtonCameraTypeStreetView->setText(QApplication::translate("TrainingDataGenerationDialog", "Street view", 0));
        radioButtonCameraTypeAerialView->setText(QApplication::translate("TrainingDataGenerationDialog", "Aerial view", 0));
        checkBoxCentering->setText(QApplication::translate("TrainingDataGenerationDialog", "Centering", 0));
    } // retranslateUi

};

namespace Ui {
    class TrainingDataGenerationDialog: public Ui_TrainingDataGenerationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRAININGDATAGENERATIONDIALOG_H
