from PyQt5 import QtCore, QtGui
from PyQt5 import Qt
import sys
import socket
import threading
import cv2
import numpy as np
import time
from datetime import datetime

# Variables for holding information about connections
connections = []
total_connections = 0
# https://github.com/pricheal/python-client-server

# Client class, new instance created for each connected client
# Each instance has the socket and address that is associated with items
# Along with an assigned ID and a name chosen by the client
class Client(threading.Thread):
    def __init__(self, socket, address, id, name, signal):
        threading.Thread.__init__(self)
        self.socket = socket
        self.address = address
        self.id = id
        self.name = name
        self.signal = signal

    def __str__(self):
        return str(self.id) + " " + str(self.address)

    # Attempt to get data from client
    # If unable to, assume client has disconnected and remove him from server data
    # If able to and we get data back, print it in the server and send it back to every
    # client aside from the client that has sent it
    # .decode is used to convert the byte data into a printable string
    def run(self):
        concat_data = bytearray()
        strlen = 0
        to_taken_filesize = 0
        header_size = 260
        while self.signal:
            try:
                data = self.socket.recv(header_size)
                # concat_data += data
            except:
                print("Client " + str(self.address) + " has disconnected")
                self.signal = False
                connections.remove(self)
                break
            if data != "":
                # concat_data += data
                # print("receive :", len(data))
                strlen += len(data)
                # concat_data.append(data)
                concat_data += data
                # print("receive :", strlen)
                if strlen <= header_size:
                    decode_str = concat_data[:header_size].decode("utf-8")
                    msg = decode_str.strip("\x00").split(':')
                    to_taken_filesize = int(msg[1])

                elif strlen == to_taken_filesize + header_size:
                    print("complete data:", len(concat_data))
                    np_buffer = np.frombuffer(concat_data[header_size:], dtype=np.uint8)
                    image = cv2.imdecode(np_buffer, cv2.IMREAD_UNCHANGED)

                    now = datetime.now()
                    nowstr = now.strftime("%Y_%M_%S")
                    cv2.imwrite("load_image{}.png".format(nowstr), image)


                    # processing
                    self.socket.sendall(concat_data)
                    # cv2.imencode()

                    # for client in connections:
                    #     if client.id != self.id:
                    #         client.socket.sendall('complete'.encode())

                # print(strlen, to_taken_filesize + header_size, to_taken_filesize, header_size)
                # print("ID " + str(self.id) + ": " + str(data.decode("utf-8")))
                # print("ID " + str(self.id))
                # self.translateMsg()
                # for client in connections:
                #     if client.id != self.id:
                #         client.socket.sendall(data)
    def image_encoding_write(self, image):
        rszimg = cv2.resize(image, (image.shape[1] // 10, image.shape[0] // 10))
        encode_buffer = cv2.imencode(".png", rszimg)
        result, img_ability_png = cv2.imencode('.png', rszimg)
        print("encoded image:", encode_buffer)

        rsz_encode = cv2.imdecode(img_ability_png, cv2.IMREAD_UNCHANGED)
        cv2.imwrite("rszimag.png", rsz_encode)

        # print("receive complete:{}".format(len(concat_data)))




# Wait for new connections
def newConnections(socket):
    while True:
        sock, address = socket.accept()
        global total_connections
        connections.append(Client(sock, address, total_connections, "Name", True))
        connections[len(connections) - 1].start()
        print("New connection at ID " + str(connections[len(connections) - 1]))
        total_connections += 1


def socekt_server_run():
    # Get host and port
    # host = input("Host: ")
    # port = int(input("Port: "))
    host = ""
    port = 5560

    # Create new server socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host, port))
    sock.listen(5)

    # Create new thread to wait for connections
    newConnectionsThread = threading.Thread(target=newConnections, args=(sock,))
    newConnectionsThread.start()

class MainWindow(Qt.QMainWindow):

    def __init__(self, parent = None):
        Qt.QMainWindow.__init__(self, parent)
        socekt_server_run()
        self.show()
        # self.showMaximized()

def catch_exceptions(t, val, tb):
    # Qt.QMessageBox.critical(None,
    #                                "An exception was raised",
    #                                "Exception type: {}".format(t))
    raise RuntimeError("An exception was raised",
                                   "Exception type: {}".format(t))
    exit()
    old_hook(t, val, tb)
if __name__ == "__main__":
    old_hook = sys.excepthook
    sys.excepthook = catch_exceptions
    app = Qt.QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
