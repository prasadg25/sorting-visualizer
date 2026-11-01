// ============================================================
//  Statistics.h  –  Live metrics for the Sorting Visualizer
//  DSA Mini Project | B.Tech Computer Engineering
// ============================================================
#pragma once
#include <string>
#include <chrono>

// -------------------------------------------------------
// Statistics – tracks comparisons, swaps, array-accesses,
// and wall-clock time for one sorting run.
// -------------------------------------------------------
class Statistics
{
public:
    // ── Counters ───────────────────────────────────────────
    long long comparisons   = 0;
    long long swaps         = 0;
    long long arrayAccesses = 0;

    // ── Timer ──────────────────────────────────────────────
    std::chrono::high_resolution_clock::time_point startTime;
    double elapsedMs = 0.0;   // populated after stopTimer()

    // ── State ──────────────────────────────────────────────
    bool  isSorting   = false;
    bool  isPaused    = false;
    bool  isComplete  = false;
    std::string status = "Ready";   // human-readable status label

    // ── Lifecycle ──────────────────────────────────────────

    /** Zero all counters and reset flags. */
    void reset()
    {
        comparisons = 0;
        swaps = 0;
        arrayAccesses = 0;
        elapsedMs = 0.0;
        isSorting = false;
        isPaused = false;
        isComplete = false;
        status = "Ready";
    }

    /** Record the current wall-clock time as t=0. */
    void startTimer()
    {
        startTime = std::chrono::high_resolution_clock::now();
    }

    /** Compute elapsedMs from startTime to now. */
    void stopTimer()
    {
        auto now = std::chrono::high_resolution_clock::now();
        elapsedMs = std::chrono::duration<double, std::milli>(now - startTime).count();
    }

    /** Update elapsedMs without stopping the timer (live display). */
    void updateElapsed()
    {
        auto now = std::chrono::high_resolution_clock::now();
        elapsedMs = std::chrono::duration<double, std::milli>(now - startTime).count();
    }

    // ── Convenience increment helpers ─────────────────────

    void addComparison   (long long n = 1) { comparisons   += n; }
    void addSwap         (long long n = 1) { swaps         += n; }
    void addArrayAccess  (long long n = 1) { arrayAccesses += n; }

    // ── Derived ────────────────────────────────────────────
    long long totalOps() const { return comparisons + swaps + arrayAccesses; }
};
