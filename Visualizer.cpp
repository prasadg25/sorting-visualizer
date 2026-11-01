// ============================================================
//  Visualizer.cpp  –  Full GUI implementation (SFML)
//  DSA Mini Project | B.Tech Computer Engineering
// ============================================================
#include "Visualizer.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <numeric>

// ── Platform font paths ─────────────────────────────────────
#ifdef _WIN32
  static const std::string FONT_REG  = "C:/Windows/Fonts/segoeui.ttf";
  static const std::string FONT_BOLD = "C:/Windows/Fonts/segoeuib.ttf";
#else
  static const std::string FONT_REG  = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
  static const std::string FONT_BOLD = "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf";
#endif

// ===========================================================
//  Button helpers
// ===========================================================
void Button::draw(sf::RenderWindow& win)
{
    if (!visible)
        return;
    if (disabled)
        shape.setFillColor(Colors::ButtonBg);
    else if (hovered)
        shape.setFillColor(Colors::ButtonHover);
    else
        shape.setFillColor(Colors::ButtonBg);
    win.draw(shape);
    win.draw(label);
}
bool Button::contains(sf::Vector2i pos) const
{
    if (!visible)
        return false;
    return shape.getGlobalBounds().contains((float)pos.x, (float)pos.y);
}

// ===========================================================
//  Slider helpers
// ===========================================================
void Slider::draw(sf::RenderWindow& win)
{
    win.draw(track);
    win.draw(thumb);
    win.draw(label);
    win.draw(valueText);
}
bool Slider::contains(sf::Vector2i pos) const
{
    sf::FloatRect r = track.getGlobalBounds();
    r.top    -= 8; r.height += 16;
    return r.contains((float)pos.x, (float)pos.y);
}
void Slider::setValue(float val)
{
    value = std::clamp(val, minVal, maxVal);
    float left  = track.getPosition().x;
    float width = track.getSize().x;
    float t     = (value - minVal) / (maxVal - minVal);
    float thumbX = left + t * width - thumb.getSize().x / 2.f;
    thumb.setPosition(thumbX, thumb.getPosition().y);

    if (maxVal >= 1000.f) {
        if (value >= 1000.f) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (value / 1000.f) << "s";
            valueText.setString(oss.str());
        } else {
            valueText.setString(std::to_string((int)value) + "ms");
        }
    } else {
        valueText.setString(std::to_string((int)value));
    }
}

void Slider::updateFromMouse(float mouseX)
{
    float left  = track.getPosition().x;
    float width = track.getSize().x;
    float t     = std::clamp((mouseX - left) / width, 0.f, 1.f);
    value = minVal + t * (maxVal - minVal);

    float thumbX = left + t * width - thumb.getSize().x / 2.f;
    thumb.setPosition(thumbX, thumb.getPosition().y);

    if (maxVal >= 1000.f) {
        if (value >= 1000.f) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (value / 1000.f) << "s";
            valueText.setString(oss.str());
        } else {
            valueText.setString(std::to_string((int)value) + "ms");
        }
    } else {
        valueText.setString(std::to_string((int)value));
    }
}

// ===========================================================
//  AlgoButton helpers
// ===========================================================
void AlgoButton::draw(sf::RenderWindow& win)
{
    if (selected) {
        shape.setFillColor(sf::Color{30, 58, 95, 255});
        shape.setOutlineColor(Colors::Accent);
        shape.setOutlineThickness(2.f);
    } else {
        shape.setFillColor(Colors::ButtonBg);
        shape.setOutlineColor(Colors::BorderDim);
        shape.setOutlineThickness(1.f);
    }
    win.draw(shape);
    win.draw(label);
}
bool AlgoButton::contains(sf::Vector2i pos) const
{
    return shape.getGlobalBounds().contains((float)pos.x, (float)pos.y);
}

// ===========================================================
//  Visualizer – constructor / destructor
// ===========================================================
Visualizer::Visualizer()
    : window(sf::VideoMode(WIN_W, WIN_H), "Sorting Algorithm Visualizer | DSA Mini Project",
             sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(120);

    // Load fonts
    if (!font.loadFromFile(FONT_REG))
        font.loadFromFile("arial.ttf");   // fallback
    if (!fontBold.loadFromFile(FONT_BOLD))
        fontBold = font;

    initAlgorithms();
    initUI();
    generateArray(ArrayMode::RANDOM);
}

Visualizer::~Visualizer()
{
    stopSorting();
}

// ===========================================================
//  initAlgorithms – register the 8 sort objects
// ===========================================================
void Visualizer::initAlgorithms()
{
    algorithms.push_back(std::make_unique<BubbleSort>());
    algorithms.push_back(std::make_unique<SelectionSort>());
    algorithms.push_back(std::make_unique<InsertionSort>());
    algorithms.push_back(std::make_unique<MergeSort>());
    algorithms.push_back(std::make_unique<QuickSort>());
    algorithms.push_back(std::make_unique<HeapSort>());
    algorithms.push_back(std::make_unique<ShellSort>());
    algorithms.push_back(std::make_unique<RadixSort>());
}

