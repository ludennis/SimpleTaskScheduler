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

private:
  size_t mSize;
  Error mError;
  std::mutex mMutx;
};

Scheduler::Scheduler(size_t size, Error error)
: mSize(size)
, mError(error)
{}

int main(int argc, char **argv)
{

  Scheduler(10, [](const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  });

  return 0;
}
