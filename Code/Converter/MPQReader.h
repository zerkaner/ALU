#pragma once
#include <Data/Primitives.h>
#include <stdio.h>


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


/** MPQ archive structure. */
struct MPQArchive {
  WORD version;                // MPQ version.
  WORD blocksize;              // Size of one block.
  DWORD hashtableSize;         // Number of entries in hash table.
  DWORD blocktableSize;        // Number of entries in block table.
  HashTableEntry* hashtable;   // Hash table link.
  BlockTableEntry* blocktable; // The block table.
};



/** MPQ reader class. Full of black Blizzard magic. */
class MPQReader {

  private:   
    long _cryptTable[1280];   // Crypt table array.


    /** Set up the encryption hash table. */
    void PrepareCryptTable();


    /** Create a one-way hash from a file name.
     * @param filename The string that shall be hashed.
     * @param hashtype The type of hash to compute (0-Offset, 1-Hash1, 2-Hash2, 3-Table). 
     * @return The computed hash. */
    unsigned long HashString(char* filename, unsigned long hashtype);
  

    /** Decrypt a data block from the MPQ. 
     * @param block Pointer to the data block to decode [also result].
     * @param length Block length. 
     * @param key Decoding key. */
    void DecryptBlock(void* block, long length, unsigned long key);


    /** Get the hash entry to a file.
     * @param filename The file to search for.
     * @param hashtable MPQ hash table reference.
     * @param tablesize Size of hash table. 
     * @return The file's hash table index. */
    int GetHashTablePos(char* filename, HashTableEntry* hashtable, DWORD tablesize);


    /** Opens a MPQ archive. 
     * @param fp File stream pointer to read.
     * @return Reference to the MPQ archive metadata structure. */
    MPQArchive* OpenArchive(FILE* fp);


    void Test();


  public:

    /** Instantiate a new reader. */
    MPQReader() {
      PrepareCryptTable();
      Test();
    }
};
