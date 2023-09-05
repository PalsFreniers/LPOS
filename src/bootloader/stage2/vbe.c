#include "vbe.h"

bool VBE_GetController(struct VBEInfoBlock* outInfos) {
    if(x86_Video_Get_VBE_Info(outInfos) == 0x004F) {
        outInfos->VideoModePtr = LINEAR(outInfos->VideoModePtr);
        return true;
    }
    return false;
}

bool VBE_GetModeInfos(struct VBEModeInfo *outInfos, u16 mode) {
    return x86_Video_Get_VBE_Mode_Info(outInfos, mode) == 0x004F;
}

bool VBE_SetMode(u16 mode) {
    return x86_Video_Set_VBE_Mode(mode) == 0x004F;
}
