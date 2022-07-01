#include "Lib.h"
long freq = 500;

std::ofstream fout;
std::mutex mtx;
std::condition_variable cond;
unsigned counter=0;
unsigned char writePosition=0;
long writeIndex=0;
std::queue<elementOfQueue> queueOfdData;

unsigned* bufer = new unsigned[256*1024*1024/sizeof(unsigned)];

elementOfQueue::elementOfQueue(unsigned * ptr, long s)
{
    this->position=ptr;
    this->size=s;
}
unsigned * elementOfQueue::GetPosition(){return this->position;}
long elementOfQueue::GetSize(){return this->size;}

void GenerateData()
{
    while (1)
    {

        for(long i=0; i<1024*1024/sizeof(unsigned); i++)
        {
            *(bufer+writeIndex)=counter++;
            writeIndex++;
        }

        elementOfQueue oneMb(bufer+writePosition, 1024*1024/sizeof(unsigned));
        std::unique_lock<std::mutex> lock(mtx);
        if(queueOfdData.size()>=256)
        {
            std::cout <<"Queue is overfull" << std::endl;
            break;
        }
        queueOfdData.push(oneMb);
        cond.notify_all();
        lock.unlock();
        writeIndex=1024*1024/sizeof(unsigned)*(++writePosition);
        std::this_thread::sleep_for(std::chrono::microseconds (100000/freq));
    }

}

void PrintData()
{
    fout.open("Data.txt");
    fout.close();

    while (1)
    {
        double start = clock();
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [] {
            return !queueOfdData.empty();
        });


        fout.open("Data.txt", std::ofstream::app);
        if (fout.is_open())
        {
            for(long i=0; i<queueOfdData.front().GetSize(); i++)
                fout<< *(queueOfdData.front().GetPosition()+i);
            queueOfdData.pop();

            fout.close();
        }
        lock.unlock();
        std::cout << 1/((clock() - start) / CLOCKS_PER_SEC) << " MB/S"<< std::endl;
    }
}