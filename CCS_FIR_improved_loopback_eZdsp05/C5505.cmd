/****************************************************************************/
/*  C5505.cmd                                                               */
/*  Copyright (c) 2010  Texas Instruments Incorporated                      */
/*  Authored by: Rafael de Souza  											*/
/*  Modified by: Jose R. Sanchez											*/
/*           on: January 13, 2014                                           */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on a C5505.              */
/*                 Use it as a guideline.  You will want to                 */
/*                 change the memory layout to match your specific          */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

-stack    0x2000      /* Primary stack size   */
-sysstack 0x1000      /* Secondary stack size */
-heap     0x2000      /* Heap area size       */

-c                    /* Use C linking conventions: auto-init vars at runtime */
-u _Reset             /* Force load of reset interrupt handler                */

MEMORY
{
    MMR:     o = 0x000000  l = 0x0000c0  /* 192B Memory Mapped Registers */
    DARAM:   o = 0x0000C0  l = 0x00FF40  /* On Chip Dual Access RAM */
    SARAM0:  o = 0x010000  l = 0x010000  /* On Chip SARAM - Bank 1 ( 64 kB) */
	SARAM1:  o = 0x020000  l = 0x020000  /* On Chip SARAM - Bank 2 (128 kB)*/
	SARAM2:  o = 0x040000  l = 0x00FE00  /* On Chip SARAM - Bank 3 ( 64 kB)*/

  	CS0:     o = 0x050000  l = 0x7B0000  /* 8MB CS0 external memory space */
    CS2:     o = 0x800000  l = 0x400000  /* 4MB CS2 external memory space */
    CS3:     o = 0xC00000  l = 0x200000  /* 2MB CS3 external memory space */
    CS4:     o = 0xE00000  l = 0x100000  /* 1MB CS4 external memory space */
    CS5:     o = 0xF00000  l = 0x0E0000  /* 1MB CS5 external memory space */
    ROM:     o = 0xFE0000  l = 0x01FF00  /* 128kB ROM (MPNMC=0) or CS5 (MPNMC=1) */
    VECS:    o = 0xFFFF00  l = 0x000100  /* reset vector */
}

SECTIONS
{
    vectors (NOLOAD) >  VECS  /* If MPNMC = 1, remove the NOLOAD directive */
    vector		   >  DARAM	 ALIGN = 256
    .cinit         >  DARAM
    .text          >  SARAM0
    .stack         >  DARAM
    .sysstack      >  DARAM
    .sysmem        >  DARAM
    .data          >  DARAM
    .cio           >  DARAM
    .bss           >  DARAM
	.usect		   >  DARAM
	.switch		   >  SARAM2
	.cinit         >  SARAM2
	.pinit         >  SARAM2
	.cio           >  SARAM2
	.args          >  SARAM2
	.const		   >  DARAM
	expdata0	   >  SARAM1
	expdata1	   >  SARAM2

}
