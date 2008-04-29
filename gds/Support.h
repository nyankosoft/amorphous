

/// helper functions for window-related operations
/// availability: Windows
#include <Support/WindowMisc_Win32.h>

#include <Support/2DArray.h>
//#include <Support/2DGraph.h>
//#include <Support/BMPImageExporter.h>

// 3D camera related operations
// - should be moved to 3DCommon
#include <Support/CameraController_Win32.h>
//#include <Support/CorrelationGraph.h>
#include <Support/DebugOutput.h>

//#include <Support/FileOpenDialog_Win32.h>
//#include <Support/FileOpenDialog_Win32_VC8.h>
//#include <Support/FileSaveDialog_Win32.h>

//#include <Support/fixed_prealloc_pool.h>
#include <Support/FixedStackVector.h>
#include <Support/FixedVector.h>
#include <Support/FloatLookUpTable.h>

/// functions for simple filename and file/directory-related operations
#include <Support/fnop.h>

#include <Support/ImageArchive.h>
//#include <Support/LineSegmentRenderer.h>

/// basic macros
/// - should move this to root directory
#include <Support/Macro.h>

#include <Support/memory_helpers.h>

//#include <Support/MinMax.h>

/// print a message in a message box
/// availability: Windows
#include <Support/msgbox.h>
//#include <Support/namedresourcehandle.h>
//#include <Support/OgreBitwise.h>

#include <Support/ParamLoader.h>

/// used to load values and text data from text
#include <Support/TextFileScanner.h>

#include <Support/prealloc_pool.h>

#include <Support/Profile.h>

/// checked delete functions
#include <Support/SafeDelete.h>
#include <Support/SafeDeleteVector.h>

//#include <Support/Singleton.h>

/// general purpose stream buffers
/// - inherits IArchiveObjectBase (serializable)
#include <Support/stream_buffer.h>

#include <Support/StringAux.h>

#include <Support/Vec3_StringAux.h>


///
/// Log
/// 
#include <Support/Log/DefaultLog.h>


///
/// Serialization
/// 
#include <Support/Serialization/Serialization.h>
#include <Support/Serialization/ArchiveObjectFactory.h>
using namespace GameLib1::Serialization;


/// deprecated headers

//#include <Support/PreAllocDynamicLinkList.h>

//#include <Support/PrecisionTimer.h>
