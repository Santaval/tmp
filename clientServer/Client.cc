#include "Client.hh"

Client::Client(int serverPort, const char * ip) {
  this->serverPort = serverPort;
  this->myAddress = ip;
}

Client::~Client() {
	
}

void Client::start(const char* otherAddress) {
	//this->initSocket = new SSLSocket(false);

	this->initSocket = new Socket('s');

	if (this->initSocket == nullptr) {
		std::runtime_error( "Client::start");
		return;
	}
	if (this->initSocket->MakeConnection(otherAddress, this->serverPort) == 0) {
		printf("[-] Connected to server from %s\n", otherAddress);
		int stop_signal = 0;
		while (stop_signal == 0) { 
			stop_signal = this->makeRequests();
			if ( stop_signal == 1) {
				std::cout << "[-] Disconnecting from server\n";
			} else if (stop_signal == -1) {
				std::cout << "[-] Server closed connection\n";
			}
		}
		
	} else {
		printf("Connection to server failed\n");
	}
	this->initSocket->Close();
}

int Client::makeRequests(){
	std::string request;
	char response[MAX_SIZE];
	
	struct sockaddr_in clientAddr;
	memset( &clientAddr, 0, sizeof( clientAddr ) );
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(this->serverPort);
	int n = inet_pton( AF_INET, this->myAddress, &clientAddr.sin_addr ); 
	// changed IP to local address
	
	if ( 1 != n ) {
		std::cout << "Error converting from IP address\n";
		exit( 23 );
	}
	std::cout << "[-] Type request: ";
 	std::cin >> request;
	if (request == "exit") return 1; // exit signal

	const char* builtRequest = this->buildRequest(request);
	if (strcmp(builtRequest, "invalid") == 0) return 0; // request incorrect

	this->initSocket->sendTo((void *)builtRequest, strlen(builtRequest),
	 (void *) & clientAddr);
	n = this->initSocket->recvFrom((void *)response, MAX_SIZE, 
	 (void *) & clientAddr );
	if (n <= 0) return -1; // no answer from server

	response[n] = '\0';
	std::cout << "[-] Response from server: " << std::endl << response << std::endl;
	
	return 0;

}

const char* Client::buildRequest(std::string data){

	if (data == "/resources")  {	// asking for the whole directory
		return "GET /resources";
	} else {
		size_t found = data.find("/resources/");
		if (found != std::string::npos) {
			
			static std::string httpReq = "GET " + data;

			const char* array = httpReq.c_str();
			return array;
		}
	}
	std::cout << "[!] Invalid request" << std::endl;
	return "invalid";
}