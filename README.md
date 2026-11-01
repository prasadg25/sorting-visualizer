# Sorting Algorithm Visualizer

An interactive desktop application for visualizing eight sorting algorithms.
Built with **C++17** and **SFML 2.6**.

> DSA Mini Project | B.Tech Computer Engineering

## Features

- Visualize Bubble, Selection, Insertion, Merge, Quick, Heap, Shell, and Radix
  Sort.
- Choose ascending or descending order.
- Watch comparisons, swaps/moves, array accesses, total operations, and elapsed
  time update during a run.
- Pause, resume, reset, shuffle, or advance through a sort one step at a time.
- Compare all eight algorithms using the same input.
- Generate random, nearly sorted, reverse, or duplicate-heavy arrays, or enter
  custom positive integers.
- View each algorithm's time and space complexity, stability, and in-place
  properties.

## Algorithms at a glance

| Algorithm | Best time | Average time | Worst time | Extra space | Stable |
| --- | --- | --- | --- | --- | --- |
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1) | No |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) | Yes |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n)* | No |
| Heap Sort | O(n log n) | O(n log n) | O(n log n) | O(1) | No |
| Shell Sort | O(n log n) | O(n log² n)* | O(n²)* | O(1) | No |
| Radix Sort | O(nk) | O(nk) | O(nk) | O(n + k) | Yes |

- `n` is the number of elements. For Radix Sort, `k` is the number of digits in
  the largest value.
- Quick Sort uses a median-of-three pivot. Its recursion can use O(n) stack
  space in the worst case.
- Shell Sort's bounds depend on its gap sequence. This implementation uses
  Knuth gaps (`1, 4, 13, 40, ...`); the displayed average and worst-case bounds
  are estimates for this sequence.
- Stability describes whether equal-valued elements keep their original
  relative order. The visualizer sorts integers, so stability is an algorithm
  property rather than a visible distinction between equal values.

## How each algorithm works

The traces below show representative ascending runs. They illustrate key
intermediate steps; the visualizer animates the complete run.

### 1. Bubble Sort

Repeatedly compares neighboring values and swaps them when they are out of
order. After each pass, the largest remaining value has moved to the end.

**Example**

```text
Start:          [5, 1, 4, 2]
After pass 1:   [1, 4, 2, 5]
After pass 2:   [1, 2, 4, 5]
Sorted:         [1, 2, 4, 5]
```

### 2. Selection Sort

Finds the smallest value in the unsorted section and swaps it into the next
position in the sorted section.

**Example**

```text
Start:                  [29, 10, 14, 37, 13]
Select 10 for position 0: [10, 29, 14, 37, 13]
Select 13 for position 1: [10, 13, 14, 37, 29]
Sorted:                 [10, 13, 14, 29, 37]
```

### 3. Insertion Sort

Builds a sorted prefix one value at a time, inserting each next value into its
correct position in that prefix.

**Example**

```text
Start:             [5, 2, 4, 6, 1]
Insert 2:          [2, 5, 4, 6, 1]
Insert 4:          [2, 4, 5, 6, 1]
Insert 1:          [1, 2, 4, 5, 6]
```

### 4. Merge Sort

Recursively divides the array into smaller sections, sorts those sections, and
merges them back together in order.

**Example**

```text
Start:               [38, 27, 43, 3]
Split and sort:      [27, 38] + [3, 43]
Merge sorted halves: [3, 27, 38, 43]
```

### 5. Quick Sort

Chooses a pivot, partitions values around it, then recursively sorts each side.
This implementation selects its pivot using the median of the first, middle,
and last values.

**Example**

```text
Start:                 [8, 3, 7, 4, 9, 2, 6]
First pivot:           6
After first partition: [3, 4, 2] + [6] + [9, 7, 8]
Sorted:                [2, 3, 4, 6, 7, 8, 9]
```

### 6. Heap Sort

Builds a max-heap, then repeatedly moves the largest value to the end of the
unsorted section and restores the heap.

**Example**

