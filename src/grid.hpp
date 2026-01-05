#ifndef GRID_HPP_INCLUDED
#define GRID_HPP_INCLUDED
//
// grid.hpp
//
#include "config.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <vector>

namespace gameoflife
{

    class Grid
    {
      public:
        Grid();

        void setup(const Config & t_config);
        
        void draw(
            const Config & t_config,
            sf::RenderTarget & t_target,
            const sf::RenderStates & t_states) const;

      private:
        const sf::Vector2f cellPositionToScreenPosition(const sf::Vector2i & t_position) const;

      private:
        sf::Vector2f m_cellSize;
        sf::FloatRect m_gridRegion;
        std::vector<sf::RectangleShape> m_rectangles;
        std::vector<std::vector<unsigned char>> m_grid;
    };

} // namespace gameoflife

#endif // GRID_HPP_INCLUDED
