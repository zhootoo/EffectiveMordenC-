#include <iostream>
#include <string>
#include "DeducingTypes/DeducingTypes.h"
int main() {
    int x = 42;
    const int* const cpc = &x;
    printTypeP(cpc);
    return 0;
}
