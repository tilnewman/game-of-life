#include <cstdlib>
#include <exception>
#include <iostream>

#include "coordinator.hpp"

int main(void)
{
    try
    {
        using namespace gameoflife;

        Config config;
        Coordinator coordinator;
        coordinator.run(config);
    }
    catch (const std::exception & ex)
    {
        std::cout << "Exception Error: " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Exception Error: (non-standard exception)" << std::endl;
    }

    return EXIT_SUCCESS;
}