#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <cstdio>
#include <bitset>





class elementOfQueue
{
    unsigned *position;
    long size;
public:
    elementOfQueue(unsigned * ptr, long s);
    unsigned * GetPosition();
    long GetSize();
};

void GenerateData(int freq);
void PrintData();