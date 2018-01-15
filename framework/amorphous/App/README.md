# App Module
This directory contains classes related to application instances.

## Roles of primary application classes
platform-agnostic Class inheritance hierarcy of primary application classes.

### ApplicationCore
Declares a minimal set of functions called in the main loop.

### ApplicationBase
Inherits ApplicationCore. Defines StartApp() function which needs to be called
from client code.
Declares Run() as a pure virtual function intended to be implemented by the user.

### GraphicsApplicationBase
Inherits ApplicationBase(). Provides interfaces for simple stateless applications.
Accesses GameWindowManager and creates an application window in the implemented Run() function.

## GameWindowManager classes

### GameWindowManager (base class)
Defines an interface for creating a window, CreateGameWindow()

### Derived classes of GameWindowManager
Each derived class implements the CreateGameWindow() function to create a window using a particular window/graphics library. They also defines a main loop function for each specific window system.

## Application classes and game window manager classes
Application classes access game window manager classes, i.e. the direction of reference is from Application* classes to GameWindowManager* classes,
except that main loop of window classes access interfaces of ApplicationCore
to invoke callback functions.
