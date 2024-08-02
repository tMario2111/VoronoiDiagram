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
    enum DistanceType
    {
        Euclidean,
        Manhattan,
        Chebyshev,
    };

    Voronoi(sf::RenderWindow& win, bool use_gpu, DistanceType distance_type);
    ~Voronoi();

    void update(sf::Time dt);
    void render();
    
private:
    sf::RenderWindow& win;
    const bool use_gpu;

    const DistanceType distance_type;

    bool print_points = true;

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

    sf::Clock render_clock;
    sf::Time render_time;
    bool time_set = false;

    sf::RenderTexture render_texture;
    sf::RenderTexture present_texture;
    sf::Shader shader;
    sf::Sprite sprite;

    float squaredDistance(const sf::Vector2f a, const sf::Vector2f b);
    float manhattanDistance(const sf::Vector2f a, const sf::Vector2f b);
    float chebyshevDistance(const sf::Vector2f a, const sf::Vector2f b);

    void setupRandom();
    void setupPoints();
    void setupMapCPU();
    void processVoronoiCPU(unsigned int c_thread);
    void setupMapGPU();
};