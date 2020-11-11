#include <chrono>
#include <iostream>
#include <thread>

typedef void (*Job) ();

class Scheduler
{
private:
public:
  Scheduler(const Job job, long time);
};

/* time = microseconds */
Scheduler::Scheduler(const Job job, long time)
{
  // start job with a thread
  std::this_thread::sleep_for(std::chrono::microseconds(time));

  // execute job
  std::thread t(job);

  t.join();
}

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
