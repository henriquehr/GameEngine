
#include "engine.h"

#include <iostream>

int main(int argc, char *args[]) {

    try {
        Engine e{};
        e.run();
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}