import sys,socket
import time
from server import Server
from utils import *
TIMEOUT = 1
if __name__=='__main__':
    args = process_args(sys.argv[1:])
    constructor_args = constructor_args(args)

    server = Server(**constructor_args)
    print('Chosen parameters: Server IP->'+str(server.server_ip) +
           ' | Server UDP port->'+str(server.server_port) + 
           ' | Maximum UDP payload size->'+str(server.segment_size)
          )
    
    to_send = 'src/server_assets/'
    flag = False
    file = None
    filename = None
    #initial request from client
    type_,payload,_,_,addr = server.receive_request()
    #set timeout for ACKs
    server.socket.settimeout(TIMEOUT)
    #enforce that first message is a request
    if type_ == message_type.REQUEST_MESSAGE:
        filename = to_send+payload
        with open(filename,'rb') as f:
            file = f.read()
    else:
        print('First message needs to be a request')
        exit()
    #split file to send into equal chunks of a byte size of segment_size
    chunks = [file[i:i + server.segment_size] for i in range(0, len(file), server.segment_size)]
    start = time.time()

    print(f'There are {len(chunks)} chunks to send')
    #transmission loop
    for i, chunk in enumerate(chunks):
        #check if last chunk to send
        flag = (i == len(chunks) - 1)
        server.send_chunk(chunk,addr,flag)
        #try to receive a message from client before timeout
        count = 1
        while(count<=3):
            try:
                server.try_to_accept_ack(filename=file,chunk=chunk,flag=flag)
                break
            except socket.timeout:
                print(f'Client has timed out. Attempting retransmission #{count}')
                server.send_chunk(chunk,addr,flag)
                count+=1
            except Exception as e:
                print('An unexpected error occured.')
                print(f'Error: {e}')
        if count==4:
            print('Failed to continue transaction with client after 3 retransmissions. Aborting.')
            break
    server.socket.close()
    print('End of the server')
    end = time.time()
    completion = end-start
    print(f'The transfer took {completion}s.')
