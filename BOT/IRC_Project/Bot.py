import socket

#method called when needing to receive text from server
def get_text():
    text=client.recv(2040)  #receive the text

    if text.find('PING'.encode()) != -1:                      
        client.send(f'PONG {text.split() [1]} \r\n'.encode()) 

    return text

#initialising variables
HOST = 'localhost'
PORT = 6667
ident = "SuperBot"
channel = "#hello"

#connecting
client = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
client.connect((HOST, PORT))

#sending client details
client.send(f"NICK {ident}\r\n".encode('utf-8'))
client.send(f"USER {ident} 0 * :realname\r\n".encode('utf-8'))

#asking user if they want to join the channel
data = input("would you like to join the hello channel? y/n ")

if (data.lower() == "y"): #accpeting uppercase and lowercase from user
    client.send(f"JOIN {channel}\r\n".encode('utf-8')) #joins channel
    print("Joined")
else:
    print("Leaving")
    client.shutdown(1)
    client.close()
    exit() #leaving the and closing the socket

#displaying and saving initial server message
from_server = get_text()
print(from_server)
print ("Saved initial message")
serverMessage = from_server
while True: #keeping connection with the server
    try:
        from_server = get_text()
        print(from_server) # Display server messages

    except socket.error as e: #error handeling
        print(f"Socket error: {e}")
        client.shutdown(1)
        client.close() #closing after socket error
        break
    
