/**
 * r3dfb -- 3D rendering library framebuffer for STM32F429-DISCOVERY
 * author: Andreas Mantler (ands)
 */

#ifndef R3DFB_H
#define R3DFB_H

#define R3DFB_PIXEL_WIDTH ((uint16_t) 240)
#define R3DFB_PIXEL_HEIGHT ((uint16_t) 320)

#define R3DFB_COLOR_BUFFER_SIZE (R3DFB_PIXEL_WIDTH * R3DFB_PIXEL_HEIGHT * sizeof(uint16_t))
#define R3DFB_DEPTH_BUFFER_SIZE (R3DFB_PIXEL_WIDTH * R3DFB_PIXEL_HEIGHT * sizeof(uint16_t))

#define R3DFB_BUFFER_OFFSET ((uint32_t) 0x50000)
#define R3DFB_BUFFER0 ((uint32_t) 0xD0000000)
#define R3DFB_BUFFER1 (R3DFB_BUFFER0 + R3DFB_BUFFER_OFFSET)
#define R3DFB_DEPTH_BUFFER (R3DFB_BUFFER1 + R3DFB_BUFFER_OFFSET)

void r3dfb_init(void);
void r3dfb_clear(void); // clears color back buffer and depth buffer
void r3dfb_swap_buffers(void); // swaps back and front color buffers

#endif
