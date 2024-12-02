#pragma once

#include <ctime>
#include <chrono>

namespace LOG {

class CTimer {
  private:
    std::chrono::steady_clock::time_point start;

  public:
    void Start();
    long long GetTime();
};

}
