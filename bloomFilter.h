#ifndef _BLOOM_FILTER_
#define _BLOOM_FILTER_

#include <cmath>
#include <utility>
#include <list>
#include "murmurhash.h"

template <class T>
class BloomFilter {
private:
  unsigned char *bitVector;
  int elements; // number of elements inserted
  unsigned int capacity; // total number of elements possible
  double errorRate; // user specified error rate
  unsigned int bitVectorSize; // size of data array
  std::list<int> hashSeeds;

public:
  BloomFilter(unsigned int capacity, double errorRate);
  ~BloomFilter();
  BloomFilter(BloomFilter &&rhs);
  BloomFilter(const BloomFilter &rhs);
  BloomFilter<T>& operator=(BloomFilter&& rhs);
  BloomFilter<T>& operator=(const BloomFilter &rhs);

  // return true if full, false if not
  bool full() { return elements == capacity; }

  // add element to bloom filter given data and sizeof(data);
  // return true if inserted, false if full
  bool add(T &data);

  // add element to bloom filter given data of type T and a byteCnt
  // return true if inserted, false if full
  bool add(T &data, int byteCnt);
  
  // checks for existence given data of type T and sizeof(data)
  bool exists(T &data);

  // checks for existance given data of type T and a byteCnt
  bool exists(T &data, int bytCnt);
};


// Constructor
template <class T>
BloomFilter<T>::BloomFilter(unsigned int capacity, double errorRate) : elements{0}, capacity{capacity}, errorRate{errorRate} {
  // calculate number of bits that will give desired error rate with desired capacity
  bitVectorSize = (unsigned int)ceil((capacity*-log(errorRate))/(log(2)*log(2)));
  
  unsigned int dataSize = bitVectorSize/8;
  if(bitVectorSize % 8)
    dataSize++;
  bitVector = new unsigned char[dataSize];

  bitVectorSize = dataSize * 8;

  // calculate number of functions -> use bitVectorSize * 8 insetad of numBits
  // because numBits may underestimate the size if bitVectorSize isn't divisible
  // by 8 (bits in a byte (char))
  unsigned int k = (bitVectorSize/capacity) * log(2);
  for(int i=0; i<k; i++) {
    // need to do some research to ensure sequential seeds won't cause issues
    // technically can store k instead and seed with [0..k) but want flexibility
    // for alternative seed list
    hashSeeds.push_back(i);
  }
}

// Move Constructor
template <class T>
BloomFilter<T>::BloomFilter(BloomFilter &&rhs): bitVector{rhs.bitVector}, elements{rhs.elements}, capacity{rhs.capacity}, errorRate{rhs.errorRate}, bitVectorSize{rhs.bitVectorSize} {
  hashSeeds = std::move(rhs.hashSeeds);
  rhs.bitVector = nullptr;
  elements = capacity = errorRate = bitVectorSize = 0;
}

// Copy Constructor
template <class T>
BloomFilter<T>::BloomFilter(const BloomFilter &rhs): elements{rhs.elements}, capacity{rhs.capacity}, errorRate{rhs.errorRate}, bitVectorSize{rhs.bitVectorSize}, hashSeeds{rhs.hashSeeds} {
  bitVector = new char[bitVectorSize];
  for(int i=0; i<bitVectorSize; i++) {
    bitVector[i] = rhs.bitVector[i];
  }
}

// move = operator
template <class T>
BloomFilter<T>& BloomFilter<T>::operator=(BloomFilter &&rhs) {
  if(*this != rhs) {
    hashSeeds = std::move(rhs.hashSeeds);
    bitVector = rhs.bitVector;
    elements = rhs.elements;
    capacity = rhs.capacity;
    errorRate = rhs.errorRate;
    bitVectorSize = rhs.bitVectorSize;
    rhs.bitVector = nullptr;
    rhs.elements = rhs.capacity = rhs.errorRate = rhs.bitVectorSize = 0;
  }
  return *this;
}

// copy = operator
template <class T>
BloomFilter<T>& BloomFilter<T>::operator=(const BloomFilter &rhs) {
  if(*this != rhs) {
    // delete if data is not null and bitVectorSizes aren't equal
    if(bitVector != nullptr && bitVectorSize != rhs.bitVectorSize) {
      delete []bitVector;
    }
    
    // allocate memory if data is null
    if(bitVector == nullptr) {
      bitVector = new char[bitVectorSize];
    }
    
    // copy all data to lhs
    for(int i=0; i<bitVectorSize; i++) {
      bitVector[i] = rhs.bitVector[i];
    }
    hashSeeds = rhs.hashSeeds;
    elements = rhs.elements;
    capacity = rhs.capacity;
    errorRate = rhs.errorRate;
    bitVectorSize = rhs.bitVectorSize;
  }
  return *this;
}

template <class T>
BloomFilter<T>::~BloomFilter() {
  delete []bitVector;
}



// add element to bloom filter given data and sizeof(data);
// return true if inserted, false if full
template <class T>
bool BloomFilter<T>::add(T &data) {
  return add(data, sizeof(data));
}

// add element to bloom filter given data of type T and a byteCnt
// return true if inserted, false if full
template <class T>
bool BloomFilter<T>::add(T &data, int byteCnt) {
  unsigned int hashVal, dataIdx, bitIdx;
  
  if(full()) {
    return false;
  }

  elements++;

  for(auto i : hashSeeds) {
    hashVal = murmur3_32(&data, byteCnt, i) % bitVectorSize;
    dataIdx = hashVal/8;
    bitIdx = hashVal % 8;

    bitVector[dataIdx] |= (1 << bitIdx);
  }

  return true;
}

// checks for existence given data of type T and sizeof(data)
template <class T>
bool BloomFilter<T>::exists(T &data) {
  return exists(data, sizeof(data));
}

// checks for existance given data of type T and a byteCnt
template <class T>
bool BloomFilter<T>::exists(T &data, int byteCnt) {
  unsigned int hashVal, dataIdx, bitIdx, hashResult;
  
  for(auto i : hashSeeds) {
    hashVal = murmur3_32(&data, byteCnt, i) % bitVectorSize;
    dataIdx = hashVal/8;
    bitIdx = hashVal % 8;

    hashResult = bitVector[dataIdx] & (1 << bitIdx);
    if(hashResult == 0)
      return false;
    
  }
  
  return true;
}

#endif
