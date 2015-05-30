#pragma warning(disable: 4996)
#pragma once
#include <Data/Primitives.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>


/** MPQ reader class. Full of black Blizzard magic. */
class MPQReader {


  private:
    
    long _cryptTable[1280];   // Crypt table array.


    /** Set up the encryption hash table. */
    void PrepareCryptTable() {
      unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;

      for (index1 = 0; index1 < 0x100; index1++) {
        for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100) {
          unsigned long temp1, temp2;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp1 = (seed & 0xFFFF) << 0x10;
          seed = (seed * 125 + 3) % 0x2AAAAB;
          temp2 = (seed & 0xFFFF);
          _cryptTable[index2] = (temp1 | temp2);
        }
      }
    }


    /** Create a one-way hash from a file name.
     * @param filename The string that shall be hashed.
     * @param hashtype The type of hash to compute (0-Offset, 1-Hash1, 2-Hash2, 3-Table). 
     * @return The computed hash. */
    unsigned long HashString(char* filename, unsigned long hashtype) {
      unsigned char* key = (unsigned char*) filename;
      unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
      int ch;

      while (*key != 0) {
        ch = toupper(*key++);
        seed1 = _cryptTable[(hashtype << 8) + ch] ^ (seed1 + seed2);
        seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
      }
      return seed1;
    }
  

    /** Decrypt a data block from the MPQ. 
     * @param block Pointer to the data block to decode [also result].
     * @param length Block length. 
     * @param key Decoding key. */
    void DecryptBlock(void* block, long length, unsigned long key) {
      unsigned long seed = 0xEEEEEEEE;
      unsigned long ch;
      unsigned long* castBlock = (unsigned long*) block;  
      while (length-- > 0) {
        seed += _cryptTable[0x400 + (key & 0xFF)];
        ch = *castBlock ^ (key + seed);
        key = ((~key << 0x15) + 0x11111111) | (key >> 0x0B);
        seed = ch + seed + (seed << 5) + 3;
        *castBlock++ = ch;
      }
    }


    /** Hash entry. All files in the archive are searched by their hashes. */
    struct HashTableEntry {
      DWORD hash1;      // First hash of the full file name.   
      DWORD hash2;      // The second hash.
      WORD locale;      // File language (Windows LANGID). '0' is default (American English or neutral).
      WORD platform;    // The platform the file is used for. Always '0'.            
      DWORD blockindex; // If entry valid, this is the block index. Otherwise it is one of the following:
      //- 0xFFFFFFFF: Hash table entry is empty, and has always been empty. Terminates search.
      //- 0xFFFFFFFE: Entry is empty, but was valid at some point (deleted file). Does not terminate search.
    };


    /** File description block containing information about a file. */
    struct BlockTableEntry {
      DWORD position;         // Offset of the file data, relative to the beginning of the archive.
      DWORD compressedSize;   // Compressed file size.
      DWORD uncompressedSize; // Size of uncompressed file.
      DWORD flags;            // Flags for the file. See http://www.zezula.net/en/mpq/mpqformat.html.
    };


    /** Get the hash entry to a file.
     * @param filename The file to search for.
     * @param hashtable MPQ hash table reference.
     * @param tablesize Size of hash table. 
     * @return The file's hash table index. */
    int GetHashTablePos(char* filename, HashTableEntry* hashtable, DWORD tablesize) {
      DWORD nHash = HashString(filename, 0);
      DWORD nHashA = HashString(filename, 1);
      DWORD nHashB = HashString(filename, 2);
      int nHashStart = nHash % tablesize;
      int nHashPos = nHashStart;

      while (hashtable[nHashPos].blockindex != 0xFFFFFFFF) {
        if (hashtable[nHashPos].hash1 == nHashA &&
          hashtable[nHashPos].hash2 == nHashB) return nHashPos;
        else nHashPos = (nHashPos + 1) % tablesize;
        if (nHashPos == nHashStart) {
          printf("All around, still not found!");
          break;
        }
      }
      return -1; // Return error value ("not found"). 
    }


    void Test() {
      const char* path = "C:/Games/World of Warcraft 2.4.3 [8606]/Data/deDE/patch-deDE.mpq";
      
      FILE* fp = fopen(path, "rb");
      if (fp != NULL) {

        DWORD signature;
        fread(&signature, sizeof(DWORD), 1, fp);

        if (signature == '\x1AQPM') {  // MPQ\x1A => Header block.      
          DWORD headerSize;      // Size of the archive header.
          DWORD archiveSize;     // Size of MPQ archive (deprecated since Burning Crusade).
          WORD version;          // 0 = Original, 1 = Burning Crusade, 2+ WotLK and newer.
          WORD blocksize;        // The size of each logical sector in the archive is 512 * 2^wBlockSize.
          DWORD hashtablePos;    // Hash table offset (relative to archive beginning)
          DWORD blocktablePos;   // Block table beginning offset.
          DWORD hashtableSize;   // Number of entries in the hash table (power of two).
          DWORD blocktableSize;  // Number of entries in the block table.
          fread(&headerSize,     sizeof(DWORD), 1, fp);
          fread(&archiveSize,    sizeof(DWORD), 1, fp);
          fread(&version,        sizeof(WORD),  1, fp);
          fread(&blocksize,      sizeof(WORD),  1, fp);
          fread(&hashtablePos,   sizeof(DWORD), 1, fp);
          fread(&blocktablePos,  sizeof(DWORD), 1, fp);
          fread(&hashtableSize,  sizeof(DWORD), 1, fp);
          fread(&blocktableSize, sizeof(DWORD), 1, fp);
          
          printf("MPQ version: %u  [header size: %u]\n", version, headerSize);
          printf("Hashtable  : Start: %u, Size: %u\n", hashtablePos, hashtableSize);
          printf("Blocktable : Start: %u, Size: %u\n", blocktablePos, blocktableSize);


          QWORD highBlocktablePos; // Offset to the beginning of array of 16-bit high parts of file offsets.
          WORD highHashtable;      // High 16 bits of the hash table offset for large archives.
          WORD highBlocktable;     // High 16 bits of the block table offset for large archives.
          if (version == 1) {      // Burning Crusade header contains additional 12 bytes. 
            fread(&highBlocktablePos, sizeof(QWORD), 1, fp);
            fread(&highHashtable,  sizeof(WORD), 1, fp);
            fread(&highBlocktable, sizeof(WORD), 1, fp);
          }

          // Join the hash table position in a 64-bit quadruple word.
          QWORD hashtableHiPos = (QWORD)highHashtable << 32 | (QWORD)hashtablePos; 

          // Create storage for hash and block table and read them from file. Decrypt tables afterwards.
          HashTableEntry*  hashtable  =  (HashTableEntry*) calloc(hashtableSize,  sizeof(HashTableEntry));
          BlockTableEntry* blocktable = (BlockTableEntry*) calloc(blocktableSize, sizeof(BlockTableEntry));  
          _fseeki64(fp, (long long) hashtableHiPos, SEEK_SET);
          fread(hashtable, sizeof(HashTableEntry), hashtableSize, fp);
          _fseeki64(fp, (long long) blocktablePos, SEEK_SET);
          fread(blocktable, sizeof(BlockTableEntry), blocktableSize, fp);      
          DecryptBlock(hashtable,  hashtableSize  * 4, HashString("(hash table)",  3));
          DecryptBlock(blocktable, blocktableSize * 4, HashString("(block table)", 3));


          // Query a file:
          char* testfile = "Sound\\Creature\\Anetheron\\CAV_Anath_Slay01.wav";
          int result = GetHashTablePos(testfile, hashtable, hashtableSize);
          printf("File '%s' has index %d\n", testfile, result);
          if (result != -1) {
            printf("- Blockindex: %u\n", hashtable[result].blockindex);
            printf("- Compressed: %u\n", blocktable[hashtable[result].blockindex].compressedSize);
            printf("- Uncompr.  : %u\n", blocktable[hashtable[result].blockindex].uncompressedSize);
            printf("- Flags     : %u\n", (int)blocktable[hashtable[result].blockindex].flags);
          }


          free(hashtable);
          free(blocktable);
        }
        else if (signature == '\x1BQPM') {  // MPQ\x1B => Shunt block.
          printf("USERDATA found!\n");
        }
        fclose(fp);
      }
      getchar();
    }


  public:

    /** Instantiate a new reader. */
    MPQReader() {
      PrepareCryptTable();
      Test();
    }

};
