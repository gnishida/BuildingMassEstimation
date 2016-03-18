/********************************************************************************
** Form generated from reading UI file 'MCMCConfigDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MCMCCONFIGDIALOG_H
#define UI_MCMCCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MCMCConfigDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEditIterations;
    QLineEdit *lineEditCGAFilename;
    QLineEdit *lineEditTargetFilename;
    QPushButton *pushButtonCGAFilename;
    QPushButton *pushButtonTargetFilename;
    QPushButton *pushButtonStart;
    QPushButton *pushButtonCancel;

    void setupUi(QDialog *MCMCConfigDialog)
    {
        if (MCMCConfigDialog->objectName().isEmpty())
            MCMCConfigDialog->setObjectName(QStringLiteral("MCMCConfigDialog"));
        MCMCConfigDialog->resize(452, 166);
        label = new QLabel(MCMCConfigDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 71, 16));
        label_2 = new QLabel(MCMCConfigDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 50, 81, 16));
        label_3 = new QLabel(MCMCConfigDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 80, 81, 16));
        lineEditIterations = new QLineEdit(MCMCConfigDialog);
        lineEditIterations->setObjectName(QStringLiteral("lineEditIterations"));
        lineEditIterations->setGeometry(QRect(110, 20, 91, 20));
        lineEditCGAFilename = new QLineEdit(MCMCConfigDialog);
        lineEditCGAFilename->setObjectName(QStringLiteral("lineEditCGAFilename"));
        lineEditCGAFilename->setGeometry(QRect(110, 50, 291, 20));
        lineEditTargetFilename = new QLineEdit(MCMCConfigDialog);
        lineEditTargetFilename->setObjectName(QStringLiteral("lineEditTargetFilename"));
        lineEditTargetFilename->setGeometry(QRect(110, 80, 291, 20));
        pushButtonCGAFilename = new QPushButton(MCMCConfigDialog);
        pushButtonCGAFilename->setObjectName(QStringLiteral("pushButtonCGAFilename"));
        pushButtonCGAFilename->setGeometry(QRect(410, 50, 31, 23));
        pushButtonTargetFilename = new QPushButton(MCMCConfigDialog);
        pushButtonTargetFilename->setObjectName(QStringLiteral("pushButtonTargetFilename"));
        pushButtonTargetFilename->setGeometry(QRect(410, 80, 31, 23));
        pushButtonStart = new QPushButton(MCMCConfigDialog);
        pushButtonStart->setObjectName(QStringLiteral("pushButtonStart"));
        pushButtonStart->setGeometry(QRect(270, 120, 81, 31));
        pushButtonCancel = new QPushButton(MCMCConfigDialog);
        pushButtonCancel->setObjectName(QStringLiteral("pushButtonCancel"));
        pushButtonCancel->setGeometry(QRect(360, 120, 81, 31));

        retranslateUi(MCMCConfigDialog);

        QMetaObject::connectSlotsByName(MCMCConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *MCMCConfigDialog)
    {
        MCMCConfigDialog->setWindowTitle(QApplication::translate("MCMCConfigDialog", "MCMCConfigDialog", 0));
        label->setText(QApplication::translate("MCMCConfigDialog", "#Iterations:", 0));
        label_2->setText(QApplication::translate("MCMCConfigDialog", "CGA filename:", 0));
        label_3->setText(QApplication::translate("MCMCConfigDialog", "Target filename:", 0));
        pushButtonCGAFilename->setText(QApplication::translate("MCMCConfigDialog", "...", 0));
        pushButtonTargetFilename->setText(QApplication::translate("MCMCConfigDialog", "...", 0));
        pushButtonStart->setText(QApplication::translate("MCMCConfigDialog", "Start", 0));
        pushButtonCancel->setText(QApplication::translate("MCMCConfigDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class MCMCConfigDialog: public Ui_MCMCConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MCMCCONFIGDIALOG_H
