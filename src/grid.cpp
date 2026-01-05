//
// grid.cpp
//
#include "grid.hpp"

#include "sfml-util.hpp"

#include <cmath>

namespace gameoflife
{

    Grid::Grid()
        : m_cellSize{}
        , m_gridRegion{}
        , m_rectangles{}
        , m_grid{}
    {}

    void Grid::setup(const Config & t_config)
    {
        const sf::Vector2f screenSize{ t_config.video_mode.size };
        const sf::Vector2f padSize{ t_config.screen_edge_pad_ratio * screenSize };
        const sf::FloatRect gridRegionRaw{ padSize, { screenSize - (padSize * 2.0f) } };

        const sf::Vector2f cellSizeRaw{ gridRegionRaw.size / sf::Vector2f{ t_config.cell_counts } };

        const float cellDimm{ std::floor(std::min(cellSizeRaw.x, cellSizeRaw.y)) };
        m_cellSize.x = cellDimm;
        m_cellSize.y = cellDimm;

        m_gridRegion.size     = { m_cellSize * sf::Vector2f{ t_config.cell_counts } };
        m_gridRegion.position = { (screenSize * 0.5f) - (m_gridRegion.size * 0.5f) };

        sf::RectangleShape rectangle;
        rectangle.setFillColor(t_config.grid_color_background);
        rectangle.setOutlineColor(t_config.grid_color_outline);
        rectangle.setOutlineThickness(t_config.grid_line_thickness);
        rectangle.setSize(m_cellSize);

        for (int y{ 0 }; y < static_cast<int>(t_config.cell_counts.y); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(t_config.cell_counts.x); ++x)
            {
                rectangle.setPosition(cellPositionToScreenPosition({ x, y }));
                m_rectangles.push_back(rectangle);
            }
        }
    }

    void Grid::draw(
        const Config & ,
        sf::RenderTarget & t_target,
        const sf::RenderStates & t_states) const
    {
        for (const sf::RectangleShape & rectangle : m_rectangles)
        {
            t_target.draw(rectangle, t_states);
        }
    }

    const sf::Vector2f Grid::cellPositionToScreenPosition(const sf::Vector2i & t_position) const
    {
        return (m_gridRegion.position + (sf::Vector2f{ t_position } * m_cellSize));
    }

} // namespace gameoflife
