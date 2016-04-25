/********************************************************************************
** Form generated from reading UI file 'VisualizePredictedDataDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VISUALIZEPREDICTEDDATADIALOG_H
#define UI_VISUALIZEPREDICTEDDATADIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_VisualizePredictedDataDialog
{
public:
    QLabel *label;
    QLineEdit *lineEditDataDirectory;
    QPushButton *pushButtonDataDirectory;
    QPushButton *pushButtonCGADirectory;
    QLabel *label_2;
    QLineEdit *lineEditCGADirectory;
    QPushButton *pushButtonOK;
    QPushButton *pushButtonCancel;
    QLabel *label_3;
    QLineEdit *lineEditXrot;
    QLabel *label_4;
    QLineEdit *lineEditYrot;
    QLabel *label_5;
    QLabel *label_6;
    QLineEdit *lineEditXrotRange;
    QLineEdit *lineEditYrotRange;
    QLabel *label_10;
    QLineEdit *lineEditFovMin;
    QLineEdit *lineEditFovMax;
    QLabel *label_11;
    QRadioButton *radioButtonCameraTypeAerialView;
    QRadioButton *radioButtonCameraTypeStreetView;
    QLabel *label_12;

    void setupUi(QDialog *VisualizePredictedDataDialog)
    {
        if (VisualizePredictedDataDialog->objectName().isEmpty())
            VisualizePredictedDataDialog->setObjectName(QStringLiteral("VisualizePredictedDataDialog"));
        VisualizePredictedDataDialog->resize(538, 230);
        label = new QLabel(VisualizePredictedDataDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 41, 121, 16));
        lineEditDataDirectory = new QLineEdit(VisualizePredictedDataDialog);
        lineEditDataDirectory->setObjectName(QStringLiteral("lineEditDataDirectory"));
        lineEditDataDirectory->setGeometry(QRect(130, 41, 361, 20));
        pushButtonDataDirectory = new QPushButton(VisualizePredictedDataDialog);
        pushButtonDataDirectory->setObjectName(QStringLiteral("pushButtonDataDirectory"));
        pushButtonDataDirectory->setGeometry(QRect(495, 39, 31, 23));
        pushButtonCGADirectory = new QPushButton(VisualizePredictedDataDialog);
        pushButtonCGADirectory->setObjectName(QStringLiteral("pushButtonCGADirectory"));
        pushButtonCGADirectory->setGeometry(QRect(495, 9, 31, 23));
        label_2 = new QLabel(VisualizePredictedDataDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 121, 16));
        lineEditCGADirectory = new QLineEdit(VisualizePredictedDataDialog);
        lineEditCGADirectory->setObjectName(QStringLiteral("lineEditCGADirectory"));
        lineEditCGADirectory->setGeometry(QRect(130, 10, 361, 20));
        pushButtonOK = new QPushButton(VisualizePredictedDataDialog);
        pushButtonOK->setObjectName(QStringLiteral("pushButtonOK"));
        pushButtonOK->setGeometry(QRect(330, 190, 91, 31));
        pushButtonCancel = new QPushButton(VisualizePredictedDataDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(430, 190, 91, 31));
        label_3 = new QLabel(VisualizePredictedDataDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(80, 100, 41, 20));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrot = new QLineEdit(VisualizePredictedDataDialog);
        lineEditXrot->setObjectName(QStringLiteral("lineEditXrot"));
        lineEditXrot->setGeometry(QRect(130, 100, 61, 20));
        label_4 = new QLabel(VisualizePredictedDataDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(80, 130, 41, 20));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditYrot = new QLineEdit(VisualizePredictedDataDialog);
        lineEditYrot->setObjectName(QStringLiteral("lineEditYrot"));
        lineEditYrot->setGeometry(QRect(130, 130, 61, 20));
        label_5 = new QLabel(VisualizePredictedDataDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(197, 100, 16, 20));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_6 = new QLabel(VisualizePredictedDataDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(197, 130, 16, 20));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditXrotRange = new QLineEdit(VisualizePredictedDataDialog);
        lineEditXrotRange->setObjectName(QStringLiteral("lineEditXrotRange"));
        lineEditXrotRange->setGeometry(QRect(220, 100, 61, 20));
        lineEditYrotRange = new QLineEdit(VisualizePredictedDataDialog);
        lineEditYrotRange->setObjectName(QStringLiteral("lineEditYrotRange"));
        lineEditYrotRange->setGeometry(QRect(220, 130, 61, 20));
        label_10 = new QLabel(VisualizePredictedDataDialog);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(50, 160, 71, 16));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lineEditFovMin = new QLineEdit(VisualizePredictedDataDialog);
        lineEditFovMin->setObjectName(QStringLiteral("lineEditFovMin"));
        lineEditFovMin->setGeometry(QRect(130, 160, 61, 20));
        lineEditFovMax = new QLineEdit(VisualizePredictedDataDialog);
        lineEditFovMax->setObjectName(QStringLiteral("lineEditFovMax"));
        lineEditFovMax->setGeometry(QRect(220, 160, 61, 20));
        label_11 = new QLabel(VisualizePredictedDataDialog);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(193, 160, 16, 20));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        radioButtonCameraTypeAerialView = new QRadioButton(VisualizePredictedDataDialog);
        radioButtonCameraTypeAerialView->setObjectName(QStringLiteral("radioButtonCameraTypeAerialView"));
        radioButtonCameraTypeAerialView->setGeometry(QRect(230, 70, 82, 17));
        radioButtonCameraTypeStreetView = new QRadioButton(VisualizePredictedDataDialog);
        radioButtonCameraTypeStreetView->setObjectName(QStringLiteral("radioButtonCameraTypeStreetView"));
        radioButtonCameraTypeStreetView->setGeometry(QRect(130, 70, 82, 17));
        label_12 = new QLabel(VisualizePredictedDataDialog);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(50, 70, 71, 16));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        QWidget::setTabOrder(lineEditCGADirectory, pushButtonCGADirectory);
        QWidget::setTabOrder(pushButtonCGADirectory, lineEditDataDirectory);
        QWidget::setTabOrder(lineEditDataDirectory, pushButtonDataDirectory);
        QWidget::setTabOrder(pushButtonDataDirectory, radioButtonCameraTypeStreetView);
        QWidget::setTabOrder(radioButtonCameraTypeStreetView, radioButtonCameraTypeAerialView);
        QWidget::setTabOrder(radioButtonCameraTypeAerialView, lineEditXrot);
        QWidget::setTabOrder(lineEditXrot, lineEditXrotRange);
        QWidget::setTabOrder(lineEditXrotRange, lineEditYrot);
        QWidget::setTabOrder(lineEditYrot, lineEditYrotRange);
        QWidget::setTabOrder(lineEditYrotRange, lineEditFovMin);
        QWidget::setTabOrder(lineEditFovMin, lineEditFovMax);
        QWidget::setTabOrder(lineEditFovMax, pushButtonOK);
        QWidget::setTabOrder(pushButtonOK, pushButtonCancel);

        retranslateUi(VisualizePredictedDataDialog);

        QMetaObject::connectSlotsByName(VisualizePredictedDataDialog);
    } // setupUi

    void retranslateUi(QDialog *VisualizePredictedDataDialog)
    {
        VisualizePredictedDataDialog->setWindowTitle(QApplication::translate("VisualizePredictedDataDialog", "VisualizePredictedDataDialog", 0));
        label->setText(QApplication::translate("VisualizePredictedDataDialog", "Training data directory:", 0));
        pushButtonDataDirectory->setText(QApplication::translate("VisualizePredictedDataDialog", "...", 0));
        pushButtonCGADirectory->setText(QApplication::translate("VisualizePredictedDataDialog", "...", 0));
        label_2->setText(QApplication::translate("VisualizePredictedDataDialog", "CGA directory:", 0));
        pushButtonOK->setText(QApplication::translate("VisualizePredictedDataDialog", "OK", 0));
        pushButtonCancel->setText(QApplication::translate("VisualizePredictedDataDialog", "Cancel", 0));
        label_3->setText(QApplication::translate("VisualizePredictedDataDialog", "Xrot:", 0));
        label_4->setText(QApplication::translate("VisualizePredictedDataDialog", "Yrot:", 0));
        label_5->setText(QApplication::translate("VisualizePredictedDataDialog", "+/-", 0));
        label_6->setText(QApplication::translate("VisualizePredictedDataDialog", "+/-", 0));
        label_10->setText(QApplication::translate("VisualizePredictedDataDialog", "Field of view:", 0));
        label_11->setText(QApplication::translate("VisualizePredictedDataDialog", "~", 0));
        radioButtonCameraTypeAerialView->setText(QApplication::translate("VisualizePredictedDataDialog", "Aerial view", 0));
        radioButtonCameraTypeStreetView->setText(QApplication::translate("VisualizePredictedDataDialog", "Street view", 0));
        label_12->setText(QApplication::translate("VisualizePredictedDataDialog", "Camera type:", 0));
    } // retranslateUi

};

namespace Ui {
    class VisualizePredictedDataDialog: public Ui_VisualizePredictedDataDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VISUALIZEPREDICTEDDATADIALOG_H
