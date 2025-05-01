# i-have-no-clue

## Maintainers

- Athena Boose (lab section 09) [`<pestpestthechicken@yahoo.com>`](mailto:pestpestthechicken@yahoo.com)
- Kodiak O'Masta (lab section 13) [`<meepsterIQ@yahoo.com>`](mailto:meepsterIQ@yahoo.com)
- Ryan Gradinaru (lab section 05)

## Build Instructions (Windows)

Clone this project to your computer, and open it in your choice of Visual Studio, CLion, or another IDE that supports CMake.
It may take awhile to open, as dependencies must be downloaded when CMake configures.
You can then build and run it normally.
Again, this may take some time as CMake must compile dependencies.

If you don't want to use a full IDE, you might be able to use the Windows terminal or VSCode to build if you have CMake and MSVC installed.
That process is likely somewhat similar to the process for building on a Unix client (ignore the part about installing SFML dependencies), although none of us have tested it.

## Build Instructions (Unix client)

First, you must download the dependencies for SFML, which are listed [here](https://github.com/SFML/cmake-sfml-project).
You'll also need `git` installed to clone this project, as well as a C/C++ compiler, `make`, and CMake.
Install these through your package manager.

Then, clone this project with git, navigate to its directory in terminal, and enter `cmake -B build` to configure CMake.
You can then enter `cmake --build build` to build the project.
The client executable will be located at `./build/client/client-bin`.

## Build Instructions (Linux server)

The server for this project uses Linux-specific syscalls, like `epoll`. 
`kqueue` support might happen in future to make it portable to BSD systems, but this is not implemented yet.
As such, it currently is only supported to build on a Linux system of some sort.
Again, you'll need `git` installed to clone this project, as well as a C/C++ compiler, `make`, and CMake.
The dependencies for SFML are unnecessary if you only wish to build the server portion.

Clone this project, navigate to its directory in terminal, and enter `cmake -B build -DBUILD_SERVER=ON -DBUILD_CLIENT=OFF`.
You can then enter `cmake --build build` to build the project.
The server executable will be located at `./build/server/server-bin`.
You can run the server's unit tests with `ctest --test-dir build`, and if you have `valgrind` installed, you can use `ctest -T memcheck --test-dir build` to check for memory leaks.
(Source code for tests is located in various tests/ folders scattered throughout the server/ directory.)

## Build Instructions (Docker)

With Linux as the host OS, at least, you can just run `docker compose up` in the project's root directory.
The project should automatically configure and run the server for you.
Optionally, you can append the `-d` flag to run in the background.
When you're done, press Control-C to quit (only if you didn't append the `-d` flag) and then run `docker compose down`.

You may have to run this as superuser.
Also, you must have the docker daemon running; you can run it in the background using something like `systemd` or just run `dockerd` yourself in another terminal window.

MacOS is pretty similar to this, with the exception that you likely just need to install Docker Desktop to run the docker daemon.
I (Athena) haven't used Docker on Windows so I can't give any instructions there.

## More Build Info

This project checks for 2 cmake variables when configuring itself; `BUILD_SERVER` and `BUILD_CLIENT`.
The server only builds if `BUILD_SERVER` has a definition and is defined to a true value, like `ON`.
The client only builds if `BUILD_CLIENT` is undefined or is defined to a true value, like `ON`.

Essentially, if you don't define these variables on the command line when configuring (as most IDEs don't), the project will default to just building the client, which is intended, as the server will fail to build on some systems. You must explicitly set `BUILD_SERVER` to `ON` to build the server, and `BUILD_CLIENT` to `OFF` to avoid building the client.

# Notes

`compile-docs` is a shell script to compile the documentation for this project.
It uses `pandoc` so you'll want to have that installed.
Just run it, and optionally pass it your desired file extension as the first argument.

`configure-debug` is a shell script that configures a debug version of the server of this project.

## Play Instructions

Upon starting the game, you will be presented with a character select screen. Click on the boxes of the characters you would like to play as. Order does not matter. Once you have selected who you are playing as, press the done button or the window close button to start the game. This will happen automatically if all characters are selected.
Once the game has started, the first player will be randomly chosen, identified by the red squares next to their token. Click on one of those red squares to move to that location. The next player's turn will then be automatically queued up.
There is also a scratch pad that you can take notes on (though there is no need to), and this functions as a simple set of text boxes. When your turn ends the data written is saved and when your turn comes back around, the data is retrieved.
