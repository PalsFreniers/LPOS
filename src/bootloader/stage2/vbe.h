#pragma once

#include "types.h"
#include "stdmem.h"
#include "x86.h"

#define COLOR(r, g, b) ((b) | (g << 8) | (r << 16))

struct VBEInfoBlock {
    char VBESignature[4];
    u16 VBEVersion;
    u16 OEMStringPtr[2];
    u8  Capabilities[4];
    u32 VideoModePtr;
    u16 TotalMemory;
    u8  reserved[492];
} __attribute__((packed));

struct VBEModeInfo {
	u16 attributes;		
	u8 window_a;			
	u8 window_b;			
	u16 granularity;		
	u16 window_size;
	u16 segment_a;
	u16 segment_b;
	u32 win_func_ptr;		
	u16 pitch;			
	u16 width;			
	u16 height;			
	u8 w_char;			
	u8 y_char;			
	u8 planes;
	u8 bpp;			
	u8 banks;			
	u8 memory_model;
	u8 bank_size;		
	u8 image_pages;
	u8 reserved0;
 
	u8 red_mask;
	u8 red_position;
	u8 green_mask;
	u8 green_position;
	u8 blue_mask;
	u8 blue_position;
	u8 reserved_mask;
	u8 reserved_position;
	u8 direct_color_attributes;
 
	u32 framebuffer;		
	u32 off_screen_mem_off;
	u16 off_screen_mem_size;	
	u8 reserved1[206];
} __attribute__((packed));

bool VBE_GetController(struct VBEInfoBlock* outInfos);
bool VBE_GetModeInfos(struct VBEModeInfo* outInfos, u16 mode);
bool VBE_SetMode(u16 mode);

