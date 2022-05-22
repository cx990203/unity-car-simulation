/********************************************************************************
** Form generated from reading UI file 'navigation.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NAVIGATION_H
#define UI_NAVIGATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NavigationClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QTextBrowser *ServerDataRecShow;
    QTextBrowser *ServerDataSendShow;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *ServerIp;
    QLabel *label_5;
    QLabel *ServerPort;
    QCheckBox *ServerRecDataShowCheck;
    QLabel *label_3;
    QLabel *MapShow;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *NavigationClass)
    {
        if (NavigationClass->objectName().isEmpty())
            NavigationClass->setObjectName(QString::fromUtf8("NavigationClass"));
        NavigationClass->resize(1078, 575);
        centralWidget = new QWidget(NavigationClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        ServerDataRecShow = new QTextBrowser(groupBox_2);
        ServerDataRecShow->setObjectName(QString::fromUtf8("ServerDataRecShow"));
        ServerDataRecShow->setStyleSheet(QString::fromUtf8(""));

        gridLayout_3->addWidget(ServerDataRecShow, 2, 0, 1, 1);

        ServerDataSendShow = new QTextBrowser(groupBox_2);
        ServerDataSendShow->setObjectName(QString::fromUtf8("ServerDataSendShow"));
        ServerDataSendShow->setStyleSheet(QString::fromUtf8(""));

        gridLayout_3->addWidget(ServerDataSendShow, 4, 0, 1, 1);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_3->addWidget(label, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(11);
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_2);

        ServerIp = new QLabel(groupBox_2);
        ServerIp->setObjectName(QString::fromUtf8("ServerIp"));
        ServerIp->setFont(font);
        ServerIp->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 0, 0, 100);"));
        ServerIp->setFrameShape(QFrame::Panel);
        ServerIp->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(ServerIp);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_5);

        ServerPort = new QLabel(groupBox_2);
        ServerPort->setObjectName(QString::fromUtf8("ServerPort"));
        ServerPort->setFont(font);
        ServerPort->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 0, 0, 100);"));
        ServerPort->setFrameShape(QFrame::Panel);
        ServerPort->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(ServerPort);

        ServerRecDataShowCheck = new QCheckBox(groupBox_2);
        ServerRecDataShowCheck->setObjectName(QString::fromUtf8("ServerRecDataShowCheck"));
        ServerRecDataShowCheck->setChecked(false);

        horizontalLayout->addWidget(ServerRecDataShowCheck);


        gridLayout_3->addLayout(horizontalLayout, 0, 0, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_3->addWidget(label_3, 3, 0, 1, 1);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);

        MapShow = new QLabel(centralWidget);
        MapShow->setObjectName(QString::fromUtf8("MapShow"));
        MapShow->setMinimumSize(QSize(500, 500));
        MapShow->setFont(font);
        MapShow->setStyleSheet(QString::fromUtf8(""));
        MapShow->setFrameShape(QFrame::Panel);
        MapShow->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(MapShow, 0, 1, 1, 1);

        NavigationClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(NavigationClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1078, 23));
        NavigationClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(NavigationClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        NavigationClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(NavigationClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        NavigationClass->setStatusBar(statusBar);

        retranslateUi(NavigationClass);

        QMetaObject::connectSlotsByName(NavigationClass);
    } // setupUi

    void retranslateUi(QMainWindow *NavigationClass)
    {
        NavigationClass->setWindowTitle(QCoreApplication::translate("NavigationClass", "Navigation", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("NavigationClass", "Navigation connect test", nullptr));
        ServerDataRecShow->setHtml(QCoreApplication::translate("NavigationClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Receive data:</p></body></html>", nullptr));
        ServerDataSendShow->setHtml(QCoreApplication::translate("NavigationClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Send data:</p></body></html>", nullptr));
        label->setText(QCoreApplication::translate("NavigationClass", "receive", nullptr));
        label_2->setText(QCoreApplication::translate("NavigationClass", "Server ip:", nullptr));
        ServerIp->setText(QString());
        label_5->setText(QCoreApplication::translate("NavigationClass", "Server port:", nullptr));
        ServerPort->setText(QString());
        ServerRecDataShowCheck->setText(QCoreApplication::translate("NavigationClass", "Receive/Send Data Show", nullptr));
        label_3->setText(QCoreApplication::translate("NavigationClass", "send", nullptr));
        MapShow->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class NavigationClass: public Ui_NavigationClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NAVIGATION_H
