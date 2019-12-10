# import socket programming library 
import socket
# https://gist.github.com/zerobell-lee/8204110d5be881aeb27eb5ce248ee718#file-server_1-py
# import thread module 
from _thread import *
import threading

print_lock = threading.Lock()

def send(sock):
    while True:
        sendData = input('>>>')
        sock.send(sendData.encode('utf-8'))


def receive(sock):
    while True:
        recvData = sock.recv(1024)
        print(recvData)
        # print('상대방 :', recvData.decode('utf-8'))
        print_str = recvData.decode() + " [from server]"
        # print(recvData)
        sock.send(print_str.encode())
        # sock.send((recvData.decode('utf-8') +  "from server"))

# thread function
def threaded(c):
    while True:

        # data received from client 
        data = c.recv(1024)
        print("received data:", data)
        if not data:
            print('Bye')

            # lock released on exit 
            print_lock.release()
            break

        # reverse the given string from client 
        data = data[::-1]

        # send back reversed string to client 
        c.send(data)

        # connection closed
    c.close()


def Main():
    host = ""

    # reverse a port on your computer 
    # in our case it is 12345 but it 
    # can be anything 
    port = 10200
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    print("socket binded to port", port)


    # put the socket into listening mode 
    s.listen(5)
    print("socket is listening")

    # a forever loop until client wants to exit 
    while True:
        # establish connection with client
        c, addr = s.accept()

        # lock acquired by client 
        print_lock.acquire()
        print('Connected to :', addr[0], ':', addr[1], type(c))

        # Start a new thread and return its identifier 
        # start_new_thread(threaded, (c,))
        start_new_thread(send, (c,))
        start_new_thread(receive, (c,))
    s.close()



if __name__ == '__main__':
    Main() 