#include "Lib.h"



std::mutex mtx;
std::condition_variable cond;
std::queue<elementOfQueue> queueOfdData;



elementOfQueue::elementOfQueue(unsigned * ptr, long s)
{
    this->position=ptr;
    this->size=s;
}
unsigned * elementOfQueue::GetPosition(){return this->position;}
long elementOfQueue::GetSize(){return this->size;}

void GenerateData(int freq, unsigned* buffer)
{
    long mbCounter=0;

    unsigned counter=0;
    int writePosition=0;
    long writeIndex=0;

    while (mbCounter<limit)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for(long i=0; i<oneMbSize; i++)
        {
            *(buffer+writeIndex)=counter++;
            writeIndex++;
        }

        writeIndex=writeIndex % (bufferSizeMb*oneMbSize);

        elementOfQueue oneMb(buffer+writePosition*oneMbSize, oneMbSize);

        if(queueOfdData.size()>=bufferSizeMb)
        {
            std::cout <<"Queue is overfull" << std::endl;
            break;
        }
        queueOfdData.push(oneMb);
        cond.notify_all();
        writePosition = ++writePosition % bufferSizeMb;
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

        std::this_thread::sleep_for(std::chrono::microseconds (1000000 /freq - duration));
        mbCounter++;
    }
}

void PrintData()
{
    std::ofstream fout;
    fout.open("Data.bin");
    fout.close();
    long mbCounter=0;
    long mbCounter2;
    while (mbCounter<limit)
    {
        mbCounter2=mbCounter;
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [] {
            return !queueOfdData.empty();
        });
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        fout.open("Data.bin",  std::ios::app|std::ios::binary );
        if (fout.is_open())
        {
            while (!queueOfdData.empty())
            {
                fout.write((char *)(queueOfdData.front().GetPosition()), oneMbSize* sizeof(unsigned ));
                queueOfdData.pop();
                mbCounter++;
            }
            fout.close();
        }
        lock.unlock();
        long sizeWritenData=mbCounter-mbCounter2;
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        float duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cout << sizeWritenData*1000000/duration << " MB/S " << sizeWritenData << " MB was writen" << std::endl;
    }
}