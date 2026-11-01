// ============================================================
//  SortingAlgorithms.cpp  –  Complete implementations
//  DSA Mini Project | B.Tech Computer Engineering
// ============================================================
#include "SortingAlgorithms.h"
#include <algorithm>
#include <cmath>
#include <numeric>   // std::iota

// ── Helper: build a states vector of uniform color ─────────
[[maybe_unused]]
static std::vector<BarState> makeStates(size_t n,
                                        BarState def = BarState::NORMAL)
{
    return std::vector<BarState>(n, def);
}

// ── Helper: mark a range as SORTED, rest as def ────────────
[[maybe_unused]]
static std::vector<BarState> makeSortedStates(size_t n, int sortedFrom,
                                               int sortedTo,
                                               BarState def = BarState::NORMAL)
{
    std::vector<BarState> s(n, def);
    for (int i = sortedFrom; i <= sortedTo && i < (int)n; ++i)
        s[i] = BarState::SORTED;
    return s;
}

// ── Comparison helper (respects ascending flag) ────────────
// Returns true if a should come before b in the final order
static bool cmp(int a, int b, bool ascending)
{
    return ascending ? (a < b) : (a > b);
}

// ===========================================================
//  1. BUBBLE SORT
//     Time:  O(n) best | O(n²) avg/worst
//     Space: O(1)  |  Stable  |  In-Place
// ===========================================================
void BubbleSort::sort(std::vector<int>& arr, Statistics& stats,
                      StepCallback cb, bool ascending)
{
    int n = (int)arr.size();
    std::vector<BarState> states(n, BarState::NORMAL);

    for (int i = 0; i < n - 1; ++i)
    {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j)
        {
            // Mark comparing
            states.assign(n, BarState::NORMAL);
            for (int k = n - i - 1; k < n; ++k) states[k] = BarState::SORTED;
            states[j]   = BarState::COMPARING;
            states[j+1] = BarState::COMPARING;
            stats.addComparison();
            stats.addArrayAccess(2);
            cb(arr, states);

            if (!cmp(arr[j], arr[j+1], ascending))
            {
                // Mark swapping
                states[j]   = BarState::SWAPPING;
                states[j+1] = BarState::SWAPPING;
                cb(arr, states);

                std::swap(arr[j], arr[j+1]);
                stats.addSwap();
                stats.addArrayAccess(2);
                swapped = true;
            }
        }
        // Mark latest sorted position
        states.assign(n, BarState::NORMAL);
        for (int k = n - i - 1; k < n; ++k) states[k] = BarState::SORTED;
        cb(arr, states);

        if (!swapped) break;  // already sorted
    }
    // All sorted
    states.assign(n, BarState::SORTED);
    cb(arr, states);
}

AlgorithmInfo BubbleSort::info() const
{
    return {
        "Bubble Sort",
        "Repeatedly steps through the list, compares adjacent elements and "
        "swaps them if they are in the wrong order. After each full pass the "
        "largest unsorted element 'bubbles' to its correct position.",
        "O(n)",
        "O(n^2)",
        "O(n^2)",
        "O(1)",
        true,   // stable
        true    // in-place
    };
}

// ===========================================================
//  2. SELECTION SORT
//     Time:  O(n²) best/avg/worst
//     Space: O(1)  |  Unstable  |  In-Place
// ===========================================================
void SelectionSort::sort(std::vector<int>& arr, Statistics& stats,
                         StepCallback cb, bool ascending)
{
    int n = (int)arr.size();
    std::vector<BarState> states(n, BarState::NORMAL);

    for (int i = 0; i < n - 1; ++i)
    {
        int minIdx = i;

        for (int j = i + 1; j < n; ++j)
        {
            states.assign(n, BarState::NORMAL);
            for (int k = 0; k < i; ++k) states[k] = BarState::SORTED;
            states[minIdx] = BarState::SELECTED;
            states[j]      = BarState::COMPARING;
            stats.addComparison();
            stats.addArrayAccess(2);
            cb(arr, states);

            if (cmp(arr[j], arr[minIdx], ascending))
                minIdx = j;
        }

        if (minIdx != i)
        {
            states.assign(n, BarState::NORMAL);
            for (int k = 0; k < i; ++k) states[k] = BarState::SORTED;
            states[i]      = BarState::SWAPPING;
            states[minIdx] = BarState::SWAPPING;
            cb(arr, states);

            std::swap(arr[i], arr[minIdx]);
            stats.addSwap();
            stats.addArrayAccess(2);
        }

        states.assign(n, BarState::NORMAL);
        for (int k = 0; k <= i; ++k) states[k] = BarState::SORTED;
        cb(arr, states);
    }
    states.assign(n, BarState::SORTED);
    cb(arr, states);
}

