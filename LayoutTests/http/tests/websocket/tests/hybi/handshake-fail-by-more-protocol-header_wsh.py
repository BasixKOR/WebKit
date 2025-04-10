from pywebsocket3 import handshake
from pywebsocket3.handshake.hybi import compute_accept_from_unicode


def web_socket_do_extra_handshake(request):
    msg = b'HTTP/1.1 101 Switching Protocols\r\n'
    msg += b'Upgrade: websocket\r\n'
    msg += b'Connection: Upgrade\r\n'
    msg += b'Sec-WebSocket-Accept: %s\r\n'
    msg += compute_accept_from_unicode(request.headers_in['Sec-WebSocket-Key'])
    msg += b'\r\nSec-WebSocket-Protocol: MatchProtocol\r\n'
    msg += b'Sec-WebSocket-Protocol: MismatchProtocol\r\n'
    msg += b'\r\n'
    request.connection.write(msg)
    raise handshake.AbortedByUserException('Abort the connection') # Prevents pywebsocket from sending its own handshake message.


def web_socket_transfer_data(request):
    pass
