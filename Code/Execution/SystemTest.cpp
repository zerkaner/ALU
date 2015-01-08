#ifndef _WIN32
#define _WIN32
#endif

#include <Execution/ALU.h>
#include <stdio.h>


int main (int argc, char** argv) {

  // Build the ALU initialization object.
  //TODO ...

  // Start the runtime environment and enter main loop.
  ALU* alu = new ALU();
  alu->Start();

  // Exit and return.
  delete(alu);
  printf("Press any key to exit.");
  getchar();
  return 0;
}
