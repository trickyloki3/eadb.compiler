/*
 *   file: table.h
 *   date: 12/9/2014
 *   auth: trickyloki3
 * github: https://github.com/trickyloki3
 *  email: tricky.loki3@gmail.com
 */
#ifndef TABLE_H
#define TABLE_H
	#include "util.h"
   char * race_tbl(int i);				/* case 'r': return 3;	// Race */
   char * ele_tbl(int i);				/* case 'l': return 4;	// Element */
   char * eff_tbl(int i);				/* case 'e': return 7;	// Effect */
   char * size_tbl(int i);				/* case 's': return 10;	// Size */
   char * class_tbl(int i);         /* case 'j': return x;  // Class */
	char * regen_tbl(int i);			/* case 'g': return 15;	// Regen HP or SP */
	char * spgd_tbl(int i);				/* case 'h': return 17;	// SP Gain or Drain */
	char * castgt_tbl(int i); 			/* case 'v': return 18;	// Cast Self, Enemy */
   char * flagtgt_tbl(int i);			/* case 'b': return 23;	// Flag Bitfield On Self | Random Level */
   char * weapon_tbl(int i);			/* case 'i': return 24;	// Weapon Type */
   char * itemgrp_tbl(int i);
   char * job_tbl(int i);
   char * block_tbl(int i);
#endif
