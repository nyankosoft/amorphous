#ifndef  __DEBUGWINDOW_H__
#define  __DEBUGWINDOW_H__

class CFont;
class CTextureFont;

//extern void Render();

//extern void DrawDebugInfo();
extern int g_iShowDebugInfo;

/**
 * 0: no debug info
 * 1: player position, velocity, etc.
 * 2: profile result
 * 3: log
 */
#define NUM_DEBUG_DISPLAYS	4;

extern CFont g_Font;
extern CTextureFont g_TexFont;

#endif  /*  __DEBUGWINDOW_H__  */