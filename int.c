// int.c: Setting for PIC to send interrupt request
#include "bootpack.h"
#include <stdio.h>

void init_pic(void)
{
  io_out8(PIC0_IMR, 0xff); // Deny all interrupt requests
  io_out8(PIC1_IMR, 0xff); // Deny all interrupt requests

  io_out8(PIC0_ICW1, 0x11);   // Edge triger mode
  io_out8(PIC0_ICW2, 0x20);   // IPQ-7 recieves by INT20-27
  io_out8(PIC0_ICW3, 1 << 2); // PIC1 connects IRQ2
  io_out8(PIC0_ICW4, 0x01);   // Non buffer mode

  io_out8(PIC1_ICW1, 0x11); // Edge triger mode
  io_out8(PIC1_ICW2, 0x28); // IRQ8-15 recieves by INT28-2f
  io_out8(PIC1_ICW3, 2);    // PIC1 connects IEQ2
  io_out8(PIC1_ICW4, 0x01); // Non buffer mode

  io_out8(PIC0_IMR, 0xfb); // 11111011 Deny all except PIC1
  io_out8(PIC1_IMR, 0xff); // 11111111 Deny all interrupt requests

  return;
}

void inthandler27(int* esp)
{
  io_out8(PIC0_OCW2, 0x67);
  return;
}
