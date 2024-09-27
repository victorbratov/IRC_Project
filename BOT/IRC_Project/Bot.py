import socket
import sys

if len(sys.argv) <3:
        print("invalid argument")
        exit()
# Initializing variables
HOST: str = sys.argv[1]  # Server host provided as command line argument
PORT: int = int(sys.argv[2])  # Server port provided as command line argument
ident: str = "SuperBot"  # Bot nickname/identifier
channel: str = "#hello"  # IRC channel
activeUser : list=[] #list to keep track of the active users

# method called when needing to receive text from server
def get_text() -> bytes:
    text: bytes = client.recv(2040)  # receive the text

    if text.find('PING'.encode()) != -1:  # Check if 'PING' is present
        client.send(f'PONG {text.split()[1]} \r\n'.encode())  # Send back 'PONG'    
    return text

# Connecting to the IRC server
client: socket.socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
client.connect((HOST, PORT))

# Sending client details (NICK and USER commands)
client.send(f"NICK {ident}\r\n".encode('utf-8'))
client.send(f"USER {ident} 0 * :realname\r\n".encode('utf-8'))

# Asking user if they want to join the channel
data: str = input("Would you like to join the hello channel? y/n ")

if data.lower() == "y":  # Accepting uppercase and lowercase input from user
    client.send(f"JOIN {channel}\r\n".encode('utf-8'))  # Joins the channel
    print("Joined")
else:
    print("Leaving")
    client.shutdown(1)
    client.close()
    exit()  # Exiting and closing the socket if the user opts not to join

# Displaying and saving initial server message
from_server: bytes = get_text()
print(from_server)
print("Saved initial message")
serverMessage: bytes = from_server

# Keeping connection with the server in a loop
while True:
    try:
        from_server = get_text()
        print(from_server)  # Display server messages

        #keeping track of active users
        message=from_server.decode('utf-8', errors='ignore')
        if "JOIN" in message:
            user = message.split('!')[0][1:]  
            if user not in activeUser:
                activeUser.append(user)
                print(f"{user} joined. Active users: {activeUser}")

        elif "PART" in message or "QUIT" in message:
            user = message.split('!')[0][1:]  

            if user in activeUser:  # Check if user exists before removing the users
                activeUser.remove(user)
                print(f"{user} left. Active users: {activeUser}")
          


    except socket.error as e:  # Error handling
        print(f"Socket error: {e}")
        client.shutdown(1)
        client.close()  # Closing after a socket error
        break
