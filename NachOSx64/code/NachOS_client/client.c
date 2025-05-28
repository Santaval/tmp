#include "syscall.h"

void showHelp();
int stringToInt(const char* str, int length);
int isCommand(const char*, const char*);

int main() {
int id;
	char serverIP[32];
	char portBuffer[6];
	int server_port = 5051;

	// Ask for IP
	Write("Enter server IP: ", 18, ConsoleOutput);
	int ipLength = Read(serverIP, 32, ConsoleInput);
	serverIP[ipLength] = '\0';

	// Ask for port
	Write("Enter server port: ", 20, ConsoleOutput);
	int portLength = Read(portBuffer, 6, ConsoleInput);
	portBuffer[portLength] = '\0';
	server_port = stringToInt(portBuffer, portLength);

	// Crear socket y conectar
	id = Socket(AF_INET_NachOS, SOCK_STREAM_NachOS);
	if (id == -1) {
		Write("Failed to create socket\n", 25, ConsoleOutput);
		Exit(0);
		return;
	}
	int con = Connect(id, serverIP, server_port);
	if (con == -1) {
		Write("Failed to connect to server\n", 29, ConsoleOutput);
		Exit(0);
		return;
	}
	Write("[-] Connected to server\n", 25, ConsoleOutput);
	Write("Type 'help' for commands\n",26,ConsoleOutput);
	int running = 1;
	char request[ 128 ];

	while (running) {
		Write( "[-] Type a request: ", 21, ConsoleOutput);
		int inputBytes = Read(request, 128, ConsoleInput);
		if (inputBytes > 0) {
			if (isCommand(request, "exit") == 1) {
				// let server know that client disconnected
				Write(request, inputBytes, id);
				running = 0;
				
			} else if (isCommand(request, "help") == 1) {
				showHelp();
			} else {				
				// sending request to server
				Write(request, inputBytes, id);
				char response[128];
				int done = 0;
				Write("[-] Response from Server:\n", 27, ConsoleOutput);
				// read server response part by part, until reads <END>
				while (done == 0) {
					int n = Read(response, 128, id);
					if (n <= 0) {
						Write("[-] Server disconnected\n", 25, ConsoleOutput);
						done = 1;
						running = 0;
					} else {
						if (n >= 5 && response[n-5] == '<' && response[n-4] == 'E') {
							Write(response, n - 5, ConsoleOutput);
							Write("\n", 1, ConsoleOutput);
							done = 1;  // detecting <END> flag from server
						} else {
							Write(response, n, ConsoleOutput);
						}					
					}				
				}	
			} 
		}	
	} 
	Exit(0);
	return;

}

void showHelp() {
	Write("Available commands:\n",22,ConsoleOutput);
	Write("directory --- GET /resources\n",30,ConsoleOutput);
	Write("file --- GET /resources/*.txt\n",31,ConsoleOutput);
	return;
}

int isCommand(const char* request, const char* command) {
    int i = 0;
    while (request[i] != '\0' && command[i] != '\0') {
        if (request[i] != command[i]) return -1;
        i++;
    }
    // if both end at the same time, they are equals
    return 1;
}

int stringToInt(const char* str, int length) {
	int i = 0;
	int result = 0;
	while (i < length && str[i] >= '0' && str[i] <= '9') {
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return result;
}

