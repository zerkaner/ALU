#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

//#include <stdio.h>


/** This stopwatch offers OS-independent time measurement. */
class Stopwatch {

  private:
    typedef unsigned long long UInt64;  // Redefinition for comfort.
    UInt64 _last;                       // Time (ms) of last call.


  public:

    /** Create a new stopwatch. */
    Stopwatch() {
      GetInterval();  // Set _last value accordingly. 
    }


    /** Returns the amount of milliseconds passed since the last call.
     * @return Milliseconds (10^-3) as long value. */
    long GetInterval() {
      UInt64 cur;

      /* Version for Windows. */
      #ifdef _WIN32 
      FILETIME ft;
      LARGE_INTEGER li;
      GetSystemTimeAsFileTime(&ft); // Amount of 100ns intervals elapsed since January 1, 1601 (UTC).    
      li.LowPart = ft.dwLowDateTime;
      li.HighPart = ft.dwHighDateTime;
      cur = li.QuadPart;     
      cur -= 116444736000000000LL;  // Convert from file time to UNIX epoch time.
      cur /= 10000;                 // From 100ns (10^-7) to 1ms (10^-3) intervals.

      /* Do it the Linux / Unix way. */
      #else           
      timeval tv;   // struct before...?
      gettimeofday(&tv, NULL);
      cur = tv.tv_usec;  
      cur /= 1000;               // Convert from us (10^-6) to ms (10^-3).
      cur += (tv.tv_sec * 1000); // Transform seconds to ms and add them.
      #endif

      // Set current value as reference and return offset. 
      long diff = (long) (cur - _last);
      //printf("last: %llu  cur: %llu  diff: %ld\n", _last, cur, diff);
      _last = cur;
      return diff;
    } 
};
