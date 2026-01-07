#ifndef COORDINATOR_HPP_INCLUDED
#define COORDINATOR_HPP_INCLUDED
//
// coordinator.hpp
//
#include "config.hpp"
#include "grid.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <memory>

namespace gameoflife
{

    class Coordinator
    {
      public:
        Coordinator();
        void run(const Config & t_config);

      private:
        void setup(const Config & t_config);
        void loop();
        void teardown();

        void setupRenderWindow(sf::VideoMode & t_videoMode);

        void handleEvents();
        void handleEvent(const sf::Event & t_event);
        void update(const float t_elapsedTimeSec);
        void draw();
        void reset();

      private:
        Config m_config;
        sf::RenderStates m_renderStates;
        sf::RenderWindow m_renderWindow;
        Grid m_grid;
        bool m_isRunning;
        float m_elapsedTimeSec;
        float m_stepDelaySec;
        bool m_isPaused;
        std::size_t m_stepCounter;
    };

} // namespace gameoflife

#endif // COORDINATOR_HPP_INCLUDED