// ===========================================================
//  initUI – build all buttons, sliders, algo-buttons
// ===========================================================
void Visualizer::initUI()
{
    // Helper to center text in a button
    auto centerText = [](sf::Text& txt, const sf::RectangleShape& box) {
        sf::FloatRect bounds = txt.getLocalBounds();
        sf::Vector2f pos = box.getPosition();
        sf::Vector2f sz  = box.getSize();
        txt.setPosition(
            std::floor(pos.x + (sz.x - bounds.width) / 2.f - bounds.left),
            std::floor(pos.y + (sz.y - bounds.height) / 2.f - bounds.top - 1.f)
        );
    };

    // ── Algorithm selection buttons (4 rows x 2 columns) ──────
    const std::vector<std::string> names = {
        "Bubble Sort", "Selection Sort", "Insertion Sort", "Merge Sort",
        "Quick Sort",  "Heap Sort",      "Shell Sort",     "Radix Sort"
    };

    float bx   = VIZ_W + 12.f;
    float by   = 30.f;
    float colW = 134.f;
    float bh   = 26.f;
    float gapX = 8.f;
    float gapY = 4.f;

    for (int i = 0; i < (int)names.size(); ++i)
    {
        int row = i / 2;
        int col = i % 2;
        float ax = bx + col * (colW + gapX);
        float ay = by + row * (bh + gapY);

        AlgoButton ab;
        ab.index = i;
        ab.selected = (i == 0);
        ab.shape.setPosition(ax, ay);
        ab.shape.setSize({colW, bh});
        ab.shape.setFillColor(Colors::ButtonBg);
        ab.shape.setOutlineThickness(1.f);
        ab.shape.setOutlineColor(Colors::BorderDim);

        ab.label.setFont(font);
        ab.label.setString(names[i]);
        ab.label.setCharacterSize(12u);
        ab.label.setFillColor(Colors::TextPrimary);
        centerText(ab.label, ab.shape);

        algoButtons.push_back(std::move(ab));
    }

    // ── Control buttons ─────────────────────────────────────
    auto makeBtn = [&](ControlButton id, const std::string& lbl, float x, float y, float w, float h, std::function<void()> fn) {
        Button b;
        b.id = id;
        b.shape.setPosition(x, y);
        b.shape.setSize({w, h});
        b.shape.setFillColor(Colors::ButtonBg);
        b.shape.setOutlineThickness(1.f);
        b.shape.setOutlineColor(Colors::BorderDim);
        b.onClick = fn;

        b.label.setFont(font);
        b.label.setString(lbl);
        b.label.setCharacterSize(12u);
        b.label.setFillColor(Colors::TextPrimary);
        centerText(b.label, b.shape);
        return b;
    };

    float fullW = PANEL_W - 24.f; // 276 px

    // Execution Buttons
    buttons.push_back(makeBtn(ControlButton::StartSorting, "Start Sorting", bx, 174.f, fullW, 30.f, [this]{ startSorting(); }));
    buttons.push_back(makeBtn(ControlButton::PauseResume, "Pause / Resume", bx, 208.f, fullW, 28.f, [this]{ pauseResumeSorting(); }));
    buttons.push_back(makeBtn(ControlButton::ResetArray, "Reset Array", bx, 240.f, fullW, 28.f, [this]{ resetArray(); }));
    buttons.push_back(makeBtn(ControlButton::ShuffleCurrent, "Shuffle Current", bx, 272.f, fullW, 26.f, [this]{ shuffleArray(); }));

    // Modes & Tools
    buttons.push_back(makeBtn(ControlButton::Order, "Order: Ascending", bx, 324.f, colW, 26.f, [this]{
        ascending = !ascending;
        setButtonLabel(ControlButton::Order, ascending ? "Order: Ascending" : "Order: Descending");
    }));

    buttons.push_back(makeBtn(ControlButton::StepMode, "Step: OFF", bx + colW + gapX, 324.f, colW, 26.f, [this]{
        stepMode = !stepMode;
        setButtonLabel(ControlButton::StepMode, stepMode ? "Step: ON" : "Step: OFF");
    }));

    buttons.push_back(makeBtn(ControlButton::NextStep, "Next Step (->)", bx, 354.f, fullW, 26.f, [this]{ stepForward(); }));

    buttons.push_back(makeBtn(ControlButton::CompareAll, "Compare All (8)", bx, 384.f, colW, 26.f, [this]{ runComparison(); }));
    buttons.push_back(makeBtn(ControlButton::CustomInput, "Custom Input", bx + colW + gapX, 384.f, colW, 26.f, [this]{
        inputMode = !inputMode;
        inputBuffer.clear();
    }));

    // Array Presets
    buttons.push_back(makeBtn(ControlButton::RandomPreset, "Random", bx, 436.f, colW, 25.f, [this]{ generateArray(ArrayMode::RANDOM); }));
    buttons.push_back(makeBtn(ControlButton::NearlySortedPreset, "Nearly Sorted", bx + colW + gapX, 436.f, colW, 25.f, [this]{ generateArray(ArrayMode::NEARLY_SORTED); }));
    buttons.push_back(makeBtn(ControlButton::ReversePreset, "Reverse", bx, 465.f, colW, 25.f, [this]{ generateArray(ArrayMode::REVERSE); }));
    buttons.push_back(makeBtn(ControlButton::DuplicatePreset, "Duplicates", bx + colW + gapX, 465.f, colW, 25.f, [this]{ generateArray(ArrayMode::DUPLICATES); }));

    // ── Sliders ─────────────────────────────────────────────
    float sy = 516.f;

    sizeSlider.minVal = 10; sizeSlider.maxVal = 150; sizeSlider.value = 60;
    sizeSlider.track.setPosition(bx, sy + 18.f);
    sizeSlider.track.setSize({fullW, 4.f});
    sizeSlider.track.setFillColor(Colors::SliderTrack);
    float t = (sizeSlider.value - sizeSlider.minVal) / (sizeSlider.maxVal - sizeSlider.minVal);
    sizeSlider.thumb.setSize({14.f, 14.f});
    sizeSlider.thumb.setFillColor(Colors::SliderThumb);
    sizeSlider.thumb.setPosition(bx + t * fullW - 7.f, sy + 18.f - 5.f);
    sizeSlider.label.setFont(font);
    sizeSlider.label.setString("Array Size");
    sizeSlider.label.setCharacterSize(11u);
    sizeSlider.label.setFillColor(Colors::TextMuted);
    sizeSlider.label.setPosition(bx, sy);
    sizeSlider.valueText.setFont(font);
    sizeSlider.valueText.setString("60");
    sizeSlider.valueText.setCharacterSize(11u);
    sizeSlider.valueText.setFillColor(Colors::Accent);
    sizeSlider.valueText.setPosition(bx + fullW - 30.f, sy);

    float spy = 554.f;
    speedSlider.minVal = 5; speedSlider.maxVal = 2000; speedSlider.value = 100;
    speedSlider.track.setPosition(bx, spy + 18.f);
    speedSlider.track.setSize({fullW, 4.f});
    speedSlider.track.setFillColor(Colors::SliderTrack);
    float t2 = (speedSlider.value - speedSlider.minVal) / (speedSlider.maxVal - speedSlider.minVal);
    speedSlider.thumb.setSize({14.f, 14.f});
    speedSlider.thumb.setFillColor(Colors::SliderThumb);
    speedSlider.thumb.setPosition(bx + t2 * fullW - 7.f, spy + 18.f - 5.f);
    speedSlider.label.setFont(font);
    speedSlider.label.setString("Speed (Delay)");
    speedSlider.label.setCharacterSize(11u);
    speedSlider.label.setFillColor(Colors::TextMuted);
    speedSlider.label.setPosition(bx, spy);
    speedSlider.valueText.setFont(font);
    speedSlider.valueText.setString("100ms");
    speedSlider.valueText.setCharacterSize(11u);
    speedSlider.valueText.setFillColor(Colors::Accent);
    speedSlider.valueText.setPosition(bx + fullW - 46.f, spy);

    // Speed Preset Buttons (3 buttons under the slider)
    float pbtnW = (fullW - 8.f) / 3.f;
    float pbtnY = 586.f;
    buttons.push_back(makeBtn(ControlButton::FastSpeed, "Fast: 20ms", bx, pbtnY, pbtnW, 20.f, [this]{
        speedSlider.setValue(20.f);
        speedDelay = 20;
    }));
    buttons.push_back(makeBtn(ControlButton::MediumSpeed, "Med: 200ms", bx + pbtnW + 4.f, pbtnY, pbtnW, 20.f, [this]{
        speedSlider.setValue(200.f);
        speedDelay = 200;
    }));
    buttons.push_back(makeBtn(ControlButton::TeachingSpeed, "Teach: 800ms", bx + 2.f * (pbtnW + 4.f), pbtnY, pbtnW, 20.f, [this]{
        speedSlider.setValue(800.f);
        speedDelay = 800;
    }));
}

