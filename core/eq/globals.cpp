/*
 * MacroQuest: The extension platform for EverQuest
 * Copyright (C) 2002-present MacroQuest Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "pch.h"
#include "globals.h"
#include "forward_decls.h"
#include "login_frontend.h"
#include "eq.h"

namespace eqlib {

// These don't change during the execution of the program. They can be loaded
// at static initialization time because of this.
uintptr_t EQGameBaseAddress = (uintptr_t)GetModuleHandle(nullptr);
uintptr_t EQMainBaseAddress = NULL;

//============================================================================
// Data
//============================================================================

//============================================================================


//============================================================================
// Offset Definitions & Initialization
//============================================================================

#pragma region eqgame.exe offsets
//============================================================================
//
// eqgame.exe Offsets

INITIALIZE_EQGAME_OFFSET(pinstCEverQuest);
INITIALIZE_EQGAME_OFFSET(pinstEverQuestInfo);


//----------------------------------------------------------------------------
// Instance Pointers
//----------------------------------------------------------------------------
EverQuestinfo* pEverQuestInfo = nullptr;
ForeignPointer<CEverQuest, EVERQUEST>  pEverQuest;

void InitializeEQGameOffsets()
{
	if (!EQGameBaseAddress)
	{
		return;
	}
	pEverQuestInfo = (EverQuestinfo*)pinstEverQuestInfo;
	pEverQuest = pinstCEverQuest;
}

#pragma endregion

#pragma region eqmain.dll offsets

//============================================================================
//
// EQMain.dll

//----------------------------------------------------------------------------
// offsets / patterns

uintptr_t EQMain__CEQSuiteTextureLoader__GetTexture = 0;
uintptr_t EQMain__CLoginViewManager__HandleLButtonUp = 0;
uintptr_t EQMain__CXWndManager__GetCursorToDisplay = 0;
uintptr_t EQMain__LoginController__GiveTime = 0;
uintptr_t EQMain__LoginController__ProcessKeyboardEvents = 0;
uintptr_t EQMain__LoginController__ProcessMouseEvents = 0;
uintptr_t EQMain__LoginController__Shutdown = 0;
uintptr_t EQMain__LoginServerAPI__JoinServer = 0;
uintptr_t EQMain__WndProc = 0;

uintptr_t EQMain__pinstCEQSuiteTextureLoader = 0;
uintptr_t EQMain__pinstCLoginViewManager = 0;
uintptr_t EQMain__pinstCSidlManager = 0;
uintptr_t EQMain__pinstCXWndManager = 0;
uintptr_t EQMain__pinstLoginController = 0;
uintptr_t EQMain__pinstLoginServerAPI = 0;
uintptr_t EQMain__pinstLoginClient = 0;

ForeignPointer<LoginClient> g_pLoginClient;
ForeignPointer<LoginController> g_pLoginController;
ForeignPointer<LoginServerAPI> g_pLoginServerAPI;

bool InitializeEQMainOffsets()
{
	if (EQMainBaseAddress == NULL) {
		EQMainBaseAddress = (uintptr_t)GetModuleHandle(L"eqmain.dll");
	}

		EQMain__CEQSuiteTextureLoader__GetTexture = FixEQMainOffset(EQMain__CEQSuiteTextureLoader__GetTexture_x);

		EQMain__LoginController__GiveTime = FixEQMainOffset(EQMain__LoginController__GiveTime_x);
		EQMain__LoginServerAPI__JoinServer = FixEQMainOffset(EQMain__LoginServerAPI__JoinServer_x);
		EQMain__LoginController__Shutdown = FixEQMainOffset(EQMain__LoginController__Shutdown_x);
		EQMain__WndProc = FixEQMainOffset(EQMain__WndProc_x);

		EQMain__pinstCEQSuiteTextureLoader = FixEQMainOffset(EQMain__pinstCEQSuiteTextureLoader_x);
		EQMain__pinstCSidlManager = FixEQMainOffset(EQMain__pinstCSidlManager_x);
		EQMain__pinstCXWndManager = FixEQMainOffset(EQMain__pinstCXWndManager_x);
		EQMain__pinstLoginController = FixEQMainOffset(EQMain__pinstLoginController_x);
		EQMain__pinstLoginServerAPI = FixEQMainOffset(EQMain__pinstLoginServerAPI_x);
		EQMain__pinstLoginClient = FixEQMainOffset(EQMain__pinstCLoginViewManager_x) - sizeof(uintptr_t);

		if (EQMain__LoginController__GiveTime)
		{
#if defined(_M_AMD64)
			//.text:18001BAC0                     public: void __thiscall LoginController::GiveTime(void) proc near
			//.text:18001BAC0 40 53                               push    rbx
			//.text:18001BAC2 48 83 EC 20                         sub     rsp, 20h
			//.text:18001BAC6 48 8B D9                            mov     rbx, rcx
			//.text:18001BAC9 E8 02 02 00 00                      call    LoginController::PollAndProcessDXKeyboard(void)
			EQMain__LoginController__ProcessKeyboardEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 9, 1, 4);
			//.text:18001BACE 48 8B CB                            mov     rcx, rbx
			//.text:18001BAD1 48 83 C4 20                         add     rsp, 20h
			//.text:18001BAD5 5B                                  pop     rbx
			//.text:18001BAD6 E9 25 06 00 00                      jmp     LoginController::PollAndProcessDXMouse(void)
			EQMain__LoginController__ProcessMouseEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 22, 1, 4);
#else
			//.text:10014B00                      public: void __thiscall LoginController::GiveTime(void) proc near
			//.text:10014B00 56                                   push    esi
			//.text:10014B01 8B F1                                mov     esi, this
			//.text:10014B03 E8 D8 06 00 00                       call    LoginController::ProcessKeyboardEvents(void)
			EQMain__LoginController__ProcessKeyboardEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 3, 1, 4);
			//.text:10014B08 8B CE                                mov     this, esi       ; this
			//.text:10014B0A 5E                                   pop     esi
			//.text:10014B0B E9 A0 08 00 00                       jmp     LoginController::ProcessMouseEvents(void)
			EQMain__LoginController__ProcessMouseEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 11, 1, 4);
#endif
		}

		g_pLoginController = EQMain__pinstLoginController;
		g_pLoginServerAPI = EQMain__pinstLoginServerAPI;
		g_pLoginClient = EQMain__pinstLoginClient;

		return true;
}

void CleanupEQMainOffsets()
{
	EQMainBaseAddress = 0;

	EQMain__CEQSuiteTextureLoader__GetTexture = 0;
	EQMain__CXWndManager__GetCursorToDisplay = 0;
	EQMain__LoginController__GiveTime = 0;
	EQMain__LoginController__ProcessKeyboardEvents = 0;
	EQMain__LoginController__ProcessMouseEvents = 0;
	EQMain__LoginController__Shutdown = 0;
	EQMain__LoginServerAPI__JoinServer = 0;
	EQMain__WndProc = 0;

	EQMain__pinstCEQSuiteTextureLoader = 0;
	EQMain__pinstCSidlManager = 0;
	EQMain__pinstCXWndManager = 0;
	EQMain__pinstLoginController = 0;
	EQMain__pinstLoginServerAPI = 0;
	EQMain__pinstLoginClient = 0;

	g_pLoginController.reset();
	g_pLoginServerAPI.reset();
	g_pLoginClient.reset();
}

#pragma endregion

//============================================================================
//


void InitializeGlobals()
{
	InitializeEQGameOffsets();
	InitializeEQMainOffsets();
}

} // namespace eqlib