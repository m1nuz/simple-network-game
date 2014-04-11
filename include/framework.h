#pragma once

#ifdef _WIN32

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>

#include <gl/gl.h>
#include <gl/glu.h>

#endif

#define MBUTTON0    0x01
#define MBUTTON1    0x02
#define MBUTTON2    0x04
#define MBUTTON3    0x08
#define MBUTTON4    0x10

int openDisplay(const char *title, int width, int height, int fullscreen, int cursor);
void updateDisplay();
void closeDisplay();
void setTitle(const char *title);

float time();

void* createThread(void (*func)(void *), void *user);
void destroyThread(void* thread);

extern void onQuit();
extern void onKeyPress(unsigned char key);
extern void onKeyRelease(unsigned char key);
extern void onMousePress(int x, int y, unsigned char buttons);
extern void onMouseRelease(int x, int y, unsigned char buttons);
extern void onMouseMove(int x, int y, unsigned char buttons);
