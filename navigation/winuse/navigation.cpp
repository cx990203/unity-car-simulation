#include "navigation.h"
#include <time.h>
#include <fstream>
#include <iostream>
#include <direct.h>



#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define buffer_size 1024

enum WinUpdateCmd
{
	ConnectStateChange = 0,			/* 连接状态改变 */
	GetMesFromServer,				/* 从服务器获取到了数据 */
	SendDataToServer,				/* 发送数据给服务器 */
	ShowMap2d,						/* 显示地图 */
	UpdateLadar16,					/* 更新16线雷达数据 */
};

/*-------- 标志位设置 --------*/
bool usingLadar16 = true;			//使用16线雷达

/*-------- 控件映射 --------*/
struct {
	QLabel *IPLabel;
	QLabel *PortLabel;
}Controls;

/*-------- socket参数设置 --------*/
SOCKET sockClient;
sockaddr_in addrServer = {};

/* 窗口运行参数设置 */
const char* client_ip = "127.0.0.1";		//服务器ip地址
const unsigned int client_port = 8081;		//服务器端口号
bool connect_flag = false;					//服务器连接标志位
const string file_type = ".loc";			//文件类型后缀
const string file_path = "../../../__buffer__/__loc__/";
const string file_path16 = "../../../__buffer__/__loc16__/";

/*-------- 程序运行参数 --------*/
//接收到的消息
typedef struct {
	queue<string> mes_queue;			//消息显示队列，用于显示
	queue<string> t_queue;				//时间队列，和消息显示队列同步
	queue<string> mes_pro;				//消息处理队列，将消息处理与消息显示分开操作
}MesRece_t;		
static MesRece_t MesRece;
//发送的消息
typedef struct {
	vector<string> stack;				//消息栈区
	vector<string> t_stack;				//时间栈区，和消息栈区同步
}MesSend_t;		
static MesSend_t MesSend;
queue<int> cmd_queue;					//界面命令队列
//地图
int Map2d_h = 500, Map2d_w = 500;
Mat Map2d(Map2d_h, Map2d_w, CV_8UC3);
float Map2dRuler = 0.3;					//一个像素等于多少米
//地图颜色信息
Scalar MapColor_wall(0, 0, 0);
Scalar MapColor_ground(255, 255, 255);
Scalar MapColor_map(127, 127, 127);
//位置信息
typedef struct {
	//车辆信息
	float x, y, z;					//位置信息
	float speed;					//速度信息
	float angle;					//旋转角度
	//激光雷达信息
	float hit_x, hit_y, hit_z;		//扫描到的点的位置
	//16线激光雷达信息
	float hit_x16[16], hit_y16[16], hit_z16[16];		//扫描到的点的位置
}locData_t;
static locData_t locData;

/*-------- 函数声明 --------*/
DWORD WINAPI ClientThread(LPVOID lpParam);			//客户端运行子线程
DWORD WINAPI ClientDataRecThread(LPVOID lpParam);	//客户端数据接收子线程
QString str2qstr(const string str);
void QClear(queue<string>& q);						//清空队列
void GetlocData(string path);						//获取locData
void Map2dInit();									//初始化地图
void DrawMap2d();									//绘制地图
QImage cvMat2QImage(const cv::Mat& mat);

