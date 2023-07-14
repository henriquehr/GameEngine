
#include "engine.h"

int main(int argc, char *args[]) {

    engine e = {};
    e.init();
    e.run();
    e.cleanup();

    return 0;
}