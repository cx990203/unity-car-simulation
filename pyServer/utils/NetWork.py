import socket
import threading
import datetime
import asyncio
import time

buffer_size = 1024
close_message: str = 'close'
return_massage: str = 'return'


class TcpServe:
    def __init__(self):
        # ---------------------------------
        # 设置tcp服务器参数
        # ---------------------------------
        self.ip_port = ('127.0.0.1', 8080)
        self.back_log = 5
        # ---------------------------------
        # 初始化tcp服务器
        # ---------------------------------
        # 创建一个TCP套接字
        self.ser = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 套接字类型AF_INET, socket.SOCK_STREAM   tcp协议，基于流式的协议
        self.ser.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # 对socket的配置重用ip和端口号
        # 绑定端口号
        self.ser.bind(self.ip_port)  # 写哪个ip就要运行在哪台机器上
        # 设置半连接池
        self.ser.listen(self.back_log)  # 最多可以连接多少个客户端
        # ---------------------------------
        # 创建子线程，用于接收客户端消息
        # ---------------------------------
        self.tcp_thread = threading.Thread(target=self.TcpThread, name='tcp server')
        self.tcp_thread.start()
        # ---------------------------------
        # 参数设置
        # ---------------------------------
        # 连接标志位
        self.connect_flag = False
        # 消息队列
        self.message_stack = []

    # ------------------------------------------------------------------服务器子线程------------------------------------------------------------------
    def TcpThread(self):
        while True:
            self.connect_flag = False
            # 阻塞等待，创建连接
            con, address = self.ser.accept()  # 在这个位置进行等待，监听端口号
            self.connect_flag = True
            while True:
                try:
                    # 接受套接字的大小，怎么发就怎么收
                    msg = con.recv(buffer_size)
                    msg_str = msg.decode('utf-8')
                    if len(self.message_stack) < 100:
                        # 对消息栈区大小设置限制
                        self.message_stack.append(msg_str)
                    # 关闭这个端口的连接
                    if msg_str == close_message:
                        # 断开连接
                        con.close()
                except Exception as e:
                    break


class NavigationServer:
    def __init__(self):
        # ---------------------------------
        # 设置tcp服务器参数
        # ---------------------------------
        self.ip_port = ('127.0.0.1', 8081)
        self.back_log = 1
        # ---------------------------------
        # 初始化tcp服务器
        # ---------------------------------
        # 创建一个TCP套接字
        self.ser = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 套接字类型AF_INET, socket.SOCK_STREAM   tcp协议，基于流式的协议
        self.ser.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # 对socket的配置重用ip和端口号
        # 绑定端口号
        self.ser.bind(self.ip_port)  # 写哪个ip就要运行在哪台机器上
        # 设置半连接池
        self.ser.listen(self.back_log)  # 最多可以连接多少个客户端
        # 设置连接端口
        self.con, self.address = None, None
        # ---------------------------------
        # 创建子线程，用于接收客户端消息
        # ---------------------------------
        self.tcp_thread = threading.Thread(target=self.NavigationServerThread, name='navigation server')
        self.tcp_thread.start()
        # ---------------------------------
        # 创建子线程，用于发送消息
        # ---------------------------------
        self.tcp_send_thread = threading.Thread(target=self.NavigationServerSendThread, name='navigation server send')
        self.tcp_send_thread.start()
        # ---------------------------------
        # 参数设置
        # ---------------------------------
        # 连接标志位
        self.connect_flag = False
        # 消息队列
        self.message_stack = []

    def SendMessage(self, mes: str):
        if self.connect_flag:
            self.con.send(mes.encode('utf-8'))

    # ------------------------------------------------------------------服务器子线程------------------------------------------------------------------
    def NavigationServerThread(self):
        while True:
            self.connect_flag = False
            # 阻塞等待，创建连接
            self.con, self.address = None, None
            self.con, self.address = self.ser.accept()  # 在这个位置进行等待，监听端口号
            self.connect_flag = True
            while True:
                try:
                    # 接受套接字的大小，怎么发就怎么收
                    msg = self.con.recv(buffer_size)
                    msg_str = msg.decode('utf-8')
                    if len(self.message_stack) < 100:
                        # 对消息栈区大小设置限制
                        self.message_stack.append(msg_str)
                    # 关闭这个端口的连接
                    if msg_str == close_message:
                        # 断开连接
                        self.con.close()
                except Exception as e:
                    break

    def NavigationServerSendThread(self):
        while True:
            if self.connect_flag:
                # 如果连接连上了
                # self.SendMessage('data0.loc')
                time.sleep(1)

