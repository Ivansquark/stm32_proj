#ifndef NORMALQUEUE_H
#define NORMALQUEUE_H

#include "stdint.h"

class NormalQueue8 {
public:  
	static constexpr uint16_t SIZE = 128*16; //queue size = 2048 bytes
	static NormalQueue8* pThis;
 
  NormalQueue8();
  uint8_t arr[SIZE];
  uint8_t* head{nullptr};
  uint8_t* tail{nullptr};

  uint16_t QueSize();
  void push(uint8_t val);
  uint8_t pop();
  bool isEmpty();
  
	void clear();
};


#endif //NORMALQUEUE_H