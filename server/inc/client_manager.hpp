#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <mutex>
#include <string>
#include <unordered_map>

struct Client {
  int socket;
  std::string username;
};

class ClientManager {
private:
  std::unordered_map<int, Client> clients;
  std::mutex mtx;

public:
  void add_client(int socket, const std::string &username);
  void remove_client(int socket);
  void broadcast(const std::string &message, int sender_socket);
  int get_socket_by_username(const std::string &username);
  bool username_exists(const std::string &username);
  std::string get_username(int socket);
  std::string get_all_users();
};

#endif