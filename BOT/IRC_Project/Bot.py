import socket
import sys
import argparse
import random
 
class IRCBot:  # Bot class
    def __init__(self, host: str, port: int, nick: str, channel: str) -> None:
        self.host: str = host
        self.port: int = port
        self.nick: str = nick
        self.channel: str = channel
        self.client: socket.socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        self.users: list = []  # Track users in the channel
 
    def connect(self) -> None:  # Connect to server
        try:
            self.client.connect((self.host, self.port))
            self.send_command(f"NICK {self.nick}")
            self.send_command(f"USER {self.nick} 0 * :{self.nick}")
        except socket.error as e:  # Error handling
            print(f"Error connecting to IRC server: {e}")
            sys.exit(1)
 
    def send_command(self, command: str) -> None:  # Command for sending messages
        self.client.send(f"{command}\r\n".encode('utf-8'))
 
    def join_channel(self) -> None:  # Join channel
        self.send_command(f"JOIN {self.channel}")
        print(f"Joined {self.channel}")
 
    def get_text(self) -> bytes:  # Receiving everything from server
        try:
            text: bytes = self.client.recv(2040)
            return text
        except socket.error as e:
            print(f"Socket error: {e}")
            self.close_connection()
            return b''  # Return an empty byte string on error
 
    def handle_private_message(self, message: str) -> None:  # Respond to private message with a random response
        sender = message.split('!')[0][1:]
        Responses = self.read_responses()
        random_response = random.choice(Responses)
        self.send_command(f"PRIVMSG {sender} :{random_response}")
        print(f"Sent a private message to {sender}: {random_response}")
 
    def read_responses(self) -> list:
        try:
            with open("Responses.txt", 'r') as file:
                lines = [line.strip() for line in file.readlines()]
            return lines
        except FileNotFoundError:
            print(f"Error: The file was not found.")
            return []
        except Exception as e:
            print(f"An error occurred: {e}")
            return []
 
    def process_server_message(self, message: bytes) -> None:
        decoded_message: str = message.decode('utf-8').strip()
        messages = decoded_message.split('\r\n')
        for msg in messages:
            if msg:
                self.handle_individual_message(msg)
 
    def handle_individual_message(self, message: str) -> None:
        print(f"Server: {message}")
 
        if '353' in message:
            self.handle_names_response(message)
        elif '366' in message:
            print(f"End of NAMES list. Active users: {self.users}")
        elif 'PRIVMSG'.lower() in message.lower():
            self.handle_commands(message)
        if f"PRIVMSG {self.nick}".lower() in message.lower():
            self.handle_private_message(message)
        elif 'JOIN'.lower() in message.lower():
            sender = message.split('!')[0][1:]
            if sender not in self.users:
                self.users.append(sender)
                print(f"User {sender} joined the channel. Active users: {self.users}")
        elif 'PART'.lower() in message.lower() or 'QUIT'.lower() in message.lower():
            sender = message.split('!')[0][1:]
            if sender in self.users:
                self.users.remove(sender)
                print(f"User {sender} left the channel. Active users: {self.users}")
 
    def handle_names_response(self, message: str) -> None:
        if '353' in message:
            user_list = message.split(':')[-1].split()
            for user in user_list:
                if user != self.nick and user not in self.users:
                    self.users.append(user)
            print(f"Updated users: {self.users}")
 
    def handle_commands(self, message: str) -> None:  # Handle specific commands (!hello, !slap)
        sender = message.split('!')[0][1:]
        if "!hello" in message.lower():
            self.send_command(f"PRIVMSG {self.channel} :Hello, {sender}!")
        elif "!slap" in message.lower():
            self.handle_slap(sender)
 
    def handle_slap(self, sender: str) -> None:
        if len(self.users) > 1:
            # Convert users list to a set to perform the subtraction
            possible_targets = list(set(self.users) - {sender})
            if possible_targets:
                target = random.choice(possible_targets)
                self.send_command(f"PRIVMSG {self.channel} :{sender} slaps {target} with a large trout!")
            else:
                self.send_command(f"PRIVMSG {self.channel} :No one to slap!")
        else:
            self.send_command(f"PRIVMSG {self.channel} :No other users available to slap!")
    
    def change_nick(self)-> None:
        NewNick = input("What would you like your new name to be? ")
        self.nick = NewNick
        self.send_command(f"NICK {NewNick}")
 
    def listen(self) -> None:  # Get all messages from the server
        while True:
            try:
                from_server: bytes = self.get_text()
                if 'PING'.encode() in from_server:
                    pong_response: str = f'PONG {from_server.split()[1].decode()}'
                    self.send_command(pong_response)
 
                if from_server:
                    self.process_server_message(from_server)
 
            except KeyboardInterrupt:
                print("Interrupt received, closing connection.")
                self.close_connection()
                break
 
    def close_connection(self) -> None:  # Shut down socket properly
        self.client.shutdown(socket.SHUT_RDWR)
        self.client.close()
        sys.exit(0)
 
def main() -> None:  # Main code calling functions
    parser: argparse.ArgumentParser = argparse.ArgumentParser()
 
    # Arguments for host, port, nickname, and channel
    parser.add_argument('-H', '--host', type=str, default="localhost", help="Server host")
    parser.add_argument('-P', '--port', type=int, default=6667, help="Server port")
    parser.add_argument('-i', '--name', type=str, default="SuperBot", help="Bot nickname/identifier (default: SuperBot)")
    parser.add_argument('-c', '--channel', type=str, default="#hello", help="IRC channel (default: #hello)")
 
    args = parser.parse_args()
 
    bot: IRCBot = IRCBot(host=args.host, port=args.port, nick=args.name, channel=args.channel)
 
    bot.connect()
 
    # Ask the user if they want to join the hello channel
    data: str = input(f"Would you like to join the {bot.channel} channel? y/n ").lower()
    if data == "y":
        data2 = input("Would you like to change your name? y/n ").lower()
        if data2 == "y": 
            bot.change_nick()
        else:
            print("continuing")
        bot.join_channel()
    else:
        print("Leaving...")
        bot.close_connection()
 
    # Start listening for messages
    bot.listen()
 
# Call main code
if __name__ == "__main__":
    main()
