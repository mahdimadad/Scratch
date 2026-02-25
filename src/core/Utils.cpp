#include "core/Utils.h"
#include <chrono>

int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}
unsigned long long nowMs() {
    using namespace std::chrono;
    return (unsigned long long)duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}