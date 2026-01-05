#ifndef CONFIG_HPP_INCLUDED
#define CONFIG_HPP_INCLUDED
//
// config.hpp
//
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace gameoflife
{

    struct Config
    {
        sf::VideoMode video_mode{ { 4112u, 2580u }, 32u };
        unsigned framerate_limit{ 0 };
        float screen_edge_pad_ratio{ 0.035f };
        sf::Vector2u cell_counts{ 30u, 20u };
        float grid_line_thickness{ 4.0f };
        sf::Color grid_color_background{ 28, 28, 28 };
        sf::Color grid_color_outline{ 0, 0, 0 };
        sf::Color grid_color_on{ 250, 230, 110 };
    };

} // namespace gameoflife

#endif // CONFIG_HPP_INCLUDED
