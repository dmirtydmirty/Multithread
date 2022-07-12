#include "Lib.h"

int main() {

    int frequency=60; // 1/s
   std::thread th1(GenerateData, frequency);
    std::thread th2(PrintData);
    th1.join();
    th2.join();
    return 0;
}
