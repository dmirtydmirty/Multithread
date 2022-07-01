#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <cstdio>





class elementOfQueue
{
    unsigned *position;
    long size;
public:
    elementOfQueue(unsigned * ptr, long s);
    unsigned * GetPosition();
    long GetSize();
};

void GenerateData();
void PrintData();