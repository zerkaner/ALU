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
    long _cryptTable[1280];      // Crypt table array.
    MPQArchive* _archive = NULL; // Link to the currently open archive.
    FILE* _fp = NULL;            // Archive input file stream.


    /** Set up the encryption hash table. */
    void PrepareCryptTable();


    /** Create a one-way hash from a file name.
     * @param filename The string that shall be hashed.
     * @param hashtype The type of hash to compute (0-Offset, 1-Hash1, 2-Hash2, 3-Table).
     * @return The computed hash. */
    unsigned long HashString(const char* filename, unsigned long hashtype);


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
    int GetHashTablePos(const char* filename, HashTableEntry* hashtable, DWORD tablesize);


    /** Opens a MPQ archive.
     * @param fp File stream pointer to read.
     * @return Reference to the MPQ archive metadata structure. */
    MPQArchive* OpenArchive(FILE* fp);


    /** Reads a MPQ file sector and decompresses / decrypts the content, if needed.
     * @param destination Memory pointer where to store the content of this sector.
     * @param fp Input file stream (correctly positioned).
     * @param sectorSize Size of sector. Also used to determine if compression is applied (sector size < data size).
     * @param dataSize Size of (decompressed) content.
     * @return Reading and decompression success indicator. */
    bool ReadSector(void* destination, FILE* fp, int sectorSize, int dataSize);


    /** Reads a file from a MPQ archive.
     * @param fp File stream to use as input (set to the archive).
     * @param archive Pointer to the MPQ archive to read.
     * @param filename Name of the file to extract.
     * @param filesize Pointer to a variable to store the file size in.
     * @return The unified file data or 'NULL' on failure. */
    BYTE* ReadFile(FILE* fp, MPQArchive* archive, const char* filename, DWORD* filesize);


  public:

    /** Instantiate a new MPQ archive reader and keeps the connection open.
     * @param archivePath Full path to the MPQ archive to open. */
    MPQReader(const char* archivePath);


    /** Destructor, calls Close() method. */
    ~MPQReader();


    /** Extracts a file from the archive.
    * @param filename The name of the file to extract.
    * @param filesize Pointer for file size storage [return value].
    * @return The acquired file data or 'NULL', if file could not be read. */
    BYTE* ExtractFile(const char* filename, DWORD* filesize);


    /** Closes an opened archive and releases reserved memory. */
    void Close();


    /** Opens an archive for a single file extraction.
     * @param archivePath Path to the archive to open.
     * @param filename The name of the file to extract.
     * @param filesize Pointer for file size storage [return value].
     * @return The acquired file data or 'NULL', if file could not be read. */
    static BYTE* ExtractFileFromArchive(const char* archivePath, char* filename, DWORD* filesize);
};
