#pragma once 
#include <stdint.h>
#include <iostream>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef float real32 ;
typedef double real64 ;
typedef uint32_t bool32;

#define global_variable  static

#define local_persist static 
#define internal static  

#define Pi32 3.14159265359f
#define ArrayCount(Array) (sizeof(Array)/sizeof(Array[0]))

#define KiloBytes(Value) (Value*1024)
#define MegaBytes(Value) (KiloBytes(Value)*1024)
#define GigaBytes(Value) (MegaBytes(Value)*1024)
#define TeraBytes(Value) (GigaBytes(Value)*1024)

#if GAME_SLOW
#define Assert(Expression) \
	if(!Expression){*(int*)0 = 0;}
#else
#define Assert(Expression)
#endif

template<typename T>
internal void
PrintArray(T Array[], int start, int end, const std::string & label)
{
    std::cout << label << ": [ ";
    for (int i = start; i <= end; ++i) {
        std::cout << Array[i] << " ";
    }
    std::cout << "]\n";
}


template<typename T> 
internal void 
Swap(T* NewInput, T *OldInput)
{
	T temp = *NewInput;
	*NewInput = *OldInput;
     *OldInput = temp;
}
