#include "c_chuck.h" // C wrapper
#include "chuck.h" // ChucK single header magic
extern "C" {
    ChucK* ChucK_newChucK() {
        return new ChucK();
    }
    void ChucK_destroyChucK(ChucK* v) {
        delete v;
    }
    Chuck_Carrier* Chuck_Carrier_newChuck_Carrier() {
        return new Chuck_Carrier();
    }
    void Chuck_Carrier_destroyChuck_Carrier(Chuck_Carrier* v) {
        delete v;
    }
    Chuck_VM* Chuck_VM_newChuck_VM() {
        return new Chuck_VM();
    }
    void Chuck_VM_destroyChuck_VM(Chuck_VM* v) {
        delete v;
    }
    Chuck_VM_Code* Chuck_VM_Code_newChuck_VM_Code() {
        return new Chuck_VM_Code();
    }
    void Chuck_VM_Code_destroyChuck_VM_Code(Chuck_VM_Code* v) {
        delete v;
    }
    Chuck_VM_Shred* Chuck_VM_Shred_newChuck_VM_Shred() {
        return new Chuck_VM_Shred();
    }
    void Chuck_VM_Shred_destroyChuck_VM_Shred(Chuck_VM_Shred* v) {
        delete v;
    }
    Chuck_VM_Object* Chuck_VM_Object_newChuck_VM_Object() {
        return new Chuck_VM_Object();
    }
    void Chuck_VM_Object_destroyChuck_VM_Object(Chuck_VM_Object* v) {
        delete v;
    }
    // Chuck_IO_Serial* Chuck_IO_Serial_newChuck_IO_Serial() {
    //     return new Chuck_IO_Serial();
    // }
    // void Chuck_IO_Serial_destroyChuck_IO_Serial(Chuck_IO_Serial* v) {
    //     delete v;
    // }
    Chuck_Compiler* Chuck_Compiler_newChuck_Compiler() {
        return new Chuck_Compiler();
    }
    void Chuck_Compiler_destroyChuck_Compiler(Chuck_Compiler* v) {
        delete v;
    }
    HidInManager* HidInManager_newHidInManager() {
        return new HidInManager();
    }
    void HidInManager_destroyHidInManager(HidInManager* v) {
        delete v;
    }
}
