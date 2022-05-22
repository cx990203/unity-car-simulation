/********************************************************************************
** Form generated from reading UI file 'opengl.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGL_H
#define UI_OPENGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>
#include <myopengl.h>

QT_BEGIN_NAMESPACE

class Ui_Opengl
{
public:
    QGridLayout *gridLayout;
    myOpenGl *openGLWidget;

    void setupUi(QWidget *Opengl)
    {
        if (Opengl->objectName().isEmpty())
            Opengl->setObjectName(QString::fromUtf8("Opengl"));
        Opengl->resize(546, 418);
        gridLayout = new QGridLayout(Opengl);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        openGLWidget = new myOpenGl(Opengl);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));

        gridLayout->addWidget(openGLWidget, 0, 0, 1, 1);


        retranslateUi(Opengl);

        QMetaObject::connectSlotsByName(Opengl);
    } // setupUi

    void retranslateUi(QWidget *Opengl)
    {
        Opengl->setWindowTitle(QCoreApplication::translate("Opengl", "Opengl", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Opengl: public Ui_Opengl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGL_H
