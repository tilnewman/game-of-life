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
        , m_grid{}
        , m_lineVerts{}
        , m_backgroundRectangle{}
    {}

    void Grid::setup(const Config & t_config)
    {
        // setup/size the grid vectors
        reset(t_config);

        // establish cell size
        const sf::Vector2f screenSize{ t_config.video_mode.size };
        const sf::Vector2f padSize{ t_config.screen_edge_pad_ratio * screenSize };
        const sf::FloatRect gridRegionRaw{ padSize, { screenSize - (padSize * 2.0f) } };

        const sf::Vector2f cellSizeRaw{ gridRegionRaw.size / sf::Vector2f{ t_config.cell_counts } };

        const float cellDimm{ std::floor(std::min(cellSizeRaw.x, cellSizeRaw.y)) };
        m_cellSize.x = cellDimm;
        m_cellSize.y = cellDimm;

        // establish where on screen the grid is (center it)
        m_gridRegion.size       = { m_cellSize * sf::Vector2f{ t_config.cell_counts } };
        m_gridRegion.position   = { (screenSize * 0.5f) - (m_gridRegion.size * 0.5f) };
        m_gridRegion.position.x = std::floor(m_gridRegion.position.x);
        m_gridRegion.position.y = std::floor(m_gridRegion.position.y);

        // setup the grid background color
        m_backgroundRectangle.setFillColor(t_config.grid_color_off);
        m_backgroundRectangle.setSize(m_gridRegion.size);
        m_backgroundRectangle.setPosition(m_gridRegion.position);

        // setup the grid cell lines
        for (int x{ 0 }; x <= static_cast<int>(t_config.cell_counts.x); ++x)
        {
            const float horizPos{ m_gridRegion.position.x +
                                  (static_cast<float>(x) * m_cellSize.x) };

            m_lineVerts.emplace_back(
                sf::Vector2f{ horizPos, m_gridRegion.position.y }, t_config.grid_color_outline);

            m_lineVerts.emplace_back(
                sf::Vector2f{ horizPos, util::bottom(m_gridRegion) }, t_config.grid_color_outline);
        }

        for (int y{ 0 }; y <= static_cast<int>(t_config.cell_counts.y); ++y)
        {
            const float vertPos{ m_gridRegion.position.y + (static_cast<float>(y) * m_cellSize.y) };

            m_lineVerts.emplace_back(
                sf::Vector2f{ m_gridRegion.position.x, vertPos }, t_config.grid_color_outline);

            m_lineVerts.emplace_back(
                sf::Vector2f{ util::right(m_gridRegion), vertPos }, t_config.grid_color_outline);
        }
    }

    void Grid::draw(
        const Config & t_config,
        sf::RenderTarget & t_target,
        const sf::RenderStates & t_states) const
    {
        t_target.draw(m_backgroundRectangle, t_states);
        t_target.draw(&m_lineVerts[0], m_lineVerts.size(), sf::PrimitiveType::Lines);

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

    bool Grid::isGridPositionValid(const GridPos_t & t_position) const
    {
        return (
            (t_position.x >= 0) && (t_position.y >= 0) &&
            (t_position.y < static_cast<int>(m_grid.size())) &&
            (t_position.x < static_cast<int>(m_grid.front().size())));
    }

    CellType_t Grid::getCellValue(const GridPos_t & t_position) const
    {
        if (isGridPositionValid(t_position))
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
        if (!isGridPositionValid(t_position))
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
        std::vector<GridPos_t> positionsToFlip;
        positionsToFlip.reserve(m_grid.size() * m_grid.front().size());

        for (int y{ 0 }; y < static_cast<int>(m_grid.size()); ++y)
        {
            for (int x{ 0 }; x < static_cast<int>(m_grid.front().size()); ++x)
            {
                const std::size_t surroundingAliveCells{ getAliveCountAroundGridPosition(
                    { x, y }) };

                if (getCellValue({ x, y }) == 0)
                {
                    if (surroundingAliveCells == 3)
                    {
                        positionsToFlip.emplace_back(x, y);
                    }
                }
                else
                {
                    if ((surroundingAliveCells < 2) || (surroundingAliveCells > 3))
                    {
                        positionsToFlip.emplace_back(x, y);
                    }
                }
            }
        }

        for (const GridPos_t & position : positionsToFlip)
        {
            if (getCellValue(position) == 0)
            {
                setCellValue(position, 1);
            }
            else
            {
                setCellValue(position, 0);
            }
        }
    }

    void Grid::reset(const Config & t_config)
    {
        m_grid.clear();

        m_grid.resize(
            t_config.cell_counts.y, std::vector<unsigned char>(t_config.cell_counts.x, 0));
    }

    std::size_t Grid::getAliveCountAroundGridPosition(const GridPos_t & t_position) const
    {
        std::size_t count{ 0 };

        for (int y{ t_position.y - 1 }; y <= (t_position.y + 1); ++y)
        {
            for (int x{ t_position.x - 1 }; x <= (t_position.x + 1); ++x)
            {
                if (!isGridPositionValid({ x, y }))
                {
                    continue;
                }

                if ((t_position.x == x) && (t_position.y == y))
                {
                    continue;
                }

                if (m_grid.at(static_cast<std::size_t>(y)).at(static_cast<std::size_t>(x)) != 0)
                {
                    ++count;
                }
            }
        }

        return count;
    }

} // namespace gameoflife
