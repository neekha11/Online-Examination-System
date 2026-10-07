#ifndef TIMER_H
#define TIMER_H

class Timer {
private:
    int duration;
    bool running;

public:
    Timer(int duration);

    void start();
    void stop();
    void displayTimer();
};

#endif