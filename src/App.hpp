#pragma once

#include "Voronoi.hpp"

#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include <memory>

class App
{
public:
    App();
    
private:
    sf::RenderWindow win;

    std::unique_ptr<Voronoi> voronoi;

    sf::Clock win_clock;
    sf::Time dt;
    
    void setupWin();
    void setupGui();
    
    void winEvents();
    void run();
};