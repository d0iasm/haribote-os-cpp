// bootpack.h: Common definition
#ifndef _BOOTPACK_H_
#define _BOOTPACK_H_


/* -- dsctbl.c start -- */
#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

// Each GDT size is 8 bytes and there are 8192 segmentations (0~8191)
// GDT is global segment descriptor table
// Set this desctiption address to GDTR resistor
struct SEGMENT_DESCRIPTOR {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

// IDT size is 8 bytes and there are 256 intteruptions (0~255)
// IDT is interrupt descriptor table
struct GATE_DESCRIPTOR {
  short offset_low, selector;
  char dw_count, access_right;
  short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
/* -- dsctbl.c end -- */


/* -- fifo.c start -- */
#define FLAGS_OVERRUN 0x0001

struct FIFO8 {
  unsigned char *buf;
  int p, q;
  int size, free, flags;
};

void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);
/* -- fifo.c end -- */


/* -- graphic.c start -- */
#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
    int pysize, int px0, int py0, char *buf, int bxsize);
/* -- graphic.c end -- */


/* -- int.c start -- */
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

#define PORT_KEYDAT 0x0060

void init_pic(void);
void inthandler27(int *esp);
/* -- int.c end -- */


/* -- keyboard.c start -- */
#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

extern struct FIFO8 keyfifo;

void inthandler21(int *esp);
void wait_KBC_sendready(void);
void init_keyboard(void);
/* -- keyboard.c end -- */


/* -- memory.c start -- */
#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000
#define MEMMAN_FREES 4090 // About 32kB
#define MEMMAN_ADDR 0x003c0000

struct FREEINFO {
  unsigned int addr, size;
};

struct MEMMAN {
  int frees, maxfrees, lostsize, losts;
  struct FREEINFO free[MEMMAN_FREES];
};

unsigned int memtest(unsigned int start, unsigned int end);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
/* -- meomry.c end -- */


/* -- mouse.c start -- */
#define MOUSECMD_ENABLE 0xf4
#define KEYCMD_SENDTO_MOUSE 0xd4

extern struct FIFO8 mousefifo;

struct MOUSE_DEC {
  unsigned char buf[3], phase;
  int x, y, btn;
};

void inthandler2c(int *esp);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);
/* -- mouse.c end -- */


/* -- nasmfunc.asm start --*/
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
int load_cr0(void);
void store_cr0(int cr0);
/* -- nasmfunc.asm end --*/


/* -- nasmhead.asm start --*/
#define ADR_BOOTINFO	0x00000ff0

// A struct size is 9 bytes
struct BOOTINFO { // 0x0ff0 ~ 0x0fff
  char cyls; // The place boot sector read
  char leds; // The statue of keyboard LED
  char vmode; // The number of bits color
  char reserve;
  short scrnx, scrny; // Screen resolution
  char *vram;
};
/* -- nasmhead.asm end --*/


/* -- sheet.c start --*/
#define MAX_SHEETS 256
#define SHEET_USE 1

struct SHEET {
  unsigned char *buf;
  int bxsize, bysize, vx0, vy0, col_inv, height, flags;
  struct SHTCTL *ctl;
};

struct SHTCTL { // sheet control
  unsigned char *vram, *map;
  int xsize, ysize, top;
  struct SHEET *sheets[MAX_SHEETS];
  struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);
/* -- sheet.c end --*/


/* -- tsprintf.c start --*/
int tsprintf(char *str, const char *fmt, ...);
void strcls(char *str);
int figure(int value, int n);
void int2char(char *, int);
void int2hex(char *, int, int);
void int2dec(char *, int);
void int2str(char *, int);
void int2float(char *, int);
/* -- tsprintf.c end --*/

#endif // _BOOTPACK_H
