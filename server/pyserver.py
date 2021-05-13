import socket, struct

UDP_LOCAL_IP = 'aaaa::1'
UDP_LOCAL_PORT = 5678


UDP_REMOTE_IP = 'bbbb::c30c:0:0:1'
UDP_REMOTE_PORT = 60001


try:
    socket_rx = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    socket_rx.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    socket_rx.bind((UDP_LOCAL_IP, UDP_LOCAL_PORT))
except Exception:
    print "ERROR: Server Port Binding Failed"

print 'UDP server ready: %s'% UDP_LOCAL_PORT
print

while True:
#    //data, addr = socket_rx.recvfrom(1024)
#    //print "address : ", addr
#    //print "received message: ", data
#    //print "\n"
    socket_rx.sendto("Hello from server\n", (UDP_REMOTE_IP, UDP_REMOTE_PORT))
