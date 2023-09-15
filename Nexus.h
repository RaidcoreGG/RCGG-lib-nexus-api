#ifndef NEXUS_H
#define NEXUS_H

#include <Windows.h>
#include <d3d11.h>
#include <mutex>

#include "imgui/imgui.h"

#define NEXUS_API_VERSION 1

typedef void (*ADDON_RENDER)(bool aIsUIVisible);
typedef void (*GUI_REGISTER)(ADDON_RENDER aRenderCallback);
typedef void (*GUI_UNREGISTER)(ADDON_RENDER aRenderCallback);

// MinHook Error Codes.
enum MH_STATUS
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
};

typedef MH_STATUS	(__stdcall* MINHOOK_CREATE)				(LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal);
typedef MH_STATUS	(__stdcall* MINHOOK_REMOVE)				(LPVOID pTarget);
typedef MH_STATUS	(__stdcall* MINHOOK_ENABLE)				(LPVOID pTarget);
typedef MH_STATUS	(__stdcall* MINHOOK_DISABLE)			(LPVOID pTarget);

enum class ELogLevel : unsigned char
{
	OFF			= 0,
	CRITICAL	= 1,
	WARNING		= 2,
	INFO		= 3,
	DEBUG		= 4,
	TRACE		= 5,
	ALL
};

struct LogEntry
{
	ELogLevel			LogLevel;
	unsigned long long	Timestamp;
	std::wstring		Message;
};

class ILogger
{
	public:
		ILogger()			= default;
		virtual	~ILogger()	= default;

		ELogLevel		GetLogLevel();
		void			SetLogLevel(ELogLevel aLogLevel);

		virtual void	LogMessage(LogEntry aLogEntry) = 0;

	protected:
		ELogLevel		LogLevel;
		std::mutex		MessageMutex;
};

typedef void		(*LOGGER_LOGA)							(ELogLevel aLogLevel, const char* aFmt, ...);
typedef void		(*LOGGER_ADDREM)						(ILogger* aLogger);

typedef void		(*EVENTS_CONSUME)						(void* aEventArgs);
typedef void		(*EVENTS_RAISE)							(const char* aEventName, void* aEventData);
typedef void		(*EVENTS_SUBSCRIBE)						(const char* aEventName, EVENTS_CONSUME aConsumeEventCallback);

struct Keybind
{
	WORD Key;
	bool Alt;
	bool Ctrl;
	bool Shift;
};

typedef void (*KEYBINDS_PROCESS)(const char* aIdentifier);
typedef void (*KEYBINDS_REGISTER)(const char* aIdentifier, KEYBINDS_PROCESS aKeybindHandler, const char* aKeybind);
typedef void (*KEYBINDS_UNREGISTER)(const char* aIdentifier);
typedef bool (*ADDON_WNDPROC)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef void (*KEYBINDS_REGISTERWNDPROC)(ADDON_WNDPROC aWndProcCallback);
typedef void (*KEYBINDS_UNREGISTERWNDPROC)(ADDON_WNDPROC aWndProcCallback);

typedef void*		(*DATALINK_GETRESOURCE)					(const char* aIdentifier);
typedef void*		(*DATALINK_SHARERESOURCE)				(const char* aIdentifier, size_t aResourceSize);

struct Texture
{
	unsigned					Width;
	unsigned					Height;
	ID3D11ShaderResourceView*	Resource;
};

typedef void		(*TEXTURES_RECEIVECALLBACK)				(const char* aIdentifier, Texture* aTexture);
typedef Texture*	(*TEXTURES_GET)							(const char* aIdentifier);
typedef void		(*TEXTURES_LOADFROMFILE)				(const char* aIdentifier, const char* aFilename, TEXTURES_RECEIVECALLBACK aCallback);
typedef void		(*TEXTURES_LOADFROMRESOURCE)			(const char* aIdentifier, unsigned aResourceID, HMODULE aModule, TEXTURES_RECEIVECALLBACK aCallback);

typedef void		(*QUICKACCESS_SHORTCUTRENDERCALLBACK)	();
typedef void		(*QUICKACCESS_ADDSHORTCUT)				(const char* aIdentifier, const char* aTextureIdentifier, const char* aTextureHoverIdentifier, const char* aKeybindIdentifier, const char* aTooltipText);
typedef void		(*QUICKACCESS_REMOVESHORTCUT)			(const char* aIdentifier);
typedef void		(*QUICKACCESS_ADDSIMPLE)				(const char* aIdentifier, QUICKACCESS_SHORTCUTRENDERCALLBACK aShortcutRenderCallback);
typedef void		(*QUICKACCESS_REMOVESIMPLE)				(const char* aIdentifier);

