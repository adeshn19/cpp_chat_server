#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8083

void receive_messages(int sock) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer), 0);

        if (bytes <= 0) {
            std::cout << "\nDisconnected from server\n";
            break;
        }

        std::string msg(buffer, bytes);
        std::cout << "\r" << msg << "\n> " << std::flush;  // clean UI
    }
}

int main() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin, username);

    send(sock, username.c_str(), username.size(), 0);

    std::thread recv_thread(receive_messages, sock);

    while (true) {
        std::string msg;
        std::cout << "> ";
        std::getline(std::cin, msg);

        send(sock, msg.c_str(), msg.size(), 0);
    }

    recv_thread.join();
    close(sock);

    return 0;
}