#pragma once
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

#pragma once

#include "common.h"
#include "eqgame.h"
#include "cxstr.h"
#include "eqgraphics.h"
#include "eqmain.h"
#include "forward_decls.h"
#include "eq.h"
#include "login_frontend.h"
#include <dinput.h>

namespace eqlib {

	// the preferred base of eqgame.exe
#if defined(_M_AMD64)
	constexpr uintptr_t EQGamePreferredAddress = 0x140000000;
#else
	constexpr uintptr_t EQGamePreferredAddress = 0x400000;
#endif // defined(_M_AMD64)

#if defined(_M_AMD64)
	constexpr uintptr_t EQLibraryPreferredAddress = 0x180000000;
#else
	constexpr uintptr_t EQLibraryPreferredAddress = 0x10000000;
#endif

	// the base address of eqgame.exe
	extern uintptr_t EQGameBaseAddress;

	// the base address of eqmain.dll
	extern uintptr_t EQMainBaseAddress;


	// These macros are used for statically building offsets. If using dynamic offset generation
	// with the pattern matching, don't use the macro.

	// These functions are used for dynamically building offsets.

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
	inline uintptr_t FixEQGameOffset(T nOffset)
	{
		return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQGamePreferredAddress) + EQGameBaseAddress;
	}


	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
	inline uintptr_t FixEQMainOffset(T nOffset)
	{
		return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQLibraryPreferredAddress) + EQMainBaseAddress;
	}

#define INITIALIZE_EQGAME_OFFSET(var) uintptr_t var = FixEQGameOffset(var##_x)
#define INITIALIZE_EQMAIN_OFFSET(var) uintptr_t var = FixEQMainOffset(var##_x)

	//============================================================================
	// EQGame.exe Offsets
	//============================================================================
	extern uintptr_t pinstCEverQuest;
	extern uintptr_t pinstEverQuestInfo;
	extern ForeignPointer<CEverQuest, EVERQUEST> pEverQuest;
	extern EverQuestinfo* pEverQuestInfo;
	//============================================================================
	// eqmain.dll Offsets
	//============================================================================

	extern uintptr_t EQMain__CEQSuiteTextureLoader__GetTexture;
	extern uintptr_t EQMain__CLoginViewManager__HandleLButtonUp;
	extern uintptr_t EQMain__CXWndManager__GetCursorToDisplay;
	extern uintptr_t EQMain__LoginController__GiveTime;
	extern uintptr_t EQMain__LoginController__ProcessKeyboardEvents;
	extern uintptr_t EQMain__LoginController__ProcessMouseEvents;
	extern uintptr_t EQMain__LoginController__Shutdown;
	extern uintptr_t EQMain__LoginServerAPI__JoinServer;
	extern uintptr_t EQMain__WndProc;

	extern uintptr_t EQMain__pinstCEQSuiteTextureLoader;
	extern uintptr_t EQMain__pinstCLoginViewManager;
	extern uintptr_t EQMain__pinstCSidlManager;
	extern uintptr_t EQMain__pinstCXWndManager;
	extern uintptr_t EQMain__pinstLoginController;
	extern uintptr_t EQMain__pinstLoginServerAPI;
	extern uintptr_t EQMain__pinstLoginClient;

	extern ForeignPointer<LoginClient> g_pLoginClient;
	extern ForeignPointer<LoginController> g_pLoginController;
	extern ForeignPointer<LoginServerAPI> g_pLoginServerAPI;

	//----------------------------------------------------------------------------

	void InitializeGlobals();

	bool InitializeEQMainOffsets();
	void CleanupEQMainOffsets();

	void InitializeEQLib();
	void ShutdownEQLib();

	//----------------------------------------------------------------------------

} // namespace eqlib