/* Navigation构造函数 */
Navigation::Navigation(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	/* 设置控件属性 */
	ui.ServerIp->setText(QString(client_ip));
	char port_str[10];
	sprintf(port_str, "%d", client_port);
	ui.ServerPort->setText(QString(port_str));
	/* 创建socket客户端连接 */
	WSADATA wsaDate = {};
	WSAStartup(MAKEWORD(2, 2), &wsaDate);
	sockClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	addrServer.sin_family = PF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr(client_ip);
	addrServer.sin_port = htons(client_port);
	if (::connect(sockClient, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR) {
		;
	}
	else {
		connect_flag = true;			//设置连接标志位
		cmd_queue.push(ConnectStateChange);		//发送消息信号，表示可以已经连接
	}

	/* 初始化地图，将地图初始化为灰色 */
	Map2dInit();

	/* 创建界面运行主线程 */
	Mainloop *t = new Mainloop();
	QObject::connect(t, SIGNAL(WinUpdateSig(int)), this, SLOT(WinUpdate(int)));
	t->start();

	/* 创建客户端运行线程，主要用于消息发送 */
	HANDLE clientThread;
	DWORD clientThreadId;
	clientThread = CreateThread(NULL, NULL, ClientThread, NULL, 0, &clientThreadId);

	/* 创建客户端信息接接收子线程 */
	HANDLE clientDataRecdThread;
	DWORD clientDataRecThreadId;
	clientDataRecdThread = CreateThread(NULL, NULL, ClientDataRecThread, NULL, 0, &clientDataRecThreadId);

	/* 初始化运行指令 */
	cmd_queue.push(ShowMap2d);		//先默认显示初始地图
}

/* 界面更新槽函数 */
void Navigation::WinUpdate(int argv) {
	
	static int rece_index = 0, send_index = 0;
	if (argv == ConnectStateChange) {
		/* 如果连接成功，则修改控件颜色 */
		ui.ServerIp->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 100);"));
		ui.ServerPort->setStyleSheet(QString::fromUtf8("background-color: rgba(0, 255, 0, 100);"));
	}
	else if (argv == GetMesFromServer) {
		/* 如果消息队列当中有从服务器中获取的消息 */
		if (MesRece.mes_queue.empty())
			return;
		if (!ui.ServerRecDataShowCheck->isChecked())
			return;
		/* 显示一条消息 */
		ui.ServerDataRecShow->append(str2qstr(MesRece.t_queue.front() + MesRece.mes_queue.front()));
		/* 消息出列 */
		MesRece.mes_queue.pop();
		/* 如果消息过多，则进行清除 */
		rece_index++;
		if (rece_index > 1000) {
			ui.ServerDataRecShow->clear();
			rece_index = 0;
		}
	}
	else if (argv == SendDataToServer) {
		/* 显示发送给服务器的消息 */
		if (!ui.ServerRecDataShowCheck->isChecked())
			return;
		/* 显示最后一条消息 */
		ui.ServerDataSendShow->append(str2qstr(MesSend.t_stack[MesSend.t_stack.size() - 1] + MesSend.stack[MesSend.stack.size() - 1]));
		/* 如果消息过多，则进行清除 */
		send_index++;
		if (send_index > 1000) {
			ui.ServerDataSendShow->clear();
			send_index = 0;
		}
	}
	else if (argv == ShowMap2d) {
		/* 将地图从mat类型转为qpixmap */
		QPixmap imgmap = QPixmap::fromImage(cvMat2QImage(Map2d));
		/* 显示地图 */
		ui.MapShow->setPixmap(imgmap);
	}
	else if (argv == UpdateLadar16) {
		/* 更新16线雷达 */
		this->opengl_win->GetLadarData(locData.hit_x16, locData.hit_y16, locData.hit_z16);
		this->opengl_win->update();
	}
}

/* 连接opengli界面 */
void Navigation::ConnectOpengl(myOpenGl *win) {
	this->opengl_win = win;
	this->opengl_win->show();
}


/*-------------------------------------------------- 子线程处理 --------------------------------------------------*/
Mainloop::Mainloop(QObject *parent) :
	QThread(parent)
{
}

