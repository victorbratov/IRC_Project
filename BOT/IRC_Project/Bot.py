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
 
    def send_command(self, command: str) -> None:  # function for sending all commands
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
        random_response = random.choice(Responses) #getting random response
        self.send_command(f"PRIVMSG {sender} :{random_response}")
        print(f"Sent a private message to {sender}: {random_response}") 
 
    def read_responses(self) -> list: #function for getting random response from file
        try:
            with open("Responses.txt", 'r') as file: #read file
                lines = [line.strip() for line in file.readlines()] #gett all lines
            return lines
        except FileNotFoundError:
            print(f"Error: The file was not found.")
            return []
        except Exception as e:
            print(f"An error occurred: {e}")
            return []
 
    def process_server_message(self, message: bytes) -> None: #getting server messages
        decoded_message: str = message.decode('utf-8').strip()
        messages = decoded_message.split('\r\n')
        for msg in messages: #messages may be concatinated so splitting them is required 
            if msg:
                self.handle_individual_message(msg)
 
    def handle_individual_message(self, message: str) -> None: #dealing with individual messages
        print(f"Server: {message}")
 
        if '353' in message: #if message from server is names list
            self.handle_names_response(message)

        elif '366' in message:
            print(f"End of NAMES list. Active users: {self.users}") #if all names are through

        elif 'PRIVMSG'.lower() in message.lower(): #if message in channel
            self.handle_commands(message)

        if f"PRIVMSG {self.nick}".lower() in message.lower(): #if orivate message to bot is read
            self.handle_private_message(message)

        elif 'JOIN'.lower() in message.lower(): #if a user joins the channel they need to be added to the user list
            sender = message.split('!')[0][1:]
            if sender not in self.users:
                self.users.append(sender)
                print(f"User {sender} joined the channel. Active users: {self.users}")

        elif 'PART'.lower() in message.lower() or 'QUIT'.lower() in message.lower(): #if a user leaves the channel they must be removed from the user list
            sender = message.split('!')[0][1:]
            if sender in self.users:
                self.users.remove(sender)
                print(f"User {sender} left the channel. Active users: {self.users}")
 
    def handle_names_response(self, message: str) -> None: #adding users to the user list
        if '353' in message:
            user_list = message.split(':')[-1].split()
            for user in user_list:
                if user != self.nick and user not in self.users:
                    self.users.append(user)
            print(f"Updated users: {self.users}")
 
    def handle_commands(self, message: str) -> None:  # Handle specific commands (!hello, !slap, !kick)
        sender = message.split('!')[0][1:]
        if "!hello" in message.lower():
            self.send_command(random.choice(self.random_hellos(sender)))
        elif "!slap" in message.lower():
            # Check if a target is provided after the !slap command
            parts = message.split(' ')
            if len(parts) > 4:  # Expecting at least "!slap target"
                target = parts[4].strip()
                self.handle_slap(sender, target)
            else:
                self.handle_slap(sender)  # No target specified
        elif "!kick" in message.lower():#used to kick user
            parts = message.split(' ')
            if len(parts) > 4:
                target = parts[4].strip()
                self.kick_bot(sender,target)
            else:#error handeling
                self.send_command(f"PRIVMSG {self.channel} :Incorrect use of Kick")

    
    def random_hellos(self, sender) -> list: #random list of responses for !hello command
        random_res = [
            f"PRIVMSG {self.channel} :Hello, {sender}!",
            f"PRIVMSG {self.channel} :How do you do, {sender}?",
            f"PRIVMSG {self.channel} :Hey there, {sender}! How's it going?",
            f"PRIVMSG {self.channel} :Good to see you, {sender}!",
            f"PRIVMSG {self.channel} :What's up, {sender}?",
            f"PRIVMSG {self.channel} :Greetings, {sender}! Hope you're doing well!",
            f"PRIVMSG {self.channel} :Hi, {sender}! How are you today?",
            f"PRIVMSG {self.channel} :Hello, {sender}! Great to have you here!",
            f"PRIVMSG {self.channel} :Hey {sender}, what's new with you?",
            f"PRIVMSG {self.channel} :Yo {sender}, how's it hanging?",
            f"PRIVMSG {self.channel} :Hi there, {sender}! Nice to see you!",
            f"PRIVMSG {self.channel} :Hello, hello, {sender}! Welcome!"
        ]
        return random_res

    def random_responses(self, sender, target, random_target, choice: str) -> str: #random list of responses for !slap

        targeted_attack_array = [ #if !slap was targeted using nick
        f"PRIVMSG {self.channel} :WOOOOWWWWWW {sender} has slapped {target} with a FAT trout!",
        f"PRIVMSG {self.channel} :HOLLYYYY why did {sender} slap {target} with that trout!",
        f"PRIVMSG {self.channel} :OMG {sender} is crazy, they just slapped {target} with that HUGEEEEEEE trout!",
        f"PRIVMSG {self.channel} :Yikes! {sender} just gave {target} a wet slap with a giant trout!",
        f"PRIVMSG {self.channel} :WHAM! {sender} slapped {target} so hard with a trout, it’s still flopping!",
        f"PRIVMSG {self.channel} :That trout came out of nowhere! {sender} just smacked {target} with it!",
        f"PRIVMSG {self.channel} :Watch out! {sender} just unleashed a trout attack on {target}!",
        f"PRIVMSG {self.channel} :Did you see that? {sender} just gave {target} a fishy smack with a massive trout!",
        f"PRIVMSG {self.channel} :SPLASH! {target} just got slapped with a trout by {sender}—what a hit!",
        f"PRIVMSG {self.channel} :OUCH! {target} will never forget that trout slap from {sender}!",
        f"PRIVMSG {self.channel} :Whoa, {sender} just landed a trout smackdown on {target}—that’s gotta hurt!",
        f"PRIVMSG {self.channel} :The trout wars have begun! {sender} just slapped {target} with a slimy fish!",
        f"PRIVMSG {self.channel} :Fish slap alert! {target} got hit by {sender} with a huge trout, it's pandemonium!"
        ]

        random_attack_array = [ #if !slap was random
            f"PRIVMSG {self.channel} :OMGGG {sender} randomly slapped {random_target} with a GIANT trout!",
            f"PRIVMSG {self.channel} :This is crazy! {sender} randomly fwacked {random_target} with a shark of a trout!",
            f"PRIVMSG {self.channel} :Out of nowhere, {sender} just whacked {random_target} with a massive trout!",
            f"PRIVMSG {self.channel} :Whoa! {random_target} just got a surprise trout slap from {sender}!",
            f"PRIVMSG {self.channel} :Look out! {sender} just randomly fish-slapped {random_target} with a huge trout!",
            f"PRIVMSG {self.channel} :BAM! {random_target} got a totally unexpected trout smack from {sender}!",
            f"PRIVMSG {self.channel} :That’s wild! {sender} just unleashed a random trout slap on {random_target}!",
            f"PRIVMSG {self.channel} :Watch out, {random_target}! {sender} came out of nowhere with a trout slap!",
            f"PRIVMSG {self.channel} :No one saw it coming! {sender} randomly smacked {random_target} with a trout!",
            f"PRIVMSG {self.channel} :OUCH! {sender} just trout-slammed {random_target} out of the blue!",
            f"PRIVMSG {self.channel} :What just happened? {sender} randomly slapped {random_target} with a monster trout!",
            f"PRIVMSG {self.channel} :SLAP! {random_target} got blindsided by a trout from {sender}!",
            f"PRIVMSG {self.channel} :Total chaos! {sender} randomly slapped {random_target} with a flopping trout!"
        ]
        missed_attack = [ #if the user incorrectly used the slap
            f"PRIVMSG {self.channel} :{sender} missed their attack and hit themselves instead.",
            f"PRIVMSG {self.channel} :{sender} tried slapping {target} with a trout but missed and smacked themselves.",
            f"PRIVMSG {self.channel} :Oops! {sender} swung at {target} but ended up hitting themselves!",
            f"PRIVMSG {self.channel} :Yikes! {sender} missed {target} and slapped their own face with a trout!",
            f"PRIVMSG {self.channel} :Embarrassing! {sender} aimed at {target} but whacked themselves instead!",
            f"PRIVMSG {self.channel} :Whoops! {sender} tried to slap {target} but ended up slapping themselves!",
            f"PRIVMSG {self.channel} :Oh no! {sender} swung a trout at {target} and ended up smacking themselves!",
            f"PRIVMSG {self.channel} :What a fail! {sender} aimed at {target} but trout-smacked themselves!",
            f"PRIVMSG {self.channel} :{sender} flung the trout at {target} and it backfired—now they're the one hurting!",
            f"PRIVMSG {self.channel} :LOL! {sender} tried to hit {target} but ended up trout-slapping themselves instead!",
            f"PRIVMSG {self.channel} :Disaster! {sender} totally missed {target} and hit themselves with the trout!",
            f"PRIVMSG {self.channel} :{sender} got a bit too confident and missed {target}, slapping themselves!"
        ]

        #returns correct attack
        if choice == "Targ_attack":
            line = random.choice(targeted_attack_array)
        elif choice == "Rand_attack":
            line = random.choice(random_attack_array)
        elif choice == "Missed_attack":
            line = random.choice(missed_attack)
        return line

 
    def handle_slap(self, sender: str, target: str = None) -> None: #handles all the slap options
        if target and target in self.users and target != self.nick:  # Check if the target is in the user list
            self.send_command(self.random_responses(sender, target, None,"Targ_attack"))
        else:
            if target:  # Target is specified but not in the channel
                self.send_command(self.random_responses(sender, target, None, "Missed_attack"))
            elif len(self.users) > 2:
                # Select a random user to slap if no valid target is provided
                possible_targets = list(set(self.users) - {sender,self.nick})
                if possible_targets:
                    random_target = random.choice(possible_targets)
                    self.send_command(self.random_responses(sender, None, random_target,"Rand_attack"))
                else:
                    self.send_command(f"PRIVMSG {self.channel} :There is no one to slap here bro")
            else:
                self.send_command(f"PRIVMSG {self.channel} :You cant slap anyone")
    
    def kick_bot(self,sender, target)->None: #if the user uses !kick and the bot is this one
        if target == self.nick:
            self.send_command(f"PART {self.channel}")


    def change_nick(self)-> None: #If you want to change your nick beore entering the channel
        NewNick = input("What would you like your new name to be? ") #in command line
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
