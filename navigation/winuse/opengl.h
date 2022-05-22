#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_opengl.h"


class Opengl : public QWidget
{
	Q_OBJECT

public:
	Opengl(QWidget *parent = Q_NULLPTR);

private:
	Ui::Opengl ui;
};



