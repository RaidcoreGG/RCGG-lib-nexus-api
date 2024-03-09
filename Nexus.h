#ifndef NEXUS_H
#define NEXUS_H

#include <windows.h>
#include <d3d11.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#define NEXUS_API_VERSION 2

typedef enum ERenderType
{
	ERenderType_PreRender,
	ERenderType_Render,
	ERenderType_PostRender,
	ERenderType_OptionsRender
} ERenderType;

///----------------------------------------------------------------------------------------------------
/// GUI_RENDER:
/// 	Render callback signature.
///----------------------------------------------------------------------------------------------------
typedef void (*GUI_RENDER) (void);
///----------------------------------------------------------------------------------------------------
/// GUI_ADDRENDER:
/// 	Registers a render callback, ERenderType is either Pre, Present, Post or Options,
///		callback should be of void func().
///----------------------------------------------------------------------------------------------------
typedef void (*GUI_ADDRENDER) (ERenderType aRenderType, GUI_RENDER aRenderCallback);

///----------------------------------------------------------------------------------------------------
/// GUI_REMRENDER:
/// 	Removes the registered render callback that is passed.
///----------------------------------------------------------------------------------------------------
typedef void (*GUI_REMRENDER) (GUI_RENDER aRenderCallback);

///----------------------------------------------------------------------------------------------------
/// PATHS_GETGAMEDIR:
/// 	Returns the path to the game directory. E.g. "C:\Program Files\Guild Wars 2\"
///----------------------------------------------------------------------------------------------------
typedef const char* (*PATHS_GETGAMEDIR)(void);
///----------------------------------------------------------------------------------------------------
/// PATHS_GETADDONDIR:
/// 	Returns a path to /addons/{aName} for addons to use.
///		Passing nullptr or empty string "" returns /addons without trailing slash.
///----------------------------------------------------------------------------------------------------
typedef const char* (*PATHS_GETADDONDIR)(const char* aName);
///----------------------------------------------------------------------------------------------------
/// PATHS_GETCOMMONDIR:
///		Returns the path to the common addon folder.
/// 	Synonymous to PATHS_GETADDONDIR("common").
///----------------------------------------------------------------------------------------------------
typedef const char* (*PATHS_GETCOMMONDIR)(void);

typedef enum EMHStatus
{
	MH_UNKNOWN = -1,
	MH_OK = 0,
	MH_ERROR_ALREADY_INITIALIZED,
	MH_ERROR_NOT_INITIALIZED,
	MH_ERROR_ALREADY_CREATED,
	MH_ERROR_NOT_CREATED,
	MH_ERROR_ENABLED,
	MH_ERROR_DISABLED,
	MH_ERROR_NOT_EXECUTABLE,
	MH_ERROR_UNSUPPORTED_FUNCTION,
	MH_ERROR_MEMORY_ALLOC,
	MH_ERROR_MEMORY_PROTECT,
	MH_ERROR_MODULE_NOT_FOUND,
	MH_ERROR_FUNCTION_NOT_FOUND
} EMHStatus;

