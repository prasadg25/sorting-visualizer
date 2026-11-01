// ============================================================
//  Visualizer.h  –  SFML-based GUI for the Sorting Visualizer
//  DSA Mini Project | B.Tech Computer Engineering
// ============================================================
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

#include "SortingAlgorithms.h"
#include "Statistics.h"

// ── Layout constants (all pixels) ──────────────────────────
constexpr int WIN_W        = 1400;
constexpr int WIN_H        = 820;
constexpr int PANEL_W      = 300;   // right-side control panel width
constexpr int VIZ_W        = WIN_W - PANEL_W;
constexpr int VIZ_TOP      = 60;    // top bar height
constexpr int VIZ_BOT      = 160;   // bottom stats bar height
constexpr int VIZ_H        = WIN_H - VIZ_TOP - VIZ_BOT;

// ── Bar color palette ───────────────────────────────────────
namespace Colors {
    const sf::Color Background  { 15,  17,  26, 255};
    const sf::Color PanelBg     { 22,  25,  37, 255};
    const sf::Color Accent      { 99, 179, 237, 255};   // blue
    const sf::Color Normal      {100, 116, 139, 255};   // slate
    const sf::Color Comparing   {251, 191,  36, 255};   // yellow
    const sf::Color Swapping    {249, 115,  22, 255};   // orange
    const sf::Color Sorted      { 34, 197,  94, 255};   // green
    const sf::Color Pivot       {239,  68,  68, 255};   // red
    const sf::Color Heap        { 34, 211, 238, 255};   // cyan
    const sf::Color Merging     {168,  85, 247, 255};   // purple
    const sf::Color Selected    { 59, 130, 246, 255};   // blue2
    const sf::Color TextPrimary {226, 232, 240, 255};
    const sf::Color TextMuted   {100, 116, 139, 255};
    const sf::Color ButtonBg    { 30,  41,  59, 255};
    const sf::Color ButtonHover { 51,  65,  85, 255};
    const sf::Color ButtonActive{ 99, 179, 237, 255};
    const sf::Color SliderTrack { 51,  65,  85, 255};
    const sf::Color SliderThumb { 99, 179, 237, 255};
    const sf::Color BorderDim   { 51,  65,  85, 255};
    const sf::Color BorderBright{ 99, 179, 237, 255};
    const sf::Color Success     { 34, 197,  94, 255};
    const sf::Color Warning     {251, 191,  36, 255};
    const sf::Color Danger      {239,  68,  68, 255};
}

// ── UI element types ────────────────────────────────────────
enum class ControlButton
{
    StartSorting,
    PauseResume,
    ResetArray,
    ShuffleCurrent,
    Order,
    StepMode,
    NextStep,
    CompareAll,
    CustomInput,
    RandomPreset,
    NearlySortedPreset,
    ReversePreset,
    DuplicatePreset,
    FastSpeed,
    MediumSpeed,
    TeachingSpeed
};

struct Button {
    ControlButton      id = ControlButton::StartSorting;
    sf::RectangleShape shape;
    sf::Text           label;
    bool               hovered  = false;
    bool               disabled = false;
    bool               visible  = true;
    std::function<void()> onClick;

    void draw(sf::RenderWindow& win);
    bool contains(sf::Vector2i pos) const;
};

struct Slider {
    sf::RectangleShape track;
    sf::RectangleShape thumb;
    sf::Text           label;
    sf::Text           valueText;
    float              minVal, maxVal, value;
    bool               dragging = false;

    void draw(sf::RenderWindow& win);
    bool contains(sf::Vector2i pos) const;
    void updateFromMouse(float mouseX);
    void setValue(float val);
    float getValue() const { return value; }
    int   getInt()   const { return static_cast<int>(value); }
};

struct AlgoButton {
    sf::RectangleShape shape;
    sf::Text           label;
    bool               selected = false;
    int                index    = 0;

    void draw(sf::RenderWindow& win);
    bool contains(sf::Vector2i pos) const;
};

// ── Array-generation mode ───────────────────────────────────
enum class ArrayMode { RANDOM, NEARLY_SORTED, REVERSE, DUPLICATES, CUSTOM };