void Visualizer::setButtonLabel(ControlButton id, const std::string& text)
{
    auto it = std::find_if(buttons.begin(), buttons.end(),
                           [id](const Button& button) { return button.id == id; });
    if (it == buttons.end())
        throw std::logic_error("Control button is missing.");

    it->label.setString(text);
    sf::FloatRect bounds = it->label.getLocalBounds();
    sf::Vector2f pos = it->shape.getPosition();
    sf::Vector2f size = it->shape.getSize();
    it->label.setPosition(
        std::floor(pos.x + (size.x - bounds.width) / 2.f - bounds.left),
        std::floor(pos.y + (size.y - bounds.height) / 2.f - bounds.top - 1.f)
    );
}

// ===========================================================
//  Array generation
// ===========================================================
void Visualizer::generateArray(ArrayMode mode)
{
    stopSorting();
    stats.reset();
    arrayMode = mode;
    arraySize = sizeSlider.getInt();

    if (mode == ArrayMode::CUSTOM)
    {
        arr = customArray;
        arraySize = (int)arr.size();
    }
    else
    {
        arr.resize(arraySize);
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(5, WIN_H - VIZ_TOP - VIZ_BOT - 10);

    switch (mode)
    {
    case ArrayMode::RANDOM:
        for (auto& v : arr) v = dist(rng);
        break;

    case ArrayMode::NEARLY_SORTED:
        std::iota(arr.begin(), arr.end(), 1);
        // Scale to visible range
        for (auto& v : arr) v = (int)(5 + (double)v / arraySize * (WIN_H - VIZ_TOP - VIZ_BOT - 10));
        // Randomly swap ~5% of elements
        {
            int swaps = std::max(1, arraySize / 20);
            std::uniform_int_distribution<int> idxDist(0, arraySize - 1);
            for (int i = 0; i < swaps; ++i)
                std::swap(arr[idxDist(rng)], arr[idxDist(rng)]);
        }
        break;

    case ArrayMode::REVERSE:
        std::iota(arr.begin(), arr.end(), 1);
        for (auto& v : arr) v = (int)(5 + (double)v / arraySize * (WIN_H - VIZ_TOP - VIZ_BOT - 10));
        std::reverse(arr.begin(), arr.end());
        break;

    case ArrayMode::DUPLICATES:
        {
            int unique = std::max(3, arraySize / 5);
            std::uniform_int_distribution<int> uniq(5, WIN_H - VIZ_TOP - VIZ_BOT - 10);
            std::vector<int> palette(unique);
            for (auto& v : palette) v = uniq(rng);
            std::uniform_int_distribution<int> pick(0, unique - 1);
            for (auto& v : arr) v = palette[pick(rng)];
        }
        break;

    case ArrayMode::CUSTOM:
        break;
    }
    barStates.assign(arraySize, BarState::NORMAL);
    appState = AppState::IDLE;
    stats.status = "Ready";
}

void Visualizer::initArray()
{
    generateArray(ArrayMode::RANDOM);
}

void Visualizer::parseCustomInput()
{
    std::vector<int> tmp;
    std::stringstream ss(inputBuffer);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        try {
            int v = std::stoi(token);
            if (v > 0) tmp.push_back(v);
        } catch (...) {}
    }
    if (!tmp.empty())
    {
        stopSorting();
        stats.reset();
        customArray = tmp;
        arrayMode = ArrayMode::CUSTOM;
        arr = tmp;
        arraySize = (int)arr.size();
        barStates.assign(arraySize, BarState::NORMAL);
        appState = AppState::IDLE;
        stats.status = "Custom array loaded (" + std::to_string(arraySize) + " items)";

        // Small array: automatically adjust to slow teacher speed for viva!
        if (arraySize <= 15) {
            speedSlider.setValue(700.f);
            speedDelay = 700;
        }
    }
    inputMode = false;
    inputBuffer.clear();
}

