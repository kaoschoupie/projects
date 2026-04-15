import socket
import sys
import pickle
from utils import *
import random
class Client:
    def __init__(self,server_ip = IP,server_port = PORT,segment_size = SEG_SIZE):
        self.socket = socket.socket(family=socket.AF_INET,type = socket.SOCK_DGRAM)
        #self.socket.bind((server_ip,server_port))
        self.server_ip = server_ip
        self.server_port = server_port
        self.segment_size = segment_size
        self.connection_id = 0
        self.sequence_nb = 0

    def send_message(self,to_send = message_type.REQUEST_MESSAGE,type_ = message_type.REQUEST_MESSAGE):
        """
        Send a Udp_packet to the server
        :param connection_id: connection id for the whole transfer
        :param to_send: what to send(eg: filename, ACK or whatever is relevant to send)
        :param type_: type of message(eg: 'REQUEST','ACK')
        """
        print('Sending request')
        if self.connection_id == 0:#connection ID is only 0 when initializing client
            self.connection_id = random.randint(a=1,b=255)
        if sys.getsizeof(to_send)>self.segment_size:
            print('Payload too big for segment size')
        message = Udp_packet(type_ = type_,payload = to_send, connection_id = self.connection_id,sequence_nb = self.sequence_nb)
        payload = pickle.dumps(message)
        self.socket.sendto(payload,(self.server_ip,self.server_port))
        
    def receive_message(self):
        """
        Receive a message from the server and deal with it
        If message is a succesful data transmission, updates the sequence number(0 or 1)
        :return payload,flag: payload is encoded (see send_message()) and flag (True or False) indicates whether it's the last transmission
        """
        print('Receiving data')
        data,_ = self.socket.recvfrom(self.segment_size*2)
        decoded_udp_message = pickle.loads(data)
        return decoded_udp_message.unpack()

    def handle_wrong_message(self,type_,data,connection_id,sequence_nb,last_data):
        #if server sent an error
        if type_ is message_type.ERROR_MESSAGE:
            if data=='500':
                print('Server couldn\'t find requested file. Aborting')
            else:
                print('Received an unknown error from server. Aborting')
            sys.exit()    
        #if not an error but connectin ID doesn't match
        if connection_id!=self.connection_id:
            print('Connection ID received doesn\'t match expected connection ID. Aborting')
            sys.exit()
        #if not an error but sequence nb doesn't match
        if sequence_nb!=self.sequence_nb:
            print('Sequence number received doesn\'t match expected sequence number')
            #if if data received corresponds to the previous packet, assume server sent a duplicate packet on purpose
            if data == last_data:
                print('Retransmission from server detected')
                print('Conclusion: client ACK was lost in transit to server.')
                self.send_message(type_=message_type.ACK_MESSAGE,to_send=message_type.ACK_MESSAGE)
                print('Sent new ACK')
            #else it's BS so ignore and wait for new
            else:
                print('Dropping received packet, waiting for retransmission')