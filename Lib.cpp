#include "Lib.h"
//long freq = 50;


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

void GenerateData(int freq)
{
    long mbCounter=0;
    const long limit=1;
    unsigned counter=0;
    int writePosition=0;
    long writeIndex=0;
    unsigned* buffer = new unsigned[256*1024*1024/sizeof(unsigned)];
    while (mbCounter<=limit)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for(long i=0; i<1024*1024/sizeof(unsigned); i++)
        {
            *(buffer+writeIndex)=counter++;
            writeIndex++;
        }


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
        writePosition = ++writePosition % 256;
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

        std::this_thread::sleep_for(std::chrono::microseconds (1000000 /freq - duration));
        mbCounter++;
    }
    delete [] buffer;

}

void PrintData()
{
    std::ofstream fout;
    fout.open("Data.txt");
    fout.close();

    while (1)
    {
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [] {
            return !queueOfdData.empty();
        });


        fout.open("Data.txt",  std::ios::app);
        if (fout.is_open())
        {
            for(long i=0; i<queueOfdData.front().GetSize(); i++)
            fout.write((char*)(queueOfdData.front().GetPosition()+i), sizeof(unsigned));
            queueOfdData.pop();
            fout.close();
        }
        lock.unlock();
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        float duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        std::cout << 1000000/duration << " MB/S" << std::endl;

    }
}