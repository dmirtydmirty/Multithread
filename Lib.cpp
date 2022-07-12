#include "Lib.h"
long freq = 50;

std::ofstream fout;
std::mutex mtx;
std::condition_variable cond;
unsigned counter=0;
int writePosition=0;
long writeIndex=0;
std::queue<elementOfQueue> queueOfdData;

unsigned* buffer = new unsigned[256*1024*1024/sizeof(unsigned)];

elementOfQueue::elementOfQueue(unsigned * ptr, long s)
{
    this->position=ptr;
    this->size=s;
}
unsigned * elementOfQueue::GetPosition(){return this->position;}
long elementOfQueue::GetSize(){return this->size;}

void GenerateData()
{
    for (int j=0; j<32000 ; j++)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for(long i=0; i<1024*1024/sizeof(unsigned); i++)
        {
            *(buffer+writeIndex)=counter++;
            writeIndex++;
        }
        writeIndex=writeIndex % (256*1024*1024/sizeof(unsigned));

        elementOfQueue oneMb(buffer+writePosition*1024*1024/sizeof(unsigned), 1024*1024/sizeof(unsigned));
        std::unique_lock<std::mutex> lock(mtx);
        if(queueOfdData.size()>=256)
        {
            std::cout <<"Queue is overfull" << std::endl;
            break;
        }
        queueOfdData.push(oneMb);
        cond.notify_all();
        lock.unlock();
        writePosition++;
        writePosition = writePosition % (1024*1024/sizeof(unsigned)*256);
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::this_thread::sleep_for(std::chrono::microseconds (100000/freq - duration));
    }

}

void PrintData()
{
    fout.open("Data.txt");
    fout.close();

    while (1)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [] {
            return !queueOfdData.empty();
        });


        fout.open("Data.txt", std::ios::binary | std::ios::app);
        if (fout.is_open())
        {
            for(long i=0; i<queueOfdData.front().GetSize(); i++)
                fout<< std::bitset<32>(*(queueOfdData.front().GetPosition()+i));
            queueOfdData.pop();

            fout.close();
        }
        lock.unlock();
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        float duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cout << 1000000/duration << " MB/S" << std::endl;

    }
}