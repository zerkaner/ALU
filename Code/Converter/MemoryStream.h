#pragma once
#pragma warning(disable: 4996)
#include "MPQReader.h"
#include <Data/Primitives.h>
#include <stdlib.h>
#include <string.h>

/** This class facilitates stream operations (like 'fread') for in-memory data. */
class MemoryStream {

  private:

    DWORD _position;  // Current stream position.
    DWORD _size;      // Memory data size.
    BYTE* _data;      // The data to read from.


  public:

    /** Stream positioning descriptor. */
    enum Position {START, CURRENT, END};


    /** Create a memory stream for a byte array.
     * @param data The byte array. 
     * @param size The length of the array. */
    MemoryStream(BYTE* data, DWORD size) {
      _data = data;
      _size = size;
      _position = 0;
    }


    /** Create memory stream for a file from a MPQ archive.
     * @param reader An initialized MPQ reader pointing to the right file.
     * @param file The file to extract from the archive. */
    MemoryStream(MPQReader* reader, char* file) {
      _data = reader->ExtractFile(file, &_size);
      if (_data == NULL) printf("[MemoryStream] Error reading file '%s' from archive!\n", file);
    }


    /** Create memory stream for default files.
     * @param filepath The file to load. */
    MemoryStream(const char* filepath) {
      FILE* fp = fopen(filepath, "rb");
      if (fp == NULL) printf("[MemoryStream] Error opening file '%s'!\n", filepath);
      else {
        fseek(fp, 0L, SEEK_END);
        _size = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        _data = (BYTE*) malloc(_size);
        fread(_data, sizeof(BYTE), _size, fp);
        fclose(fp);
      }
    }


    /** Reads data from the current position of the stream.
     * @param destination Target address to write data to. 
     * @param bytes The number of bytes to read. */
    void Read(void* destination, int bytes) {
      if (_data != NULL && _position + bytes < _size) {
        memcpy(destination, &_data[_position], bytes);
        _position += bytes;
      }
      else printf("[MemoryStream] Address violation at position '%u'!\n", (_position + bytes));
    }


    /** Repositioning of the stream pointer.
     * @param position Position reference ('START', 'CURRENT', 'END').
     * @param offset Offset value, relative to the descriptor above [default: 0]. */
    void Seek(Position position, long offset = 0) {
      switch (position) {
        case START  : _position = 0 + offset;        break;
        case CURRENT: _position += offset;           break;
        case END    : _position = (_size-1)+offset;  break;
      }
    }


    /** Prints the current stream position. */
    void Tell() {
      printf("[MemoryStream] Current position: %u / %u.\n", _position, _size);
    }


    /** Closes the stream by releasing the allocated memory. */
    void Close() {
      if (_data != NULL) free(_data);
      _data = NULL;
    }


    /** On destructor call, automatically free memory. */
    ~MemoryStream() {
      Close();
    }
};