void Mainloop::run()
{
	/* 界面运行主线程 */
	while (true)
	{
		if (!cmd_queue.empty()) {
			/* 如果有命令没有执行完，则直接发送 */
			emit WinUpdateSig(cmd_queue.front());
			cmd_queue.pop();
		}
		if (!MesRece.mes_pro.empty()) {
			/* 如果消息接收队列中有消息未处理 */
			int ret;
			//获取文件名
			string filename = MesRece.mes_pro.front();
			//判断文件类型是否正确
			ret = filename.find(file_type);
			if (ret != -1) {
				/* 如果当前文件名是可以处理的文件，则读取文件内容 */
				string path;
				if (!usingLadar16)
					path = file_path + filename;
				else
					path = file_path16 + filename;
				/* 获取locData */
				GetlocData(path);
				/* 绘制图片 */
				DrawMap2d();
			}
			//消息出队列
			MesRece.mes_pro.pop();
		}
	}
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
	/* 客户端子线程运行主循环 */
	while (1) {
		if (!connect_flag) {
			/* 如果没有连接成功，则继续尝试连接服务器 */
			if (::connect(sockClient, (SOCKADDR*)&addrServer, sizeof(addrServer)) == SOCKET_ERROR)
				;
			else {
				connect_flag = true;			//设置连接标志位
				cmd_queue.push(ConnectStateChange);		//发送消息信号，表示可以已经连接
			}
			Sleep(1000);
		}
		else {
			///* 如果服务器连接成功了 */
			//string str = "navigation tcp connect test";
			///* 发送消息 */
			//::send(sockClient, str.c_str(), str.length(), 0);
			//MesSend.stack.push_back(str);
			////获取系统时间  
			//time_t now_time = time(NULL);
			////获取本地时间  
			//tm*  t_tm = localtime(&now_time);
			//MesSend.t_stack.push_back(asctime(t_tm));
			////告诉主线程已经发送消息给服务器了
			//cmd_queue.push(SendDataToServer);
			Sleep(500);
		}
	}
}

DWORD WINAPI ClientDataRecThread(LPVOID lpParam) {
	/* 客户端消息接收线程 */
	while (1) {
		if (connect_flag) {
			/* 如果连接上服务器 */
			char buf[buffer_size];
			int len = ::recv(sockClient, buf, buffer_size, NULL);
			string str = buf;
			//获取系统时间  
			time_t now_time = time(NULL);
			//获取本地时间  
			tm*  t_tm = localtime(&now_time);
			//将消息放入队列
			if (MesRece.mes_queue.size() > 1000) {
				QClear(MesRece.mes_queue);
				QClear(MesRece.t_queue);
			}
			MesRece.mes_queue.push(str);
			MesRece.mes_pro.push(str);
			MesRece.t_queue.push(asctime(t_tm));
			cmd_queue.push(GetMesFromServer);		//发送命令告知主程序消息已经放在队列里了
		}
	}
}

/*-------------------------------------------------- 工具函数 --------------------------------------------------*/
QString str2qstr(const string str)
{
	return QString::fromLocal8Bit(str.data());
}

void QClear(queue<string>& q) {
	/* 清空队列，且保持STL容器标准 */
	queue<string> empty;
	swap(empty, q);
}

