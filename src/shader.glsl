#define POINT_COUNT 800

uniform vec2 u_point_position[POINT_COUNT];
uniform vec3 u_point_color[POINT_COUNT];

// 0 - Euclidean
// 1 - Manhattan
// 2 - Chebyshev
uniform int u_distance_type;

void squaredDistance(in vec2 a, in vec2 b, out float distance)
{
    distance = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

void manhattanDistance(in vec2 a, in vec2 b, out float distance)
{
    distance = abs(a.x - b.x) + abs(a.y - b.y);
}

void chebyshevDistance(in vec2 a, in vec2 b, out float distance)
{
    distance = max(abs(a.x - b.x), abs(a.y - b.y));
}

void main()
{
    vec2 position = gl_FragCoord.xy - vec2(0.5, 0.5);
    float min = 999999.0;
    float distance;
    vec3 min_color;

    for (int i = 0; i < POINT_COUNT; i++)
    {
        if (u_distance_type == 0)
            squaredDistance(position, u_point_position[i], distance);
        else if (u_distance_type == 1)
            manhattanDistance(position, u_point_position[i], distance);
        else if (u_distance_type == 2)
            manhattanDistance(position, u_point_position[i], distance);
        if (distance < min)
        {
            min = distance;
            min_color = u_point_color[i];
        }
    }
    gl_FragColor = vec4(min_color.x, min_color.y, min_color.z, 1.0);
}