struct AddonAPI
{
	/* Renderer */
	IDXGISwapChain*				SwapChain;
	ImGuiContext*				ImguiContext;
	GUI_REGISTER				RegisterRender;
	GUI_UNREGISTER				UnregisterRender;

	/* Minhook */
	MINHOOK_CREATE				CreateHook;
	MINHOOK_REMOVE				RemoveHook;
	MINHOOK_ENABLE				EnableHook;
	MINHOOK_DISABLE				DisableHook;

	/* Logging */
	LOGGER_LOGA					Log;
	LOGGER_ADDREM				RegisterLogger;
	LOGGER_ADDREM				UnregisterLogger;

	/* Events */
	EVENTS_RAISE				RaiseEvent;
	EVENTS_SUBSCRIBE			SubscribeEvent;
	EVENTS_SUBSCRIBE			UnsubscribeEvent;

	/* Keybinds */
	KEYBINDS_REGISTER			RegisterKeybind;
	KEYBINDS_UNREGISTER			UnregisterKeybind;
	KEYBINDS_REGISTERWNDPROC	RegisterWndProc;
	KEYBINDS_UNREGISTERWNDPROC	UnregisterWndProc;

	/* DataLink */
	DATALINK_GETRESOURCE		GetResource;
	DATALINK_SHARERESOURCE		ShareResource;

	/* Textures */
	TEXTURES_GET				GetTexture;
	TEXTURES_LOADFROMFILE		LoadTextureFromFile;
	TEXTURES_LOADFROMRESOURCE	LoadTextureFromResource;

	/* Shortcuts */
	QUICKACCESS_ADDSHORTCUT		AddShortcut;
	QUICKACCESS_REMOVESHORTCUT  RemoveShortcut;
	QUICKACCESS_ADDSIMPLE		AddSimpleShortcut;
	QUICKACCESS_REMOVESIMPLE	RemoveSimpleShortcut;

	/* API */
		// GW2 API FUNCS
		// LOGITECH API FUNCS
};

enum class EAddonFlags
{
	None = 0,
	HasOptions = 1 << 1,           /* should an options button be drawn and the event fired? */
	IsVolatile = 1 << 2,           /* is hooking functions or doing anything else that's volatile and game build dependant */
};

enum class EUpdateProvider
{
	None = 0,		/* Does not support auto updating */
	Raidcore = 1,	/* Provider is Raidcore (via API) */
	GitHub = 2,		/* Provider is GitHub Releases */
	Direct = 3		/* Provider is direct file link */
};

struct NexusLinkData
{
	unsigned	Width;
	unsigned	Height;
	float		Scaling;

	bool		IsMoving;
	bool		IsCameraMoving;
	bool		IsGameplay;

	ImFont*		Font;
	ImFont*		FontBig;
	ImFont*		FontUI;
};

typedef void (*ADDON_LOAD)(AddonAPI aHostApi, void* mallocfn, void* freefn);
typedef void (*ADDON_UNLOAD)();

struct AddonVersion
{
	signed short	Major;
	signed short	Minor;
	signed short	Build;
	signed short	Revision;
};

struct AddonDefinition
{
	/* required */
	signed int      Signature;      /* Raidcore Addon ID, set to random unqiue negative integer if not on Raidcore */
	signed int		APIVersion;		/* Determines which AddonAPI struct revision the Loader will pass, use the NEXUS_API_VERSION define from Nexus.h */
	const char*		Name;           /* Name of the addon as shown in the library */
	AddonVersion	Version;
	const char*		Author;         /* Author of the addon */
	const char*		Description;    /* Short description */
	ADDON_LOAD      Load;           /* Pointer to Load Function of the addon */
	ADDON_UNLOAD    Unload;         /* Pointer to Unload Function of the addon */
	EAddonFlags     Flags;          /* Information about the addon */

	/* update fallback */
	EUpdateProvider Provider;       /* What platform is the the addon hosted on */
	const char* UpdateLink;			/* Link to the update resource */
};

#endif