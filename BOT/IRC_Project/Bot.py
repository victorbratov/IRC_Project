import socket
import sys
import argparse


class IRCBot: #Bot class
    def __init__(self, host: str, port: int, nick: str, channel: str) -> None:
        self.host: str = host
        self.port: int = port
        self.nick: str = nick
        self.channel: str = channel
        self.client: socket.socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)

    def connect(self) -> None: #connect to server
        try:
            self.client.connect((self.host, self.port))
            self.send_command(f"NICK {self.nick}")
            self.send_command(f"USER {self.nick} 0 * :{self.nick}")
        except socket.error as e: #error handeling
            print(f"Error connecting to IRC server: {e}")
            sys.exit(1)

    def send_command(self, command: str) -> None: #command for sending messages
        self.client.send(f"{command}\r\n".encode('utf-8'))

    def join_channel(self) -> None: #joining channel
        data: str = input(f"Would you like to join the {self.channel} channel? y/n ").lower()
        if data == "y":
            self.send_command(f"JOIN {self.channel}")
            print(f"Joined {self.channel}")
        else:
            print("Leaving...")
            self.close_connection()

    def get_text(self) -> bytes: #recieving everything from server
        try:
            text: bytes = self.client.recv(2040)

            # Respond to PING messages with PONG
            if 'PING'.encode() in text:
                pong_response: str = f'PONG {text.split()[1].decode()}'
                self.send_command(pong_response)

            return text
        except socket.error as e:
            print(f"Socket error: {e}")
            self.close_connection()
            return b''  # Return an empty byte string on error

    def process_server_message(self, message: bytes) -> None: #decoder
        decoded_message: str = message.decode('utf-8').strip()
        print(f"Server: {decoded_message}")

    def listen(self) -> None: #uses other funcitons to get all the messages from the server
        while True:
            try:
                from_server: bytes = self.get_text()
                if from_server:
                    self.process_server_message(from_server)
            except KeyboardInterrupt:
                print("Interrupt received, closing connection.")
                self.close_connection()
                break

    def close_connection(self) -> None: #shuting down socket properly
        self.client.shutdown(socket.SHUT_RDWR)
        self.client.close()
        sys.exit(0)


def main() -> None: # main code calling functions
    parser: argparse.ArgumentParser = argparse.ArgumentParser()

    # Arguments for host, port, nickname, and channel
    parser.add_argument('-H', '--host', type=str, required=True, help="Server host")
    parser.add_argument('-P', '--port', type=int, required=True, help="Server port")
    parser.add_argument('-i', '--nick', type=str, default="SuperBot", help="Bot nickname/identifier (default: SuperBot)")
    parser.add_argument('-c', '--chan', type=str, default="#hello", help="IRC channel (default: #hello)")

    # Parse the command-line arguments
    args = parser.parse_args()

    # Create an instance of IRCBot and start the connection
    bot: IRCBot = IRCBot(host=args.host, port=args.port, nick=args.nick, channel=args.chan)

    # Connect to the server and join the channel
    bot.connect()
    #re do this as we can only currently join one channel
    bot.join_channel()

    # Start listening for messages
    bot.listen()

#call main code
if __name__ == "__main__":
    main()
