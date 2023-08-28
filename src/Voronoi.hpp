#pragma once

#include <SFML/Graphics.hpp>

#include <random>
#include <thread>
#include <vector>

class Voronoi
{
public:
    Voronoi(sf::RenderWindow& win);
    ~Voronoi();

    void update();
    void render();
    
private:
    sf::RenderWindow& win;

    struct Point
    {
        sf::Vector2u position;
        sf::Color color;
    };

    const unsigned int point_count = 250;
    std::vector<Point> points;
    sf::VertexArray point_vertices;

    sf::VertexArray map_vertices;

    std::default_random_engine random;

    unsigned int thread_count;
    std::vector<std::thread> threads;

    float squaredDistance(const sf::Vector2f a, const sf::Vector2f b);

    void setupRandom();
    void setupPoints();
    void setupMap();
    void processVoronoi(unsigned int c_thread);
};