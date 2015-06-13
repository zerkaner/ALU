#ifndef _WIN32
#define _WIN32
#endif

#include <Converter/Converter.h>
#include <Converter/DBCParser.h>
#include <Converter/MPQReader.h>
#include <Execution/ALU.h>
#include <stdio.h>

#pragma warning(disable: 4996)
#include <stdlib.h> //DBG

// [Converter test] --conv ..\Output\cali10.glf ..\Output\out.txt

int main (int argc, char** argv) {

  // Run converter on parameter "--conv". 
  if (argc > 1 && (strcmp(argv[1], "--conv") == 0)) {
    Converter(argc - 2, &argv[2]);
  }

  // "--load": Start the MPQ-DBC-M2 parsing toolchain [work has just begun!].
  else if (argc > 1 && (strcmp(argv[1], "--load") == 0)) {
    
    const char* archive = "C:/Games/World of Warcraft 2.4.3 [8606]/Data/common.mpq";
    char* file = "CHARACTER\\Nightelf\\Female\\NightelfFemale.m2";
    DWORD filesize;  
    BYTE* data = MPQReader::ExtractFileFromArchive(archive, file, &filesize);
    
    if (data != NULL) {
      printf("Extraction successfully!\n");
      FILE* writer = fopen("test.m2", "wb");
      fwrite(data, sizeof(BYTE), filesize, writer);
      fclose(writer);    
      free(data);
    }

    getchar();

    //DBCParser(argc - 2, &argv[2]); 
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
  //printf("Press any key to exit.");
  //getchar();
  return 0;
}
