

#include <cstdint>
#include <iostream>
#include <vector>
#include <map>


#include "AttributeArray.hpp"

std::map<int, int> test;
std::map<int, int>::value_type vt;

bool mypredicate (int32_t i, int32_t j) {
  return (i==j);
}

struct Sum
{
    Sum(): sum{0} { }
    void operator()(int n) { sum += n; }
    int sum;
};

template <typename T>
void print(const T& v)
{
  std::cout << v << " ";
}

int main(int argc, char *argv[])
{
  std::cout << "AttributeArrayIterator Test Starting...." << std::endl;

  AttributeArray<int32_t> i32Array(10, "Test Array");
  AttributeArray<int32_t> i32Array1(10, "Other Array");

  // Initialize with a value
  for(auto& value : i32Array)
  {
    value = 5;
  }
  // Initialize with a value
  for(auto& value : i32Array1)
  {
    value = 5;
  }

  std::cout << "before:";
  std::for_each(i32Array.begin(), i32Array.end(), print<int32_t>);
  std::cout << '\n';

  std::cout << "using default comparison:" << std::endl;
  if ( std::equal (i32Array.begin(), i32Array.end(), i32Array1.begin()) )
  {std::cout << "The contents of both sequences are equal.\n";}
  else
  {std::cout << "The contents of both sequences differ.\n";}

  std::for_each(i32Array1.begin(), i32Array1.end(), [](int32_t &n){ n++; });

  std::cout << "using predicate comparison:" << std::endl;
  if ( std::equal (i32Array.begin(), i32Array.end(), i32Array1.begin(), mypredicate) )
  {std::cout << "The contents of both sequences are equal.\n";}
  else
  {std::cout << "The contents of both sequences differ.\n";}

  std::cout << "Using another predicate to sum the values" << std::endl;
  // calls Sum::operator() for each number
  Sum s = std::for_each(i32Array1.begin(), i32Array1.end(), Sum());

  std::cout << "after: ";
  std::for_each(i32Array1.begin(), i32Array1.end(), print<int32_t>);
  std::cout << '\n';
  std::cout << "sum: " << s.sum << '\n';


  std::cout << "Modify Array using [] operator..." << std::endl;
  for(AttributeArray<int32_t>::size_type i = 0; i < i32Array.size(); i++)
  {
    i32Array[i] = static_cast<int32_t>(i * 10);
  }

  std::cout << "Print using range based loop..." << std::endl;
  for(const auto& value : i32Array)
  {
    std::cout << value << std::endl;
  }

  std::cout << "std::fill" << std::endl;
  // std::fill the array with a set value
  std::fill(i32Array.begin(), i32Array.end(), -1);
  std::for_each(i32Array.begin(), i32Array.end(), print<int32_t>);
  std::cout << "" << std::endl;


  std::cout << "std::transform from int32_t to float using a back_inserter" << std::endl;
  AttributeArray<float> f32Array(0, "Float Array");
  std::transform(i32Array.begin(), i32Array.end(), std::back_inserter(f32Array),
                 [](int32_t i) -> float { return i * 2.5; });
  std::for_each(f32Array.begin(), f32Array.end(), print<float>);
  std::cout << std::endl;

  // Get the front ane back
  int32_t front = i32Array1.front();
  float back = f32Array.back();

  std::cout << "## Assign Content(1)" << std::endl;
  std::vector<int> foo(60, 33);
  i32Array.assign(foo.begin(), foo.end());
  std::cout << "i32Array.size() after assign(1): " << i32Array.size() << std::endl;

  i32Array.assign(static_cast<size_t>(22), 333);
  std::cout << "i32Array.size() after assign(2): " << i32Array.size() << std::endl;

  i32Array.assign(std::initializer_list<int32_t>{3,4,5,6,7,8,9,});
  std::cout << "i32Array.size() after assign(3): " << i32Array.size() << std::endl;

  int32_t* data = i32Array.data();


  // Stick AttributeArray into a STL container
  std::vector<AttributeArray<float>> vecOfFloatArrays;


  // -----------------------------------------------------------------------------
  // Let's try something that has components, like an ARGB array
  using RgbaType = AttributeArray<uint8_t>;
  using CompDimsType = RgbaType::comp_dims_type;
  using RgbaIterator = RgbaType::tuple_iterator;

  CompDimsType cDims = {4};
  RgbaType rgba(static_cast<size_t>(10), cDims, "RGBA Array");

  RgbaIterator begin = rgba.begin<RgbaIterator>();
  rgba.initializeWithValue(0xFF);
  rgba[0] = 0x65; rgba[1] = 0x66; rgba[2] = 0x67; rgba[3] = 0x68;

  for(RgbaIterator rgbaIter = rgba.begin<RgbaIterator>(); rgbaIter != rgba.end<RgbaIterator>(); rgbaIter++)
  {
    std::cout << "rgba: " << static_cast<int>(*rgbaIter) << std::endl;
    std::cout << " rgba[0] " << static_cast<int>(rgbaIter.comp_value(0))
              <<  " rgba[1] " << static_cast<int>(rgbaIter.comp_value(1))
               <<  " rgba[2] " << static_cast<int>(rgbaIter.comp_value(2))
                <<  " rgba[3] " << static_cast<int>(rgbaIter.comp_value(3)) << std::endl;

    for(size_t i = 0; i < 4; i++)
    {
      rgbaIter.comp_value(i) = 55 + i;
    }
    std::cout << "rgba: " << static_cast<int>(*rgbaIter) << std::endl;
    std::cout << " rgba[0] " << static_cast<int>(rgbaIter.comp_value(0))
              <<  " rgba[1] " << static_cast<int>(rgbaIter.comp_value(1))
               <<  " rgba[2] " << static_cast<int>(rgbaIter.comp_value(2))
                <<  " rgba[3] " << static_cast<int>(rgbaIter.comp_value(3)) << std::endl;


  }
  return 0;
}
