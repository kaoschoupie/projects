Possible arguments for Client script:
--ip:choose an IP address
--port:choose a port
--filename:choose the filename
--segment-size:choose maximum payload size

client.py is the definition of the client and its methods
client_driver.py is the working implementation

server.py is the definition of the server and its methods
server_driver.py is the working implementation

utils.py is a utilies file where a Udp_packet is defined for app layer rdt, global constants and console arguments processing


Part 1 commands:
py server_driver.py --segment-size 512 --ip localhost --port 8000
py client_driver.py --filename images.jpg --segment-size 512 --ip localhost --port 8000