#pragma warning(disable: 4996)
#include "MPQReader.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void MPQReader::Test() {
  FILE* fp = fopen("C:/Games/World of Warcraft 2.4.3 [8606]/Data/common.mpq", "rb");
  if (fp == NULL) { printf("Error opening archive!\n");  return; } 
  MPQArchive* archive = OpenArchive(fp);

  // Query a file:
  char* testfile = "CHARACTER\\Human\\Male\\HumanMale.M2";
  int result = GetHashTablePos(testfile, archive->hashtable, archive->hashtableSize);
  printf("File '%s' has index %d\n", testfile, result);
  if (result != -1) {
    printf("- Blockindex: %u\n", archive->hashtable[result].blockindex);
    printf("- Compressed: %u\n", archive->blocktable[archive->hashtable[result].blockindex].compressedSize);
    printf("- Uncompr.  : %u\n", archive->blocktable[archive->hashtable[result].blockindex].uncompressedSize);
 

    DWORD flags = archive->blocktable[archive->hashtable[result].blockindex].flags;
    bool encrypted   = (flags & (1 << 15)) != 0;
    bool compressed  = (flags & (1 <<  9)) != 0;
    bool singleBlock = (flags & (1 << 23)) != 0;
    char buffer[33];
    itoa(flags, buffer, 2);
    printf("- Flags     : %s  |  compressed: %d | single block: %d | encrypted: %d\n", buffer, compressed, singleBlock, encrypted);

    DWORD compressedSize = archive->blocktable[archive->hashtable[result].blockindex].compressedSize;
    DWORD uncompressedSize = archive->blocktable[archive->hashtable[result].blockindex].uncompressedSize;
    int blocks = uncompressedSize / archive->blocksize;
    if ((uncompressedSize % archive->blocksize) != 0) blocks ++;
    printf("- Blocksize : %d\n", archive->blocksize);
    printf("- Blocks    : %d\n", blocks);

    // Set stream pointer to begin of file data and read the block sizes.
    _fseeki64(fp, (long long) archive->blocktable[archive->hashtable[result].blockindex].position, SEEK_SET);
    DWORD* comprBlocks = (DWORD*) calloc(blocks, sizeof(DWORD));
    fread(comprBlocks, sizeof(DWORD), blocks, fp);

    DWORD dbuf;
    for (int i = 0; i < 2; i ++) {
      fread(&dbuf, sizeof(DWORD), 1, fp);
      printf("%i | %u\n", i, dbuf);
    }

    // Just read the first part and decode that. If that works, we have already won.
    BYTE* bytes = (BYTE*) calloc(comprBlocks[0], sizeof(BYTE));
    fread(bytes, sizeof(BYTE), comprBlocks[0], fp);

    
    for (int i = 0; i < 10; i ++) printf("[%d] %#04x\n", i, bytes[i]);
    // Soll: 4D 44 32 30 - 'MD20'
    /*
    FILE* writer = fopen("test.txt", "wb");
    fwrite(bytes, sizeof(BYTE), comprBlocks[0], writer);
    fclose(writer);
    */
    free(bytes);
    free(comprBlocks);
  }


  free(archive->hashtable);
  free(archive->blocktable);
  fclose(fp);
  getchar();
}



//_________________________________________________________________________________________________
// PRIVATE FUNCTIONS.

