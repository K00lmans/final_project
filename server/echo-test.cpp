#include <unordered_map>
#include <socket-handling/fd-poll.hpp>
#include <socket-handling/connection-factory.hpp>
#include <socket-handling/input-buffer.hpp>
#include <socket-handling/output-buffer.hpp>
#include <socket-handling/shutdown.hpp>
static void kill_sock(int sock, FdPoll &poll, Shutdown<> &shut, std::unordered_map<int, std::pair<InputBuffer<63>, OutputBuffer>> &fdmap) {
    epoll_event ev;
    poll.ctl(EPOLL_CTL_DEL, sock, ev);
    shut.shutdown(sock, std::move(fdmap.at(sock).second));
    fdmap.erase(sock);
    std::cout << "fuck you" << std::endl;
}
int main(void) {
    FdPoll poll;
    ConnectionFactory connfact("12345");
    epoll_event ev{.events = EPOLLIN, .data = {.fd = connfact.get_fd()}};
    poll.ctl(EPOLL_CTL_ADD, connfact.get_fd(), ev);
    std::unordered_map<int, std::pair<InputBuffer<63>, OutputBuffer>> fdmap;
    std::array<epoll_event, 1> ep_array;
    Shutdown<> shut;
    ev.data.fd = shut.get_fd();
    poll.ctl(EPOLL_CTL_ADD, shut.get_fd(), ev);

    while (true) {
        auto update = poll.wait(std::span(ep_array), -1);
        if (update[0].data.fd == connfact.get_fd()) {
            int new_fd = connfact.get_new_connection().value();
            fdmap.try_emplace(
                new_fd,
                std::move(std::pair(std::move(InputBuffer<63>()), std::move(OutputBuffer())))
            );
            ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
            ev.data.fd = new_fd;
            poll.ctl(EPOLL_CTL_ADD, new_fd, ev);
            continue;
        }
        if (update[0].data.fd == shut.get_fd()) {
            shut.callback();
            continue;
        }
        if (update[0].events & EPOLLRDHUP ||
            update[0].events & EPOLLERR ||
            update[0].events & EPOLLHUP
        ) {
            kill_sock(update[0].events, poll, shut, fdmap);
            continue;
        }
        if (update[0].events & EPOLLIN) {
            InputBuffer<63> &inbuf = fdmap.at(update[0].data.fd).first;
            OutputBuffer &outbuf = fdmap.at(update[0].data.fd).second;
            std::optional<std::size_t> num_parsed;
            switch(inbuf.buf_read(update[0].data.fd)) {
            case SocketStatus::ZeroReturned:
            case SocketStatus::Error:
                kill_sock(update[0].data.fd, poll, shut, fdmap);
                break;
            case SocketStatus::Blocked:
            case SocketStatus::Finished:
                num_parsed = inbuf.get_msg_end();
                if (!num_parsed.has_value() && inbuf.full()) {
                    kill_sock(update[0].data.fd, poll, shut, fdmap);
                    break;
                }
                else if (num_parsed.has_value()) {
                    std::string out_str;
                    for (std::size_t i = 0; i < num_parsed.value(); ++i) {
                        if (inbuf[i] == 0x03) {
                            kill_sock(update[0].data.fd, poll, shut, fdmap);
                            goto endloop;
                        }
                        out_str += inbuf[i];
                    }
                    inbuf.pop_front(num_parsed.value());
                    outbuf.add_message(std::shared_ptr<std::string>(new std::string(out_str)));
                    switch (outbuf.flush(update[0].data.fd)) {
                    case SocketStatus::Finished:
                    case SocketStatus::Blocked:
                        break;
                    case SocketStatus::ZeroReturned:
                    case SocketStatus::Error:
                        kill_sock(update[0].data.fd, poll, shut, fdmap);
                        break;
                    }
                }
            }
            continue;
        }
        if (update[0].events & EPOLLOUT) {
            switch (fdmap.at(update[0].data.fd).second.flush(update[0].data.fd)) {
            case SocketStatus::Finished:
            case SocketStatus::Blocked:
                break;
            case SocketStatus::ZeroReturned:
            case SocketStatus::Error:
                kill_sock(update[0].data.fd, poll, shut, fdmap);
                break;
            }
            continue;
        }
        abort();
        endloop:
    }
}
