import sys
import os
import threading
import datetime
import copy
import numpy as np
import time
from PyQt5.QtGui import QPixmap, QImage

sys.path.append("..")
from PyQt5 import QtWidgets
from win.py_test import Ui_pyTest
from PIL import Image
import mmap
from utils.signal import Sig
from utils.NetWork import TcpServe, NavigationServer
import cv2

class PyTestWin(QtWidgets.QWidget, Ui_pyTest):
    def __init__(self):
        super(PyTestWin, self).__init__()
        self.setupUi(self)
        # ---------------------------------
        # 运行参数设置
        # ---------------------------------
        self.rawImage_stack = []        # 图片堆栈
        self.cameraPath = ''            # 图片路径
        # ---------------------------------
        # 设置控件颜色
        # ---------------------------------
        self.ip.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
        self.port.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
        # ---------------------------------
        # 创建tcp服务器线程
        # ---------------------------------
        self.tcp = TcpServe()
        self.ip.setText(self.tcp.ip_port[0])
        self.port.setText(f'{self.tcp.ip_port[1]}')
        # ---------------------------------
        # 创建navigation服务器连接
        # ---------------------------------
        self.navi_server = NavigationServer()
        self.NaviIp.setText(self.navi_server.ip_port[0])
        self.NaviPort.setText(f'{self.navi_server.ip_port[1]}')
        # ---------------------------------
        # 创建信号
        # ---------------------------------
        self.sig = Sig()
        self.sig.senmsg.connect(self.TestSigPro)
        # ---------------------------------
        # 创建测试子线程
        # ---------------------------------
        self.test_thread = threading.Thread(target=self.TestThread, name='test')
        self.test_thread.start()
        # ---------------------------------
        # 创建图像处理子线程
        # ---------------------------------
        self.image_pro_thread = threading.Thread(target=self.ImageProThread, name='image pro')
        self.image_pro_thread.start()

    def TestSigPro(self, argv: str):
        if argv == 'RecDataBrowserClear':
            self.RecDataBrowser.clear()
            self.RecDataBrowser.append('TCP Receive data:')
        elif argv == 'NaviRecDataBrowserClear':
            self.NaviRecDataBrowser.clear()
            self.NaviRecDataBrowser.append('Receive data:')

    # ------------------------------------------------------------------测试界面子线程------------------------------------------------------------------
    def TestThread(self):
        connect_flag_last = False   # tcp服务器连接标志位
        navi_connect_flag_last = False      # navigation服务器连接标志位
        msg_stack_len = 0       # 记录显示的消息数量
        navi_msg_stack_len = 0  # 记录显示的消息数量
        while True:
            # ---------------------------------
            # 判断当前tcp服务器是否有连接
            # ---------------------------------
            if connect_flag_last != self.tcp.connect_flag:
                if self.tcp.connect_flag:
                    self.ip.setStyleSheet("background-color: rgba(0, 255, 0, 100);")
                    self.port.setStyleSheet("background-color: rgba(0, 255, 0, 100);")
                else:
                    self.ip.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
                    self.port.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
                connect_flag_last = self.tcp.connect_flag
            if navi_connect_flag_last != self.navi_server.connect_flag:
                if self.navi_server.connect_flag:
                    self.NaviIp.setStyleSheet("background-color: rgba(0, 255, 0, 100);")
                    self.NaviPort.setStyleSheet("background-color: rgba(0, 255, 0, 100);")
                else:
                    self.NaviIp.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
                    self.NaviPort.setStyleSheet("background-color: rgba(255, 0, 0, 100);")
                navi_connect_flag_last = self.navi_server.connect_flag
            # ---------------------------------
            # 判断当前是否有收到消息
            # ---------------------------------
            # 常规tcp服务器连接测试
            if len(self.tcp.message_stack) > 0:
                t = datetime.datetime.now().strftime("%Y-%m-%d %H:%M.%S")
                # 获取顶端消息
                mes = self.tcp.message_stack[0]
                # 将消息根据结束符进行分割
                for m in mes.split('\t'):
                    if m == '':
                        continue
                    # 如果是图片信息，则对图片进行读取
                    if 'imageHead' in m:
                        cameraPath = r'../../__buffer__/' + m.split(':')[-1]
                        self.cameraPath = copy.deepcopy(cameraPath)
                        # 判断文件是否存在
                        if os.path.exists(cameraPath):
                            img = cv2.imread(cameraPath)
                            if img is not None:
                                # 将图像放入堆栈
                                self.rawImage_stack.append(copy.deepcopy(img))
                                # 原图读取与显示
                                qimg = QPixmap(ImageTransformToQPixmap(img, self.imageShowLabel.width(), self.imageShowLabel.height()))
                                self.imageShowLabel.setPixmap(qimg)
                    # 如果是位置速度消息，则进行转发
                    elif 'locHead' in m:
                        self.navi_server.SendMessage(m.split(':')[-1])
                    # 显示消息部分代码
                    if self.RecDataShowCheck.isChecked():
                        # 消息显示部分，如果checkbox有选中，则进行消息显示
                        # 如果消息过多，则自动清空
                        if msg_stack_len > 10000:
                            self.sig.run('RecDataBrowserClear')
                            msg_stack_len = 0
                        msg_stack_len += 1
                        # 添加下一条需要显示的消息
                        self.RecDataBrowser.append(f'{t}: {m}')
                        self.RecDataBrowser.moveCursor(QtWidgets.QTextBrowser.textCursor(self.RecDataBrowser).End)
                # 清除消息队列的第一条消息
                del self.tcp.message_stack[0]
            # navigation连接服务器测试
            if len(self.navi_server.message_stack) > 0:
                t = datetime.datetime.now().strftime("%Y-%m-%d %H:%M.%S")
                for m in self.navi_server.message_stack:
                    if self.NaviRecDataShowCheck.isChecked():
                        if navi_msg_stack_len > 10000:
                            self.sig.run('NaviRecDataBrowserClear')
                            navi_msg_stack_len = 0
                        navi_msg_stack_len += 1
                        # 添加下一条需要显示的消息
                        self.NaviDataRecShow.append(f'{t}: {m}')
                        self.NaviDataRecShow.moveCursor(QtWidgets.QTextBrowser.textCursor(self.NaviDataRecShow).End)
                # 清除消息栈区
                self.navi_server.message_stack.clear()

    # ------------------------------------------------------------------图像处理子线程------------------------------------------------------------------
    def ImageProThread(self):
        while True:
            index = len(self.rawImage_stack)
            if index > 0:
                img = copy.deepcopy(self.rawImage_stack[index - 1])
                self.rawImage_stack.clear()
                # ---------------------------------
                # opencv图像处理与显示
                # ---------------------------------
                gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)        # 获取灰度图
                otsu_threshold, image_result = cv2.threshold(gray_img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)  # 大津阈值法
                edges_image = cv2.Canny(image_result, 180, 240)  # canny卷积核
                self.RoadOpencv.setPixmap(QPixmap(ImageTransformToQPixmap(edges_image, self.RoadOpencv.width(), self.RoadOpencv.height())))
                # ---------------------------------
                # deeplab图像语义分割
                # ---------------------------------
                # pr_img = self.net.detect_image(Image.fromarray(np.uint8(img)))
                # self.RoadDeeplab.setPixmap(QPixmap(ImageTransformToQPixmap(pr_img, self.RoadDeeplab.width(), self.RoadDeeplab.height())))


