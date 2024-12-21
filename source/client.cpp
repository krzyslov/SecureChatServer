#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main()
{
	int sock = 0;	
	char buffer[1024] = {0};
	struct sockaddr_in server_address;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 1){
		cerr << "Socket creation error << " << endl;
		return -1;
	}

	server_address.sin_family 	= AF_INET;
	server_address.sin_port 	= htons(8080);


	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) == -1) {
		cerr << "Invalid address / Address not suported" << endl;
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1){
		cerr << "Connection failed" << endl;
		cout << "Enter any data to exit:";
		cin>> buffer;
		return -1;
	}

	cout << "Connection with server established" << endl;
	

	while(true)
	{
		read(sock, buffer, 1024);
		cout << ">Server Response: " << buffer << endl;
		cin>> buffer;
	
		if (strcmp(buffer,"exit") == 0 ){
			break;
		}
	
		send(sock, buffer, sizeof(buffer), 0);
	}

	
	
	close(sock);
	return 0;
}
