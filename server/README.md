# Server code notes/ideas for improvement

### Author: Athena Boose

This server uses the `epoll` system call available on Linux to wait for IO events to occur.
It then processes as much IO as possible without blocking.
It's written in plain C++, no external libraries, since I thought it would be fun/interesting to do.
This definitely took more time as a result (and is probably less efficient if I'm perfectly honest, although I haven't benchmarked).
I learned a lot, though, so it was worthwhile.

The following resources were quite helpful when writing this.
I didn't directly use either as a tutorial, but they provided enough background information to give me a rough idea of where to start.

- <http://www.kegel.com/c10k.html>
    - Discusses various ways of designing high-performance web servers, very helpful for seeing what's out there and ensuring I haven't missed a good way of doing this.
- <https://beej.us/guide/bgnet/html/> 
    - Basics of working with network sockets

I also heavily used the Linux (and occasionally FreeBSD) manpages, honestly moreso than either of the above resources.

If I were I to do this again I'd probably use C++20 coroutines instead, or potentially async/await.
I initially thought that this style of server architecture would work a bit better for this sort of problem.
I don't think it actually did, to be honest; the code is probably kinda confusing.

The code is mostly broken up into two subdirectories.
`game` handles the actual game logic; or, at least, how the server manages receiving and sending messages.
`socket-handling` provides convenience classes that handle things like creating new TCP connections, buffering input/output, polling file descriptors, setting timers, and shutting down connections cleanly.

Some things aren't designed the best - polling, especially.
The `FdPoll` class is a *very* thin wrapper over `epoll` (and not even a good one, idk why I passed `epoll_event` in as a reference instead of a pointer, it's very inconvenient).

Furthermore, the buffers are probably a bit more inefficient than necessary.
`read(2)` and `write(2)` only do partial reads/writes in situations where they will block on the next call (thus meaning that edge-triggered epoll will send another notification when they're available again).
So, a few system calls could probably be avoided there with a quick refactor.

Also, having separate returns for completed and blocked IO was kinda dumb in retrospect.
Either will reset epoll; there is literally no difference between the two for my usecases here.

Finally, this code could probably be tested more rigorously than it currently is.
There are some test cases (see the `game/tests` and `socket-handling/tests` directories), but network code is kinda just inherently hard to test.
I went to the effort of making testing versions of `read`/`write`/`readv`/`writev` that override the ones in `libc` when linked, but for other functions (`shutdown`, `close`, `accept`) I simply didn't have enough time.
The code was mostly debugged by directly connecting to the server with `telnet` and `netcat` and feeding it various inputs in hopes it would break, then debugging when it did.
I also made a little fuzzing script `fuzz.sh` that simply connects to the server a bunch and spams random data at it in hopes of crashing it.
Again, if I wanted to do this properly, I'd use something like `libFuzzer`.

