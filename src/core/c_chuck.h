/*  This file was handwritten to wrap C++ code in C to facilitate using
    ChucK from other languages through FFI.
    Andrew Prentice (c) 2019-2021 Licensed under GPL3
*/
#include "chuck_def.h"
#include "chuck_errmsg.h"
#ifndef C_CHUCK_H
#define C_CHUCK_H
#include <stdio.h>
#include <stdint.h>
#if defined _WIN32 || defined __CYGWIN__
    #ifdef C_CHUCK_NO_EXPORT
        #define API
    #else
        #define API __declspec(dllexport)
    #endif
    #ifndef __GNUC__
    #define snprintf sprintf_s
    #endif
#else
    #define API
#endif

#ifdef __cplusplus
    #define EXTERN extern "C"
#else
    #include <stdarg.h>
    #include <stdbool.h>
    #define EXTERN extern
#endif

#define CC_API EXTERN API
#define CONST const

#ifdef __cplusplus
    extern "C" {
#endif
#ifndef ENUMS_AND_STRUCTS
    #define ENUMS_AND_STRUCTS
#endif
#ifdef ENUMS_AND_STRUCTS
// C++ class typedef
typedef struct ChucK ChucK;
typedef struct Chuck_Carrier Chuck_Carrier;
typedef struct Chuck_VM Chuck_VM;
typedef struct Chuck_VM_Code Chuck_VM_Code;
typedef struct Chuck_VM_Shred Chuck_VM_Shred;
typedef struct Chuck_VM_Object Chuck_VM_Object;
typedef struct Chuck_IO_Serial Chuck_IO_Serial;
typedef struct Chuck_Compiler Chuck_Compiler;
struct ChucK;
struct Chuck_Carrier;
struct Chuck_VM;
struct Chuck_VM_Code;
struct Chuck_VM_Shred;
struct Chuck_VM_Object;
struct Chuck_IO_Serial;
struct Chuck_Compiler;
// C++ class constructor
ChucK *ChucK_newChucK();
// C++ class methods
// void ChucK_setParam(ChucK *){}
// void ChucK_getParamInt(ChucK *){}
// C++ class destructor
void ChucK_destroyChucK(ChucK* v);
Chuck_Carrier *Chuck_Carrier_newChuck_Carrier();
void Chuck_Carrier_destroyChuck_Carrier(Chuck_Carrier* v);
Chuck_VM *Chuck_VM_newChuck_VM();
void Chuck_VM_destroyChuck_VM(Chuck_VM* v);
Chuck_VM_Code *Chuck_VM_Code_newChuck_VM_Code();
void Chuck_VM_Code_destroyChuck_VM_Code(Chuck_VM_Code* v);
Chuck_VM_Shred *Chuck_VM_Shred_newChuck_VM_Shred();
void Chuck_VM_Shred_destroyChuck_VM_Shred(Chuck_VM_Shred* v);
Chuck_VM_Object *Chuck_VM_Object_newChuck_VM_Object();
void Chuck_VM_destroyChuck_VM(Chuck_VM* v);
Chuck_IO_Serial *Chuck_IO_Serial_newChuck_IO_Serial();
void Chuck_IO_Serial_destroyChuck_IO_Serial(Chuck_IO_Serial* v);
Chuck_Compiler *Chuck_Compiler_newChuck_Compiler();
void Chuck_Compiler_destroyChuck_Compiler(Chuck_Compiler* v);
#else // ENUMS_AND_STRUCTS
    CC_API ChucK* ChucK_newChucK(void);
    CC_API void ChucK_destroyChucK(ChucK* self);
    CC_API Chuck_Carrier* Chuck_Carrier_newChuck_Carrier(void);
    CC_API void Chuck_Carrier_destroyChuck_Carrier(Chuck_Carrier* self);
    CC_API Chuck_VM* Chuck_VM_newChuck_VM(void);
    CC_API void Chuck_VM_destroyChuck_VM(Chuck_VM* self);
    // CC_API Chuck_VM_Code* Chuck_VM_Code_newChuck_VM_Code(void);
    // CC_API void Chuck_VM_Code_destroyChuck_VM_Code(Chuck_VM* self);
    // CC_API Chuck_VM_Shred* Chuck_VM_Shred_newChuck_VM_Shred(void);
    // CC_API void Chuck_VM_Shred_destroyChuck_VM_Shred(Chuck_VM* self);
    // CC_API Chuck_VM_Object* Chuck_VM_Object_newChuck_VM_Object(void);
    // CC_API void Chuck_VM_Object_destroyChuck_VM_Object(Chuck_VM* self);
    CC_API Chuck_IO_Serial* Chuck_IO_Serial_newChuck_IO_Serial(void);
    CC_API void Chuck_IO_Serial_destroyChuck_IO_Serial(Chuck_VM* self);
    // CC_API Chuck_Compiler* Chuck_Compiler_newChuck_Compiler(void);
    // CC_API void Chuck_Compiler_destroyChuck_Compiler(Chuck_VM* self);
    // CC_API HidInManager* HidInManager_newHidInManager(void);
    // CC_API void HidInManager_destroyHidInManager(Chuck_VM* self);
#endif
#ifdef __cplusplus
    }
#endif
#endif // C_CHUCK_H
