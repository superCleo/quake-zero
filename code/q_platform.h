#pragma once

#include <stdint.h>

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;
typedef int B32;

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef I32 fixed16; // 16 bits for mantissa
typedef I32 fixed20; // 20 bits for mantissa

#define INTERNAL_LINKAGE static

#define MAX_OS_PATH_LENGTH 256

#if QUAKEREMAKE_SLOW
#define ASSERT(expression) if (!(expression)) { *((int *)0) = 0; }
#else
#define ASSERT(expression)
#endif

#define ARRAY_COUNT(array) sizeof(array) / sizeof(array[0])

#define KILO_BYTES(val) ((val) * 1024LL)
#define MEGA_BYTES(val) (KILO_BYTES(val) * 1024LL)
#define GIGA_BYTES(val) (MEGA_BYTES(val) * 1024LL)
#define TERA_BYTES(val) (GIGA_BYTES(val) * 1024LL)

inline fixed20 FloatToFixed20(float v)
{
    // 0x100000 = 2^20
    fixed20 result = (I32)(v * 0x100000);
    return result;
}

inline float Fixed20ToFloat(fixed20 f)
{
    float r = (float)f / 0x100000;
    return r;
}

struct ThreadContext
{
    int placeHolder;
};

//=== Services the platform layer provides to the game ===
   
#if QUAKEREMAKE_INTERNAL 

struct DebugReadFileResult
{
    int contentSize;
    void *content;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(ThreadContext *thread, DebugReadFileResult *file)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(DebugPlatformFreeFileMemory_t);

#define DEBUG_PLATFORM_READ_WHOLE_FILE(name) DebugReadFileResult name(ThreadContext *thread, const char *filename)

typedef DEBUG_PLATFORM_READ_WHOLE_FILE(DebugPlatformReadWholeFile_t);

#define DEBUG_PLATFORM_WRITE_WHOLE_FILE(name) bool name(ThreadContext *thread, const char *filename, void *memory, U32 memorySize)

typedef DEBUG_PLATFORM_WRITE_WHOLE_FILE(DebugPlatformWriteWholeFile_t);

#endif


//========================================================

//=== Services that the game provides to the platform layer
struct GameOffScreenBuffer
{
    I32 width;
    I32 height;
    I32 bytesPerPixel;
    I32 bytesPerRow;
    void *memory;
    U8 *palette;
};

struct GameSoundOutputBuffer
{
    I32 samplesPerSecond;
    I32 sampleCount;
    I16 *samples;
};

//     |  player makes input  |   we process input  |   inputs take effect
//  frame0                  frame1                frame2
//  so, input is always one frame behind
struct GameInput
{
    I32 keydown[256];
};

#define SYS_ERROR(name) void name(char *format, ...)
typedef SYS_ERROR(SysError_t);

#define SYS_SET_PALETTE(name) void name(U8 *palette)
typedef SYS_SET_PALETTE(SysSetPalette_t);

struct PlatformAPI
{
    SysError_t *SysError;
    SysSetPalette_t *SysSetPalette;
};

PlatformAPI g_platformAPI;

struct GameMemory
{
    void *gameMemory;
    I32 gameMemorySize;

    PlatformAPI platformAPI;

    char gameAssetDir[MAX_OS_PATH_LENGTH];

    GameOffScreenBuffer offscreenBuffer;
};

#define GAME_INIT(name) void name(GameMemory *memory)
typedef GAME_INIT(GameInit_t);

GAME_INIT(GameInit_stub) { }

#define GAME_UPDATE_AND_RENDER(name) \
    void name(GameInput *gameInput)

typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender_t);

GAME_UPDATE_AND_RENDER(GameUpdateAndRender_stub) { }

#define GAME_GET_SOUND_SAMPLES(name) \
    void name(ThreadContext *thread, GameMemory *gameMemory, GameSoundOutputBuffer *soundBuffer)

typedef GAME_GET_SOUND_SAMPLES(GameGetSoundSamples_t);

// GAME_GET_SOUND_SAMPLES(GameGetSoundSamples_stub) { }