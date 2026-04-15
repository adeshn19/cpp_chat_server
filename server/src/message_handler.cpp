#include "message_handler.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
void process_message(const std::string &message, int sender_socket,
                     ClientManager &manager) {

  // Private message
  if (message.rfind("/msg ", 0) == 0) {
    std::istringstream iss(message);
    std::string cmd, target, msg;

    iss >> cmd >> target;
    std::getline(iss, msg);

    if (msg.empty())
      return;

    std::string sender = manager.get_username(sender_socket);

    int target_socket = manager.get_socket_by_username(target);

    if (target_socket == -1) {
      std::string err = "User not found\n";
      send(sender_socket, err.c_str(), err.size(), 0);
      return;
    }

    std::string final_msg = "[PM from " + sender + "] " + msg + "\n";
    send(target_socket, final_msg.c_str(), final_msg.size(), 0);
    return;
  } else {
    if (message == "/list") {
      std::string users = manager.get_all_users();
      send(sender_socket, users.c_str(), users.size(), 0);
      return;
    }
    if (message == "/help") {
      std::string help = "Commands:\n"
                         "/msg <user> <message> - Private message\n"
                         "/list - Show users\n"
                         "/help - Show commands\n";

      send(sender_socket, help.c_str(), help.size(), 0);
      return;
    }
    std::string sender = manager.get_username(sender_socket);
    std::string final_msg = sender + ": " + message + "\n";

    manager.broadcast(final_msg, sender_socket);
  }
}