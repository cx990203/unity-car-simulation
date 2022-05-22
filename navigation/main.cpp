#include "navigation.h"
#include <QtWidgets/QApplication>
#include <fstream>
#include "opengl.h"

int main(int argc, char *argv[])
{
	/* 打开console输出信息 */
	/*AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);*/
	/* 创建qt程序运行环境，并打开窗口 */
    QApplication a(argc, argv);
	/* 导航主界面 */
    Navigation w;
	/* opengl界面 */
	/*myOpenGl opengl_win;*/
	/* 主界面操作 */
	//w.ConnectOpengl(&opengl_win);		//连接opengl界面
    w.show();			//主界面显示
    return a.exec();
}
