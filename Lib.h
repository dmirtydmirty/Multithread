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



const int bufferSizeMb =256;
const long oneMbSize=1000*1000/sizeof(unsigned);
const long limit=100;

class elementOfQueue
{
    unsigned *position;
    long size;
public:
    elementOfQueue(unsigned * ptr, long s);
    unsigned * GetPosition();
    long GetSize();
};

void GenerateData(int freq, unsigned* buffer);
void PrintData();