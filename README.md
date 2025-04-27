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

## More Build Info

This project checks for 2 cmake variables when configuring itself; `BUILD_SERVER` and `BUILD_CLIENT`.
The server only builds if `BUILD_SERVER` has a definition and is defined to a true value, like `ON`.
The client only builds if `BUILD_CLIENT` is undefined or is defined to a true value, like `ON`.

Essentially, if you don't define these variables on the command line when configuring (as most IDEs don't), the project will default to just building the client, which is intended, as the server will fail to build on some systems. You must explicitly set `BUILD_SERVER` to `ON` to build the server, and `BUILD_CLIENT` to `OFF` to avoid building the client.