// ===========================================================
//  makeCallback – called from sort thread each visual step
// ===========================================================
StepCallback Visualizer::makeCallback()
{
    return [this](const std::vector<int>& a, const std::vector<BarState>& s)
    {
        // Wait while paused
        while (pauseFlag.load() && !stopFlag.load())
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (stopFlag.load()) return;

        // Step mode: wait for user key press
        if (stepMode)
        {
            waitForStep.store(true);
            while (waitForStep.load() && !stopFlag.load())
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (stopFlag.load()) return;
        }

        {
            std::lock_guard<std::mutex> lock(arrMutex);
            arr       = a;
            barStates = s;
        }
        stats.updateElapsed();

        // Delay for animation speed
        if (speedDelay > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(speedDelay));
    };
}

// ===========================================================
//  Sorting control
// ===========================================================
void Visualizer::startSorting()
{
    if (appState == AppState::SORTING) return;
    if (arr.empty()) return;

    stopSorting();
    stats.reset();
    barStates.assign(arr.size(), BarState::NORMAL);
    appState = AppState::SORTING;
    stats.isSorting = true;
    stats.status    = "Sorting...";
    speedDelay      = speedSlider.getInt();

    pauseFlag.store(false);
    stopFlag.store(false);

    auto cb = makeCallback();

    // Capture copies for the thread
    int  algo = selectedAlgo;
    bool asc  = ascending;

    sortThread = std::thread([this, algo, asc, cb]() mutable {
        stats.startTimer();
        algorithms[algo]->sort(arr, stats, cb, asc);
        stats.stopTimer();

        if (!stopFlag.load())
        {
            appState = AppState::COMPLETE;
            stats.isComplete = true;
            stats.isSorting  = false;
            stats.status     = "Sorted!";
            std::lock_guard<std::mutex> lock(arrMutex);
            barStates.assign(arr.size(), BarState::SORTED);
        }
    });
}

void Visualizer::pauseResumeSorting()
{
    if (appState == AppState::SORTING)
    {
        pauseFlag.store(true);
        appState     = AppState::PAUSED;
        stats.isPaused = true;
        stats.status   = "Paused";
    }
    else if (appState == AppState::PAUSED)
    {
        pauseFlag.store(false);
        appState       = AppState::SORTING;
        stats.isPaused = false;
        stats.status   = "Sorting...";
    }
}

void Visualizer::stopSorting()
{
    stopFlag.store(true);
    pauseFlag.store(false);
    waitForStep.store(false);
    if (sortThread.joinable())
        sortThread.join();
    stopFlag.store(false);
    appState = AppState::IDLE;
}

void Visualizer::resetArray()
{
    stopSorting();
    generateArray(arrayMode);
}

void Visualizer::shuffleArray()
{
    if (arr.empty()) return;

    stopSorting();
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(arr.begin(), arr.end(), rng);
    barStates.assign(arr.size(), BarState::NORMAL);
    stats.reset();
    appState = AppState::IDLE;
    stats.status = "Array shuffled";
}

void Visualizer::stepForward()
{
    if (!stepMode) return;
    waitForStep.store(false);
    if (appState == AppState::IDLE)
        startSorting();
}

// ===========================================================
//  Comparison mode – run all 8 algorithms on same data
// ===========================================================
void Visualizer::runComparison()
{
    stopSorting();
    compResults.clear();
    showComparison = true;
    appState = AppState::COMPARISON;
    stats.status = "Running comparison...";

    // Use a copy of current array for reproducibility
    std::vector<int> original = arr;

    for (int i = 0; i < (int)algorithms.size(); ++i)
    {
        std::vector<int> tmp = original;
        Statistics       st;
        st.reset();
        st.startTimer();

        // Silent callback – just measure, don't animate
        algorithms[i]->sort(tmp, st, [](const std::vector<int>&,
                                         const std::vector<BarState>&){}, true);
        st.stopTimer();

        CompResult cr;
        cr.name         = algorithms[i]->info().name;
        cr.comparisons  = st.comparisons;
        cr.swaps        = st.swaps;
        cr.arrayAccesses= st.arrayAccesses;
        cr.totalOps     = st.totalOps();
        cr.timeMs       = st.elapsedMs;
        compResults.push_back(cr);
    }
    stats.status = "Comparison complete";
}

// ===========================================================
//  Event handling
// ===========================================================
void Visualizer::handleEvents()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed)
        {
            stopSorting();
            window.close();
        }
        else if (e.type == sf::Event::MouseButtonPressed)
            handleMousePressed(e);
        else if (e.type == sf::Event::MouseMoved)
            handleMouseMoved(e);
        else if (e.type == sf::Event::MouseButtonReleased)
            handleMouseReleased(e);
        else if (e.type == sf::Event::KeyPressed)
            handleKeyPressed(e);
        else if (e.type == sf::Event::TextEntered)
            handleTextEntered(e);
    }
}

void Visualizer::handleMousePressed(sf::Event& e)
{
    sf::Vector2i pos = {e.mouseButton.x, e.mouseButton.y};

    // Algo buttons
    for (auto& ab : algoButtons)
    {
        if (ab.contains(pos) && appState != AppState::SORTING)
        {
            selectedAlgo = ab.index;
            for (auto& a : algoButtons) a.selected = false;
            ab.selected = true;
            return;
        }
    }

    // Control buttons
    for (auto& b : buttons)
    {
        if (b.id == ControlButton::NextStep && !stepMode)
            continue;
        if (b.visible && !b.disabled && b.contains(pos))
        {
            if (b.onClick) b.onClick();
            return;
        }
    }

    // Sliders
    if (sizeSlider.contains(pos))
    {
        sizeSlider.dragging = true;
        sizeSlider.updateFromMouse((float)pos.x);
        updateSizeFromSlider();
    }
    if (speedSlider.contains(pos))
    {
        speedSlider.dragging = true;
        speedSlider.updateFromMouse((float)pos.x);
        updateSpeedFromSlider();
    }

    // Close comparison view on click inside viz area
    if (showComparison && pos.x < VIZ_W)
    {
        showComparison = false;
        appState = AppState::IDLE;
    }
}

