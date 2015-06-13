#include <Data/Primitives.h>

/** Compression / decompression class based on the brilliant miniz.c. */
class Archiver {

  public:

    /** ZLib-style byte compression. 
     * @param input Uncompressed byte stream. 
     * @param inLength Length of input data.
     * @param output Reference to compression pointer. 
     * @param outLength Pointer to store compression size into. 
     * @param mode Compression mode (0-9). If not set or invalid, defaults are used. 
     * @return Compression success state. */
    static bool Compress(BYTE* input, DWORD inLength, BYTE** output, DWORD* outLength, int mode = -1);


    /** Decompression function.
    * @param input Compressed data.
    * @param inLength Length of compressed data stream.
    * @param outLength Size of uncompressed data.
    * @return The uncompressed data stream. */
    static BYTE* Decompress(BYTE* input, DWORD inLength, DWORD outLength);
};
