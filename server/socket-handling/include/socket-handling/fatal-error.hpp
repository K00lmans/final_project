#pragma once
#include <exception>
#include <string>
#include <cstring>

// i could've (and did at one point) use std::system_error, but this does technically satisfy the rule to use inheritance somewhere in the code and we didn't find another good place for it
class FatalError : public std::exception {
    public:
    FatalError(int error_code = -1, const std::string &info = std::string()) : error_code(error_code), info(info) {
        if (error_code == -1) {
            return;
        }
        char *strerror_result = strerror(error_code);
        this->info += " : ";
        for (std::size_t i = 0; strerror_result[i] != '\0'; ++i) {
            this->info.push_back(strerror_result[i]);
        }
    }
    ~FatalError() {}
    const char *what() const noexcept { return info.c_str(); }
    private:
    int error_code;
    std::string info;
};
