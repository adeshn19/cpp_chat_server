#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include <errno.h>
#include <sys/socket.h> // Needed for socket-related functions

#define PORT 8083

std::vector<int> clients;
std::mutex clients_mutex;

void broadcast_message(const std::string message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);

    for (int client : clients) {
        if (client != sender_socket) {
            // Check if the socket is still valid before sending
            if (client > 0) { // Check for a valid socket descriptor
                ssize_t bytes_sent = send(client, message.c_str(), message.size(), 0);
                if (bytes_sent == -1) {
                    std::cerr << "Error sending message to client " << client << ": " << strerror(errno) << std::endl;
                    // Optionally remove the client from the list if sending fails consistently
                }
            } else {
                std::cerr << "Invalid socket descriptor encountered in broadcast_message." << std::endl;
            }
        }
    }
}

void handle_client(int client_socket) {
    std::cout << "Handling client: " << client_socket << std::endl; // Log client socket
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes <= 0) {
            if (bytes == 0) {
                std::cout << "Client disconnected normally.\n";
            } else {
                std::cerr << "Client disconnected with error: " << strerror(errno) << std::endl;
            }
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                // Remove the client from the vector
                for (size_t i = 0; i < clients.size(); ++i) {
                    if (clients[i] == client_socket) {
                        clients.erase(clients.begin() + i);
                        break;
                    }
                }
            }
            return; // Exit the thread
        }

        std::string message(buffer);
        std::cout << "Message from client " << client_socket << ": " << message << std::endl;

        broadcast_message(message, client_socket);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Chat server started....\n";

    while (true) {
        sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);

        int client_socket = accept(server_fd, (sockaddr*)&client_addr, &addr_size);
        if (client_socket == -1) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;  // Continue to listen for more connections
        }
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }
    return 0;
}