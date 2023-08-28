#include "Voronoi.hpp"

Voronoi::Voronoi(sf::RenderWindow& win) :
    win{ win }
{
    thread_count = std::thread::hardware_concurrency();
    threads.resize(thread_count);

    this->setupRandom();
    this->setupPoints();
    this->setupMap();
}

Voronoi::~Voronoi()
{
    for (auto& thread : this->threads)
        thread.join();
}

float Voronoi::squaredDistance(const sf::Vector2f a, const sf::Vector2f b)
{
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

void Voronoi::setupRandom()
{
    std::random_device r{};
    this->random.seed(r());
}

void Voronoi::setupPoints()
{
    const auto max_x = static_cast<unsigned int>(this->win.getSize().x);
    const auto max_y = static_cast<unsigned int>(this->win.getSize().y);
    Point point{};
    for (unsigned int i = 0; i < this->point_count; i++)
    {
        point.position.x = std::uniform_int_distribution<unsigned int>(0, max_x)(this->random);
        point.position.y = std::uniform_int_distribution<unsigned int>(0, max_y)(this->random);

        point.color.r = static_cast<sf::Uint8>(std::uniform_int_distribution<unsigned int>(0, 255)(this->random));
        point.color.g = static_cast<sf::Uint8>(std::uniform_int_distribution<unsigned int>(0, 255)(this->random));
        point.color.b = static_cast<sf::Uint8>(std::uniform_int_distribution<unsigned int>(0, 255)(this->random));

        this->points.push_back(point);
    }

    this->point_vertices.setPrimitiveType(sf::PrimitiveType::Quads);
    sf::Vector2f point_as_float{};
    for (const auto point : this->points)
    {
        point_as_float.x = static_cast<float>(point.position.x);
        point_as_float.y = static_cast<float>(point.position.y);

        point_vertices.append(sf::Vertex{ point_as_float + sf::Vector2f{ -3.f, -3.f }, sf::Color::Red });
        point_vertices.append(sf::Vertex{ point_as_float + sf::Vector2f{ +3.f, -3.f }, sf::Color::Red });
        point_vertices.append(sf::Vertex{ point_as_float + sf::Vector2f{ +3.f, +3.f }, sf::Color::Red });
        point_vertices.append(sf::Vertex{ point_as_float + sf::Vector2f{ -3.f, +3.f }, sf::Color::Red });
    }
}

void Voronoi::setupMap()
{
    this->map_vertices.setPrimitiveType(sf::PrimitiveType::Points);
    this->map_vertices.resize(static_cast<std::vector<unsigned int>::size_type>
        (this->win.getSize().x) * this->win.getSize().y);

    for (unsigned int i = 0; i < this->thread_count; i++)
        this->threads[i] = std::thread{ &Voronoi::processVoronoi, this, i };

    // for (auto& thread : threads)
    //    thread.join();
}

void Voronoi::processVoronoi(unsigned int c_thread)
{
    auto column_length = this->win.getSize().x / this->thread_count;
    if (this->thread_count * column_length < this->win.getSize().x)
        column_length++;

    unsigned int k;
    float distance{};
    float min_distance{};
    sf::Color min_color{};
    unsigned int i, j;
    for (i = c_thread * column_length; i < (c_thread + 1) * column_length; i++)
    {
        for (j = 0; j < this->win.getSize().y; j++)
        {
            k = i * this->win.getSize().y + j;
            this->map_vertices[k].position = sf::Vector2f{ static_cast<float>(i), static_cast<float>(j) };
            
            min_distance = INFINITY;
            for (const auto& point : this->points)
            {
                distance = squaredDistance(this->map_vertices[k].position, static_cast<sf::Vector2f>(point.position));
                if (distance < min_distance)
                {
                    min_distance = distance;
                    min_color = point.color;
                }
            }

            this->map_vertices[k].color = min_color;
        }
    }
}

void Voronoi::update() 
{

}

void Voronoi::render()
{
    this->win.draw(map_vertices);
    this->win.draw(point_vertices);
}