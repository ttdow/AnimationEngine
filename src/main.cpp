#include "App.h"

#include <iostream>
#include <memory>

// Entry point.
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::unique_ptr<App> app = std::make_unique<App>();

    try
    {
        app->Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}