//
// coordinator.cpp
//
#include "coordinator.hpp"

#include "sfml-util.hpp"

#include <SFML/System/Clock.hpp>

#include <iostream>

namespace gameoflife
{

    Coordinator::Coordinator()
        : m_config{}
        , m_renderStates{}
        , m_renderWindow{}
        , m_grid{}
        , m_isRunning{ true }
        , m_elapsedTimeSec{ 0.0f }
        , m_stepDelaySec{ 0.25f }
        , m_isPaused{ true }
        , m_stepCounter{ 0 }
    {}

    void Coordinator::run(const Config & t_config)
    {
        setup(t_config);
        loop();
        teardown();
    }

    void Coordinator::setup(const Config & t_config)
    {
        m_config = t_config;
        setupRenderWindow(m_config.video_mode);
        m_grid.setup(m_config);
    }

    void Coordinator::loop()
    {
        sf::Clock frameClock;
        while (m_renderWindow.isOpen() && m_isRunning)
        {
            handleEvents();
            update(frameClock.restart().asSeconds());
            draw();
        }
    }

    void Coordinator::teardown() { std::cout << "Step Count=" << m_stepCounter << '\n'; }

    void Coordinator::setupRenderWindow(sf::VideoMode & t_videoMode)
    {
        std::cout << "Attempting video mode " << t_videoMode << "...";

        if (!m_config.video_mode.isValid())
        {
            std::cout << "but that is not suported.  Valid video modes at "
                      << m_config.video_mode.bitsPerPixel << "bpp:\n"
                      << util::makeSupportedVideoModesString(m_config.video_mode.bitsPerPixel)
                      << '\n';

            t_videoMode = util::findHighestVideoMode(m_config.video_mode.bitsPerPixel);
            setupRenderWindow(t_videoMode);
            return;
        }

        m_renderWindow.create(t_videoMode, "CastleCrawl2", sf::State::Fullscreen);

        // sometimes the resolution of the window created does not match what was specified
        const unsigned actualWidth  = m_renderWindow.getSize().x;
        const unsigned actualHeight = m_renderWindow.getSize().y;
        if ((m_config.video_mode.size.x == actualWidth) &&
            (m_config.video_mode.size.y == actualHeight))
        {
            std::cout << "Success." << std::endl;
        }
        else
        {
            std::cout << "Failed" << ".  ";

            m_config.video_mode.size.x = actualWidth;
            m_config.video_mode.size.y = actualHeight;

            std::cout << "Using " << m_config.video_mode << " instead." << std::endl;
        }

        if (m_config.framerate_limit > 0)
        {
            m_renderWindow.setFramerateLimit(m_config.framerate_limit);
        }
    }

    void Coordinator::handleEvents()
    {
        while (const auto eventOpt = m_renderWindow.pollEvent())
        {
            handleEvent(eventOpt.value());
        }
    }

    void Coordinator::handleEvent(const sf::Event & t_event)
    {
        if (t_event.is<sf::Event::Closed>())
        {
            m_isRunning = false;
            std::cout << "Stopping because window was closed externally.\n";
        }
        else if (const auto * keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Escape)
            {
                m_isRunning = false;
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Up)
            {
                m_stepDelaySec *= 0.9f;
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Down)
            {
                m_stepDelaySec *= 1.1f;
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Space)
            {
                m_isPaused = !m_isPaused;
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Right)
            {
                m_grid.processStep();
                ++m_stepCounter;
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::R)
            {
                reset();
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num1)
            {
                // Glider
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, 1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, 2 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, 2 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 1 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num2)
            {
                // The R-pentomino
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, 1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, 2 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -1, 1 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num3)
            {
                // Diehard
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -5, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -4, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -4, 1 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, 1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, 1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, -1 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num4)
            {
                // Acorn
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, -2 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, -1 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, 0 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num5)
            {
                // Infinite Block 1
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -4, 0 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, -1 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, -2 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, -3 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, -4 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, -3 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, -4 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, -5 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, -4 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num6)
            {
                // Infinite Block 2
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -1, 0 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 0 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 1 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 0, 2 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 2 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -2, 3 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -1, 3 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 3 }, 1);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 4 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -1, 4 }, 1);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 4 }, 1);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num7)
            {
                // Penta-decathlon
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                for (int y{ -1 }; y < 2; ++y)
                {
                    for (int x{ -4 }; x < 4; ++x)
                    {
                        m_grid.setCellValue(centerPosition + sf::Vector2i{ x, y }, 1);
                    }
                }

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, 0 }, 0);
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Num8)
            {
                // ?
                reset();
                const sf::Vector2i centerPosition{ m_config.cell_counts / 2u };
                for (int x{ -19 }; x <= 19; ++x)
                {
                    m_grid.setCellValue(centerPosition + sf::Vector2i{ x, 0 }, 1);
                }

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -11, 0 }, 0);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ -5, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -4, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ -3, 0 }, 0);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 1, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 2, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 3, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 4, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 5, 0 }, 0);
                m_grid.setCellValue(centerPosition + sf::Vector2i{ 6, 0 }, 0);

                m_grid.setCellValue(centerPosition + sf::Vector2i{ 14, 0 }, 0);
            }
        }
        else if (const auto * mousePtr = t_event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (m_isPaused)
            {
                const GridPos_t gridPos{ m_grid.screenPositionToGridPosition(
                    sf::Vector2f{ mousePtr->position }) };

                if (m_grid.getCellValue(gridPos) == 0)
                {
                    m_grid.setCellValue(gridPos, 1);
                }
                else
                {
                    m_grid.setCellValue(gridPos, 0);
                }
            }
        }
    }

    void Coordinator::update(const float t_elapsedTimeSec)
    {
        if (m_isPaused)
        {
            return;
        }

        // lazy and greedy take their turns at regular intervals
        m_elapsedTimeSec += t_elapsedTimeSec;
        if (m_elapsedTimeSec > m_stepDelaySec)
        {
            m_elapsedTimeSec = 0.0f;
            m_grid.processStep();
            ++m_stepCounter;
        }
    }

    void Coordinator::draw()
    {
        m_renderWindow.clear(sf::Color::Black);
        m_grid.draw(m_config, m_renderWindow, m_renderStates);
        m_renderWindow.display();
    }

    void Coordinator::reset()
    {
        m_isPaused = true;
        m_grid.reset(m_config);
        m_stepCounter = 0;
    }

} // namespace gameoflife
