#ifndef __amorphous_GLHeaders_H__
#define __amorphous_GLHeaders_H__

#ifdef __ANDROID__

#include <EGL/egl.h>
#include <GLES2/gl2.h>

// Stub functions and macros to compile the source code with GLES APIs.
inline void glLoadMatrixf(const GLfloat * m) {}
inline void glShadeModel(GLenum mode) {}
inline void glAlphaFunc(GLenum func, GLclampf ref) {}
inline void glMatrixMode(GLenum mode) {}
inline void glClipPlane(GLenum plane, const double *equation) {}
inline void glBegin(GLenum mode) {}
inline void glEnd() {}
inline void glVertex3f(float x,float y,float z) {}
inline void glColor4f(float r,float g,float b,float a) {}
inline void glFogi(GLenum pname,GLint param) {}
inline void glFogf(GLenum pname,GLfloat param) {}
inline void glFogfv(GLenum pname,const GLfloat * params) {}
inline void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels) {}
inline void glClearDepth(float f) {glClearDepthf(f);} // (Non-ES) OpenGL only supports glClearDepthf() when it's 4.1 or higher.
inline void glLightfv(GLenum light,GLenum pname,const GLfloat * params) {}
inline void glDrawRangeElements(GLenum mode,GLuint start,GLuint end,GLsizei count,GLenum type,const GLvoid *indices) {}

#define GL_SMOOTH 0
#define GL_STACK_OVERFLOW 1
#define GL_STACK_UNDERFLOW 2
#define GL_MODELVIEW 0
#define GL_MODELVIEW_MATRIX 0
#define GL_PROJECTION 0
#define GL_CLAMP 0
#define GL_PRIMARY_COLOR 0
#define GL_MODULATE 0
#define GL_PREVIOUS 0
#define GL_ALPHA_TEST 0
#define GL_LIGHTING 0
#define GL_CLIP_PLANE0 0
#define GL_DIFFUSE 0
#define GL_PROXY_TEXTURE_2D 0
#define GL_TEXTURE_WRAP_R 0
#define GL_FOG 0
#define GL_FOG_MODE 0
#define GL_EXP 0
#define GL_EXP2 0
#define GL_FOG_COLOR 0
#define GL_FOG_START 0
#define GL_FOG_END 0
#define GL_FOG_DENSITY 0
#define GL_FOG_HINT 0
#define GL_GEOMETRY_SHADER 0
#define GL_BGRA 0
#define GL_RGBA16F 0
#define GL_R32F 0

#else /* __ANDROID__ */

// Comented out; do not include gl/gl.h when glew is used.
// Including gl/gl.h before GL/glew.h causes an error,
// with glew complaining 'gl.h' was included before glew.h.
//#include <gl/gl.h>			// Header File For The OpenGL32 Library

#include "GL/glew.h"

#endif /* __ANDROID__ */

#endif // __amorphous_GLHeaders_H__