AlgorithmInfo SelectionSort::info() const
{
    return {
        "Selection Sort",
        "Divides the array into sorted and unsorted regions. In each pass it "
        "finds the minimum (or maximum) element in the unsorted region and "
        "places it at the beginning of that region.",
        "O(n^2)",
        "O(n^2)",
        "O(n^2)",
        "O(1)",
        false,  // unstable
        true    // in-place
    };
}

// ===========================================================
//  3. INSERTION SORT
//     Time:  O(n) best | O(n²) avg/worst
//     Space: O(1)  |  Stable  |  In-Place
// ===========================================================
void InsertionSort::sort(std::vector<int>& arr, Statistics& stats,
                         StepCallback cb, bool ascending)
{
    int n = (int)arr.size();
    std::vector<BarState> states(n, BarState::NORMAL);

    states[0] = BarState::SORTED;
    cb(arr, states);

    for (int i = 1; i < n; ++i)
    {
        int key = arr[i];
        int j   = i - 1;

        states.assign(n, BarState::NORMAL);
        for (int k = 0; k < i; ++k) states[k] = BarState::SORTED;
        states[i] = BarState::COMPARING;
        cb(arr, states);

        while (j >= 0 && !cmp(arr[j], key, ascending))
        {
            stats.addComparison();
            stats.addArrayAccess(2);

            states.assign(n, BarState::NORMAL);
            for (int k = 0; k <= i; ++k) states[k] = BarState::SORTED;
            states[j]   = BarState::SWAPPING;
            states[j+1] = BarState::SWAPPING;
            cb(arr, states);

            arr[j + 1] = arr[j];
            stats.addSwap();
            --j;
        }
        stats.addComparison();  // final failed comparison
        arr[j + 1] = key;
        stats.addArrayAccess();

        states.assign(n, BarState::NORMAL);
        for (int k = 0; k <= i; ++k) states[k] = BarState::SORTED;
        cb(arr, states);
    }
    states.assign(n, BarState::SORTED);
    cb(arr, states);
}

AlgorithmInfo InsertionSort::info() const
{
    return {
        "Insertion Sort",
        "Builds the sorted array one element at a time by inserting each new "
        "element into its correct position among the already-sorted elements. "
        "Very efficient for small or nearly-sorted arrays.",
        "O(n)",
        "O(n^2)",
        "O(n^2)",
        "O(1)",
        true,  // stable
        true   // in-place
    };
}

// ===========================================================
//  4. MERGE SORT
//     Time:  O(n log n) all cases
//     Space: O(n)  |  Stable  |  Not In-Place
// ===========================================================
void MergeSort::sort(std::vector<int>& arr, Statistics& stats,
                     StepCallback cb, bool ascending)
{
    mergeSort(arr, 0, (int)arr.size() - 1, stats, cb, ascending);

    std::vector<BarState> states(arr.size(), BarState::SORTED);
    cb(arr, states);
}

void MergeSort::mergeSort(std::vector<int>& arr, int l, int r,
                          Statistics& stats, StepCallback& cb, bool ascending)
{
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l,   m, stats, cb, ascending);
    mergeSort(arr, m+1, r, stats, cb, ascending);
    merge(arr, l, m, r, stats, cb, ascending);
}

