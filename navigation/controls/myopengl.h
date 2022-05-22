#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <opencv2/opencv.hpp>



class myOpenGl : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
	Q_OBJECT
public:
	explicit myOpenGl(QWidget *parent = nullptr);
	void GetLadarData(float *x, float *y, float *z);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	QOpenGLShaderProgram ShaderProgram;
};
