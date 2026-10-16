#include "../../include/models/Timer.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;

Timer::Timer(int durationMinutes)
    : duration(max(0, durationMinutes)),
      running(false),
      accumulatedSeconds(0) {
}

long long Timer::secondsSinceStart() const {
    return chrono::duration_cast<chrono::seconds>(
               chrono::steady_clock::now() - startTime).count();
}

long long Timer::elapsedSeconds() const {
    return accumulatedSeconds + (running ? secondsSinceStart() : 0);
}

void Timer::start() {
    if (running)
        return;

    startTime = chrono::steady_clock::now();
    running = true;
}

void Timer::stop() {
    if (!running)
        return;

    accumulatedSeconds += secondsSinceStart();
    running = false;
}

bool Timer::isRunning() const {
    return running;
}

bool Timer::isExpired() const {
    return elapsedSeconds() >= duration * 60LL;
}

int Timer::remainingSeconds() const {
    return static_cast<int>(max(0LL, duration * 60LL - elapsedSeconds()));
}

void Timer::displayTimer() const {
    cout << "Duration: " << duration << " minutes" << endl;

    if (isExpired())
        cout << "Status: Expired" << endl;
    else if (running)
        cout << "Status: Running" << endl;
    else
        cout << "Status: Stopped" << endl;

    const int remaining = remainingSeconds();
    cout << "Remaining: "
         << setfill('0') << setw(2) << remaining / 60 << ":"
         << setw(2) << remaining % 60 << setfill(' ') << endl;
}