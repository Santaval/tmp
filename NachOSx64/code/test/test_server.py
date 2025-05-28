import socket
import time
s = socket.socket()
s.bind(('0.0.0.0', 8080))
s.listen(1)

print("[+] Server ready for NachOS connections...")
conn, addr = s.accept()
with conn:
    print("[+] Connected by", addr)
    while True:
        data = conn.recv(20)
        print("[+] Client from NachOS: ", data.decode(errors='replace'))
        if data.decode(errors='replace') == "exit":
            break
        message = "Hello from server!!\n"
        conn.sendall(message.encode())

    conn.close()
