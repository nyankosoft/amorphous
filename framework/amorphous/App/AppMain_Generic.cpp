#include "ApplicationBase.hpp"


// Define you own application class in your source code
// and define a function named CreateApplicationInstance()
// which returns an instance of the class

//ApplicationBase *CreateApplicationInstance() {
//	return new MyApplication;
//}


int main(int argc, char *argv[]) {

	// The application enters the main loop inside StartApp().
	// In other words, the thread does not return from StartApp()
	// until an exit is requested somehow. 
	amorphous::ApplicationBase::StartApp();

	return 0;
}
