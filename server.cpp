#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <map>

#define PORT 8080
#define MAX_CLIENTS 20

using namespace std;

const char mainMenu [] = {" Main Menu: \n 1) Send message \n 2) Get server info \n 3) Exit \n >Choose option:" };

int initialize_server();
void accept_new_client(int serverDescriptor, std::vector<int>& sockets, std::map<int, std::string>& clients, int& client_id_counter);


int main()
{
	int 			serverDescriptor, clientDescriptor;
	struct sockaddr_in	ServerAddress, ClientAddress;
	int ServAddrLen 	= sizeof(ServerAddress);
	map<int,string>		clients;
	vector<int>		sockets;
	fd_set monitoredDescriptors;
    int client_id_counter = 1; // Unikalny ID dla każdego klienta
    int max_fd = 0;
	//Setting up socket
	serverDescriptor = initialize_server();

    string answer;
	
    while (true) {
        FD_ZERO(&monitoredDescriptors);
        FD_SET(serverDescriptor, &monitoredDescriptors);
        max_fd = serverDescriptor;

		for (int sock : sockets) {
            FD_SET(sock, &monitoredDescriptors);
            if (sock > max_fd) max_fd = sock;
        }

		if (select(max_fd + 1, &monitoredDescriptors, NULL, NULL, NULL) < 0) {
            cerr << "Select error";
            continue;
        }

		// ACCEPT NEW CLIENT
        if (FD_ISSET(serverDescriptor, &monitoredDescriptors)) {
            accept_new_client(serverDescriptor, sockets, clients, client_id_counter);
        }

        // Obsługa istniejących klientów
        for (auto it = sockets.begin(); it != sockets.end();) {
            if (FD_ISSET(*it, &monitoredDescriptors)) {
                char buffer[1024] = {0};
                int valread = read(*it, buffer, 1024);
                if (valread == 0) {
                    std::cout << "Rozłączono: " << clients[*it] << "\n";
                    close(*it);
                    clients.erase(*it);
                    it = sockets.erase(it);
                } else {
                    // Odczytaj wiadomość z ID odbiorcy (format: "ID:wiadomość")
                    std::string msg(buffer);
                    size_t pos = msg.find(':');
                    if (pos != std::string::npos) {
                        std::string target_id = msg.substr(0, pos);
                        std::string content = msg.substr(pos + 1);

                        // Znajdź odbiorcę po ID
                        for (auto &[fd, name] : clients) {
                            if (name == target_id) {
                                std::string full_msg = "Od " + clients[*it] + ": " + content;
                                send(fd, full_msg.c_str(), full_msg.length(), 0);
                                break;
                            }
                        }
                    }
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }


	close(serverDescriptor);


	return 0;
}


// Funkcja: Inicjalizacja serwera
int initialize_server() {
    int serverDescriptor;
    struct sockaddr_in address;

    serverDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverDescriptor == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
	inet_aton("127.0.0.1", &address.sin_addr);

    if (bind(serverDescriptor, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

	cout << "Server>Listening for connection..." << endl;
    if (listen(serverDescriptor, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    return serverDescriptor;
}

// Funkcja: Akceptowanie nowego klienta
void accept_new_client(int serverDescriptor, std::vector<int>& sockets, std::map<int, std::string>& clients, int& client_id_counter) {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int new_socket = accept(serverDescriptor, (struct sockaddr *)&address, &addrlen);

    if (new_socket < 0) {
        cerr << "Accept failed";
        return;
    }

    sockets.push_back(new_socket);
    clients[new_socket] = "Klient" + std::to_string(client_id_counter++);
    std::string welcome = "Twoje ID: " + clients[new_socket] + "\nLista klientów:\n";

    for (auto& [fd, name] : clients) {
        welcome += name + "\n";
    }

    send(new_socket, welcome.c_str(), welcome.length(), 0);
    std::cout << "Nowe połączenie: " << clients[new_socket] << "\n";
}