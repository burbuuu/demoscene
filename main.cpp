#include "demo.h"

int main(int argc, char* args[]) {
    Demo demo;

    if (!demo.init()) {
        return 1;
    }

    demo.run();

    return 0;
}
