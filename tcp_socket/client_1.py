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

REQUEST_STR = "giveme"
READY_COMPLETE = "ready_client"
is_image_request = False

is_image_transfer = False
is_image_size_transfer = False

to_taken_image_size = 0
key = 4
global_image_path = "siba.jpg"
# path = "maxresdefault.jpg"

def send(sock):
    while True:
        sendData = input()



        global  global_image_path
        if sendData == "image1":
            global_image_path = "siba.jpg"
        elif sendData == "image2":
            global_image_path = "maxresdefault.jpg"
        else:
            pass



        # sendData += "\0"
        # print(sendData)
        # sock.sendall(encrypt(sendData.encode()))
        # sock.send(sendDat.encode(''))
        # sock.send(encrypt(sendData.encode('utf-8')))
        global is_image_request
        if sendData[:5] == TRANSFER_NAME:
            global is_image_transfer
            is_image_transfer = True
            print("imgae transfer set TRUE")
            sock.send(TRANSFER_NAME.encode('unicode_escape'))
        elif sendData == REQUEST_STR:
            sock.send(REQUEST_STR.encode('unicode_escape'))
            is_image_request = True
        else:
            sock.send(sendData.encode('unicode_escape'))


        # sock.send(sendData.encode('utf-8'))


def receive(sock):
    while True:
        global is_image_transfer
        global is_image_size_transfer
        global is_image_request
        global to_taken_image_size
        global is_image_transfer
        # while True:
        print("받을 이미지 사이즈", to_taken_image_size, "image 요청 상태", is_image_request,
              "image transfoer상태", is_image_size_transfer)
        if is_image_request is True and to_taken_image_size > 0:
            recvData = sock.recv(to_taken_image_size)
            print("image buffer 받음요")
        else:
            recvData = sock.recv(1024)
            print('상대방 :', recvData.decode('unicode_escape'))


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

        path = global_image_path
        if is_image_transfer is True:
            print("is_image_transfer")
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
                sock.send(data)
                is_image_size_transfer = False

            else:
                print("뭐지",  strip_decode_str)
                if is_image_request is True:
                    if to_taken_image_size == 0:
                        try:
                            to_taken_image_size = int(strip_decode_str)
                        except:
                            print("faileed....", strip_decode_str)
                        print("전달 받을 이미지 사이즈", to_taken_image_size)
                        sock.send(READY_COMPLETE.encode('unicode_escape'))
                    else:
                        np_buffer = np.frombuffer(recvData, dtype=np.uint8)
                        decodeimage = cv2.imdecode(np_buffer, cv2.IMREAD_UNCHANGED)
                        cv2.imshow("", decodeimage)
                        cv2.waitKey()
                        is_image_request = False
                        to_taken_image_size = 0

                pass
            # 쏜 이후 상태 바꾼다
        elif is_image_request is True:
            print("is_image_request")
            if to_taken_image_size == 0:
                try:
                    to_taken_image_size = int(strip_decode_str)
                except:
                    print("faileed....", strip_decode_str)
                print("전달 받을 이미지 사이즈", to_taken_image_size)
                sock.send(READY_COMPLETE.encode('unicode_escape'))
            else:
                np_buffer = np.frombuffer(recvData, dtype=np.uint8)
                decodeimage = cv2.imdecode(np_buffer, cv2.IMREAD_UNCHANGED)
                cv2.imshow("", decodeimage)
                cv2.waitKey()
                is_image_request = False
                to_taken_image_size = 0

        else:
            print("recv..===else---")
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