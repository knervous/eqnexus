#pragma once
export module eq;

import <Windows.h>;
import <numeric>;

export struct [[offsetcomments]] EverQuestinfo
{
	/*0x00000*/ HWND              Wnd;
	/*0x00004*/ HINSTANCE         hInst;
	/*0x00008*/ int               Render_MinX;
	/*0x0000c*/ int               Render_MinY;
	/*0x00010*/ int               Render_MaxX;
	/*0x00014*/ int               Render_MaxY;
	/*0x00018*/ int               Render_XScale;
	/*0x0001c*/ int               Render_YScale;
	/*0x00020*/ int               Render_WidthScale;
	/*0x00024*/ int               Render_HeightScale;
	/*0x00028*/ int               ReadyEnterWorld;
	/*0x0002c*/ bool              InsideDoMainWhileLoop;
	/*0x00030*/ int               Hidden;
	/*0x00034*/ uint32_t          DisplayFlags;
	/*0x00038*/ uint32_t          Command;
	/*0x0003c*/ uint8_t           Unknown0x0003c;
};

export class CEverQuest
{
public:

	/*0x00000*/ // CEverQuest::`vftable'{for `CEverQuestBase'}
	/*0x00008*/ // CEverQuest::`vftable'{for `UniversalChatProxyHandler'}
	/*0x00010*/ // CEverQuest::`vftable'{for `PopDialogHandler'}
	/*0x0000c*/ void* currentChatServerApi;
	/*0x00010*/ bool                  alreadyAutoJoined;
	/*0x00018*/ int64_t               chatNotificationStamp;
	/*0x00020*/ char                  ucpAddress[128];
	/*0x000a0*/ int                   ucpPort;
	/*0x000a4*/ char                  ucpPlayerName[256];
	/*0x001a4*/ char                  ucpTicket[256];
	/*0x002a4*/ void* chatService; // 2a4
	/*0x002a8*/ int64_t               ucNotificationStamp;
	/*0x002b0*/ bool                  ucNotificationEntering; // bJoinedChannel
	/*0x002b1*/ char                  ucNotificationPlayerName[256];
	/*0x003b1*/ char                  ucNotificationChannelName[10][48];
	/*0x00594*/ DWORD                 ucNotificationChannelNumber[10];
	/*0x005bc*/ int                   ucNotificationCount; // ChannelQty
	/*0x005c0*/ void* freeTargetTracker;
	/*0x005c4*/ int                   WorldState;                   // 0 everything is fine, 1 we are getting disconnected 2 player not released from zone
	/*0x005c8*/ int                   GameState;
	
};