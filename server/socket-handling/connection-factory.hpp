#pragma once

// 
// Maintainer: Athena Boose <pestpestthechicken@yahoo.com>
// 
// Description: A class that allows for connections to be opened easily.
//              ...this isn't really a proper factory but it has factory vibes
//
// Tags:
// NO_TESTS

#include <optional>
#include <string>

class ConnectionFactory {
    public:
    ConnectionFactory() = delete;
    ConnectionFactory(const std::string &port);
    ~ConnectionFactory();

    // If there's a new connection, returns the new connection.
    // Otherwise, returns an empty optional.
    // The new file descriptor will have its O_NONBLOCK flag set for convenience.
    std::optional<int> get_new_connection();

    // Returns the socket's file descriptor.
    // Whenever there are new connections to be accepted, there will be a read event on this file descriptor in poll/epoll/select/etc.
    int get_fd() const { return socket_fd; }
    private:
    int socket_fd;
};
