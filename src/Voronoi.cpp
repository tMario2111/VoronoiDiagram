#include "Voronoi.hpp"

Voronoi::Voronoi(sf::RenderWindow& win, bool use_gpu) :
    win{ win },
    use_gpu{ use_gpu }
{
    thread_count = std::thread::hardware_concurrency();
    threads.resize(thread_count);
    finished.resize(thread_count, false);

    this->setupRandom();
    this->setupPoints();

    if (!this->use_gpu)
        this->setupMapCPU();
    else
        this->setupMapGPU();
}

Voronoi::~Voronoi()
{
    if (!this->use_gpu)
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
    for (unsigned int i = 0; i < POINT_COUNT; i++)
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

void Voronoi::setupMapCPU()
{
    this->map_vertices.setPrimitiveType(sf::PrimitiveType::Points);
    this->map_vertices.resize(static_cast<std::vector<unsigned int>::size_type>
        (this->win.getSize().x) * this->win.getSize().y);

    for (unsigned int i = 0; i < this->thread_count; i++)
        this->threads[i] = std::thread{ &Voronoi::processVoronoiCPU, this, i };

    // for (auto& thread : threads)
    //    thread.join();
}

void Voronoi::processVoronoiCPU(unsigned int c_thread)
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

    this->finished[c_thread] = true;
}

void Voronoi::setupMapGPU()
{
    this->render_texture.create(this->win.getSize().x, this->win.getSize().y);

    if (!this->shader.loadFromFile("src/shader.glsl", sf::Shader::Fragment))
        exit(EXIT_FAILURE);
    
    sf::Vector2f point_position[POINT_COUNT];
    sf::Vector3f point_color[POINT_COUNT];
    for (unsigned int i = 0; i < POINT_COUNT; i++)
    {
        point_position[i] = sf::Vector2f{ this->points[i].position };

        point_color[i].x = static_cast<float>(this->points[i].color.r) / 255.f;
        point_color[i].y = static_cast<float>(this->points[i].color.g) / 255.f;
        point_color[i].z = static_cast<float>(this->points[i].color.b) / 255.f;
    }

    this->shader.setUniformArray("u_point_position", point_position, POINT_COUNT);
    this->shader.setUniformArray("u_point_color", point_color, POINT_COUNT);

    this->sprite.setTexture(this->render_texture.getTexture());
    this->present_texture.create(this->win.getSize().x, this->win.getSize().y);
    this->present_texture.draw(this->sprite, &this->shader);
    this->present_texture.display();
}

void Voronoi::update(sf::Time dt) 
{
    if (!this->use_gpu)
    {
        ImGui::Text("%s", std::string{ "Running on CPU" }.c_str());
        bool ok = true;
        for (const auto value : this->finished)
        {
            if (!value)
            {
                ok = false;
                break;
            }
        }
        if (ok)
            ImGui::Text("%s", 
                std::string{ "Rendering took: " + std::to_string(this->render_time.asSeconds()) + " seconds" }.c_str());
        else
            this->render_time += dt;
    }
    else
    {
        ImGui::Text("%s", std::string{ "Running on GPU" }.c_str());
        if (this->render_time == sf::Time::Zero)
            this->render_time = dt;
                    ImGui::Text("%s", 
                std::string{ "Rendering took: " + std::to_string(this->render_time.asSeconds()) + " seconds" }.c_str());

    }
}

void Voronoi::render()
{
    if (!this->use_gpu)
        this->win.draw(this->map_vertices);
    else
    {
        sf::Sprite sprite{};
        sprite.setTexture(present_texture.getTexture());
        sprite.setScale(sf::Vector2f{ 1.f, -1.f });
        sprite.setPosition(sf::Vector2f{ 0.f, 
             static_cast<float>(this->win.getSize().y) });
        this->win.draw(sprite);
    }
    this->win.draw(this->point_vertices);
}