```text
Start:           [4, 10, 3, 5, 1]
Max-heap:        [10, 5, 3, 4, 1]
Extract maximum: [5, 4, 3, 1 | 10]
Sorted:          [1, 3, 4, 5, 10]
```

The vertical bar separates the sorted suffix from the remaining heap.

### 7. Shell Sort

Improves on insertion sort by first comparing values far apart, then reducing
the gap until the final gap of 1 completes an insertion-sort pass.

**Example**

```text
Start:       [8, 5, 3, 7, 6, 2, 1, 4]
Gap 4 pass:  [6, 2, 1, 4, 8, 5, 3, 7]
Gap 1 pass:  [1, 2, 3, 4, 5, 6, 7, 8]
```

For eight values, this implementation uses gaps 4 and 1.

### 8. Radix Sort

Sorts non-negative integers by processing one decimal digit at a time, starting
with the least significant digit. Each digit pass uses a stable counting-sort
step.

**Example**

```text
Start:      [170, 45, 75, 90, 802, 24, 2, 66]
Ones pass:  [170, 90, 802, 2, 24, 45, 75, 66]
Tens pass:  [802, 2, 24, 45, 66, 170, 75, 90]
Hundreds:   [2, 24, 45, 66, 75, 90, 170, 802]
```

Radix Sort in this project expects non-negative integers. The custom-input
control accepts positive integers.

## Requirements

- Windows
- SFML 2.6.x for MinGW, installed under `C:\SFML` with `include`, `lib`, and
  `bin` folders. Use SFML libraries compatible with the MSVCRT MinGW runtime.
- MinGW-w64 GCC and GDB. Install the MSVCRT WinLibs package:

  ```powershell
  winget install --id BrechtSanders.WinLibs.POSIX.MSVCRT --exact
  ```

The build script expects the default WinGet installation path for WinLibs.

## Build and run

1. Install the requirements above.
2. Double-click `build.bat`, or run it from Command Prompt.
3. The script compiles the source, copies the required SFML and MinGW runtime
   DLLs into the project folder, and launches `SortingVisualizer.exe`.

To run an existing build without compiling, launch `SortingVisualizer.exe` from
the project folder.

### VS Code

- Press `Ctrl+Shift+B` and choose **Build Sorting Visualizer** to compile.
- Choose **Build & Run Sorting Visualizer** to compile and launch.
- Start the **Debug Sorting Visualizer** configuration to build and debug.

## Controls

| Control | Action |
| --- | --- |
| Algorithm buttons | Select which sort to visualize |
| Start Sorting | Start the selected algorithm |
| Pause / Resume | Pause or continue the current run |
| Reset Array | Regenerate the current array preset |
| Shuffle Current | Shuffle the current values |
| Order | Toggle ascending and descending order |
| Step | Enable or disable step-by-step sorting |
| Next Step | Advance the sort when Step is enabled |
| Compare All | Compare all eight algorithms on the same input |
| Custom Input | Enter comma-separated positive integers; press `Enter` to apply or `Esc` to cancel |
| Array presets | Generate Random, Nearly Sorted, Reverse, or Duplicates data |
| Array Size slider | Change the generated array size |
| Speed slider and presets | Adjust the animation delay |

### Keyboard shortcuts

| Key | Action |
| --- | --- |
| `Space` | Pause or resume; advances a step when Step mode is enabled |
| `S` | Start sorting |
| `R` | Reset the current array preset |
| `Right Arrow` | Advance one step in Step mode |
| `Esc` | Stop sorting or close the comparison view |

## Project structure

```text
dsa mini project/
|-- main.cpp                   Application entry point
|-- SortingAlgorithms.h        Algorithm declarations and shared types
|-- SortingAlgorithms.cpp      Eight sorting implementations
|-- Visualizer.h               SFML GUI declarations and controls
|-- Visualizer.cpp             GUI, layout, and application logic
|-- Statistics.h               Statistics data and implementation
|-- build.bat                  Windows build-and-run script
|-- SortingVisualizer.exe      Built application
|-- *.dll                      SFML and MinGW runtime dependencies
|-- README.md                  Project documentation
`-- .vscode/                   Build tasks and debug configuration
```
