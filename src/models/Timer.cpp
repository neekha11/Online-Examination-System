#include "../../include/models/Timer.h"
#include <iostream>

using namespace std;

Timer::Timer(int duration) {
    this->duration = duration;
    this->running = false;
}

void Timer::start() {
    running = true;
    cout << "Timer started." << endl;
}

void Timer::stop() {
    running = false;
    cout << "Timer stopped." << endl;
}

void Timer::displayTimer() {
    cout << "Duration: " << duration << " minutes" << endl;

    if (running)
        cout << "Status: Running" << endl;
    else
        cout << "Status: Stopped" << endl;
}