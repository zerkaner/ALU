#include "Archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include "Miniz.h"


bool Archiver::Compress(BYTE* input, DWORD inLength, BYTE** output, DWORD* outLength, int mode) {
  *outLength = Miniz::mz_compressBound(inLength); // Calculate output size.
  *output = (BYTE*) malloc(*outLength);           // Reserve output storage. 
  int success;
  if (mode < 0 || mode > 9) success = Miniz::mz_compress(*output, outLength, input, inLength);
  else success = Miniz::mz_compress2(*output, outLength, input, inLength, mode);
  return (success == Miniz::MZ_OK);
}



BYTE* Archiver::Decompress(BYTE* input, DWORD inLength, DWORD outLength) {
  BYTE* output = (BYTE*) malloc(outLength);
  int success = Miniz::mz_uncompress(output, &outLength, input, inLength);
  if (success == Miniz::MZ_OK) return output;
  else {
    printf("[Archiver] Decompression error code %d!\n", success);
    free(output);
    return NULL;
  }
}