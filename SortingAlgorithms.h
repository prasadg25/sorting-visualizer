// ============================================================
//  SortingAlgorithms.h  –  All 8 sorting algorithm classes
//  DSA Mini Project | B.Tech Computer Engineering
//
//  Design pattern: each algorithm is a separate class that
//  inherits from SortBase.  The visualizer calls stepSort()
//  once per frame so the GUI stays responsive.
// ============================================================
#pragma once
#include <vector>
#include <string>
#include <functional>
#include "Statistics.h"

// ── Color codes used by the visualizer ─────────────────────
// The Visualizer interprets these tags to choose bar colors.
enum class BarState : int
{
    NORMAL    = 0,  // white-ish default bar
    COMPARING = 1,  // yellow – being compared right now
    SWAPPING  = 2,  // orange – swap happening
    SORTED    = 3,  // green  – in final position
    PIVOT     = 4,  // red    – pivot element (Quick Sort)
    HEAP      = 5,  // cyan   – heap frontier (Heap Sort)
    MERGING   = 6,  // purple – merge window (Merge Sort)
    SELECTED  = 7,  // blue   – selection marker
};

// ── Callback type ───────────────────────────────────────────
// The Visualizer passes a callback so each algorithm can
// "yield" after every interesting step and allow a frame draw.
// Signature: void callback(const std::vector<int>& arr,
//                          const std::vector<BarState>& states)
using StepCallback = std::function<void(const std::vector<int>&,
                                        const std::vector<BarState>&)>;

// ── Algorithm information record ───────────────────────────
struct AlgorithmInfo
{
    std::string name;
    std::string description;
    std::string bestCase;
    std::string averageCase;
    std::string worstCase;
    std::string spaceComplexity;
    bool        isStable;
    bool        isInPlace;
};

// ===========================================================
//  SortBase  –  Abstract base class for all algorithms
// ===========================================================
class SortBase
{
public:
    virtual ~SortBase() = default;

    // Run the full sort; calls cb after each visual step.
    // ascending = true → sort low-to-high; false → high-to-low
    virtual void sort(std::vector<int>& arr,
                      Statistics&       stats,
                      StepCallback      cb,
                      bool              ascending = true) = 0;

    virtual AlgorithmInfo info() const = 0;
};

// ===========================================================
//  1. Bubble Sort
// ===========================================================
class BubbleSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;
};

// ===========================================================
//  2. Selection Sort
// ===========================================================
class SelectionSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;
};

// ===========================================================
//  3. Insertion Sort
// ===========================================================
class InsertionSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;
};

// ===========================================================
//  4. Merge Sort
// ===========================================================
class MergeSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;

private:
    void mergeSort(std::vector<int>& arr, int l, int r,
                   Statistics& stats, StepCallback& cb, bool ascending);
    void merge(std::vector<int>& arr, int l, int m, int r,
               Statistics& stats, StepCallback& cb, bool ascending);
};

// ===========================================================
//  5. Quick Sort
// ===========================================================
class QuickSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;

private:
    void quickSort(std::vector<int>& arr, int low, int high,
                   Statistics& stats, StepCallback& cb, bool ascending);
    int  partition(std::vector<int>& arr, int low, int high,
                   Statistics& stats, StepCallback& cb, bool ascending);
};

// ===========================================================
//  6. Heap Sort
// ===========================================================
class HeapSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;

private:
    void heapify(std::vector<int>& arr, int n, int i,
                 Statistics& stats, StepCallback& cb, bool ascending);
};

// ===========================================================
//  7. Shell Sort
// ===========================================================
class ShellSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;
};

// ===========================================================
//  8. Radix Sort
// ===========================================================
class RadixSort : public SortBase
{
public:
    void sort(std::vector<int>& arr, Statistics& stats,
              StepCallback cb, bool ascending = true) override;
    AlgorithmInfo info() const override;

private:
    void countingSort(std::vector<int>& arr, int exp,
                      Statistics& stats, StepCallback& cb);
};
