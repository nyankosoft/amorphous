#ifndef __amorphous_X11_GLWindow_HPP__
#define __amorphous_X11_GLWindow_HPP__


#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>


namespace amorphous
{

class ApplicationCore;


class X11_GLWindow
{
public:
    Display                 *m_Display;
    Window                  m_RootWindow;
    XVisualInfo             *m_Visual;
    Colormap                m_Colormap;
    XSetWindowAttributes    m_swa;
    Window                  m_Window;
    GLXContext              m_GLContext;
    XWindowAttributes       m_gwa;

public:

    X11_GLWindow();

    /**
     * \return 0 on success, -1 on failure
     * 
     */
    int Init( int window_width, int window_height, const char *window_title );

    void MainLoop( ApplicationCore& app );
};

} // namespace amorphous

#endif // __amorphous_X11_GLWindow_HPP__