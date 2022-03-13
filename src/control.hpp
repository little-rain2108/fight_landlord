#pragma once
#ifndef CONTROL
#define CONTROL

#include<windows.h>
#include<conio.h>
#include<iostream>
#include<tchar.h>

const std::string tab[]={"  ","\u2661", "\u2662 ", "\u2663", "\u2664"};
void gotoxy(SHORT,SHORT);
#define Red 0xc
#define Black 0x0
#define White 0xf
void sred();
void sblack();
void cls();
void cp();

#endif