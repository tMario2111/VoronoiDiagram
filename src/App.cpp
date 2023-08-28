#include "App.hpp"

App::App() 
{
    this->setupWin();
    this->setupGui();
    voronoi = std::make_unique<Voronoi>(this->win);

    this->run();
}

void App::setupWin()
{
    win.create(
        sf::VideoMode{ 1800, 800 }, "Voronoi Diagram", sf::Style::Close);
    win.setFramerateLimit(0);
    win.setVerticalSyncEnabled(true);
}

void App::setupGui()
{
    if (!ImGui::SFML::Init(win))
        exit(EXIT_FAILURE);
    ImGui::GetIO().IniFilename = nullptr;
}

void App::winEvents()
{
    this->dt = this->win_clock.restart();
    for (sf::Event event{}; win.pollEvent(event);)
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            win.close();
            break;
        case sf::Event::KeyReleased:
            if (event.key.code == sf::Keyboard::R)
            {
                voronoi.release();
                voronoi = std::make_unique<Voronoi>(this->win);
            }
            break;
        default:
            break;
        }
    }
}

void App::run()
{
    while (this->win.isOpen())
    {
        this->winEvents();

        ImGui::SFML::Update(win, dt);
        this->voronoi->update();

        this->win.clear();
        this->voronoi->render();
        ImGui::SFML::Render(win);
        this->win.display();
    }

    ImGui::SFML::Shutdown();
}