#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "client_manager.hpp"
#include <string>

void process_message(const std::string &message, int sender_socket,
                     ClientManager &manager);

#endif