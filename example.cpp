#include <iostream>
#include "bloomFilter.h"

int main() {
  BloomFilter<std::string> bloomFilter(10, 0.001);
  
  std::string test1 = "2TESTING1";
  std::string test2 = "TESTING2";
  
  bloomFilter.insert(test1, test1.length());
  std::cout << "Exists: " << bloomFilter.exists(test1, test1.length()) << std::endl;
  std::cout << "Shouldn't Exist: " << bloomFilter.exists(test2, test2.length()) << std::endl;

  
  bloomFilter.insert(test2, test2.length());
  std::cout << "Exists: " << bloomFilter.exists(test2, test2.length()) << std::endl;
  

  BloomFilter<std::string> bf2(1, 0.001);
  bf2.insert(test1, test1.length());
  std::cout << "Should return false since full: " << bf2.insert(test2, test2.length()) << std::endl;;

  std::cout << "Full?: " << bf2.full() << std::endl;

  
  
}
