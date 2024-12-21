#include <iostream>
#include <cstdlib>
#include <thread>
#include <ProjectConfig.h>
using namespace std;

#define MAX_CLIENTS 20

void run_client() {
    system("konsole -e ./client.exe & || "
    "gnome-terminal -- bash -c './client.exe; exec bash' & || "
    "xterm -hold -e ./client.exe &");
}

void run_server() {
    system("konsole -e ./server.exe & || "
    "gnome-terminal -- bash -c './server.exe; exec bash' & || "
    "xterm -hold -e ./server.exe &");
}

int main()
{   
    char answer;
    int activeClients = 0;


    while(true)
    {
        cout << "Active clients: " << activeClients << endl;
        cout << "-----------------" << endl;
        cout << "Menu:" << endl;
        cout << "1) Create client" << endl;
        cout << "2) Setup server"  << endl;
        cout << "3) Exit" << endl;
        cout << "Answer:"; 
        cin >> answer;

        switch(answer)
        {
            case '1': {
                thread client_thread(run_client);
                client_thread.detach(); // Działa w tle
                break;}
            case '2': {
                thread server_thread(run_server);
                server_thread.detach(); // Działa w tle
                break;}
            case '3':
                return 0;
            break;
            default:
                cout << ">Invalid option" << endl;
        }
    }

    return 0;
}