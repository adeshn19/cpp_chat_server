#include "client_manager.hpp"
#include "logger.hpp"
#include "message_handler.hpp"
#include "thread_pool.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <sys/socket.h> // Needed for socket-related functions
#include <thread>
#include <unistd.h>
#include <vector>

#define PORT 8083
ClientManager manager;
void handle_client(int client_socket) {

  char buffer[1024];

  logMessage(LogLevel::DEBUG,
             "[NEW CONNECTION] Socket: " + std::to_string(client_socket));

  // Step 1: Get username
  int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

  if (bytes <= 0) {
    logMessage(LogLevel::ERROR, "[ERROR] Failed to receive username");

    close(client_socket);
    return;
  }

  std::string username(buffer, bytes);

  if (username.empty() || manager.username_exists(username)) {
    std::string err = "Invalid or duplicate username\n";
    send(client_socket, err.c_str(), err.size(), 0);
    close(client_socket);
    return;
  }

  logMessage(LogLevel::INFO, "[USER CONNECTED] " + username);
  manager.add_client(client_socket, username);

  std::string join_msg = username + " joined the chat\n";
  manager.broadcast(join_msg, client_socket);

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

    if (bytes <= 0) {
      logMessage(LogLevel::INFO, "[USER DISCONNECTED] " + username);

      std::string leave_msg = username + " left the chat\n";

      manager.remove_client(client_socket);
      manager.broadcast(leave_msg, client_socket);

      close(client_socket);
      return;
    }

    std::string message(buffer, bytes);

    logMessage(LogLevel::INFO, "[MESSAGE] " + username + ": " + message);

    process_message(message, client_socket, manager);
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

  if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Bind failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, 10) == -1) {
    std::cerr << "Listen failed: " << strerror(errno) << std::endl;
    close(server_fd);
    return 1;
  }

  logMessage(LogLevel::INFO, "Chat server started....\n");

  ThreadPool pool(4);
  while (true) {
    sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    int client_socket = accept(server_fd, (sockaddr *)&client_addr, &addr_size);
    if (client_socket == -1) {
      std::cerr << "Accept failed: " << strerror(errno) << std::endl;
      continue; // Continue to listen for more connections
    }
    logMessage(LogLevel::DEBUG,
               "[ACCEPTED] Client socket: " + std::to_string(client_socket));
    pool.enqueue([client_socket]() { handle_client(client_socket); });
  }
  return 0;
}