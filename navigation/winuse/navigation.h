#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_navigation.h"
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <QThread>
#include <queue>
#include <opencv2/opencv.hpp>
#include "Opengl.h"
#include "myopengl.h"
using namespace std;
using namespace cv;

#pragma comment(lib,"ws2_32")

class Navigation : public QMainWindow
{
    Q_OBJECT

public:
    Navigation(QWidget *parent = Q_NULLPTR);
	void ConnectOpengl(myOpenGl *win);

private slots:
	void WinUpdate(int);

private:
    Ui::NavigationClass ui;
	myOpenGl *opengl_win;
};

class Mainloop : public QThread
{
	Q_OBJECT
public:
	explicit Mainloop(QObject *parent = 0);

protected:
	void run();

signals:
	void WinUpdateSig(int);
};