void Visualizer::handleMouseMoved(sf::Event& e)
{
    sf::Vector2i pos = {e.mouseMove.x, e.mouseMove.y};

    for (auto& b : buttons)
        b.hovered = (b.visible && !b.disabled && b.contains(pos));

    if (sizeSlider.dragging)
    {
        sizeSlider.updateFromMouse((float)pos.x);
        updateSizeFromSlider();
    }
    if (speedSlider.dragging)
    {
        speedSlider.updateFromMouse((float)pos.x);
        updateSpeedFromSlider();
    }
}

void Visualizer::handleMouseReleased(sf::Event& /*e*/)
{
    sizeSlider.dragging  = false;
    speedSlider.dragging = false;
}

void Visualizer::handleKeyPressed(sf::Event& e)
{
    if (inputMode)
    {
        if (e.key.code == sf::Keyboard::Return)
            parseCustomInput();
        else if (e.key.code == sf::Keyboard::Escape)
        { inputMode = false; inputBuffer.clear(); }
        return;
    }

    switch (e.key.code)
    {
    case sf::Keyboard::Space:
        if (stepMode) {
            stepForward();
        } else {
            pauseResumeSorting();
        }
        break;
    case sf::Keyboard::Right: stepForward();        break;
    case sf::Keyboard::R:     resetArray();         break;
    case sf::Keyboard::S:     startSorting();       break;
    case sf::Keyboard::Escape:
        if (showComparison) { showComparison = false; appState = AppState::IDLE; }
        else stopSorting();
        break;
    default: break;
    }
}

void Visualizer::handleTextEntered(sf::Event& e)
{
    if (!inputMode) return;
    char c = (char)e.text.unicode;
    if (c == '\b' && !inputBuffer.empty())
        inputBuffer.pop_back();
    else if ((std::isdigit(c) || c == ',' || c == ' ') && inputBuffer.size() < 100)
        inputBuffer += c;
}

// ===========================================================
//  Slider -> value propagation
// ===========================================================
void Visualizer::updateSizeFromSlider()
{
    int newSize = sizeSlider.getInt();
    if (newSize != arraySize && appState == AppState::IDLE)
    {
        arraySize = newSize;
        generateArray(arrayMode);
    }
}
void Visualizer::updateSpeedFromSlider()
{
    speedDelay = speedSlider.getInt();
}

