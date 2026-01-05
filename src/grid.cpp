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
        , m_backgroundRectangles{}
        , m_grid{}
    {}

    void Grid::setup(const Config & t_config)
    {
        m_grid.resize(
            t_config.cell_counts.y, std::vector<unsigned char>(t_config.cell_counts.x, 0));

        const sf::Vector2f screenSize{ t_config.video_mode.size };
        const sf::Vector2f padSize{ t_config.screen_edge_pad_ratio * screenSize };
        const sf::FloatRect gridRegionRaw{ padSize, { screenSize - (padSize * 2.0f) } };

        const sf::Vector2f cellSizeRaw{ gridRegionRaw.size / sf::Vector2f{ t_config.cell_counts } };

        const float cellDimm{ std::floor(std::min(cellSizeRaw.x, cellSizeRaw.y)) };
        m_cellSize.x = cellDimm;
        m_cellSize.y = cellDimm;

        m_gridRegion.size       = { m_cellSize * sf::Vector2f{ t_config.cell_counts } };
        m_gridRegion.position   = { (screenSize * 0.5f) - (m_gridRegion.size * 0.5f) };
        m_gridRegion.position.x = std::floor(m_gridRegion.position.x);
        m_gridRegion.position.y = std::floor(m_gridRegion.position.y);

        sf::RectangleShape rectangle;
        rectangle.setFillColor(t_config.grid_color_background);
        rectangle.setOutlineColor(t_config.grid_color_outline);
        rectangle.setOutlineThickness(t_config.grid_line_thickness);
        rectangle.setSize(m_cellSize);

        for (int y{ 0 }; y < static_cast<int>(t_config.cell_counts.y); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(t_config.cell_counts.x); ++x)
            {
                rectangle.setPosition(gridPositionToScreenPosition({ x, y }));
                m_backgroundRectangles.push_back(rectangle);
            }
        }
    }

    void Grid::draw(
        const Config & t_config,
        sf::RenderTarget & t_target,
        const sf::RenderStates & t_states) const
    {
        for (const sf::RectangleShape & rectangle : m_backgroundRectangles)
        {
            t_target.draw(rectangle, t_states);
        }

        sf::RectangleShape rectangle;
        rectangle.setFillColor(t_config.grid_color_on);
        rectangle.setOutlineColor(t_config.grid_color_outline);
        rectangle.setOutlineThickness(t_config.grid_line_thickness);
        rectangle.setSize(m_cellSize);

        for (int y{ 0 }; y < static_cast<int>(t_config.cell_counts.y); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(t_config.cell_counts.x); ++x)
            {
                if (getCellValue({ x, y }) != 0)
                {
                    rectangle.setPosition(gridPositionToScreenPosition({ x, y }));
                    t_target.draw(rectangle, t_states);
                }
            }
        }
    }

    const sf::Vector2f Grid::gridPositionToScreenPosition(const GridPos_t & t_position) const
    {
        return (m_gridRegion.position + (sf::Vector2f{ t_position } * m_cellSize));
    }

    bool Grid::isGridPositionValid(const GridPos_t & t_position) const
    {
        return (
            (t_position.x >= 0) && (t_position.y >= 0) && (t_position.y < m_grid.size()) &&
            (t_position.x < m_grid.front().size()));
    }

    CellType_t Grid::getCellValue(const GridPos_t & t_position) const
    {
        if (isGridPositionValid(t_position))
        {
            return m_grid.at(t_position.y).at(t_position.x);
        }
        else
        {
            return 0;
        }
    }

    void Grid::setCellValue(const GridPos_t & t_position, const CellType_t t_value)
    {
        if (!isGridPositionValid(t_position))
        {
            return;
        }

        m_grid.at(t_position.y).at(t_position.x) = t_value;
    }

} // namespace gameoflife
