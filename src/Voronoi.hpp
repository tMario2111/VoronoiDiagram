#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>

#include <random>
#include <thread>
#include <vector>

constexpr unsigned int POINT_COUNT = 800;

class Voronoi
{
public:
    Voronoi(sf::RenderWindow& win, bool use_gpu);
    ~Voronoi();

    void update(sf::Time dt);
    void render();
    
private:
    sf::RenderWindow& win;
    const bool use_gpu;

    struct Point
    {
        sf::Vector2u position;
        sf::Color color;
    };

    std::vector<Point> points;
    sf::VertexArray point_vertices;

    sf::VertexArray map_vertices;

    std::default_random_engine random;

    unsigned int thread_count;
    std::vector<std::thread> threads;
    std::vector<bool> finished;

    sf::Time render_time = sf::Time::Zero;

    sf::RenderTexture render_texture;
    sf::RenderTexture present_texture;
    sf::Shader shader;
    sf::Sprite sprite;

    float squaredDistance(const sf::Vector2f a, const sf::Vector2f b);

    void setupRandom();
    void setupPoints();
    void setupMapCPU();
    void processVoronoiCPU(unsigned int c_thread);
    void setupMapGPU();
};