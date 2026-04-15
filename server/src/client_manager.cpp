#include "client_manager.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

void ClientManager::add_client(int socket, const std::string &username) {
  std::lock_guard<std::mutex> lock(mtx);
  clients[socket] = {socket, username};
}

void ClientManager::remove_client(int socket) {
  std::lock_guard<std::mutex> lock(mtx);
  clients.erase(socket);
}

void ClientManager::broadcast(const std::string &message, int sender_socket) {
  std::lock_guard<std::mutex> lock(mtx);

  for (auto &[sock, client] : clients) {
    if (sock != sender_socket) {
      send(sock, message.c_str(), message.size(), 0);
    }
  }
}

int ClientManager::get_socket_by_username(const std::string &username) {
  std::lock_guard<std::mutex> lock(mtx);

  for (auto &[sock, client] : clients) {
    if (client.username == username) {
      return sock;
    }
  }
  return -1;
}

std::string ClientManager::get_username(int socket) {
  std::lock_guard<std::mutex> lock(mtx);
  return clients[socket].username;
}

bool ClientManager::username_exists(const std::string &username) {
  for (auto &[sock, user] : clients) {
    if (user.username == username)
      return true;
  }
  return false;
}

std::string ClientManager::get_all_users() {
  std::string list = "Online Users:\n";
  for (auto &[sock, user] : clients) {
    list += "- " + user.username + "\n";
  }
  return list;
}