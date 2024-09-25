import socket

def get_text():
    text=client.recv(2040)  #receive the text

    if text.find('PING'.encode()) != -1:                      
        client.send('PONG ' + text.split() [1] + '\r\n'.encode()) 

    return text

HOST = 'localhost'
PORT = 6667
ident = "bot"
channel = "#test"
client = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
client.connect((HOST, PORT))
client.send(f"NICK {ident}\r\n".encode('utf-8'))
client.send(f"USER {ident} 0 * :realname\r\n".encode('utf-8'))
client.send(f"JOIN {channel}\r\n".encode('utf-8'))
from_server = client.recv(4096)

while 1:
    text = get_text()
    print(text)
    if "PRIVMSG".encode() in text and channel in text.decode() and ":hello\r\n'".encode() in text:
        client.send(f"{channel} Hello!".encode())

client.close()
print (from_server.decode())