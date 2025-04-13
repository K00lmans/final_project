#pragma once
#include <cstddef>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>


class Game {
    public:
    Game(std::size_t max_message_len = 256, int flags = 0) : max_message_len(max_message_len), poll_fd(epoll_create1(flags)) {
        if (poll_fd == -1) {
            error_has_occurred = true;
        }
    }
    // Closes all sockets, frees all resources.
    // WARNING: 
    // This may cause data loss if you haven't made appropriate callbacks to try_close_game()!
    ~Game();

    enum CallbackStatus {
        Continue,
        GameEnd,
        GameError,
    };

    // Gets a file descriptor that will have a read event whenever the Game receives an update from one of the players
    int fd() const { return poll_fd; }

    // A callback function that runs all possible events for the game, then exits when there are no more left.
    // You should wait on the fd returned by `fd` until it has an event, then call this function. If GameEnd or GameError is returned, then the game has ended.
    // In this case, you should call try_close_game until it returns true, then you can let an instance of this object destruct.
    CallbackStatus callback(); // REMEMBER TO CHECK FOR 0 BYTE READS

    // Attempts to add a user to the game.
    // If the attempt fails, returns false, otherwise, returns true.
    bool add_user(int fd);

    // Sends the shutdown packet to all connected sockets.
    // This alone isn't enough to close the game, you will have to call shutdown_callback repeatedly until it returns true.
    bool send_shutdown_message();

    // Attempts to close the game. If it has closed successfully, returns true, otherwise, returns false.
    // If this returns false, the fd returned by `fd` will return a read event when you can call this again.
    // 
    // Basically, after calling send_shutdown_message, you must repeatedly poll this function until it returns true.
    bool shutdown_callback();

    private:
    enum ConnectionState {
        Open, // socket is currently active
        WriteShutdown, // shutdown() called, waiting for 0-byte read
        RWShutdown // shutdown() called, received 0-byte read, ready to close()
    };
    // In this case a struct is reasonable, as it's entirely encapsulated within the `Game` class.
    struct Player {
        int fd;
        ConnectionState state;
        std::string character;
    };

    std::size_t max_message_len;
    int poll_fd;
    bool error_has_occurred = false;
    std::vector<Player> players;
};
