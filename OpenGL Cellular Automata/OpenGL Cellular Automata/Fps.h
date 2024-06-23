#pragma once

#include <chrono>

class FPS
{
public:
    /// @brief Constructor with initialization.
    ///
    FPS() : mFrame(0), mFps(0), mStartTime(std::chrono::high_resolution_clock::now()) {}

    /// @brief Get the current FPS count.
    /// @return FPS count.
    const unsigned int getFPS() const { return mFps; }

private:
    unsigned int mFrame;
    unsigned int mFps;
    std::chrono::high_resolution_clock::time_point mStartTime;

public:
    void update()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - mStartTime).count();

        if (elapsedTime >= 1)
        {
            mFps = mFrame;
            mFrame = 0;
            mStartTime = currentTime;
        }

        ++mFrame;
    }
};