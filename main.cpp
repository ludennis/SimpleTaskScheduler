#include <chrono>
#include <condition_variable>
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
  void wait();

private:
  size_t mSize;
  size_t mCount;
  Error mError;
  std::mutex mMutex;
  std::condition_variable mCondition;
};

Scheduler::Scheduler(size_t size, Error error)
: mSize(size)
, mError(error)
, mCount(0)
{}

void Scheduler::schedule(const Job job, long time)
{
  std::unique_lock<std::mutex> lock(this->mMutex);
  mCondition.wait(lock, [this] { return this->mCount < this->mSize; });
  mCount++;

  auto j = std::make_shared<Job>(job);

  std::thread t(
    [time, j, this] {
      std::this_thread::sleep_for(std::chrono::microseconds(time));
      try {
        (*j)();
      } catch (const std::exception &e) {
        this->mError(e);
      } catch (...) {
        this->mError(std::runtime_error("Unknown error"));
      }

      mCondition.notify_one();
      mCount--;
    }
  );

  t.detach();
}

void Scheduler::wait()
{
  std::unique_lock<std::mutex> lock(this->mMutex);
  mCondition.wait(lock, [this] { return this->mCount == 0; });
}

int main(int argc, char **argv)
{
  Scheduler scheduler(10, [](const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  });

  scheduler.schedule([] {
    std::cout << "Hello" << std::endl;
  }, 5000000);

  scheduler.schedule([] {
    std::cout << "Yo!" << std::endl;
  }, 1000000);

  scheduler.wait();

  return 0;
}
