// ============================================================
//  main.cpp  –  Entry point for the Sorting Visualizer
//  DSA Mini Project | B.Tech Computer Engineering
//
//  Build with build.bat or the provided task in .vscode/
// ============================================================

#include "Visualizer.h"
#include <iostream>
#include <stdexcept>

int main()
{
    std::cout << "[INFO] Starting Sorting Visualizer..." << std::endl;
    try
    {
        Visualizer app;
        std::cout << "[INFO] Visualizer created, starting main loop..." << std::endl;
        app.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "[ERROR] Exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "[ERROR] Unknown exception occurred!" << std::endl;
        return 1;
    }
    return 0;
}
