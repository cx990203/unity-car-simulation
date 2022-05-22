#include "myopengl.h"
#include "malloc.h"

using namespace std;
using namespace cv;

/*-------- 固定参数 --------*/
#define dis_max 10
#define resolution 100			//地图分辨率

/*-------- 存储参数 --------*/
unsigned char Map3d[resolution][resolution][resolution] = {};

/*-------- 程序运行参数 --------*/
float *map_show;
/* VBO和VAO对象 */
unsigned int VBO, VAO;
/* 数据定义 */
float buffer[] = {
	0.5, 0.5, 0
};
/*-------- 函数声明 --------*/


myOpenGl::myOpenGl(QWidget *parent)
	: QOpenGLWidget(parent)
{

}


/* 获取单次16线雷达扫描数据 */
void myOpenGl::GetLadarData(float * x, float * y, float * z)
{
	for (int i = 0; i < 16; i++) {
		if ((*(x + i) <= dis_max && *(x + i) >= -dis_max) &&
			(*(y + i) <= dis_max && *(y + i) >= -dis_max) &&
			(*(z + i) <= dis_max && *(z + i) >= -dis_max)) {
			/* 修改地图数据 */
			Map3d[resolution / 2 - int(resolution * (*(x + i)) / dis_max)][resolution / 2 - int(resolution * (*(y + i)) / dis_max)][resolution / 2 - int(resolution * (*(z + i)) / dis_max)] = 1;
		}
	}
}

/* opengl初始化函数 */
void myOpenGl::initializeGL()
{
	/* 申请一块控件用于存储需要显示的点 */
	map_show = (float*)malloc(resolution * resolution * resolution * 3 * sizeof(float));
	/* 初始化指针，将函数指针指向显卡提供的函数 */
	initializeOpenGLFunctions();
	/* 创建VBO、VAO对象，并赋予ID */
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	/* 绑定VBO和VAO对象 */
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	/* 复制索引数组到索引缓冲区当中 */
	glBufferData(GL_ARRAY_BUFFER, sizeof(map_show) , map_show, GL_STREAM_DRAW);
	/* 告知显卡如何解析缓冲里的属性值,即设置顶点属性指针 */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	/* 开启VAO管理的第一个属性值 */
	glEnableVertexAttribArray(0);

	/* link shaders */
	bool success;
	ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "./shaders/Vertex.txt");
	ShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "./shaders/Fragment.txt");
	success = ShaderProgram.link();
	if (!success) {
		printf("LINK ERR: %s", ShaderProgram.log());
	}
	/* 绑定着色器 */
	ShaderProgram.bind();

	/* 清空图像缓冲区 */
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	/* 释放申请的内存 */
	free(map_show);
}

/* opengl重新设置大小 */
void myOpenGl::resizeGL(int w, int h)
{

}

/* opengl绘制函数 */
void myOpenGl::paintGL()
{
	/* 申请一块控件用于存储需要显示的点 */
	map_show = (float*)malloc(resolution * resolution * resolution * 3 * sizeof(float));
	/* 找到需要显示的点 */
	unsigned int index = 0;
	for (int i = 0; i < resolution; i++)
		for (int j = 0; j < resolution; j++)
			for (int k = 0; k < resolution; k++)
				if (Map3d[i][j][k] == 1) {
					map_show[index * 3 + 0] = ((float)(resolution / 2 - i) / resolution);
					map_show[index * 3 + 1] = ((float)(resolution / 2 - j) / resolution);
					map_show[index * 3 + 2] = ((float)(resolution / 2 - k) / resolution);
					index++;
				}
	printf("index: %d\n", index);
	if (index == 0)
		glBufferData(GL_ARRAY_BUFFER, sizeof(map_show), map_show, GL_STREAM_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, index * sizeof(float), map_show, GL_STREAM_DRAW);
	glDrawArrays(GL_POINTS, 0, 16);
	/* 释放申请的内存 */
	free(map_show);
}

