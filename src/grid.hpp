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
#include <SFML/Window/Event.hpp>

#include <optional>
#include <vector>

namespace gameoflife
{

    using CellType_t = unsigned char;
    using GridPos_t  = sf::Vector2i;

    //

    class Grid
    {
      public:
        Grid();

        void setup(const Config & t_config);

        void draw(
            const Config & t_config,
            sf::RenderTarget & t_target,
            const sf::RenderStates & t_states) const;

        void handleEvent(const sf::Event & t_event);

      private:
        const sf::Vector2f gridPositionToScreenPosition(const GridPos_t & t_position) const;
        const GridPos_t screenPositionToGridPosition(const sf::Vector2f & t_position) const;

        CellType_t getCellValue(const GridPos_t & t_position) const;
        void setCellValue(const GridPos_t & t_position, const CellType_t t_value);
        bool isGridPositionValid(const GridPos_t & t_position) const;

      private:
        sf::Vector2f m_cellSize;
        sf::FloatRect m_gridRegion;
        std::vector<sf::RectangleShape> m_backgroundRectangles;
        std::vector<std::vector<CellType_t>> m_grid;
    };

} // namespace gameoflife

#endif // GRID_HPP_INCLUDED
