

/// helper functions for window-related operations
/// availability: Windows
#include <Support/WindowMisc_Win32.hpp>

#include <Support/2DArray.hpp>
#include <Support/BitmapImage.hpp>

// 3D camera related operations
// - should be moved to 3DCommon
#include <Support/CameraController_Win32.hpp>
#include <Support/DebugOutput.hpp>

//#include <Support/2DGraph.hpp>
//#include <Support/CorrelationGraph.hpp>

//#include <Support/FileOpenDialog_Win32.hpp>
//#include <Support/FileOpenDialog_Win32_VC8.hpp>
//#include <Support/FileSaveDialog_Win32.hpp>

//#include <Support/fixed_prealloc_pool.hpp>
#include <Support/FixedStackVector.hpp>
#include <Support/FixedVector.hpp>
#include <Support/FloatLookUpTable.hpp>

/// functions for simple filename and file/directory-related operations
#include <Support/lfs.hpp>

#include <Support/ImageArchive.hpp>

#include <Support/Timer.hpp>

/// basic macros
/// - should move this to root directory
#include <Support/Macro.h>

#include <Support/memory_helpers.hpp>

//#include <Support/MinMax.h>

/// print a message in a message box
/// availability: Windows
#include <Support/msgbox.hpp>
//#include <Support/namedresourcehandle.h>

#include <Support/ParamLoader.hpp>

/// used to load values and text data from text
#include <Support/TextFileScanner.hpp>

#include <Support/prealloc_pool.hpp>

#include <Support/Profile.hpp>

/// checked delete functions
#include <Support/SafeDelete.hpp>
#include <Support/SafeDeleteVector.hpp>

//#include <Support/Singleton.hpp>

/// general purpose stream buffers
/// - inherits IArchiveObjectBase (serializable)
#include <Support/stream_buffer.hpp>

#include <Support/StringAux.hpp>

#include <Support/Vec3_StringAux.hpp>


///
/// Log
/// 
#include <Support/Log/DefaultLog.hpp>


///
/// Serialization
/// 
#include <Support/Serialization/Serialization.hpp>
#include <Support/Serialization/ArchiveObjectFactory.hpp>
using namespace GameLib1::Serialization;


/// deprecated headers

//#include <Support/PreAllocDynamicLinkList.hpp>

//#include <Support/PrecisionTimer.hpp>
