#include "core/application.hpp"
#include <memory>

int main()
{
    auto app = std::make_unique<Application>("Application");
    app->Run();

    return 0;
}
