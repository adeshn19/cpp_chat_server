#include "logger.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>
#define PORT 8083

void receive_messages(int sock) {
  char buffer[1024];

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, sizeof(buffer), 0);

    if (bytes <= 0) {

      logMessage(LogLevel::ERROR, "Disconnected from server");
      break;
    }

    std::string msg(buffer, bytes);

    logMessage(LogLevel::INFO, "Message received: " + msg);
  }
}

int main() {

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::string err = strerror(errno);
    logMessage(LogLevel::ERROR, "Socket creation failed: " + err);
    return 1;
  }

  logMessage(LogLevel::INFO, "Socket created");

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    logMessage(LogLevel::ERROR, "Invalid server address");
    return 1;
  }

  if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::string err = strerror(errno);
    logMessage(LogLevel::ERROR, "Connection failed: " + err);
    return 1;
  }

  logMessage(LogLevel::INFO, "Connected to server (127.0.0.1:8083)");

  std::string username;
  std::cout << "Enter username: ";
  std::getline(std::cin, username);

  send(sock, username.c_str(), username.size(), 0);

  logMessage(LogLevel::DEBUG, "Username sent: " + username);

  std::thread recv_thread(receive_messages, sock);

  logMessage(LogLevel::DEBUG, "Receiver thread started");

  while (true) {
    std::string msg;
    std::cout << "> ";
    std::getline(std::cin, msg);

    if (msg.empty())
      continue;

    int bytes = send(sock, msg.c_str(), msg.size(), 0);

    if (bytes == -1) {
      std::string err = strerror(errno);
      logMessage(LogLevel::ERROR, "Send failed: " + err);
    } else {
      logMessage(LogLevel::DEBUG,
                 "Message sent (" + std::to_string(bytes) + " bytes): " + msg);
    }
  }

  recv_thread.join();
  close(sock);

  logMessage(LogLevel::INFO, "Client shutdown");

  return 0;
}