#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h> // Needed for socket-related functions
#include "client_manager.hpp"
#include "message_handler.hpp"
#include "thread_pool.hpp"
#define PORT 8083
ClientManager manager;
void handle_client(int client_socket)
{

    char buffer[1024];

    std::cout << "[NEW CONNECTION] Socket: " << client_socket << std::endl;

    // Step 1: Get username
    int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        std::cout << "[ERROR] Failed to receive username\n";
        close(client_socket);
        return;
    }

    std::string username(buffer, bytes);

    if (username.empty() || manager.username_exists(username))
    {
        std::string err = "Invalid or duplicate username\n";
        send(client_socket, err.c_str(), err.size(), 0);
        close(client_socket);
        return;
    }
    std::cout << "[USER CONNECTED] " << username << " (Socket: " << client_socket << ")\n";

    manager.add_client(client_socket, username);

    std::string join_msg = username + " joined the chat\n";
    manager.broadcast(join_msg, client_socket);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes <= 0)
        {
            std::cout << "[DISCONNECTED] " << username << std::endl;

            std::string leave_msg = username + " left the chat\n";

            manager.remove_client(client_socket);
            manager.broadcast(leave_msg, client_socket);

            close(client_socket);
            return;
        }

        std::string message(buffer, bytes);

        std::cout << "[MESSAGE] " << username << ": " << message << std::endl;

        process_message(message, client_socket, manager);
    }
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) == -1)
    {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Chat server started....\n";

    ThreadPool pool(4);
    while (true)
    {
        sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);

        int client_socket = accept(server_fd, (sockaddr *)&client_addr, &addr_size);
        if (client_socket == -1)
        {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue; // Continue to listen for more connections
        }
        std::cout << "[ACCEPTED] Client socket: " << client_socket << std::endl;

        pool.enqueue([client_socket]()
                     { handle_client(client_socket); });
    }
    return 0;
}