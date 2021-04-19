#include <stdarg.h>

#include <chrono>
#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>

typedef void (*Job) ();

typedef void (*Error) (const std::exception &);

/* a variadic template tuple struct */
template <class... Ts>
struct tuple {};

template <class T, class... Ts>
struct tuple<T, Ts...> : tuple<Ts...>
{
  tuple(T t, Ts... ts)
  : tuple<Ts...>(ts...)
  , tail(t)
  {}

  T tail;
};

/* template to hold an element within a variadic template tuple */
template <size_t, class>
struct elem_type_holder;

template <class T, class... Ts>
struct elem_type_holder<0, tuple<T, Ts...>>
{
  typedef T type;
};

template <size_t k, class T, class... Ts>
struct elem_type_holder<k, tuple<T, Ts...>>
{
  typedef typename elem_type_holder<k - 1, tuple<Ts...>>::type type;
};

/* get function for the variadic template tuple */
template <size_t k, class... Ts>
typename std::enable_if<
    k == 0, typename elem_type_holder<0, tuple<Ts...>>::type&>::type
get(tuple<Ts...>& t)
{
  return t.tail;
}

template <size_t k, class T, class... Ts>
typename std::enable_if<
    k != 0, typename elem_type_holder<k, tuple<T, Ts...>>::type&>::type
get(tuple<T, Ts...>& t)
{
  tuple<Ts...>& base = t;
  return get<k - 1>(base);
}

/* a variadic template function to print all arguments */
template <typename T>
void printAll(T t)
{
  std::cout << t << std::endl;
}

template <typename T, typename... Types>
void printAll(T arg, Types... args)
{
  std::cout << arg << std::endl;
  printAll(args...);
}

/* simple task scheduler */
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


//  scheduler.schedule([] {
//    std::cout << "Hello" << std::endl;
//  }, 5000000);
//
//  scheduler.schedule([] {
//    std::cout << "Yo!" << std::endl;
//  }, 1000000);
//
//  scheduler.wait();

  tuple<char, int, double> tup('g', 1, 1.3);

  std::cout << "0th element is " << get<0>(tup) << std::endl;
  std::cout << "1st element is " << get<1>(tup) << std::endl;
  std::cout << "2nd element is " << get<2>(tup) << std::endl;

  return 0;
}
