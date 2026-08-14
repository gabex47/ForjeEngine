// kidrian was here

#include "Core/Application.h"

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        const char* executablePath = argc > 0 ? argv[0] : nullptr;
        Forje::Core::Application application(executablePath);
        application.Run();
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ForjeEngine failed: " << exception.what() << '\n';
        return 1;
    }
}
