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
        reset(t_config);

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
        rectangle.setFillColor(t_config.grid_color_off);
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

    const GridPos_t Grid::screenPositionToGridPosition(const sf::Vector2f & t_position) const
    {
        sf::FloatRect screenRegion;
        screenRegion.size = m_cellSize;

        for (int y{ 0 }; y < static_cast<int>(m_grid.size()); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(m_grid.front().size()); ++x)
            {
                screenRegion.position = gridPositionToScreenPosition({ x, y });
                if (screenRegion.contains(t_position))
                {
                    return { x, y };
                }
            }
        }

        return { -1, -1 };
    }

    bool Grid::isGridPositionValid(const Grid_t & t_grid, const GridPos_t & t_position)
    {
        return (
            (t_position.x >= 0) && (t_position.y >= 0) &&
            (t_position.y < static_cast<int>(t_grid.size())) &&
            (t_position.x < static_cast<int>(t_grid.front().size())));
    }

    CellType_t Grid::getCellValue(const GridPos_t & t_position) const
    {
        if (isGridPositionValid(m_grid, t_position))
        {
            return m_grid.at(static_cast<std::size_t>(t_position.y))
                .at(static_cast<std::size_t>(t_position.x));
        }
        else
        {
            return 0;
        }
    }

    void Grid::setCellValue(const GridPos_t & t_position, const CellType_t t_value)
    {
        if (!isGridPositionValid(m_grid, t_position))
        {
            return;
        }

        m_grid.at(static_cast<std::size_t>(t_position.y))
            .at(static_cast<std::size_t>(t_position.x)) = t_value;
    }

    /*
        Any live cell with fewer than two live neighbours dies, as if by underpopulation.
        Any live cell with two or three live neighbours lives on to the next generation.
        Any live cell with more than three live neighbours dies, as if by overpopulation.
        Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    */
    void Grid::processStep()
    {
        Grid_t gridCopy = m_grid;

        for (int y{ 0 }; y < static_cast<int>(m_grid.size()); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(m_grid.front().size()); ++x)
            {
                const std::size_t surroundingAliveCells{ getAliveCountAroundGridPosition(
                    gridCopy, { x, y }) };

                if (getCellValue({ x, y }) == 0)
                {
                    if (surroundingAliveCells == 3)
                    {
                        setCellValue({ x, y }, 1);
                    }
                }
                else
                {
                    if ((surroundingAliveCells < 2) || (surroundingAliveCells > 3))
                    {
                        setCellValue({ x, y }, 0);
                    }
                }
            }
        }
    }

    void Grid::reset(const Config & t_config)
    {
        m_grid.clear();

        m_grid.resize(
            t_config.cell_counts.y, std::vector<unsigned char>(t_config.cell_counts.x, 0));
    }

    std::size_t
        Grid::getAliveCountAroundGridPosition(const Grid_t & t_grid, const GridPos_t & t_position)
    {
        std::size_t count{ 0 };

        for (int y{ t_position.y - 1 }; y <= (t_position.y + 1); ++y)
        {
            for (int x{ t_position.x - 1 }; x <= (t_position.x + 1); ++x)
            {
                if (!isGridPositionValid(t_grid, { x, y }))
                {
                    continue;
                }

                if ((t_position.x == x) && (t_position.y == y))
                {
                    continue;
                }

                if (t_grid.at(static_cast<std::size_t>(y)).at(static_cast<std::size_t>(x)) != 0)
                {
                    ++count;
                }
            }
        }

        return count;
    }

} // namespace gameoflife
