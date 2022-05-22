from PyQt5.QtCore import *


class Sig(QObject):
    """
    消息管理线程
    用于消息管理器与界面管理器之间的通信
    """
    senmsg = pyqtSignal(object)

    def __init__(self):
        super(Sig, self).__init__()

    def run(self, argv: str):
        # 发送界面切换信号
        self.senmsg.emit(argv)

