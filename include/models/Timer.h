#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    int duration;                       // total time in minutes
    bool running;
    long long accumulatedSeconds;       // time used before the last stop()
    std::chrono::steady_clock::time_point startTime;

    long long secondsSinceStart() const;
    long long elapsedSeconds() const;

public:
    explicit Timer(int durationMinutes);

    void start();                       // starts, or resumes after stop()
    void stop();                        // pauses; elapsed time is kept

    bool isRunning() const;
    bool isExpired() const;
    int remainingSeconds() const;

    void displayTimer() const;
};

#endif