void MergeSort::merge(std::vector<int>& arr, int l, int m, int r,
                      Statistics& stats, StepCallback& cb, bool ascending)
{
    int n = (int)arr.size();
    std::vector<int> left(arr.begin()+l, arr.begin()+m+1);
    std::vector<int> right(arr.begin()+m+1, arr.begin()+r+1);

    int i = 0, j = 0, k = l;
    while (i < (int)left.size() && j < (int)right.size())
    {
        stats.addComparison();
        stats.addArrayAccess(2);

        std::vector<BarState> states(n, BarState::NORMAL);
        for (int x = l; x <= r; ++x) states[x] = BarState::MERGING;
        states[l + i] = BarState::COMPARING;
        states[m + 1 + j] = BarState::COMPARING;
        cb(arr, states);

        if (cmp(left[i], right[j], ascending))
            arr[k++] = left[i++];
        else
            arr[k++] = right[j++];

        stats.addArrayAccess();
        cb(arr, states);
    }
    while (i < (int)left.size())
    {
        arr[k++] = left[i++];
        stats.addArrayAccess();
        std::vector<BarState> states(n, BarState::NORMAL);
        for (int x = l; x <= r; ++x) states[x] = BarState::MERGING;
        cb(arr, states);
    }
    while (j < (int)right.size())
    {
        arr[k++] = right[j++];
        stats.addArrayAccess();
        std::vector<BarState> states(n, BarState::NORMAL);
        for (int x = l; x <= r; ++x) states[x] = BarState::MERGING;
        cb(arr, states);
    }
}

AlgorithmInfo MergeSort::info() const
{
    return {
        "Merge Sort",
        "A divide-and-conquer algorithm that recursively splits the array in "
        "half, sorts each half, then merges the two sorted halves. Guarantees "
        "O(n log n) performance at the cost of O(n) auxiliary space.",
        "O(n log n)",
        "O(n log n)",
        "O(n log n)",
        "O(n)",
        true,   // stable
        false   // not in-place
    };
}

// ===========================================================
//  5. QUICK SORT  (median-of-three pivot)
//     Time:  O(n log n) best/avg | O(n²) worst
//     Space: O(log n)  |  Unstable  |  In-Place
// ===========================================================
void QuickSort::sort(std::vector<int>& arr, Statistics& stats,
                     StepCallback cb, bool ascending)
{
    quickSort(arr, 0, (int)arr.size() - 1, stats, cb, ascending);

    std::vector<BarState> states(arr.size(), BarState::SORTED);
    cb(arr, states);
}

void QuickSort::quickSort(std::vector<int>& arr, int low, int high,
                          Statistics& stats, StepCallback& cb, bool ascending)
{
    if (low < high)
    {
        int pi = partition(arr, low, high, stats, cb, ascending);
        quickSort(arr, low,    pi - 1, stats, cb, ascending);
        quickSort(arr, pi + 1, high,   stats, cb, ascending);
    }
    else if (low == high)
    {
        // Single element is sorted
        int n = (int)arr.size();
        std::vector<BarState> states(n, BarState::NORMAL);
        states[low] = BarState::SORTED;
        cb(arr, states);
    }
}

int QuickSort::partition(std::vector<int>& arr, int low, int high,
                         Statistics& stats, StepCallback& cb, bool ascending)
{
    int n = (int)arr.size();
    // Median-of-three pivot selection
    int mid = low + (high - low) / 2;
    if (cmp(arr[mid], arr[low], ascending)) std::swap(arr[low], arr[mid]);
    if (cmp(arr[high], arr[low], ascending)) std::swap(arr[low], arr[high]);
    if (cmp(arr[mid], arr[high], ascending)) std::swap(arr[mid], arr[high]);
    int pivot = arr[high];
    stats.addArrayAccess(3);

    std::vector<BarState> states(n, BarState::NORMAL);
    states[high] = BarState::PIVOT;
    cb(arr, states);

    int i = low - 1;
    for (int j = low; j < high; ++j)
    {
        stats.addComparison();
        stats.addArrayAccess(2);

        states.assign(n, BarState::NORMAL);
        states[high] = BarState::PIVOT;
        states[j]    = BarState::COMPARING;
        cb(arr, states);

        if (cmp(arr[j], pivot, ascending) || (arr[j] == pivot && i < j - 1))
        {
            ++i;
            if (i != j)
            {
                states[i] = BarState::SWAPPING;
                states[j] = BarState::SWAPPING;
                cb(arr, states);
                std::swap(arr[i], arr[j]);
                stats.addSwap();
                stats.addArrayAccess(2);
            }
        }
    }
    // Place pivot
    states.assign(n, BarState::NORMAL);
    states[i+1] = BarState::SWAPPING;
    states[high] = BarState::SWAPPING;
    cb(arr, states);

    std::swap(arr[i + 1], arr[high]);
    stats.addSwap();
    stats.addArrayAccess(2);

    // Pivot is now sorted
    states.assign(n, BarState::NORMAL);
    states[i + 1] = BarState::SORTED;
    cb(arr, states);

    return i + 1;
}

