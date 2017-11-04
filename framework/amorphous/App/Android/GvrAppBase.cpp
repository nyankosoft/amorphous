#include "GvrAppBase.hpp"
#include "amorphous/Support/Log/DefaultLogAux.hpp"
#include "amorphous/Graphics/GraphicsDevice.hpp"
#include "amorphous/Graphics/OpenGL/GLInitialization.hpp"
#include "amorphous/Graphics/OpenGL/GLGraphicsDevice.hpp"
#include "amorphous/Graphics/Shader/ShaderManagerHub.hpp"
//#include <native_activity.h>
#include <jni.h>
//#include <unistd.h> // chdir()
#include <sys/param.h> // MAXPATHLEN
// #include <ctime>
// #include <iomanip>

// To call Java methods when running native code inside an Android activity,
// a reference is needed to the JavaVM.
JavaVM *gJavaVM = nullptr;
bool gDrawCube = false;
bool gDrawFloor = false;

namespace amorphous
{

using namespace std;

int GetExtStoragePath(std::string& ext_storage_path) {
    
    if(gJavaVM == nullptr) {
        return -1;
    }
    
    // Make JNI calls to get the external storage directory, and cd to it.

    // To begin, get a reference to the env and attach to it.
    JNIEnv *env = nullptr;
    int isAttached = 0;
    int ret = 0;
    jthrowable exception;
    if ((gJavaVM->GetEnv( (void**)&env, JNI_VERSION_1_6) ) < 0) {
        // Couldn't get JNI environment, so this thread is native.
        if ((gJavaVM->AttachCurrentThread( &env, NULL )) < 0) {
            fprintf(stderr, "Error: Couldn't attach to Java VM.\n");
            return (-1);
        }
        isAttached = 1;
    }

    if(env == nullptr) {
        return -1;
    }

    // Get File object for the external storage directory.
    jclass classEnvironment = env->FindClass("android/os/Environment");
    if (!classEnvironment)
        return -1;
        
    jmethodID methodIDgetExternalStorageDirectory = env->GetStaticMethodID( classEnvironment, "getExternalStorageDirectory", "()Ljava/io/File;"); // public static File getExternalStorageDirectory ()
    if (!methodIDgetExternalStorageDirectory)
        return -1;

    jobject objectFile = env->CallStaticObjectMethod( classEnvironment, methodIDgetExternalStorageDirectory);
    exception = env->ExceptionOccurred();
    if (exception) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    
    // Call method on File object to retrieve String object.
    jclass classFile = env->GetObjectClass(objectFile);
    if (!classFile)
        return -1;

    jmethodID methodIDgetAbsolutePath = env->GetMethodID( classFile, "getAbsolutePath", "()Ljava/lang/String;" );
    if (!methodIDgetAbsolutePath)
        return -1;

    // The thread reaches here
    
        jstring stringPath = (jstring)env->CallObjectMethod( objectFile, methodIDgetAbsolutePath );
//        jstring stringPath = env->CallObjectMethod( classFile, methodIDgetAbsolutePath ); // Same error
    exception = env->ExceptionOccurred();
    if (exception) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }

    if(stringPath == nullptr) {
        return -1;
    }

    gDrawFloor = true;
    
    // Extract a C string from the String object, and chdir() to it.
    const char *wpath3 = env->GetStringUTFChars( stringPath, NULL );
    if(wpath3 == nullptr) {
        return -1;
    }

    // The thread reaches here, so all the functions related to get the storage path
    // seem to be working.
    
    ext_storage_path = wpath3;
    // if (chdir(wpath3) != 0) {
    //     fprintf(stderr, "Error: Unable to change working directory to %s.\n", wpath3);
    //     perror(NULL);
    // } else if (path) {
    //     if (chdir(path) != 0) {
    //         fprintf(stderr, "Error: Unable to change working directory to %s.\n", path);
    //         perror(NULL);
    //     }
    // }

    env->ReleaseStringUTFChars( stringPath, wpath3 );

    //goto retAndroid;
    
//bailAndroid:
  //  ARLOGe("Error: JNI call failure.\n");
//    ret = -1;
//retAndroid:
    if (isAttached) gJavaVM->DetachCurrentThread(); // Clean up.

    return (ret);
}

int GvrAppBase::InitBase()
{
    Result::Name res = InitializeOpenGLClasses();

	// Neither screen size nor screen mode is used in Init(), so we just pass random values here.
	int w = 1280;
	int h = 720;
	GraphicsDevice().Init( w, h, ScreenMode::FULLSCREEN );

    //std::string logfile_pathname = "/data/local/tmp/log.txt"; // No success
    //std::string logfile_pathname = "/storage/emulated/0/data/local/tmp/log.txt"; // No success
    //std::string logfile_pathname = "/mnt/sdcard/log.txt";
    std::string logfile_pathname = "/storage/emulated/0/Download/vr.log";
    int ret = InitTextlLog(logfile_pathname);

    // std::time_t t = std::time(nullptr);
//    std::string local_time = std::put_time(std::localtime(&t), "%c %Z");

    // std::tm tmobj = std::localtime(&t);

    std::string pathname;
    GetExtStoragePath(pathname);

    //string filepath = pathname + "/mytextfile.txt";
    //string filepath = "/mnt/sdcard/mytextfile.txt"; // No success
    //string filepath = "/sdcard/mytextfile.txt"; // No success
    string filepath = "/storage/emulated/0/Download/mytextfile.txt";
    FILE *fp = fopen(filepath.c_str(),"w");
    if(fp) {
        //gDrawCube = true;
        fprintf(fp,"sayhellotonyanko\n");
        fclose(fp);
    }

    // Can't open an existing file for reading either.
    // Is it an incorrect directory path?
    FILE *rfp = fopen("/storage/emulated/0/data/local/nyanko.txt","r");
    if(rfp) {
        fclose(rfp);
    }

    //LOG_PRINTF((internalDataPath));

    LOG_PRINTF(("logging"));
    return 0;
}

// inline Matrix44 GLArrayToMatrix44(const std::array<float, 16>& src)
// {
//     Matrix44 out;
//     out(0,0) = src[0];
//     out(1,0) = src[1];
//     out(2,0) = src[2];
//     out(3,0) = src[3];
// }

void GvrAppBase::RenderBase()
{
    LOG_GL_ERROR( "Clearing OpenGL errors..." );

    GetShaderManagerHub().PushViewAndProjectionMatrices( m_ViewTransform, m_ProjectionTransform );

    Render();

    GetShaderManagerHub().PopViewAndProjectionMatrices_NoRestore();
}


void GvrAppBase::SetViewTransform(const std::array<float, 16>& view)
{
    m_ViewTransform.SetData(&view[0]);
}


void GvrAppBase::SetProjectionTransform(const std::array<float, 16>& proj)
{
    // Camera class takes view transform information only in the form of
    // viewer position and orientation,
    //m_Camera.SetViewTransform(...);

    // So this time we simply store the matrix
    m_ProjectionTransform.SetData(&proj[0]);
}


} // namespace amorphous
