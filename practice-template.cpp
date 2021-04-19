#include <cstddef>
#include <iostream>

// size is needed to initialize a static array
template <class T, size_t size>
class StaticArray
{
private:
  T mArray[size];

public:
  T* getArray();
  T& operator[](int index)
  {
    return mArray[index];
  }
};

template <class T>
class MyArray
{
private:
  T* mArray;

public:
  MyArray(size_t n)
  {
    mArray = new T[n];
  }

  T* getArray()
  {
    return mArray;
  }

  T& operator[](size_t index)
  {
    return mArray[index];
  }

  ~MyArray()
  {
    delete mArray;
  }
};

template<class ... Types>
struct Tuple
{};

template<class ... Types>
void f(Types ... args);

int main(int argc, char **argv)
{
  Tuple<int, int> t1;

  f(1);

  StaticArray<int, 3> sa;
  sa[0] = 1;
  sa[1] = 2;
  sa[2] = 3;

  std::cout << sa[1] << std::endl;

  MyArray<int> ma(5);
  ma[0] = 1;
  ma[1] = 2;
  ma[2] = 3;

  std::cout << ma[2] << std::endl;

  return 0;
}
