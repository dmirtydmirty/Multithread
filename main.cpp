#include "Lib.h"

int main() {

   std::thread th1(GenerateData);
    std::thread th2(PrintData);
    th1.detach();
    th2.join();
    return 0;
}
