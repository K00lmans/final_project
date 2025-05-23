#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstring>

#include <socket-handling/connection-factory.hpp>
#include <socket-handling/fd-utils.hpp>
#include <socket-handling/fatal-error.hpp>


#include <iostream> // debugging

// Various Linux manpages as well as https://beej.us/guide/bgnet/html/ were quite helpful here.
ConnectionFactory::ConnectionFactory(const std::string &port) : socket_fd(-1) {
    addrinfo hints{};
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    addrinfo *result = NULL;
    int true_val = 1;
    int retval = getaddrinfo(NULL, port.c_str(), &hints, &result);
    if (retval == -1) {
        std::string errstring("getaddrinfo returned an error : ");
        const char *err_cstr = gai_strerror(errno);
        for (std::size_t i = 0; err_cstr[i] != '\0'; ++i) {
            errstring.push_back(err_cstr[i]);
        }
        throw FatalError(-1, errstring);
    }

    for (addrinfo *addr = result; addr != NULL; addr = addr->ai_next) {
        socket_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (socket_fd == -1) {
            continue; // couldn't create a socket
        }

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &true_val, sizeof(int)) == -1) {
            close_except(socket_fd);
            socket_fd = -1;
            continue; // couldn't set SO_REUSEADDR
        }

        if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) == -1) {
            close_except(socket_fd);
            socket_fd = -1;
            continue; // couldn't bind to port
        }
    }
    freeaddrinfo(result);

    // now socket_fd is either a socket bound to a port or -1
    if (socket_fd == -1) {
        throw FatalError(errno, "Couldn't find an address to bind to");
    }

    // 4096 is the max value Linux 5.4+ can handle, might as well use that
    // most modern linux systems are probably above 5.4. hopefully.
    if (listen(socket_fd, 4096) == -1) {
        throw FatalError(errno, "Call to listen() failed fatally.");
    }
    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
        throw FatalError(errno, "Could not set socket-descriptor to nonblocking.");
    }
}

ConnectionFactory::~ConnectionFactory() {
    close_except(socket_fd);
}

std::optional<int> ConnectionFactory::get_new_connection() {
    int new_fd = accept(socket_fd, NULL, NULL);
    if (new_fd == -1) {
        if (errno == EAGAIN || 
            errno == EWOULDBLOCK
        ) {
            return std::optional<int>(std::nullopt);
        }
        if (
            errno == ECONNABORTED ||
            errno == EINTR ||
            errno == EMFILE ||
            errno == ENFILE ||
            errno == ENOBUFS
        ) {
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            return std::optional<int>(std::nullopt);
        }
        std::cerr << errno;
        throw FatalError(errno, "Call to accept() failed fatally.");
    }
    if (fcntl(new_fd, F_SETFL, O_NONBLOCK) == -1) {
        throw FatalError(errno, "Could not set socket descriptor to nonblocking.");
    }
    return std::optional(new_fd);
}

