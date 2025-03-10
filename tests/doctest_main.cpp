#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

// Main function for the tests.
int main(int argc, char** argv) {
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    return context.run();
}
