import sys
import time
from client import Client
from utils import *

if __name__=='__main__':
    args = process_args(sys.argv[1:])
    filename = args["filename"]
    constructor_args = constructor_args(args)

    client = Client(**constructor_args)
    print('Chosen parameters: Server IP->'+str(client.server_ip) +
           ' | Server UDP port->'+str(client.server_port) + 
           ' | File name->'+str(filename) +
           ' | Maximum UDP payload size->'+str(client.segment_size)
          )
    client.socket.connect((client.server_ip,client.server_port))
    client.send_message(to_send=filename,type_=message_type.REQUEST_MESSAGE)
    packet_data = []
    is_last = False
    start = time.time()
    #reception loop
    while(is_last is not True):
        message_type,_,current_data,connection_id,sequence_nb,is_last = client.receive_message()
        #if message doesn't contain good data
        if message_type != message_type.DATA_MESSAGE or connection_id!=client.connection_id or sequence_nb != client.sequence_nb:
            print('Packet received has issues')
            last_data = packet_data[-1]
            client.handle_wrong_message(message_type,current_data,connection_id,sequence_nb,last_data)
        else:
            print('Accepting packet')
            packet_data.append(current_data)
            client.sequence_nb=abs(client.sequence_nb-1)
            client.send_message(type_=message_type.ACK_MESSAGE,to_send=message_type.ACK_MESSAGE)
    outputfile = 'src/client_assets/'+filename
    end = time.time()
    completion = end-start
    print(f'The transfer took {completion}s.')
    with open(outputfile,'wb') as f:
        for packet in packet_data:
            f.write(packet)