def ImageTransformToQPixmap(img: np.ndarray, w, h) -> QPixmap:
    """
    将图像等比缩放，并输出qpixmap格式
    :param img: 输入图像（numpy.ndarry）
    :param w: 需要得到的图像的宽
    :param h: 需要得到的图像的高
    :return: 变化后得到的图像
    """
    # 获取图像大小参数
    bytesPerComponent = 1
    if len(img.shape) == 3:
        height, width, bytesPerComponent = img.shape
    else:
        height, width = img.shape
    # 计算各边缩放比例
    raw_rate, need_rate = height / float(width), h / float(w)
    # 计算需要添加边框的大小
    h_adds, w_adds = int((width * (h / float(w)) - height) / 2.0) if raw_rate < need_rate else 0, int((height / (h / float(w)) - width) / 2.0) if raw_rate > need_rate else 0
    # 给图片添加灰边
    img = cv2.copyMakeBorder(img, h_adds, h_adds, w_adds, w_adds, cv2.BORDER_CONSTANT, value=[127, 127, 127] if bytesPerComponent == 3 else 127)
    # 图像转换
    cv2.cvtColor(img, cv2.COLOR_BGR2RGB if bytesPerComponent == 3 else cv2.COLOR_GRAY2BGR, img)
    img = cv2.resize(img, (w, h))
    if len(img.shape) == 3:
        height, width, bytesPerComponent = img.shape
    else:
        height, width = img.shape
    bytesPerLine = bytesPerComponent * width
    image = QImage(img.data, width, height, bytesPerLine, QImage.Format_RGB888 if bytesPerComponent == 3 else QImage.Format_Grayscale8)
    return QPixmap.fromImage(image)