typedef EMHStatus (__stdcall* MINHOOK_CREATE)(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
typedef EMHStatus (__stdcall* MINHOOK_REMOVE)(LPVOID pTarget);
typedef EMHStatus (__stdcall* MINHOOK_ENABLE)(LPVOID pTarget);
typedef EMHStatus (__stdcall* MINHOOK_DISABLE)(LPVOID pTarget);

typedef enum ELogLevel
{
	ELogLevel_OFF         = 0,
	ELogLevel_CRITICAL    = 1,
	ELogLevel_WARNING     = 2,
	ELogLevel_INFO        = 3,
	ELogLevel_DEBUG       = 4,
	ELogLevel_TRACE       = 5,
	ELogLevel_ALL
} ELogLevel;

///----------------------------------------------------------------------------------------------------
/// LOGGER_LOG2:
/// 	Logs a message to the file and ingame window.
///		Supports custom coloring for addon window messages like: <c=#FF0000>This text is Red</c>.
///----------------------------------------------------------------------------------------------------
typedef void (*LOGGER_LOG2)(ELogLevel aLogLevel, const char* aChannel, const char* aStr);

///----------------------------------------------------------------------------------------------------
/// EVENT_CONSUME:
/// 	Event consume callback signature.
///----------------------------------------------------------------------------------------------------
typedef void (*EVENT_CONSUME)(void* aEventArgs);
///----------------------------------------------------------------------------------------------------
/// EVENTS_RAISE:
/// 	Raises an event to all subscribing addons.
///		aEventData is a pointer to the data you want to share or nullptr.
///		Addons are responsible for knowing how to interpret this data.
///----------------------------------------------------------------------------------------------------
typedef void (*EVENTS_RAISE)(const char* aIdentifier, void* aEventData);
///----------------------------------------------------------------------------------------------------
/// EVENT_RAISENOTIFICATION:
/// 	Raises an event without a payload.
///		Synonymous with EVENTS_RAISE("EV_FOO", nullptr);
///----------------------------------------------------------------------------------------------------
typedef void (*EVENTS_RAISENOTIFICATION)(const char* aIdentifier);
///----------------------------------------------------------------------------------------------------
/// EVENTS_SUBSCRIBE:
/// 	Registers/Deregisters an event callback. Signature is the same for both.
///----------------------------------------------------------------------------------------------------
typedef void (*EVENTS_SUBSCRIBE)(const char* aIdentifier, EVENT_CONSUME aConsumeEventCallback);

///----------------------------------------------------------------------------------------------------
/// WNDPROC_CALLBACK:
/// 	Slightly different WndProc signature.
///		Return 0 if your addon handled it and you don't want it to be passed to the game.
///----------------------------------------------------------------------------------------------------
typedef UINT (*WNDPROC_CALLBACK)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
///----------------------------------------------------------------------------------------------------
/// WNDPROC_ADDREM:
/// 	Registers/Deregisters a WndProc callback.
///----------------------------------------------------------------------------------------------------
typedef void (*WNDPROC_ADDREM)(WNDPROC_CALLBACK aWndProcCallback);
///----------------------------------------------------------------------------------------------------
/// WNDPROC_SENDTOGAME:
/// 	Sends a WndProc to the game only and bypasses all other hooks.
///----------------------------------------------------------------------------------------------------
typedef LRESULT	(*WNDPROC_SENDTOGAME)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct Keybind
{
	unsigned short	Key;
	bool			Alt;
	bool			Ctrl;
	bool			Shift;
} Keybind;

///----------------------------------------------------------------------------------------------------
/// KEYBINDS_PROCESS:
/// 	KeybindHandler callback signature.
///----------------------------------------------------------------------------------------------------
typedef void (*KEYBINDS_PROCESS)(const char* aIdentifier);
///----------------------------------------------------------------------------------------------------
/// KEYBINDS_REGISTERWITHSTRING:
/// 	Registers a KeybindHandler callback for a given named keybind.
///		aKeybind is the default if not yet defined. Use as "ALT+CTRL+SHIFT+Q", "ALT+SHIFT+T", etc.
///----------------------------------------------------------------------------------------------------
typedef void (*KEYBINDS_REGISTERWITHSTRING)(const char* aIdentifier, KEYBINDS_PROCESS aKeybindHandler, const char* aKeybind);
///----------------------------------------------------------------------------------------------------
/// KEYBINDS_REGISTERWITHSTRUCT:
/// 	Same as KEYBINDS_REGISTERWITHSTRING except you pass a Nexus Keybind struct as a default bind.
///----------------------------------------------------------------------------------------------------
typedef void (*KEYBINDS_REGISTERWITHSTRUCT)(const char* aIdentifier, KEYBINDS_PROCESS aKeybindHandler, Keybind aKeybind);
///----------------------------------------------------------------------------------------------------
/// KEYBINDS_DEREGISTER:
/// 	Deregisters a KeybindHandler callback.
///----------------------------------------------------------------------------------------------------
typedef void (*KEYBINDS_DEREGISTER)(const char* aIdentifier);

///----------------------------------------------------------------------------------------------------
/// DATALINK_GETRESOURCE:
/// 	Returns a pointer to the requested resource or nullptr if not existing.
///----------------------------------------------------------------------------------------------------
typedef void* (*DATALINK_GETRESOURCE)(const char* aIdentifier);
///----------------------------------------------------------------------------------------------------
/// DATALINK_SHARERESOURCE:
/// 	Allocates a shared resource of given size and returns a pointer to it for writing.
///----------------------------------------------------------------------------------------------------
typedef void* (*DATALINK_SHARERESOURCE)(const char* aIdentifier, size_t aResourceSize);

typedef struct Texture
{
	unsigned Width;
	unsigned Height;
	ID3D11ShaderResourceView* Resource;
} Texture;

///----------------------------------------------------------------------------------------------------
/// TEXTURES_RECEIVECALLBACK:
/// 	TextureReceiver callback signature.
///----------------------------------------------------------------------------------------------------
typedef void		(*TEXTURES_RECEIVECALLBACK)(const char* aIdentifier, Texture* aTexture);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_GET:
/// 	Returns a Texture* or nullptr if it doesn't exist.
///----------------------------------------------------------------------------------------------------
typedef Texture*	(*TEXTURES_GET)(const char* aIdentifier);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_GETORCREATEFROMFILE:
/// 	Returns a Texture* or if it doesn't exist yet creates it from file.
///----------------------------------------------------------------------------------------------------
typedef Texture*	(*TEXTURES_GETORCREATEFROMFILE)(const char* aIdentifier, const char* aFilename);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_GETORCREATEFROMRESOURCE:
/// 	Returns a Texture* or if it doesn't exist yet creates it from resource.
///----------------------------------------------------------------------------------------------------
typedef Texture*	(*TEXTURES_GETORCREATEFROMRESOURCE)(const char* aIdentifier, unsigned aResourceID, HMODULE aModule);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_GETORCREATEFROMURL:
/// 	Returns a Texture* or if it doesn't exist yet creates it from URL.
///----------------------------------------------------------------------------------------------------
typedef Texture*	(*TEXTURES_GETORCREATEFROMURL)(const char* aIdentifier, const char* aRemote, const char* aEndpoint);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_GETORCREATEFROMMEMORY:
/// 	Returns a Texture* or if it doesn't exist yet creates it from memory.
///----------------------------------------------------------------------------------------------------
typedef Texture*	(*TEXTURES_GETORCREATEFROMMEMORY)(const char* aIdentifier, void* aData, size_t aSize);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_LOADFROMFILE:
/// 	Creates a texture from file and passes it to the TextureReceiver callback when finished.
///----------------------------------------------------------------------------------------------------
typedef void		(*TEXTURES_LOADFROMFILE)(const char* aIdentifier, const char* aFilename, TEXTURES_RECEIVECALLBACK aCallback);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_LOADFROMRESOURCE:
/// 	Creates a texture from resource and passes it to the TextureReceiver callback when finished.
///----------------------------------------------------------------------------------------------------
typedef void		(*TEXTURES_LOADFROMRESOURCE)(const char* aIdentifier, unsigned aResourceID, HMODULE aModule, TEXTURES_RECEIVECALLBACK aCallback);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_LOADFROMURL:
/// 	Creates a texture from URL and passes it to the TextureReceiver callback when finished.
///----------------------------------------------------------------------------------------------------
typedef void		(*TEXTURES_LOADFROMURL)(const char* aIdentifier, const char* aRemote, const char* aEndpoint, TEXTURES_RECEIVECALLBACK aCallback);
///----------------------------------------------------------------------------------------------------
/// TEXTURES_LOADFROMMEMORY:
/// 	Creates a texture from memory and passes it to the TextureReceiver callback when finished.
///----------------------------------------------------------------------------------------------------
typedef void		(*TEXTURES_LOADFROMMEMORY)(const char* aIdentifier, void* aData, size_t aSize, TEXTURES_RECEIVECALLBACK aCallback);

///----------------------------------------------------------------------------------------------------
/// QUICKACCESS_ADDSHORTCUT:
/// 	Adds a shortcut icon to the QuickAccess with given texture identifiers.
///		When clicked aKeybindIdentifier will be invoked.
///----------------------------------------------------------------------------------------------------
typedef void (*QUICKACCESS_ADDSHORTCUT)	(const char* aIdentifier, const char* aTextureIdentifier, const char* aTextureHoverIdentifier, const char* aKeybindIdentifier, const char* aTooltipText);
///----------------------------------------------------------------------------------------------------
/// QUICKACCESS_ADDSIMPLE:
/// 	Appends ImGui callback when right-clicking the Nexus icon.
///----------------------------------------------------------------------------------------------------
typedef void (*QUICKACCESS_ADDSIMPLE)	(const char* aIdentifier, GUI_RENDER aShortcutRenderCallback);
///----------------------------------------------------------------------------------------------------
/// QUICKACCESS_GENERIC:
/// 	Generic function signature for deregistering shortcuts.
///		This signature is also used to send a notification icon to a given shortcut.
///----------------------------------------------------------------------------------------------------
typedef void (*QUICKACCESS_GENERIC)		(const char* aIdentifier);

///----------------------------------------------------------------------------------------------------
/// LOCALIZATION_TRANSLATE:
/// 	Translates aIdentifier into current active language or returns aIdentifier if not available.
///----------------------------------------------------------------------------------------------------
typedef const char* (*LOCALIZATION_TRANSLATE)(const char* aIdentifier);
///----------------------------------------------------------------------------------------------------
/// LOCALIZATION_TRANSLATETO:
/// 	Same as LOCALIZATION_TRANSLATE except you can pass which language you want to translate to.
///----------------------------------------------------------------------------------------------------
typedef const char* (*LOCALIZATION_TRANSLATETO)(const char* aIdentifier, const char* aLanguageIdentifier);

typedef struct NexusLinkData
{
	unsigned	Width;
	unsigned	Height;
	float		Scaling;

	bool		IsMoving;
	bool		IsCameraMoving;
	bool		IsGameplay;

	void*		Font;		// ImFont*
	void*		FontBig;	// ImFont*
	void*		FontUI;		// ImFont*
} NexusLinkData;

typedef struct AddonAPI
{
	/* Renderer */
	IDXGISwapChain*						SwapChain;
	void*								ImguiContext; // ImGuiContext*
	void*								ImguiMalloc;
	void*								ImguiFree;
	GUI_ADDRENDER						RegisterRender;
	GUI_REMRENDER						DeregisterRender;

	/* Paths */
	PATHS_GETGAMEDIR					GetGameDirectory;
	PATHS_GETADDONDIR					GetAddonDirectory;
	PATHS_GETCOMMONDIR					GetCommonDirectory;

	/* Minhook */
	MINHOOK_CREATE						CreateHook;
	MINHOOK_REMOVE						RemoveHook;
	MINHOOK_ENABLE						EnableHook;
	MINHOOK_DISABLE						DisableHook;

	/* Logging */
	LOGGER_LOG2							Log;

	/* Events */
	EVENTS_RAISE						RaiseEvent;
	EVENTS_RAISENOTIFICATION			RaiseEventNotification;
	EVENTS_SUBSCRIBE					SubscribeEvent;
	EVENTS_SUBSCRIBE					UnsubscribeEvent;

	/* WndProc */
	WNDPROC_ADDREM						RegisterWndProc;
	WNDPROC_ADDREM						DeregisterWndProc;
	WNDPROC_SENDTOGAME					SendWndProcToGameOnly;

	/* Keybinds */
	KEYBINDS_REGISTERWITHSTRING			RegisterKeybindWithString;
	KEYBINDS_REGISTERWITHSTRUCT			RegisterKeybindWithStruct;
	KEYBINDS_DEREGISTER					DeregisterKeybind;

	/* DataLink */
	DATALINK_GETRESOURCE				GetResource;
	DATALINK_SHARERESOURCE				ShareResource;

	/* Textures */
	TEXTURES_GET						GetTexture;
	TEXTURES_GETORCREATEFROMFILE		GetTextureOrCreateFromFile;
	TEXTURES_GETORCREATEFROMRESOURCE	GetTextureOrCreateFromResource;
	TEXTURES_GETORCREATEFROMURL			GetTextureOrCreateFromURL;
	TEXTURES_GETORCREATEFROMMEMORY		GetTextureOrCreateFromMemory;
	TEXTURES_LOADFROMFILE				LoadTextureFromFile;
	TEXTURES_LOADFROMRESOURCE			LoadTextureFromResource;
	TEXTURES_LOADFROMURL				LoadTextureFromURL;
	TEXTURES_LOADFROMMEMORY				LoadTextureFromMemory;

	/* Shortcuts */
	QUICKACCESS_ADDSHORTCUT				AddShortcut;
	QUICKACCESS_GENERIC					RemoveShortcut;
	QUICKACCESS_GENERIC					NotifyShortcut;
	QUICKACCESS_ADDSIMPLE				AddSimpleShortcut;
	QUICKACCESS_GENERIC					RemoveSimpleShortcut;

	LOCALIZATION_TRANSLATE				Translate;
	LOCALIZATION_TRANSLATETO			TranslateTo;
} AddonAPI;

typedef void (*ADDON_LOAD) (AddonAPI* aAPI);
typedef void (*ADDON_UNLOAD) (void);

typedef struct AddonVersion
{
	signed short	Major;
	signed short	Minor;
	signed short	Build;
	signed short	Revision;
} AddonVersion;

typedef enum EAddonFlags
{
	EAddonFlags_None = 0,
	EAddonFlags_IsVolatile = 1,				/* is hooking functions or doing anything else that's volatile and game build dependant */
	EAddonFlags_DisableHotloading = 2		/* prevents unloading at runtime, aka. will require a restart if updated, etc. */
} EAddonFlags;

typedef enum EUpdateProvider
{
	EUpdateProvider_None		= 0,	/* Does not support auto updating */
	EUpdateProvider_Raidcore	= 1,	/* Provider is Raidcore (via API) */
	EUpdateProvider_GitHub		= 2,	/* Provider is GitHub Releases */
	EUpdateProvider_Direct		= 3		/* Provider is direct file link */
} EUpdateProvider;

typedef struct AddonDefinition
{
	/* required */
	signed int      Signature;      /* Raidcore Addon ID, set to random unqiue negative integer if not on Raidcore */
	signed int		APIVersion;		/* Determines which AddonAPI struct revision the Loader will pass, use the NEXUS_API_VERSION define from Nexus.h */
	const char*     Name;           /* Name of the addon as shown in the library */
	AddonVersion	Version;
	const char*     Author;         /* Author of the addon */
	const char*     Description;    /* Short description */
	ADDON_LOAD      Load;           /* Pointer to Load Function of the addon */
	ADDON_UNLOAD    Unload;         /* Pointer to Unload Function of the addon. Not required if EAddonFlags::DisableHotloading is set. */
	EAddonFlags     Flags;          /* Information about the addon */

	/* update fallback */
	EUpdateProvider Provider;       /* What platform is the the addon hosted on */
	const char*     UpdateLink;     /* Link to the update resource */
} AddonDefinition;

#endif
