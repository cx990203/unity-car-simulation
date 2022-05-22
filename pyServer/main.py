from PyQt5.QtWidgets import QApplication
from winpy.pyTestWin import PyTestWin

if __name__ == '__main__':
    import sys
    app = QApplication(sys.argv)
    win = PyTestWin()
    win.show()
    sys.exit(app.exec())