AlgorithmInfo QuickSort::info() const
{
    return {
        "Quick Sort",
        "Picks a pivot element and partitions the array so all elements less "
        "than the pivot come before it and all greater elements come after it. "
        "Recursively sorts the sub-arrays. Uses median-of-three pivot for "
        "better average performance.",
        "O(n log n)",
        "O(n log n)",
        "O(n^2)",
        "O(log n)",
        false,  // unstable
        true    // in-place
    };
}

// ===========================================================
//  6. HEAP SORT
//     Time:  O(n log n) all cases
//     Space: O(1)  |  Unstable  |  In-Place
// ===========================================================
void HeapSort::sort(std::vector<int>& arr, Statistics& stats,
                    StepCallback cb, bool ascending)
{
    int n = (int)arr.size();

    // Build max-heap (for ascending) or min-heap (for descending)
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i, stats, cb, ascending);

    // Extract elements from heap one by one
    for (int i = n - 1; i > 0; --i)
    {
        std::vector<BarState> states(n, BarState::NORMAL);
        for (int k = i + 1; k < n; ++k) states[k] = BarState::SORTED;
        states[0] = BarState::SWAPPING;
        states[i] = BarState::SWAPPING;
        cb(arr, states);

        std::swap(arr[0], arr[i]);
        stats.addSwap();
        stats.addArrayAccess(2);

        heapify(arr, i, 0, stats, cb, ascending);
    }
    std::vector<BarState> states(n, BarState::SORTED);
    cb(arr, states);
}

void HeapSort::heapify(std::vector<int>& arr, int n, int i,
                       Statistics& stats, StepCallback& cb, bool ascending)
{
    int extreme = i;   // largest for max-heap, smallest for min-heap
    int left    = 2 * i + 1;
    int right   = 2 * i + 2;
    int total   = (int)arr.size();

    std::vector<BarState> states(total, BarState::NORMAL);
    for (int k = n; k < total; ++k) states[k] = BarState::SORTED;
    states[i] = BarState::HEAP;
    cb(arr, states);

    if (left < n)
    {
        stats.addComparison();
        stats.addArrayAccess(2);
        if (cmp(arr[extreme], arr[left], ascending))
            extreme = left;
    }
    if (right < n)
    {
        stats.addComparison();
        stats.addArrayAccess(2);
        if (cmp(arr[extreme], arr[right], ascending))
            extreme = right;
    }

    if (extreme != i)
    {
        states[i]       = BarState::SWAPPING;
        states[extreme] = BarState::SWAPPING;
        cb(arr, states);

        std::swap(arr[i], arr[extreme]);
        stats.addSwap();
        stats.addArrayAccess(2);

        heapify(arr, n, extreme, stats, cb, ascending);
    }
}

AlgorithmInfo HeapSort::info() const
{
    return {
        "Heap Sort",
        "Builds a max-heap from the array, then repeatedly extracts the "
        "maximum element placing it at the end, and restores the heap "
        "property. Guarantees O(n log n) in all cases with no extra space.",
        "O(n log n)",
        "O(n log n)",
        "O(n log n)",
        "O(1)",
        false,  // unstable
        true    // in-place
    };
}

