#include "App.hpp"

App::App() 
{
    this->setupWin();
    this->setupGui();
    voronoi = std::make_unique<Voronoi>(this->win, this->use_gpu, this->distance_type);

    this->run();
}

void App::setupWin()
{
    win.create(
        sf::VideoMode{ 1200, 800 }, "Voronoi Diagram", sf::Style::Close);
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
        ImGui::SFML::ProcessEvent(event);
        switch (event.type)
        {
        case sf::Event::Closed:
            win.close();
            break;
        case sf::Event::KeyReleased:
            if (event.key.code == sf::Keyboard::R)
            {
                voronoi.release();
                voronoi = std::make_unique<Voronoi>(this->win, this->use_gpu, this->distance_type);
                this->dt = this->win_clock.restart();
            }
            break;
        default:
            break;
        }
    }
}

void App::updateGui()
{
    ImGui::Begin("Settings");

    if (ImGui::Checkbox("Use GPU", &this->use_gpu))
    {
        voronoi.release();
        voronoi = std::make_unique<Voronoi>(this->win, this->use_gpu, this->distance_type);
    }

    if (ImGui::Button("Restart"))
    {
        voronoi.release();
        voronoi = std::make_unique<Voronoi>(this->win, this->use_gpu, this->distance_type);
    }

    if (ImGui::BeginListBox("Distances"))
    {
        if (ImGui::Selectable("Euclidean", this->distance_type == Voronoi::DistanceType::Euclidean))
            this->distance_type = Voronoi::DistanceType::Euclidean;
        
        if (ImGui::Selectable("Manhattan", this->distance_type == Voronoi::DistanceType::Manhattan))
            this->distance_type = Voronoi::DistanceType::Manhattan;

        if (ImGui::Selectable("Chebyshev", this->distance_type == Voronoi::DistanceType::Chebyshev))
            this->distance_type = Voronoi::DistanceType::Chebyshev;

        ImGui::EndListBox();
    }
}

void App::run()
{
    while (this->win.isOpen())
    {
        this->winEvents();

        ImGui::SFML::Update(win, dt);
        this->updateGui();
        this->voronoi->update(this->dt);
        ImGui::End();

        this->win.clear();
        this->voronoi->render();
        ImGui::SFML::Render(win);
        this->win.display();
    }

    ImGui::SFML::Shutdown();
}