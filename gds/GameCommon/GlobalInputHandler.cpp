#include "GlobalInputHandler.hpp"

#include "App/GameWindowManager.hpp"
#include "App/GameApplicationBase.hpp"
#include "Task/GameTaskManager.hpp"
#include "Task/GameTask.hpp"

#include "Graphics/GraphicsresourceManager.hpp"
#include "Graphics/TextureRenderTarget.hpp"

#include "Support/DebugOutput.hpp"


namespace amorphous
{

using std::string;



#include <boost/filesystem.hpp>


inline int GetCurrentCountOfCounterFileAndIncrementCount( const std::string& counter_file_path, int max_count = 1000 )
{
	using namespace boost::filesystem;

	int current_index = 0, next_index;

	FILE *fp = fopen( counter_file_path.c_str(), "rb" );

	if(!fp)
	{
		// the file which records screenshot count is missing
		// either the application is running for the first time or the record file has been deleted
		fp = fopen( counter_file_path.c_str(), "wb" );
		if( !fp )
		{
			// probably the directory 'screenshots' does not exist
			create_directories( path(counter_file_path).parent_path() );
			fp = fopen( counter_file_path.c_str(), "wb" );
			if(!fp)
				return -1;
		}
		next_index = 1;
		fwrite( &next_index, sizeof(int), 1, fp );
		fclose(fp);
		return 0;
	}

	fread( &current_index, sizeof(int), 1, fp );
	fclose(fp);

	// Write out the new counter file

	fp = fopen( counter_file_path.c_str(), "wb" );
	if( !fp )
		return -1;

	next_index = current_index + 1;
	fwrite( &next_index, sizeof(int), 1, fp );
	fclose(fp);

	if( max_count <= current_index )
		current_index %= max_count;

	return current_index;
}


CGlobalInputHandler::CGlobalInputHandler()
{
}


CGlobalInputHandler::~CGlobalInputHandler()
{
}


void CGlobalInputHandler::ProcessInput(SInputData& input)
{
	switch( input.iGICode )
	{
	case GIC_ESC:
/*		if( input.iType == ITYPE_KEY_PRESSED )
		{
			g_bAppExitRequested = true;
		}*/
//		else if( input.iType == ITYPE_KEY_RELEASED ) {}
		break;

	case GIC_F1:
		if( input.iType == ITYPE_KEY_PRESSED )
			DebugOutput.NextDebugItem();
		break;

	case GIC_F2:
		if( input.iType == ITYPE_KEY_PRESSED )
            DebugOutput.ToggleDisplay();
		break;

	case GIC_F4:
		break;

	case GIC_F5:
		if( input.iType == ITYPE_KEY_PRESSED )
			GraphicsResourceManager().Refresh();
		break;

	case GIC_F7:	// quick load
		break;

	case GIC_F9:
		break;

	case GIC_F10:	// change screen size (to a smaller one)
		break;

	case GIC_F11:	// change screen size (to a larger one)
		break;

	case GIC_F12:
		if( input.iType == ITYPE_KEY_PRESSED )
		{
			boost::shared_ptr<CTextureRenderTarget> pTextureRenderTarget = CTextureRenderTarget::Create();
			int screen_width  = GameWindowManager().GetScreenWidth();
			int screen_height = GameWindowManager().GetScreenHeight();
			pTextureRenderTarget->Init( screen_width, screen_height );
			pTextureRenderTarget->SetRenderTarget();

			if( g_pGameAppBase
			 && g_pGameAppBase->GetTaskManager()
			 && g_pGameAppBase->GetTaskManager()->GetCurrentTask() )
			{
				g_pGameAppBase->GetTaskManager()->GetCurrentTask()->Render();
			}

			pTextureRenderTarget->ResetRenderTarget();

			int count = GetCurrentCountOfCounterFileAndIncrementCount( "./Screenshots/ssr.dat" );

			string img_ext = "png";
			string img_file_path = fmt_string( "./Screenshots/image%04d", count ) + "." + img_ext;
			pTextureRenderTarget->GetRenderTargetTexture().SaveTextureToImageFile( img_file_path );
		}
		break;

	default:
		break;	// no action is assigned to this input
	}
}


} // namespace amorphous
