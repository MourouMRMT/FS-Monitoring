import sys
from PyQt5.QtWidgets import QApplication, QLabel, QVBoxLayout, QWidget
from PyQt5.QtCore import Qt, QUrl
from PyQt5.QtWebSockets import QWebSocket
import json

wsURL='ws://172.20.10.9/ws'

class MyWindow(QWidget):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
#========================A modifier============================#
		
        self.layout = QVBoxLayout(self)
        self.label1 = QLabel('Data 1:', self)
        self.label2 = QLabel('Data 2:', self)
        self.label3 = QLabel('Data 3:', self)
        self.label4 = QLabel('Data 4:', self)
        # Add more labels for other data points

        self.layout.addWidget(self.label1)
        self.layout.addWidget(self.label2)
        self.layout.addWidget(self.label3)
        self.layout.addWidget(self.label4)
        # Add more widgets for other data points

        self.setGeometry(300, 300, 300, 200)
        self.setWindowTitle('WebSocket Data Display')
        
#===============================================================#     
        self.show()
        self.initWebSocket()
  
    def initWebSocket(self):
        
        self.websocket = QWebSocket()
        self.websocket.url = QUrl(wsURL)
        self.websocket.connected.connect(self.onWebSocketConnected)
        self.websocket.error.connect(self.onWebSocketError)
        self.websocket.textMessageReceived.connect(self.onWebSocketMessageReceived)
        # Utilisez openUrl au lieu de open
        self.websocket.open(self.websocket.url)


    def onWebSocketConnected(self):
        print('WebSocket connected')

    def onWebSocketError(self, error_code):
        print(f'WebSocket error: {error_code}')

    def onWebSocketMessageReceived(self, message):
        print('Message received:', message)
        self.updateUI(message)

    def updateUI(self, data):
        # Update the UI labels with the received data
        myObj = json.loads(data)
        
#========================A modifier============================#    
  
        self.label1.setText(f'Temperature batterie: {myObj["temperaturebat"]}')
        self.label2.setText(f'Taux de charge batterie: {myObj["TauxdeCharge"]}')
        self.label3.setText(f'Feux stop: {myObj["etatfeuxstop"]}')
        self.label4.setText(f'Vitesse moteur: {myObj["vitmoteur"]}')
        # Update other widgets for additional data points
        
#===============================================================#        


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MyWindow()
    sys.exit(app.exec_())
