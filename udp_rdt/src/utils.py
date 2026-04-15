import sys
from enum import Enum
IP = '0.0.0.0'
PORT = 8000
SEG_SIZE = 256

class message_type(Enum):
    """
    To make message type size in bytes constant since REQUEST and ACK are not the same length as text
    """
    REQUEST_MESSAGE = 0
    DATA_MESSAGE = 1
    ACK_MESSAGE = 2
    ERROR_MESSAGE = 3

class Udp_packet:
    """
    Define a custom udp message to handle connection id, sequence numbers and flag
    """
    def __init__(self, type_ = '', payload = '', connection_id = 0, sequence_nb = 0,flag = False):
        self.type_ = type_
        self.payload_length = sys.getsizeof(payload)
        self.payload = payload
        self.connection_id = connection_id
        self.sequence_nb = sequence_nb
        self.flag = flag

    def unpack(self):
        return self.type_,self.payload_length,self.payload,self.connection_id,self.sequence_nb,self.flag
    
def process_args(argv):
    """
    Process the console arguments
    :param argv: the console arguments
    :return results: the parsed arguments as a dict
    """
    allowed = {
        "--ip": ("ip", str),
        "--port": ("port", int),
        "--filename": ("filename", str),
        "--segment-size": ("segment_size", int)
    }

    result = {name: None for name, _ in allowed.values()}
    i = 0
    while i < len(argv):
        arg = argv[i]
        if arg in allowed:
            name, converter = allowed[arg]
            if i + 1 < len(argv):
                raw_value = argv[i + 1]
                try:
                    result[name] = converter(raw_value)
                except ValueError:
                    result[name] = None
                i += 2
                continue
        i += 1
    return result

def constructor_args(args):
    constructor_args = {
        "server_ip": args["ip"],
        "server_port": args["port"],
        "segment_size": max(args["segment_size"], SEG_SIZE)
    }
    constructor_args = {k: v for k, v in constructor_args.items() if v is not None}
    return constructor_args
