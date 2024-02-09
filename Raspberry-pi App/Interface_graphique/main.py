import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from gui import Ui_SplashScreen  

class MyApp(QMainWindow):
    def __init__(self):
        super().__init__()

        self.ui = Ui_SplashScreen()
        self.ui.setupUi(self)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MyApp()
    window.show()
    sys.exit(app.exec_())
