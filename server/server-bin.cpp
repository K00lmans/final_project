#include "run-server.hpp"
#include <cstring>

int main(void) {
    run_server<127>("12345", 16384);
    return EXIT_SUCCESS;
}
/*
int main(void) {
    int num_errs = 0;
    while (num_errs < 16) {
        try {
            run_server<127>("12345", 16384);
            return EXIT_SUCCESS;
        }
        catch (const std::exception &e) {
            std::cerr   << "A fatal error occurred, but was caught. All connections were dropped. Restarting...\n" 
                        << "Exception message: " << e.what() << std::endl;
            ++num_errs;
        }
    }
}
*/
