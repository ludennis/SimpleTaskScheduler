#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

typedef void (*Job) ();

class Scheduler
{
public:
  Scheduler(const Job job, long time);

private:
  std::mutex mMutx;
  Job mJob;
  long mTime;
};

/* time = microseconds */
Scheduler::Scheduler(const Job job, long time)
: mJob(job)
, mTime(time)
{}

void hello(void)
{
  std::cout << "Hello" << std::endl;
}

void printInt(int x)
{
  std::cout << x << std::endl;
}

int main(int argc, char **argv)
{
  Job job1(hello);
  Job job2(hello);

  Scheduler(job1, 5000000);
  Scheduler(job2, 1000000);

  return 0;
}
