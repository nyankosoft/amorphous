#include "X11_GLWindow.hpp"
#include "ApplicationCore.hpp"
#include <stdio.h>
#include <stdlib.h>

// Building X11 programs
// -------------------------------------------------
// C programs which make use of X11 APIs need to link with GL and X11 libraries.
//
// g++ X11_GLWindow.cpp -lX11 -lGL
//
// X11 libraires, in particular, have to be manually installed on the system first*.
// This means that the installer has to ask the user to run sudo command(s)
// Namely, on Ubuntu,
// sudo apt install xorg-dev
//
//
//
// Without OpenGL or X11 libs on the system, link errors such as the one shown below will occur
// Error caused by a lack of '-lX11' option:
// - undefined reference to 'XOpenDisplay', 'XMapWindow', or 'XCreateWindow',
// Error caused by a lack of '-lGL' option
// - undefined reference to 'glXChooseVisual'


namespace amorphous
{

X11_GLWindow::X11_GLWindow()
:
m_Display(NULL),
m_Visual(NULL),
m_GLContext(NULL)
{}

int X11_GLWindow::Init( int window_width, int window_height, const char *window_title )
{
    if(m_Display != NULL) {
        // Need to release the display first before re-initializing it
    }
    // Argument specifies the hardware display name.
    // Set to NULL, it defaults to the value of the DISPLAY environment variable,
    // which means the graphical output will be sent to the computer on which it is executed.
    m_Display = XOpenDisplay(NULL);
 
    if(m_Display == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        return -1;
    }

    // Obtain the root window, which represents the "desktop background" window.
    m_RootWindow = DefaultRootWindow(m_Display);

    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

    m_Visual = glXChooseVisual(m_Display, 0, att);
    if(m_Visual == NULL) {
        printf("No appropriate visual found.");
        return -1;
    } 
    else {
        // %p creates hexadecimal output like in glxinfo
        printf("Visual %p selected.", (void *)m_Visual->visualid);
    }

    // Create a Colormap for the window:
    m_Colormap = XCreateColormap(m_Display, m_RootWindow, m_Visual->visual, AllocNone);

    m_swa.colormap = m_Colormap;
    m_swa.event_mask = ExposureMask | KeyPressMask; // the window shall respond to Exposure and KeyPress events

    m_Window = XCreateWindow(m_Display, m_RootWindow,
    0, 0, window_width, window_height,
    0, // border_width
    m_Visual->depth,
    InputOutput, // class
    m_Visual->visual,
    CWColormap | CWEventMask, // valuemask. Specifies which window attributes are defined in the attributes argument. 
    &m_swa);

    printf("Created a window. ID: %lu",m_Window);

    // Make the window appear
    int ret = XMapWindow(m_Display, m_Window);

    printf("XMapWindow: %d",ret);

    // Change the string in the title bar:
    XStoreName(m_Display, m_Window, window_title);

    m_GLContext = glXCreateContext(m_Display, m_Visual,
    NULL, // shareList: NULL indicates that no sharing is to take place.
    GL_TRUE // enable direct rendering (true unless sending the graphical output via network)
    );

    if(glXCreateContext == NULL) {
        printf("glXCreateContext failed.");
        return -1;
    }

    ret = glXMakeCurrent(m_Display, m_Window, m_GLContext);

    printf("glXMakeCurrent: %d",ret);

    return 0;
}

void X11_GLWindow::MainLoop( ApplicationCore& app )
{
    XEvent xev;

    while(1) {
        XNextEvent(m_Display, &xev);

        if(xev.type == Expose) {
            XGetWindowAttributes(m_Display, m_Window, &m_gwa);
            //glViewport(0, 0, gwa.width, gwa.height);
            //DrawAQuad();
            app.UpdateFrame();
            glXSwapBuffers(m_Display, m_Window);
        }
                
        else if(xev.type == KeyPress) {
            app.OnKeyDown(xev.xkey.keycode);
        }
        else if(xev.type == KeyRelease) {
            app.OnKeyUp(xev.xkey.keycode);
        }
        else if(xev.type == ButtonPress) {
            app.OnButtonDown(xev.xkey.keycode);
        }
        else if(xev.type == ButtonRelease) {
            app.OnButtonUp(xev.xkey.keycode);
        }
        else if(app.IsAppExitRequested()) {
            glXMakeCurrent(m_Display, None, NULL);
            glXDestroyContext(m_Display, m_GLContext);
            XDestroyWindow(m_Display, m_Window);
            XCloseDisplay(m_Display);
            break;
        }
    }
}

} // namespace amorphous
