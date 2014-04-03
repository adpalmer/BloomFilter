#include <iostream>
#include "bloomFilter.h"

int main() {
  BloomFilter<std::string> bloomFilter(10, 0.001);
  
  std::string test1 = "2TESTING1";
  std::string test2 = "TESTING2";
  
  bloomFilter.add(test1, test1.length());
  std::cout << "Exists: " << bloomFilter.exists(test1, test1.length()) << std::endl;
  std::cout << "Shouldn't Exist: " << bloomFilter.exists(test2, test2.length()) << std::endl;
  
  bloomFilter.add(test2, test2.length());
  std::cout << "Exists: " << bloomFilter.exists(test2, test2.length()) << std::endl;
  
  
}