// ===========================================================
//  Main loop
// ===========================================================
void Visualizer::run()
{
    while (window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
}

void Visualizer::update()
{
    // Nothing heavy – rendering is purely reactive
}

// ===========================================================
//  Rendering
// ===========================================================
void Visualizer::render()
{
    window.clear(Colors::Background);

    drawTopBar();

    if (showComparison)
        drawComparisonMode();
    else
        drawVisualization();

    drawStatsBar();
    drawControlPanel();

    if (inputMode) drawInputBox();

    window.display();
}

// ── Utility text draw helper ────────────────────────────────
void Visualizer::drawText(const std::string& str, float x, float y,
                           unsigned size, sf::Color color,
                           bool bold, sf::Text::Style style)
{
    sf::Text t;
    t.setFont(bold ? fontBold : font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(color);
    t.setStyle(style);
    t.setPosition(x, y);
    window.draw(t);
}

// ── Filled rectangle helper ─────────────────────────────────
void Visualizer::drawRoundedRect(float x, float y, float w, float h,
                                  sf::Color fill, sf::Color outline,
                                  float outlineThick)
{
    sf::RectangleShape r({w, h});
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(outlineThick);
    window.draw(r);
}

// ── Bar color from state ─────────────────────────────────────
sf::Color Visualizer::barColor(BarState state) const
{
    switch (state)
    {
    case BarState::COMPARING: return Colors::Comparing;
    case BarState::SWAPPING:  return Colors::Swapping;
    case BarState::SORTED:    return Colors::Sorted;
    case BarState::PIVOT:     return Colors::Pivot;
    case BarState::HEAP:      return Colors::Heap;
    case BarState::MERGING:   return Colors::Merging;
    case BarState::SELECTED:  return Colors::Selected;
    default:                  return Colors::Normal;
    }
}

// ===========================================================
//  Top bar
// ===========================================================
void Visualizer::drawTopBar()
{
    drawRoundedRect(0, 0, WIN_W, VIZ_TOP, Colors::PanelBg);
    drawText("Sorting Algorithm Visualizer", 16, 12, 22, Colors::Accent, true);
    drawText("DSA Mini Project | B.Tech Computer Engineering",
             16, 38, 13, Colors::TextMuted);

    // Current algorithm badge
    std::string algoName = algorithms[selectedAlgo]->info().name;
    float badgeX = VIZ_W - 260.f;
    drawRoundedRect(badgeX, 14, 240, 30, sf::Color{30,58,95,220},
                    Colors::Accent, 1.5f);
    drawText(algoName, badgeX + 10, 20, 14, Colors::Accent, true);

    // Order badge
    std::string order = ascending ? "Order: Ascending" : "Order: Descending";
    float ob = badgeX - 150.f;
    drawRoundedRect(ob, 14, 140, 30, Colors::ButtonBg, Colors::BorderDim, 1.f);
    drawText(order, ob + 10, 21, 12, Colors::TextMuted);

    // Keyboard shortcuts hint
    drawText("[Space] Pause  [R] Reset  [S] Start  [->] Step  [Esc] Stop",
             VIZ_W - 500.f, 46, 11, Colors::TextMuted);
}

// ===========================================================
//  Visualization area – bars
// ===========================================================
void Visualizer::drawVisualization()
{
    // Background
    drawRoundedRect(0, VIZ_TOP, VIZ_W, VIZ_H, Colors::Background);

    drawBars();
    drawColorLegend();
}

void Visualizer::drawBars()
{
    std::lock_guard<std::mutex> lock(arrMutex);
    if (arr.empty()) return;

    int n = (int)arr.size();
    float totalW = (float)VIZ_W - 8.f;
    float barW   = totalW / n;
    float gap    = barW > 8.f ? 2.f : (barW > 4.f ? 1.f : 0.f);

    int maxVal = *std::max_element(arr.begin(), arr.end());
    if (maxVal <= 0) maxVal = 1;

    for (int i = 0; i < n; ++i)
    {
        // Reserve 46px at top for value badge and 24px at bottom for [i] index
        float availableH = VIZ_H - 74.f;
        float h = std::max(8.f, (float)(arr[i]) / maxVal * availableH);
        float x = 4.f + i * barW;
        float y = (float)(VIZ_TOP + VIZ_H - 26.f) - h;
        float w = barW - gap;

        BarState state = (i < (int)barStates.size()) ? barStates[i] : BarState::NORMAL;
        sf::Color c = barColor(state);

        // Draw bar
        sf::RectangleShape bar({std::max(1.f, w), h});
        bar.setPosition(x, y);
        bar.setFillColor(c);
        window.draw(bar);

        // Value text (large, bold, centered on the bar)
        if (n <= 35 && barW > 14.f)
        {
            std::string valStr = std::to_string(arr[i]);
            unsigned fontSize = (n <= 8) ? 18u : ((n <= 16) ? 14u : 11u);

            sf::Text valText;
            valText.setFont(fontBold);
            valText.setString(valStr);
            valText.setCharacterSize(fontSize);
            valText.setFillColor(state == BarState::NORMAL ? Colors::TextPrimary : c);

            sf::FloatRect tb = valText.getLocalBounds();
            float textX = x + (w - tb.width) / 2.f - tb.left;
            float textY = y - tb.height - 10.f;

            // Draw clean background pill behind number for small arrays
            if (n <= 16) {
                float pillW = tb.width + 12.f;
                float pillH = tb.height + 6.f;
                drawRoundedRect(textX - 6.f, textY - 2.f, pillW, pillH,
                                sf::Color{20, 25, 38, 230}, c, 1.f);
            }
            valText.setPosition(textX, textY);
            window.draw(valText);

            // Index Label [i] underneath bar at the bottom
            if (n <= 25) {
                sf::Text idxText;
                idxText.setFont(font);
                idxText.setString("[" + std::to_string(i) + "]");
                idxText.setCharacterSize(10u);
                idxText.setFillColor(Colors::TextMuted);
                sf::FloatRect ib = idxText.getLocalBounds();
                idxText.setPosition(x + (w - ib.width) / 2.f - ib.left, VIZ_TOP + VIZ_H - 20.f);
                window.draw(idxText);
            }
        }
    }
}

// ===========================================================
//  Color legend
// ===========================================================
void Visualizer::drawColorLegend()
{
    struct LegendEntry { sf::Color color; std::string label; };
    static const std::vector<LegendEntry> entries = {
        {Colors::Normal,    "Normal"},
        {Colors::Comparing, "Comparing"},
        {Colors::Swapping,  "Swapping"},
        {Colors::Sorted,    "Sorted"},
        {Colors::Pivot,     "Pivot"},
        {Colors::Heap,      "Heap"},
        {Colors::Merging,   "Merge"},
        {Colors::Selected,  "Selected"},
    };

    float lx = 8.f;
    float ly = (float)(VIZ_TOP + VIZ_H) - 22.f;
    for (auto& e : entries)
    {
        drawRoundedRect(lx, ly, 12, 12, e.color);
        drawText(e.label, lx + 15.f, ly - 1.f, 10u, Colors::TextMuted);
        lx += e.label.size() * 7.f + 20.f;
    }
}

// ===========================================================
//  Bottom stats bar
// ===========================================================
void Visualizer::drawStatsBar()
{
    float sy = (float)(VIZ_TOP + VIZ_H);
    drawRoundedRect(0, sy, VIZ_W, VIZ_BOT, Colors::PanelBg);

    // Separator line
    sf::RectangleShape sep({(float)VIZ_W, 1.f});
    sep.setPosition(0, sy);
    sep.setFillColor(Colors::BorderDim);
    window.draw(sep);

    // Status
    sf::Color statusColor = Colors::TextMuted;
    if (appState == AppState::SORTING)  statusColor = Colors::Comparing;
    if (appState == AppState::COMPLETE) statusColor = Colors::Sorted;
    if (appState == AppState::PAUSED)   statusColor = Colors::Warning;

    drawText("Status:", 10, sy + 10, 12u, Colors::TextMuted);
    drawText(stats.status, 65, sy + 10, 13u, statusColor, true);

    // Stats row 1
    float bx = 10.f, by = sy + 32.f;
    auto stat = [&](const std::string& label, const std::string& val,
                    sf::Color vc = Colors::Accent) {
        drawRoundedRect(bx, by, 130, 44, Colors::ButtonBg, Colors::BorderDim, 1.f);
        drawText(label, bx + 6, by + 6,  11u, Colors::TextMuted);
        drawText(val,   bx + 6, by + 22, 15u, vc, true);
        bx += 138.f;
    };

    // Format helpers
    auto fmtLL  = [](long long v) { return std::to_string(v); };
    auto fmtMs  = [](double v) {
        std::ostringstream oss; oss << std::fixed << std::setprecision(2) << v << " ms";
        return oss.str();
    };

    stat("Comparisons",    fmtLL(stats.comparisons),    Colors::Comparing);
    stat("Swaps/Moves",    fmtLL(stats.swaps),          Colors::Swapping);
    stat("Array Accesses", fmtLL(stats.arrayAccesses),  Colors::Heap);
    stat("Total Ops",      fmtLL(stats.totalOps()),     Colors::Accent);
    stat("Elapsed Time",   fmtMs(stats.elapsedMs),      Colors::Sorted);

    // Algorithm info summary below stats
    auto inf = algorithms[selectedAlgo]->info();
    float ix = 10.f, iy = sy + 84.f;
    drawText("Best: " + inf.bestCase,    ix,       iy, 11u, Colors::TextMuted);
    drawText("Avg: "  + inf.averageCase, ix + 130, iy, 11u, Colors::TextMuted);
    drawText("Worst: "+ inf.worstCase,   ix + 260, iy, 11u, Colors::TextMuted);
    drawText("Space: "+ inf.spaceComplexity, ix+390, iy, 11u, Colors::TextMuted);
    drawText(inf.isStable ? "Stable" : "Unstable", ix+520, iy, 11u,
             inf.isStable ? Colors::Sorted : Colors::Warning);
    drawText(inf.isInPlace ? "In-Place" : "Not In-Place", ix+590, iy, 11u,
             inf.isInPlace ? Colors::Heap : Colors::Merging);

    if (appState == AppState::COMPLETE)
        drawText("[DONE] Sorting complete! Array is fully sorted.",
                 10, sy + 104, 12u, Colors::Sorted, true);
    else if (appState == AppState::PAUSED)
        drawText("[PAUSED] Press [Space] or Pause/Resume to continue.",
                 10, sy + 104, 12u, Colors::Warning);
}

// ===========================================================
//  Right control panel
// ===========================================================
void Visualizer::drawControlPanel()
{
    // Panel background
    drawRoundedRect((float)VIZ_W, 0, PANEL_W, WIN_H, Colors::PanelBg,
                    Colors::BorderDim, 1.f);

    float px = (float)VIZ_W + 12.f;

    // Headers
    drawText("Algorithms", px, 12.f, 11u, Colors::Accent, true);
    for (auto& ab : algoButtons) ab.draw(window);

    drawText("Execution", px, 156.f, 11u, Colors::Accent, true);
    drawText("Modes & Tools", px, 306.f, 11u, Colors::Accent, true);
    drawText("Array Presets", px, 418.f, 11u, Colors::Accent, true);
    drawText("Settings", px, 498.f, 11u, Colors::Accent, true);

    // Control buttons
    for (auto& b : buttons)
    {
        b.visible = b.id != ControlButton::NextStep || stepMode;
        if (!b.visible)
            continue;

        // Disable conditions
        b.disabled = false;
        if (b.id == ControlButton::StartSorting && appState == AppState::SORTING)
            b.disabled = true;
        if (b.id == ControlButton::PauseResume && appState == AppState::IDLE)
            b.disabled = true;
        if (b.id == ControlButton::NextStep && appState == AppState::COMPLETE)
            b.disabled = true;

        // Button theme colors
        if (b.id == ControlButton::StartSorting && !b.disabled) {
            b.shape.setFillColor({22, 101, 52, 255}); // Start: Green
            b.shape.setOutlineColor({34, 197, 94, 255});
        }
        else if (b.id == ControlButton::PauseResume && !b.disabled) {
            b.shape.setFillColor({180, 83, 9, 255});  // Pause: Amber
            b.shape.setOutlineColor({251, 191, 36, 255});
        }
        else if (b.id == ControlButton::ResetArray && !b.disabled) {
            b.shape.setFillColor({30, 58, 95, 255});  // Reset: Navy
            b.shape.setOutlineColor({99, 179, 237, 255});
        }
        else if (b.id == ControlButton::StepMode && stepMode) {
            b.shape.setFillColor(sf::Color{30, 58, 95, 255}); // Step Mode Active: Blue
            b.shape.setOutlineColor(Colors::Accent);
            b.shape.setOutlineThickness(1.5f);
        }
        else if (b.id == ControlButton::NextStep && !b.disabled) {
            b.shape.setFillColor({29, 78, 216, 255}); // Next Step: Highlight
            b.shape.setOutlineColor({96, 165, 250, 255});
        }
        else if ((b.id == ControlButton::FastSpeed && speedDelay == 20) ||
                 (b.id == ControlButton::MediumSpeed && speedDelay == 200) ||
                 (b.id == ControlButton::TeachingSpeed && speedDelay == 800)) {
            b.shape.setFillColor(sf::Color{20, 80, 110, 255});
            b.shape.setOutlineColor(Colors::Accent);
        }
        else if (!b.disabled) {
            b.shape.setFillColor(b.hovered ? Colors::ButtonHover : Colors::ButtonBg);
            b.shape.setOutlineColor(Colors::BorderDim);
        }
        else {
            b.shape.setFillColor({25, 28, 38, 255});
            b.shape.setOutlineColor({40, 45, 60, 255});
        }

        b.draw(window);
    }

    // Sliders
    sizeSlider.draw(window);
    speedSlider.draw(window);

    // Algorithm info panel
    drawAlgoInfo();
}

// ===========================================================
//  Algorithm info section (bottom of right panel)
// ===========================================================
void Visualizer::drawAlgoInfo()
{
    auto inf = algorithms[selectedAlgo]->info();

    float sx = (float)VIZ_W + 12.f;
    float cy = 614.f;
    float cardW = PANEL_W - 24.f;
    float cardH = WIN_H - cy - 12.f;

    drawRoundedRect(sx, cy, cardW, cardH, Colors::ButtonBg, Colors::BorderDim, 1.f);
    float tx = sx + 10.f; float ty = cy + 10.f;

    drawText(inf.name, tx, ty, 13u, Colors::Accent, true);
    ty += 20.f;

    // Wrap description to ~34 chars per line
    std::string desc = inf.description;
    while (desc.size() > 34) {
        size_t split = desc.rfind(' ', 34);
        if (split == std::string::npos) split = 34;
        drawText(desc.substr(0, split), tx, ty, 10u, Colors::TextMuted);
        desc = desc.substr(split + 1);
        ty += 14.f;
    }
    if (!desc.empty()) { drawText(desc, tx, ty, 10u, Colors::TextMuted); ty += 18.f; }
    else ty += 8.f;

    auto row = [&](const std::string& k, const std::string& v, sf::Color vc = Colors::Accent) {
        drawText(k + ":", tx, ty, 10u, Colors::TextMuted);
        drawText(v, tx + 65.f, ty, 10u, vc, true);
        ty += 15.f;
    };
    row("Best",     inf.bestCase,         Colors::Sorted);
    row("Average",  inf.averageCase,      Colors::Comparing);
    row("Worst",    inf.worstCase,        Colors::Danger);
    row("Space",    inf.spaceComplexity,  Colors::Heap);
    row("Stable",   inf.isStable ? "Yes" : "No",
        inf.isStable ? Colors::Sorted : Colors::Warning);
    row("In-Place", inf.isInPlace ? "Yes" : "No",
        inf.isInPlace ? Colors::Sorted : Colors::Merging);
}

// ===========================================================
//  Comparison mode view
// ===========================================================
void Visualizer::drawComparisonMode()
{
    // Background
    drawRoundedRect(0, VIZ_TOP, VIZ_W, VIZ_H + VIZ_BOT,
                    sf::Color{10,13,20,255});

    drawText("Algorithm Comparison Mode", 16, VIZ_TOP + 10, 20u, Colors::Accent, true);
    drawText("All algorithms ran on identical input. Time varies with hardware.",
             16, VIZ_TOP + 36, 12u, Colors::TextMuted);
    drawText("Click anywhere to return to visualizer.", 16, VIZ_TOP + 52, 11u, Colors::TextMuted);

    if (compResults.empty()) return;

    // Column headers
    float hx = 16, hy = VIZ_TOP + 74.f;
    float cols[] = {0, 170, 290, 390, 490, 590, 700};
    std::vector<std::string> headers = {"Algorithm","Comparisons","Swaps","Accesses","Total Ops","Time (ms)"};
    for (int i = 0; i < 6; ++i)
        drawText(headers[i], hx + cols[i], hy, 12u, Colors::TextMuted, true);

    // Row separator
    drawRoundedRect(hx, hy + 17, VIZ_W - 32, 1, Colors::BorderDim);

    // Find max values for bar chart scaling
    long long maxTotalOps = 1;
    double    maxTime     = 0.001;
    for (auto& cr : compResults) {
        maxTotalOps = std::max(maxTotalOps, cr.totalOps);
        maxTime     = std::max(maxTime, cr.timeMs);
    }

    float rowH = 48.f;
    for (int i = 0; i < (int)compResults.size(); ++i)
    {
        auto& cr = compResults[i];
        float ry = hy + 20.f + i * rowH;

        // Alternating row bg
        if (i % 2 == 0)
            drawRoundedRect(hx, ry - 2, VIZ_W - 32, rowH - 2,
                            sf::Color{20,25,38,180});

        drawText(cr.name,               hx + cols[0], ry + 4,  12u, Colors::TextPrimary, true);
        drawText(std::to_string(cr.comparisons),  hx + cols[1], ry + 4, 11u, Colors::Comparing);
        drawText(std::to_string(cr.swaps),         hx + cols[2], ry + 4, 11u, Colors::Swapping);
        drawText(std::to_string(cr.arrayAccesses), hx + cols[3], ry + 4, 11u, Colors::Heap);
        drawText(std::to_string(cr.totalOps),      hx + cols[4], ry + 4, 11u, Colors::Accent);
        std::ostringstream oss; oss << std::fixed << std::setprecision(3) << cr.timeMs;
        drawText(oss.str(),             hx + cols[5], ry + 4, 11u, Colors::Sorted);

        // Mini bar for total ops
        float barMaxW = 200.f;
        float bw = (float)cr.totalOps / maxTotalOps * barMaxW;
        drawRoundedRect(hx + cols[4], ry + 18, barMaxW, 6, Colors::ButtonBg);
        drawRoundedRect(hx + cols[4], ry + 18, bw, 6, Colors::Accent);

        // Mini bar for time
        float tw = (float)(cr.timeMs / maxTime) * barMaxW;
        drawRoundedRect(hx + cols[5], ry + 18, barMaxW, 6, Colors::ButtonBg);
        drawRoundedRect(hx + cols[5], ry + 18, tw, 6, Colors::Sorted);
    }
}

// ===========================================================
//  Custom input overlay
// ===========================================================
void Visualizer::drawInputBox()
{
    // Dim overlay
    sf::RectangleShape overlay({(float)VIZ_W, (float)(VIZ_H + VIZ_BOT)});
    overlay.setPosition(0, VIZ_TOP);
    overlay.setFillColor(sf::Color{0,0,0,180});
    window.draw(overlay);

    float bx = VIZ_W / 2.f - 280.f;
    float by = WIN_H / 2.f - 70.f;
    drawRoundedRect(bx, by, 560, 140, Colors::PanelBg, Colors::Accent, 2.f);
    drawText("Custom Array Input", bx + 16, by + 12, 15u, Colors::Accent, true);
    drawText("Enter comma-separated integers (e.g. 40,15,3,90,22):",
             bx + 16, by + 36, 12u, Colors::TextMuted);

    // Input text box
    drawRoundedRect(bx + 16, by + 58, 528, 36, Colors::ButtonBg, Colors::Accent, 1.5f);
    drawText(inputBuffer + "|", bx + 24, by + 66, 14u, Colors::TextPrimary);
    drawText("[Enter] to apply   [Esc] to cancel", bx + 16, by + 108, 11u, Colors::TextMuted);
}
