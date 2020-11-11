#include <chrono>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>

typedef void (*Job) ();

typedef void (*Error) (const std::exception &);

class Scheduler
{
public:
  Scheduler(size_t size, Error error);
  void schedule(const Job job, long time);

private:
  size_t mSize;
  Error mError;
  std::mutex mMutx;
};

Scheduler::Scheduler(size_t size, Error error)
: mSize(size)
, mError(error)
{}

void Scheduler::schedule(const Job job, long time)
{
  std::thread t(job);

  t.join();
}

int main(int argc, char **argv)
{
  Scheduler scheduler(10, [](const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  });

  scheduler.schedule([]() {
    std::cout << "Hello" << std::endl;
  }, 1000000);

  return 0;
}
