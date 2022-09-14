#include "../include/application.hpp"
#include <memory>

int main()
{
    auto app = std::make_unique<Application>("Cube Control Software");
    app->Run();

    return 0;
}