void MPQReader::PrepareCryptTable() {
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



unsigned long MPQReader::HashString(char* filename, unsigned long hashtype) {
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



void MPQReader::DecryptBlock(void* block, long length, unsigned long key) {
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



int MPQReader::GetHashTablePos(char* filename, HashTableEntry* hashtable, DWORD tablesize) {
  DWORD nHash = HashString(filename, 0);
  DWORD nHashA = HashString(filename, 1);
  DWORD nHashB = HashString(filename, 2);
  int nHashStart = nHash % tablesize;
  int nHashPos = nHashStart;

  while (hashtable[nHashPos].blockindex != 0xFFFFFFFF) {
    if (hashtable[nHashPos].hash1 == nHashA &&
      hashtable[nHashPos].hash2 == nHashB) return nHashPos;
    else nHashPos = (nHashPos + 1) % tablesize;
    if (nHashPos == nHashStart) break;
  }
  return -1; // Return error value ("not found"). 
}



MPQArchive* MPQReader::OpenArchive(FILE* fp) {
  DWORD signature;

  do { // Scan for 'MPQ\x1A' header block.
    fread(&signature, sizeof(DWORD), 1, fp);
  } while(signature != '\x1AQPM'); 
  
  DWORD headerSize;      // Size of the archive header.
  DWORD archiveSize;     // Size of MPQ archive (deprecated since Burning Crusade).
  WORD version;          // 0 = Original, 1 = Burning Crusade, 2+ WotLK and newer.
  WORD blockExp;         // The size of each logical sector in the archive is 512 * 2^blockSize.
  DWORD hashtablePos;    // Hash table offset (relative to archive beginning)
  DWORD blocktablePos;   // Block table beginning offset.
  DWORD hashtableSize;   // Number of entries in the hash table (power of two).
  DWORD blocktableSize;  // Number of entries in the block table.
  fread(&headerSize,     sizeof(DWORD), 1, fp);
  fread(&archiveSize,    sizeof(DWORD), 1, fp);
  fread(&version,        sizeof(WORD),  1, fp);
  fread(&blockExp,       sizeof(WORD),  1, fp);
  fread(&hashtablePos,   sizeof(DWORD), 1, fp);
  fread(&blocktablePos,  sizeof(DWORD), 1, fp);
  fread(&hashtableSize,  sizeof(DWORD), 1, fp);
  fread(&blocktableSize, sizeof(DWORD), 1, fp);

  QWORD highBlocktablePos; // Offset to the beginning of array of 16-bit high parts of file offsets.
  WORD highHashtable;      // High 16 bits of the hash table offset for large archives.
  WORD highBlocktable;     // High 16 bits of the block table offset for large archives.
  if (version == 1) {      // Burning Crusade header contains additional 12 bytes. 
    fread(&highBlocktablePos, sizeof(QWORD), 1, fp);
    fread(&highHashtable, sizeof(WORD), 1, fp);
    fread(&highBlocktable, sizeof(WORD), 1, fp);
  }

  // Join the hash- and block table positions in a 64-bit quadruple word.
  QWORD hashtableHiPos  = (QWORD) highHashtable  << 32 | (QWORD) hashtablePos;
  QWORD blocktableHiPos = (QWORD) highBlocktable << 32 | (QWORD) blocktablePos;

  // Calculate the block size value properly.
  WORD blocksize = 512 * (WORD) powl(2, blockExp);

  // Create storage for hash and block table and read them from file. Decrypt tables afterwards.
  HashTableEntry*  hashtable = (HashTableEntry*) calloc(hashtableSize, sizeof(HashTableEntry));
  BlockTableEntry* blocktable = (BlockTableEntry*) calloc(blocktableSize, sizeof(BlockTableEntry));
  _fseeki64(fp, (long long) hashtableHiPos, SEEK_SET);
  fread(hashtable, sizeof(HashTableEntry), hashtableSize, fp);
  _fseeki64(fp, (long long) blocktableHiPos, SEEK_SET);
  fread(blocktable, sizeof(BlockTableEntry), blocktableSize, fp);
  DecryptBlock(hashtable, hashtableSize * 4, HashString("(hash table)", 3));
  DecryptBlock(blocktable, blocktableSize * 4, HashString("(block table)", 3));


  // Create archive structure and assign values, then return structure.
  MPQArchive* archive = new MPQArchive();
  archive->version = version;
  archive->blocksize = blocksize;
  archive->blocktable = blocktable;
  archive->hashtable = hashtable;
  archive->blocktableSize = blocktableSize;
  archive->hashtableSize = hashtableSize;
  return archive;
}
