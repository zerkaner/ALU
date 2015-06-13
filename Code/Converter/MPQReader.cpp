#pragma warning(disable: 4996)
#include "MPQReader.h"
#include <Data/Compression/Archiver.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//_________________________________________________________________________________________________
// PUBLIC FUNCTIONS.

MPQReader::MPQReader(const char* archivePath) {
  PrepareCryptTable();
  _fp = fopen(archivePath, "rb");
  if (_fp == NULL) printf("[MPQReader] Error opening archive '%s'!\n", archivePath);
  else _archive = OpenArchive(_fp);
}



MPQReader::~MPQReader() {
  Close();
}



void MPQReader::Close() {
  if (_archive != NULL) {
    free(_archive->hashtable);
    free(_archive->blocktable);
    _archive = NULL;
  }
  if (_fp != NULL) {
    fclose(_fp);
    _fp = NULL;
  }
}



BYTE* MPQReader::ExtractFile(char* filename, DWORD* filesize) {
  if (_archive == NULL || _fp == NULL) {
    printf("[MPQReader] Error extracting file '%s'. No archive opened!\n", filename);
    return NULL;
  }
  return ReadFile(_fp, _archive, filename, filesize);
}



BYTE* MPQReader::ExtractFileFromArchive(const char* archivePath, char* filename, DWORD* filesize) {
  MPQReader reader = MPQReader(archivePath);
  BYTE* data = reader.ExtractFile(filename, filesize);
  reader.Close();
  return data;
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



bool MPQReader::ReadSector(void* destination, FILE* fp, int sectorSize, int dataSize) {

  // The block content must be compressed by at least one byte (inclusive compression byte). Otherwise, the data
  // is not compressed at all (because it would be of no use). In that case, we just read it directly.  
  if (sectorSize < dataSize) {

    // Read compression algorithm. 
    BYTE compression = 0x00;
    fread(&compression, sizeof(BYTE), 1, fp);

    // Read compressed block.
    BYTE* compressedData = (BYTE*) malloc(sectorSize);
    fread(compressedData, sizeof(BYTE), sectorSize, fp);

    // Decompress data. Currently is only zlib supported.
    BYTE* uncompressedData = NULL;
    switch (compression) {
      case 0x02: {  // [0x02] = zlib.
        uncompressedData = Archiver::Decompress(compressedData, sectorSize, dataSize);
        break;
      }
      default: printf("[MPQReader] Decompression error. Type '%#04x' is not supported.\n", compression);
    }

    // On success, copy data to result structure and free temporary memory.
    if (uncompressedData != NULL) {
      memcpy(destination, uncompressedData, dataSize);
      free(uncompressedData);
      free(compressedData);
    }

    // Error case. Print message and stop function.
    else {
      printf("Decompression failed! Stopping extraction.\n");
      free(compressedData);
      return false;
    }
  }

  // This is the "no compression" case. Write it directly to the result data!
  else fread(destination, sizeof(BYTE), dataSize, fp);


  //TODO The decryption code goes here.
  // For now, we don't need it.
  return true;
}



BYTE* MPQReader::ReadFile(FILE* fp, MPQArchive* archive, char* filename, DWORD* filesize) {

  // Query file from hash table. Quit, if file not found!
  int result = GetHashTablePos(filename, archive->hashtable, archive->hashtableSize);
  if (result == -1) {
    printf("[MPQReader] Error reading '%s'. File not found!\n", filename);
    return NULL;
  }

  // Shortcuts for file properties from hashtable.
  DWORD index = archive->hashtable[result].blockindex;
  DWORD flags = archive->blocktable[index].flags;
  DWORD compressedSize = archive->blocktable[index].compressedSize;
  DWORD uncompressedSize = archive->blocktable[index].uncompressedSize;

  // Parse file flags.
  bool isImploded = (flags & (1 << 8)) != 0;
  bool isDeflated = (flags & (1 << 9)) != 0;
  bool isEncrypted = (flags & (1 << 16)) != 0;
  bool isSingleUnit = (flags & (1 << 24)) != 0;
  bool isCompressed = (isDeflated || isImploded);

  //___________________________
  // Attribute debug output.
  /*
  char buffer[33];
  itoa(flags, buffer, 2);
  printf("- Blockindex: %u\n", index);
  printf("- Compressed: %u\n", compressedSize);
  printf("- Uncompr.  : %u\n", uncompressedSize);
  printf("- Flags     : %s  |  imploded: %d | deflated: %d | encrypted: %d | single unit: %d\n",
    buffer, isImploded, isDeflated, isEncrypted, isSingleUnit);
  */

  // Jump out on encrypted files. We can't do that (yet).
  if (isEncrypted) {
    printf("[MPQReader] Error reading '%s'. The file is encrypted!\n", filename);
    return NULL;
  }


  // Set stream pointer to begin of file data and reserve result memory.
  _fseeki64(fp, (long long) archive->blocktable[index].position, SEEK_SET);
  BYTE* filedata = (BYTE*) malloc(uncompressedSize);
  bool success;  // Stores reading returns to determine success.

  // The file is stored as a single block.  
  if (isSingleUnit) success = ReadSector(filedata, fp, compressedSize, uncompressedSize);

  // Otherwise we have a segmentation.
  else {

    // Calculate the number of sectors.
    int nrSectors = uncompressedSize / archive->blocksize;
    if ((uncompressedSize % archive->blocksize) != 0) nrSectors ++;

    // If the data is segmented and compressed, we have a sector offset table at the beginning.
    // It tells us the starting positions of each sector (and thereby the sizes of the compressed data).
    if (isCompressed) {

      // Sector offset table exists: Calculate and read the sector sizes.   
      DWORD* sectorOffsets = (DWORD*) calloc(nrSectors, sizeof(DWORD));
      fread(sectorOffsets, sizeof(DWORD), nrSectors, fp);
      DWORD dbuf;                                                      //| At the end of the sector offset table follows
      for (int i = 0; i < 2; i ++) fread(&dbuf, sizeof(DWORD), 1, fp); //| two times the file size. Don't ask, why ...

      // Loop over all sectors.
      for (int i = 0; i < nrSectors; i ++) {

        // Calculate sector sizes.   
        int target = (i < nrSectors - 1) ? sectorOffsets[i + 1] : compressedSize;  // Offset target: Next sector or EOF. 
        int comprSize = target - sectorOffsets[i] - 1;  // Length to read. Reduced by one because compression byte is separate.
        int uncomprSize = (i < nrSectors - 1) ? archive->blocksize : uncompressedSize%archive->blocksize; // Target size.

        // Data read-in and decompression. Check for success and reloop.
        success = ReadSector(&filedata[i*archive->blocksize], fp, comprSize, uncomprSize);
        if (!success) break;
      }
    }

    // We have segmentation, but no compression. Just read a bunch of 4k blocks.
    else {
      for (int i = 0; i < nrSectors; i ++) {
        int size = (i < nrSectors - 1) ? archive->blocksize : uncompressedSize%archive->blocksize;
        success = ReadSector(&filedata[i*archive->blocksize], fp, size, size);
        if (!success) break;
      }
    }
  }


  if (success) { // Return the file and its size.
    *filesize = uncompressedSize;
    return filedata;
  }

  else { // Exit on failure.
    free(filedata);
    return NULL;
  }
}
