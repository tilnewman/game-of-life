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

    void Coordinator::teardown() {}

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
        }

        m_grid.handleEvent(t_event);
    }

    void Coordinator::update(const float t_elapsedTimeSec)
    {
        // lazy and greedy take their turns at regular intervals
        m_elapsedTimeSec += t_elapsedTimeSec;
        if (m_elapsedTimeSec > m_stepDelaySec)
        {
            m_elapsedTimeSec = 0.0f;
            // TODO
        }
    }

    void Coordinator::draw()
    {
        m_renderWindow.clear(sf::Color::Black);
        m_grid.draw(m_config, m_renderWindow, m_renderStates);
        m_renderWindow.display();
    }

} // namespace gameoflife
