#ifndef  __GM_INDEX_FWD_H__
#define  __GM_INDEX_FWD_H__


#include <memory>


namespace amorphous
{

class CGM_InputData;

class CGM_ControlDescBase;
class CGM_ControlDesc;
class CGM_StaticDesc;
class CGM_ButtonDesc;
class CGM_CheckBoxDesc;
class CGM_RadioButtonDesc;
class CGM_SubDialogButtonDesc;
class CGM_DialogCloseButtonDesc;
class CGM_ListBoxDesc;
class CGM_SliderDesc;
class CGM_ScrollBarDesc;

class CGM_ControlBase;
class CGM_Control;
class CGM_Static;
class CGM_Button;
class CGM_CheckBox;
class CGM_RadioButton;
class CGM_SubDialogButton;
class CGM_DialogCloseButton;
class CGM_ListBox;
class CGM_ListBoxItem;
class CGM_Slider;
class CGM_ScrollBar;

class CGM_DialogDesc;

class CGM_Dialog;
class CGM_DialogManager;
class CGM_ControlRendererManager;
class CGM_StdControlRendererManager;

class CGM_Event;
class CGM_EventHandlerBase;
class CGM_ButtonEventHandler;
class CGM_ListBoxEventHandler;

class CGM_ControlRenderer;
class CGM_StdControlRenderer;

class CInputHandler_Dialog;
typedef CInputHandler_Dialog CGM_DialogInputHandler;


typedef std::shared_ptr<CGM_DialogManager> CGM_DialogManagerSharedPtr;
typedef std::shared_ptr<CGM_EventHandlerBase> CGM_DialogEventHandlerSharedPtr;
typedef std::shared_ptr<CGM_ButtonEventHandler> CGM_ButtonEventHandlerSharedPtr;
typedef std::shared_ptr<CGM_ListBoxEventHandler> CGM_ListBoxEventHandlerSharedPtr;
typedef std::shared_ptr<CGM_ControlRenderer> CGM_ControlRendererSharedPtr;
typedef std::shared_ptr<CGM_ControlRendererManager> CGM_ControlRendererManagerSharedPtr;
typedef std::shared_ptr<CGM_DialogInputHandler> CGM_DialogInputHandlerSharedPtr;


class CGM_GlobalSoundPlayer;
class CGM_DialogSoundPlayer;
class CGM_ButtonSoundPlayer;
class CGM_ListBoxSoundPlayer;

typedef std::shared_ptr<CGM_GlobalSoundPlayer>  CGM_GlobalSoundPlayerSharedPtr;
typedef std::shared_ptr<CGM_DialogSoundPlayer>  CGM_DialogSoundPlayerSharedPtr;
typedef std::shared_ptr<CGM_ButtonSoundPlayer>  CGM_ButtonSoundPlayerSharedPtr;
typedef std::shared_ptr<CGM_ListBoxSoundPlayer> CGM_ListBoxSoundPlayerSharedPtr;


// callback function pointer (C style)
typedef void (*PCALLBACK_GM_GUIEVENT) ( CGM_Event& event );

} // namespace amorphous


#endif /* __GM_INDEX_FWD_H__ */
