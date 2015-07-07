#ifndef _WIN32
#define _WIN32
#endif

#include <Converter/Converter.h>
#include <Converter/DBCParser.h>
#include <Converter/MPQReader.h>
#include <Execution/ALU.h>
#include <Execution/test.h>
#include <stdio.h>

// [Converter test] --conv ..\Output\cali10.glf ..\Output\out.txt

int main (int argc, char** argv) {

  // Run converter on parameter "--conv". 
  if (argc > 1 && (strcmp(argv[1], "--conv") == 0)) {
    Converter(argc - 2, &argv[2]);
  }

  // "--load": Start the MPQ-DBC-M2 parsing toolchain [work has just begun!].
  else if (argc > 1 && (strcmp(argv[1], "--load") == 0)) {
    //TODO DBCParser(argc - 2, &argv[2]).Test();
    test();
  }

  // Normal system start-up.
  else {

    // Build the ALU initialization object.
    //TODO ...

    // Start the runtime environment and enter main loop.
    ALU* alu = new ALU();
    alu->Start();
    delete(alu);
  }
  
  // Delay exit (for debug reasons), then return. 
  printf("Press any key to exit.");
  getchar();
  return 0;
}
