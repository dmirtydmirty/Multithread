#include "Lib.h"

int main() {

    unsigned * buffer = new unsigned[bufferSizeMb*oneMbSize];
    int frequency=60000; // 1/s
   std::thread th1(GenerateData, frequency, buffer);
    std::thread th2(PrintData);
    th1.join();
    th2.join();
    delete [] buffer;
    return 0;
}
