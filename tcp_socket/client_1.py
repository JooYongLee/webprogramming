# Import socket module
import time
import cv2
import numpy as np
import os
import socket
import re

import threading

TRANSFER_NAME = "image"
REQUEST_IMAGE_INFO = "request_image_info"
REQUEST_IMAGE = "request_image"

is_image_transfer = False
is_image_size_transfer = False

key = 4


def send(sock):
    while True:
        sendData = input()
        # sendData += "\0"
        # print(sendData)
        # sock.sendall(encrypt(sendData.encode()))
        # sock.send(sendDat.encode(''))
        # sock.send(encrypt(sendData.encode('utf-8')))
        sock.send(sendData.encode('unicode_escape'))
        if sendData == TRANSFER_NAME:
            global is_image_transfer
            is_image_transfer = True
        # sock.send(sendData.encode('utf-8'))


def receive(sock):
    while True:
        recvData = sock.recv(1024)
        # print('상대방 :', decrypt(recvData).decode())
        # try:
            # print('상대방 :', recvData.decode('utf-8'))
        # try:
            # print(recvData)
        print('상대방 :', recvData.decode('unicode_escape'))
        global is_image_transfer

        decode_str = recvData.decode('unicode_escape')
        # print(decode_str.encode())
        # print()
        strip_decode_str = decode_str.rstrip('\x00')
        # print("---->", strip_decode_str, len(strip_decode_str))
        # print(type(decode_str), len(decode_str), len(SHOT_NAME))
        # strip_str = re.sub(r"\s+", "", decode_str)
        # print("--->>", "++", type(strip_str), len(strip_str))
        # print("status-->", is_image_transfer, decode_str, decode_str==SHOT_NAME,
        #       decode_str[len(SHOT_NAME):], "---->",decode_str[len(SHOT_NAME)+1].encode())
        # text = 'Hello\x00\x00\x00\x00'
        # >> > text.rstrip('\x00')
        # REQUEST_IMAGE_INFO = "request_image_info"
        # REQUEST_IMAGE = "request_image"
        global is_image_transfer
        path = "siba.jpg"
        path = "maxresdefault.jpg"
        if is_image_transfer:
            if strip_decode_str == REQUEST_IMAGE_INFO:
                # image_size = "122323"
                mystat = os.stat(path)

                image_size =  "{}".format(mystat.st_size)
                print("from os size ", image_size)
                sock.send(image_size.encode('unicode_escape'))
                print("image 쏜다!!! 받아라앗!!")
            elif strip_decode_str == REQUEST_IMAGE:

                with open(path, "rb") as f:
                    data = f.read()
                # np_data = np.frombuffer(np.array(data), np.uint8)


                # print(len(data), len(np_data.tobytes()))

                # image = img_np = cv2.imdecode(np_data, cv2.IMREAD_UNCHANGED)
                # cv2.imshow("", image)
                # cv2.waitKey()

                sock.send(data)

                # sock.send("shotted image".encode('unicode_escape'))
                is_image_size_transfer = False
            else:
                pass
            # 쏜 이후 상태 바꾼다

        else:
            print(strip_decode_str)

            # decode('unicode_escape').encode('utf-8')
        # except:
        #     pass
        #     print(recvData.decode())
            # print("---->", recvData)
            # raise ValueError

            # print(recvData)
        # print('상대방 :', recvData.decode('utf-8'))
        # print('상대방 :', recvData.decode('utf-8'))


def Main():
    # local host IP '127.0.0.1' 
    # host = '127.0.0.1'
    host = "192.168.25.19"

    # Define the port on which you want to connect 
    port = 10200

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)

    # connect to server on local computer y
    s.connect((host, port))

    sender = threading.Thread(target=send, args=(s,))
    receiver = threading.Thread(target=receive, args=(s,))

    sender.start()
    receiver.start()


    # message you send to server 
    message = "shaurya says geeksforgeeks"
    while True:
        time.sleep(1)

        # message sent to server 
        # s.send(message.encode('ascii'))

        # messaga received from server 
        # data = s.recv(1024)

        # print the received message 
        # here it would be a reverse of sent message 
        # print('Received from the server :', str(data.decode('ascii')))

        # ask the client whether he wants to continue 
        # ans = input('\nDo you want to continue(y/n) :')
        # if ans == 'y':
        #     continue
        # else:
        #     break
    # close the connection 
    s.close()


if __name__ == '__main__':
    Main() 