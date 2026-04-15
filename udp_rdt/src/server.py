import socket
import sys
import pickle
from utils import *

class Server:
    def __init__(self,server_ip = IP,server_port = PORT,segment_size = SEG_SIZE):
        self.socket = socket.socket(family=socket.AF_INET,type = socket.SOCK_DGRAM)
        self.socket.bind((server_ip,server_port))
        self.server_ip = server_ip
        self.server_port = server_port
        self.segment_size = segment_size
        self.connection_id = 0
        self.sequence_nb = 0

    def receive_request(self, connection_id =0):
        """
        :return type_,payload,connection_id,sequence_nb,addr
        """
        print('Received request')
        data,addr = self.socket.recvfrom(self.segment_size*2)#receive socket message
        decoded_data = pickle.loads(data)#decode said message
        type_,payload_length,payload,connection_id,sequence_nb,flag = decoded_data.unpack()#define all attributes
        if type_==message_type.REQUEST_MESSAGE:#if type is request
            self.connection_id = connection_id
            self.sequence_nb = sequence_nb
        if type_==message_type.ACK_MESSAGE:
            self.sequence_nb = abs(self.sequence_nb-1)
        return type_,payload,connection_id,sequence_nb,addr
        
    def send_chunk(self,data_chunk,addr,flag=False):
        print('Sending data')
        message = Udp_packet(type_ = message_type.DATA_MESSAGE,payload = data_chunk, connection_id = self.connection_id,sequence_nb = self.sequence_nb,flag=flag)
        payload = pickle.dumps(message)
        self.socket.sendto(payload,addr)

    def try_to_accept_ack(self,filename,chunk,flag):
        """
        Try to accept ACK (expected use: in a try-catch loop for timeouts)
        """
        while(True):
            type_,payload,connection_id,sequence_nb,addr = self.receive_request()                   
            if type_ == message_type.ACK_MESSAGE:
                print('Received ACK')
                return
            elif payload == filename:
                self.send_chunk(chunk,addr,flag)