// ── Application state machine ───────────────────────────────
enum class AppState { IDLE, SORTING, PAUSED, COMPLETE, COMPARISON };

// ===========================================================
//  Visualizer – main application class
// ===========================================================
class Visualizer
{
public:
    Visualizer();
    ~Visualizer();

    void run();   // main event loop

private:
    // ── SFML objects ───────────────────────────────────────
    sf::RenderWindow window;
    sf::Font         font;
    sf::Font         fontBold;

    // ── State ──────────────────────────────────────────────
    AppState  appState     = AppState::IDLE;
    int       selectedAlgo = 0;   // 0-7
    bool      ascending    = true;
    ArrayMode arrayMode    = ArrayMode::RANDOM;

    // ── Array data ─────────────────────────────────────────
    std::vector<int>      arr;
    std::vector<int>      customArray;
    std::vector<BarState> barStates;
    std::mutex            arrMutex;

    // ── Algorithm objects ──────────────────────────────────
    std::vector<std::unique_ptr<SortBase>> algorithms;

    // ── Statistics ─────────────────────────────────────────
    Statistics stats;

    // ── Sorting thread ─────────────────────────────────────
    std::thread      sortThread;
    std::atomic<bool> pauseFlag  {false};
    std::atomic<bool> stopFlag   {false};

    // ── Speed / size sliders ───────────────────────────────
    int  arraySize  = 60;    // 10..150
    int  speedDelay = 30;    // ms delay between steps (1..200)

    // ── UI elements ────────────────────────────────────────
    std::vector<Button>     buttons;
    std::vector<AlgoButton> algoButtons;
    Slider                  sizeSlider;
    Slider                  speedSlider;

    // ── Comparison mode data ───────────────────────────────
    struct CompResult {
        std::string   name;
        long long     comparisons;
        long long     swaps;
        long long     arrayAccesses;
        long long     totalOps;
        double        timeMs;
    };
    std::vector<CompResult> compResults;
    bool showComparison = false;

    // ── Custom input ───────────────────────────────────────
    bool        inputMode    = false;
    std::string inputBuffer;

    // ── Step mode ──────────────────────────────────────────
    bool stepMode     = false;
    bool stepReady    = false;
    std::atomic<bool> waitForStep{false};

    // ── Initialization helpers ─────────────────────────────
    void initAlgorithms();
    void initUI();
    void initArray();

    // ── Event handling ─────────────────────────────────────
    void handleEvents();
    void handleMousePressed(sf::Event& e);
    void handleMouseMoved(sf::Event& e);
    void handleMouseReleased(sf::Event& e);
    void handleKeyPressed(sf::Event& e);
    void handleTextEntered(sf::Event& e);

    // ── Sorting control ────────────────────────────────────
    void startSorting();
    void pauseResumeSorting();
    void resetArray();
    void shuffleArray();
    void stopSorting();
    void runComparison();
    void setButtonLabel(ControlButton id, const std::string& text);

    // ── Array generation ───────────────────────────────────
    void generateArray(ArrayMode mode);
    void parseCustomInput();

    // ── Step-mode helpers ──────────────────────────────────
    void stepForward();

    // ── Rendering ──────────────────────────────────────────
    void update();
    void render();

    void drawTopBar();
    void drawVisualization();
    void drawBars();
    void drawStatsBar();
    void drawControlPanel();
    void drawAlgoInfo();
    void drawLiveStats();
    void drawComparisonMode();
    void drawInputBox();
    void drawColorLegend();

    // ── Utility ────────────────────────────────────────────
    sf::Color barColor(BarState state) const;
    void      drawText(const std::string& str, float x, float y,
                       unsigned size, sf::Color color,
                       bool bold = false,
                       sf::Text::Style style = sf::Text::Regular);
    void      drawRoundedRect(float x, float y, float w, float h,
                              sf::Color fill, sf::Color outline = sf::Color::Transparent,
                              float outlineThick = 0.f);
    Button&   makeButton(float x, float y, float w, float h,
                         const std::string& label);
    void      updateSizeFromSlider();
    void      updateSpeedFromSlider();

    // ── Step callback (called from sort thread) ─────────────
    StepCallback makeCallback();
};
