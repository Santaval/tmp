#include "syscall.h"

int main() {
   int id;
   char a[ 128 ];

   id = Socket( AF_INET_NachOS, SOCK_STREAM_NachOS );
   
   Connect( id, "127.0.0.1", 8080);
   Write("[-] Connected to server\n", 25, ConsoleOutput);
   Write( "Hello from Client!!!", 21, id );
   
   int readBytes = Read(a, 128, id);
   if (readBytes > 0) {
      Write("[-] Server of Python: ", 23, ConsoleOutput);
      //Write(a, 128, ConsoleOutput);
   } else {
      Write("Error reading from socket\n", 26, ConsoleOutput);
   }

}