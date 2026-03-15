#include<iostream>
#include<thread>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include <errno.h>

#define PORT 8083

void receive_messages(int socket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(socket, buffer, sizeof(buffer), 0);
        std::cout<<"Bytes "<<bytes<<std::endl;
        if (bytes > 0) {
            std::cout << buffer << std::endl;
        } else if (bytes == 0) {
            std::cout << "Server disconnected.\n";
            break; // Server closed the connection
        } else {
            std::cerr << "Receive error: " << strerror(errno) << std::endl;
            break; // Error receiving data
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/Address family not supported" << std::endl;
        close(sock);
        return 1;
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Connect failed: " << strerror(errno) << std::endl;
        close(sock);
        return 1;
    }

    std::thread receiver(receive_messages, sock);
    receiver.detach();

    std::string message;

    while (true) {
        std::cout<<"Enter: "<<std::endl;
        std::getline(std::cin, message);
        send(sock, message.c_str(), message.size(), 0);
    }
    return 0;
}