/* 读取lodData */
void GetlocData(string path) {
	const char a = ',';		//数据间的分隔符
	int b, e;
	/* 创建文件输入流 */
	ifstream fi(path);
	/* 从输入流中读取数据 */
	string readbuf;
	if (!usingLadar16) {
		/* 获取第一行数据，为x, y, z */
		fi >> readbuf;
		b = readbuf.find(a);
		e = readbuf.rfind(a);
		locData.x = atof(readbuf.substr(0, b).c_str());
		locData.y = atof(readbuf.substr(b + 1, e - b - 1).c_str());
		locData.z = atof(readbuf.substr(e + 1, readbuf.length() - e).c_str());
		/* 获取第二行数据，为hit_x, hit_y, hit_z */
		fi >> readbuf;
		b = readbuf.find(a);
		e = readbuf.rfind(a);
		locData.hit_x = atof(readbuf.substr(0, b).c_str());
		locData.hit_y = atof(readbuf.substr(b + 1, e - b - 1).c_str());
		locData.hit_z = atof(readbuf.substr(e + 1, readbuf.length() - e).c_str());
	}
	else {
		/* 获取第一行数据，为x, y, z */
		fi >> readbuf;
		b = readbuf.find(a);
		e = readbuf.rfind(a);
		locData.x = atof(readbuf.substr(0, b).c_str());
		locData.y = atof(readbuf.substr(b + 1, e - b - 1).c_str());
		locData.z = atof(readbuf.substr(e + 1, readbuf.length() - e).c_str());
		/* 获取第二行数据，为hit_x, hit_y, hit_z */
		for (int i = 0; i < 16; i++) {
			fi >> readbuf;
			b = readbuf.find(a);
			e = readbuf.rfind(a);
			locData.hit_x16[i] = atof(readbuf.substr(0, b).c_str());
			locData.hit_y16[i] = atof(readbuf.substr(b + 1, e - b - 1).c_str());
			locData.hit_z16[i] = atof(readbuf.substr(e + 1, readbuf.length() - e).c_str());
		}
		locData.hit_x = locData.hit_x16[0];
		locData.hit_y = locData.hit_y16[0];
		locData.hit_z = locData.hit_z16[0];
		/* 添加命令，表示可以更新16线雷达数据了 */
		/*cmd_queue.push(UpdateLadar16);*/
	}
	/* 关闭文件 */
	fi.close();
}

/* 将实际位置映射到地图上 */
vector<int> Position2Map(const int in_x, const int in_z) {
	vector<int> res;

	int x, z;
	/* 位置映射 */
	x = int(Map2d_w / 2 - in_x / Map2dRuler);
	z = int(Map2d_h / 2 - in_z / Map2dRuler);
	/* 幅值限制 */
	x = x > Map2d_w ? Map2d_w : x;
	z = z > Map2d_h ? Map2d_h : z;
	x = x < 0 ? 0 : x;
	z = z < 0 ? 0 : z;

	res.push_back(x);
	res.push_back(z);

	return res;
}

/* 绘制地图 */
#define maxLadarDis 100		//雷达最大距离
void DrawMap2d() {
	/* 计算边界像素位置 */
	int hit2d_x, hit2d_z, max2d_x, max2d_z;
	int x, z;
	if (locData.hit_x == 0 && locData.hit_y == 0 && locData.hit_z == 0) {
		//如果坐标均为0，则说明没有碰撞
		hit2d_x = Map2d_w;
		hit2d_z = Map2d_h;
		max2d_x = Map2d_w;
		max2d_z = Map2d_h;
		return;
	}
	else {
		/* 如果有碰撞，则将碰撞点映射到地图上 */
		vector<int> p;
		/* 将hit坐标映射到地图像素上 */
		p = Position2Map(locData.hit_x, locData.hit_z);
		hit2d_x = p[0];
		hit2d_z = p[1];
		/* 将自身位置映射到地图像素上 */
		p = Position2Map(locData.x, locData.z);
		x = p[0];
		z = p[1];
	}
	/* 绘制自身点到边界的白色直线(可行使路面) */
	cv::Point p0 = cv::Point(hit2d_x, hit2d_z);
	cv::Point p1 = cv::Point(x, z);
	cv::line(Map2d, p0, p1, MapColor_ground, 3);
	/* 绘制边界(墙体) */
	cv::Point hit = cv::Point(hit2d_x, hit2d_z);
	cv::line(Map2d, hit, hit, MapColor_wall , 3);
	/* 添加命令，表示可以显示地图了 */
	cmd_queue.push(ShowMap2d);
}

QImage cvMat2QImage(const cv::Mat& mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}


	return QImage();
}

void Map2dInit() {
	vector<Mat> src;
	src.push_back(Mat::ones(Map2d_w, Map2d_h, CV_8UC1) * 127);
	src.push_back(Mat::ones(Map2d_w, Map2d_h, CV_8UC1) * 127);
	src.push_back(Mat::ones(Map2d_w, Map2d_h, CV_8UC1) * 127);
	merge(src, Map2d);
}