// ===========================================================
//  7. SHELL SORT  (Knuth gap sequence: 1, 4, 13, 40, …)
//     Time:  O(n log n) best | O(n^1.5) avg | O(n²) worst
//     Space: O(1)  |  Unstable  |  In-Place
// ===========================================================
void ShellSort::sort(std::vector<int>& arr, Statistics& stats,
                     StepCallback cb, bool ascending)
{
    int n = (int)arr.size();

    // Compute Knuth gap sequence
    int gap = 1;
    while (gap < n / 3) gap = 3 * gap + 1;

    std::vector<BarState> states(n, BarState::NORMAL);

    while (gap >= 1)
    {
        for (int i = gap; i < n; ++i)
        {
            int temp = arr[i];
            int j    = i;

            states.assign(n, BarState::NORMAL);
            states[i] = BarState::COMPARING;
            cb(arr, states);

            while (j >= gap && !cmp(arr[j - gap], temp, ascending))
            {
                stats.addComparison();
                stats.addArrayAccess(2);

                states.assign(n, BarState::NORMAL);
                states[j]       = BarState::SWAPPING;
                states[j - gap] = BarState::SWAPPING;
                cb(arr, states);

                arr[j] = arr[j - gap];
                stats.addSwap();
                j -= gap;
            }
            stats.addComparison();  // final failed comparison
            arr[j] = temp;
            stats.addArrayAccess();

            states.assign(n, BarState::NORMAL);
            states[j] = BarState::SORTED;
            cb(arr, states);
        }
        gap /= 3;
    }
    states.assign(n, BarState::SORTED);
    cb(arr, states);
}

AlgorithmInfo ShellSort::info() const
{
    return {
        "Shell Sort",
        "A generalization of Insertion Sort that allows the exchange of far-"
        "apart elements. The array is sorted at decreasing gap intervals "
        "using the Knuth sequence (1, 4, 13, 40, ...). The final pass with "
        "gap=1 is a standard insertion sort on a nearly-sorted array.",
        "O(n log n)",
        "O(n log^2 n)",
        "O(n^2)",
        "O(1)",
        false,  // unstable
        true    // in-place
    };
}

// ===========================================================
//  8. RADIX SORT  (LSD, base-10, non-negative integers)
//     Time:  O(nk) where k = digits in max element
//     Space: O(n + k)  |  Stable  |  Not In-Place
// ===========================================================
void RadixSort::sort(std::vector<int>& arr, Statistics& stats,
                     StepCallback cb, bool ascending)
{
    if (arr.empty()) return;

    // For descending we sort ascending then reverse
    int maxVal = *std::max_element(arr.begin(), arr.end());

    for (int exp = 1; maxVal / exp > 0; exp *= 10)
        countingSort(arr, exp, stats, cb);

    if (!ascending)
    {
        std::reverse(arr.begin(), arr.end());
        std::vector<BarState> states(arr.size(), BarState::MERGING);
        cb(arr, states);
    }

    std::vector<BarState> states(arr.size(), BarState::SORTED);
    cb(arr, states);
}

void RadixSort::countingSort(std::vector<int>& arr, int exp,
                             Statistics& stats, StepCallback& cb)
{
    int n = (int)arr.size();
    std::vector<int> output(n);
    std::vector<int> count(10, 0);

    // Count occurrences
    for (int i = 0; i < n; ++i)
    {
        count[(arr[i] / exp) % 10]++;
        stats.addArrayAccess();
    }
    // Cumulative count
    for (int i = 1; i < 10; ++i)
        count[i] += count[i - 1];

    // Build output array (traverse right-to-left for stability)
    for (int i = n - 1; i >= 0; --i)
    {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
        stats.addArrayAccess(2);
        stats.addSwap();  // counts as a "movement"
    }
    // Copy back & visualize each step
    for (int i = 0; i < n; ++i)
    {
        arr[i] = output[i];
        stats.addArrayAccess();

        std::vector<BarState> states(n, BarState::MERGING);
        states[i] = BarState::COMPARING;
        cb(arr, states);
    }
}

AlgorithmInfo RadixSort::info() const
{
    return {
        "Radix Sort",
        "A non-comparison sort that distributes elements into buckets "
        "according to each digit (LSD — Least Significant Digit first). "
        "Uses counting sort as a subroutine. Efficient for integers with a "
        "bounded number of digits.",
        "O(nk)",
        "O(nk)",
        "O(nk)",
        "O(n + k)",
        true,   // stable
        false   // not in-place
    };
}
