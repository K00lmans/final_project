#pragma once


// inherit from this class and provide a callback() function
class Event {
    public:
    virtual void callback(short what) = 0;
};
