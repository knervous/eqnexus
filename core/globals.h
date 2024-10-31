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

#include "eq/common.h"
#include "eq/eqgame.h"
#include "eqgraphics.h"
#include "eqmain.h"
#include "ForwardDecls.h"

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION    0x800
#endif

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
	uintptr_t EQGameBaseAddress;

	// the base address of eqgraphicsdx9.dll
	uintptr_t EQGraphicsBaseAddress;

	// the base address of eqmain.dll
	uintptr_t EQMainBaseAddress;

	// the base address of kernel32.dll
	uintptr_t Kernel32BaseAddress;

	// These macros are used for statically building offsets. If using dynamic offset generation
	// with the pattern matching, don't use the macro.

	// These functions are used for dynamically building offsets.

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
	inline uintptr_t FixEQGameOffset(T nOffset)
	{
		return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQGamePreferredAddress) + EQGameBaseAddress;
	}

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
	inline uintptr_t FixEQGraphicsOffset(T nOffset)
	{
		return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQLibraryPreferredAddress) + EQGraphicsBaseAddress;
	}

	template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
	inline uintptr_t FixEQMainOffset(T nOffset)
	{
		return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQLibraryPreferredAddress) + EQMainBaseAddress;
	}

#define INITIALIZE_EQGAME_OFFSET(var) uintptr_t var = FixEQGameOffset(var##_x)
#define INITIALIZE_EQGRAPHICS_OFFSET(var) uintptr_t var = FixEQGraphicsOffset(var##_x)
#define INITIALIZE_EQMAIN_OFFSET(var) uintptr_t var = FixEQMainOffset(var##_x)


	//============================================================================
	// Data
	//============================================================================

	const char* szCombineTypes[];
	size_t MAX_COMBINES;
	const char* szItemClasses[];
	size_t MAX_ITEMCLASSES;
	const char* szSPATypes[];
	size_t MAX_SPELLEFFECTS;
	const char* szFactionNames[];
	size_t MAX_FACTIONNAMES;

	inline namespace deprecated {
		size_t MAX_ITEMTYPES;
		const char** szItemTypes;
	}

	ServerID ServerIDArray[(uint32_t)ServerID::NumServers];
	const char* GetServerNameFromServerID(ServerID id);
	ServerID GetServerIDFromServerName(const char* serverName);

	struct ACTORDEFENTRY
	{
		uint32_t Def;
		int      ZoneID;
		char     Name[256];
	};
	ACTORDEFENTRY ActorDefList[];

	struct DIKEYID
	{
		char szName[32];
		WORD Id;
	};
	DIKEYID gDiKeyID[];
	const char* gDiKeyName[256];

	constexpr int MAX_HOTBUTTON_WNDS = 12;

	//============================================================================
	// EQGame.exe Offsets
	//============================================================================

	uintptr_t __ActualVersionBuild;
	uintptr_t __ActualVersionDate;
	uintptr_t __ActualVersionTime;
	uintptr_t __BindList;
	uintptr_t __CommandList;
	uintptr_t __CurrentMapLabel;
	uintptr_t __CurrentSocial;
	uintptr_t __do_loot;
	uintptr_t __ExceptionFilter;
	uintptr_t __gpbCommandEvent;
	uintptr_t __Guilds;
	uintptr_t __gWorld;
	uintptr_t __HandleMouseWheel;
	uintptr_t __heqmain;
	uintptr_t __HotkeyPage;
	uintptr_t __HWnd;
	uintptr_t __LabelCache;
	uintptr_t __LoginName;
	uintptr_t __Mouse;
	uintptr_t __MouseEventTime;
	uintptr_t __ScreenMode;
	uintptr_t __ServerHost;
	uintptr_t __ThrottleFrameRate;
#if defined(__ThrottleFrameRateEnd_x)
	uintptr_t __ThrottleFrameRateEnd;
#endif
	uintptr_t EQObject_Top;
	uintptr_t g_eqCommandStates;
	uintptr_t instCRaid;
	uintptr_t instDynamicZone;
	uintptr_t instEQMisc;
	uintptr_t instEQZoneInfo;
	uintptr_t instExpeditionLeader;
	uintptr_t instExpeditionName;
	uintptr_t instTributeActive;
	uintptr_t pinstActiveBanker;
	uintptr_t pinstActiveCorpse;
	uintptr_t pinstActiveGMaster;
	uintptr_t pinstActiveMerchant;
	uintptr_t pinstAltAdvManager;
	uintptr_t pinstCamActor;
	uintptr_t pinstCChatWindowManager;
	uintptr_t pinstCContainerMgr;
	uintptr_t pinstCContextMenuManager;
	uintptr_t pinstCDBStr;
	uintptr_t pinstCDisplay;
	uintptr_t pinstCEverQuest;
	uintptr_t pinstCInvSlotMgr;
	uintptr_t pinstCItemDisplayManager;
	uintptr_t pinstCMercenaryClientManager;
	uintptr_t pinstCPopupWndManager;
	uintptr_t pinstCResolutionHandler;
	uintptr_t pinstCSidlManager;
	uintptr_t pinstCSpellDisplayMgr;
	uintptr_t pinstCTaskManager;
	uintptr_t pinstCXWndManager;
	uintptr_t pinstDZMember;
	uintptr_t pinstDZTimerInfo;
	uintptr_t pinstEqLogin;
	uintptr_t pinstEQSoundManager;
#if defined(pinstEQSpellStrings_x)
	uintptr_t pinstEQSpellStrings;
#endif
	uintptr_t pinstEQSuiteTextureLoader;
	uintptr_t pinstEverQuestInfo;
	uintptr_t pinstItemIconCache;
#if defined(pinstKeypressHandler_x)
	uintptr_t pinstKeypressHandler;
#endif
	uintptr_t pinstLocalPC;
	uintptr_t pinstLocalPlayer;
	uintptr_t pinstModelPlayer;
	uintptr_t pinstRenderInterface;
	uintptr_t pinstPlayerPath;
	uintptr_t pinstSGraphicsEngine;
	uintptr_t pinstSkillMgr;
	uintptr_t pinstSpawnManager;
	uintptr_t pinstSpellManager;
	uintptr_t pinstStringTable;
	uintptr_t pinstSwitchManager;
	uintptr_t pinstTarget;
	uintptr_t pinstTargetIndicator;
	uintptr_t pinstTaskMember;
	uintptr_t pinstTrackTarget;
	uintptr_t pinstTradeTarget;
	uintptr_t pinstViewActor;
	uintptr_t pinstWorldData;

	uintptr_t __MemChecker0;
	uintptr_t __MemChecker1;
#if defined(__MemChecker4_x)
	uintptr_t __MemChecker4;
#endif
	uintptr_t __EncryptPad0;
	uintptr_t DI8__Keyboard;
	uintptr_t DI8__Main;
	uintptr_t DI8__MouseState;
	uintptr_t DI8__Mouse;

	uintptr_t __CastRay;
	uintptr_t __CastRay2;
	uintptr_t __CleanItemTags;
	uintptr_t __compress_block;
	uintptr_t __ConvertItemTags;
	uintptr_t __CopyLayout;
	uintptr_t __CreateCascadeMenuItems;
	uintptr_t __decompress_block;
	uintptr_t __DoesFileExist;
	uintptr_t __eq_delete;
	uintptr_t __eq_new;
	uintptr_t __EQGetTime;
	uintptr_t __ExecuteCmd;
	uintptr_t __FixHeading;
	uintptr_t __FlushDxKeyboard;
	uintptr_t __get_bearing;
	uintptr_t __get_melee_range;
	uintptr_t __GetAnimationCache;
	uintptr_t __GetGaugeValueFromEQ;
	uintptr_t __GetLabelFromEQ;
	uintptr_t __GetXTargetType;
	uintptr_t __HeadingDiff;
	uintptr_t __HelpPath;
	uintptr_t __InitMouse;
	uintptr_t __msgTokenTextParam;
	uintptr_t __NewUIINI;
	uintptr_t __ProcessGameEvents;
	uintptr_t __ProcessKeyboardEvents;
	uintptr_t __ProcessMouseEvents;
	uintptr_t __ProcessDeviceEvents;
	uintptr_t __SaveColors;
	uintptr_t __STMLToText;
	uintptr_t __WndProc;

	uintptr_t AchievementManager__Instance;
	uintptr_t AggroMeterManagerClient__Instance;
	uintptr_t AltAdvManager__CanSeeAbility;
	uintptr_t AltAdvManager__CanTrainAbility;
	uintptr_t AltAdvManager__GetAAById;
	uintptr_t AltAdvManager__GetCalculatedTimer;
	uintptr_t AltAdvManager__IsAbilityReady;
	uintptr_t ArrayClass__DeleteElement;
	uintptr_t CAAWnd__ShowAbility;
	uintptr_t CAAWnd__Update;
	uintptr_t CAAWnd__UpdateSelected;
	uintptr_t CAdvancedLootWnd__AddPlayerToList;
	uintptr_t CAdvancedLootWnd__DoAdvLootAction;
	uintptr_t CAdvancedLootWnd__DoSharedAdvLootAction;
	uintptr_t CAdvancedLootWnd__UpdateMasterLooter;
	uintptr_t CAltAbilityData__GetMercCurrentRank;
	uintptr_t CAltAbilityData__GetMercMaxRank;
	uintptr_t CBankWnd__GetNumBankSlots;
	uintptr_t CBankWnd__WndNotification;
	uintptr_t CBarterSearchWnd__UpdateInventoryList;
	uintptr_t CBarterSearchWnd__WndNotification;
	uintptr_t CBarterWnd__WndNotification;
	uintptr_t CBazaarSearchWnd__HandleSearchResults;
	uintptr_t CBroadcast__Get;
	uintptr_t CBuffWindow__WndNotification;
	uintptr_t CButtonWnd__vftable;
	uintptr_t CCastSpellWnd__ForgetMemorizedSpell;
	uintptr_t CCastSpellWnd__IsBardSongPlaying;
	uintptr_t CCastSpellWnd__RefreshSpellGemButtons;
	uintptr_t CCharacterListWnd__EnterWorld;
	uintptr_t CCharacterListWnd__Quit;
	uintptr_t CCharacterListWnd__SelectCharacter;
	uintptr_t CCharacterListWnd__UpdateList;
	uintptr_t CChatService__GetFriendName;
	uintptr_t CChatService__GetNumberOfFriends;
	uintptr_t CChatWindow__AddHistory;
	uintptr_t CChatWindow__CChatWindow;
	uintptr_t CChatWindow__Clear;
	uintptr_t CChatWindow__WndNotification;
	uintptr_t CChatWindowManager__CreateChatWindow;
	uintptr_t CChatWindowManager__FreeChatWindow;
	uintptr_t CChatWindowManager__GetRGBAFromIndex;
	uintptr_t CChatWindowManager__InitContextMenu;
	uintptr_t CChatWindowManager__SetLockedActiveChatWindow;
	uintptr_t CColorPickerWnd__Open;
	uintptr_t CComboWnd__DeleteAll;
	uintptr_t CComboWnd__Draw;
	uintptr_t CComboWnd__GetChoiceText;
	uintptr_t CComboWnd__GetCurChoice;
	uintptr_t CComboWnd__GetCurChoiceText;
	uintptr_t CComboWnd__GetItemCount;
	uintptr_t CComboWnd__GetListRect;
	uintptr_t CComboWnd__InsertChoice;
	uintptr_t CComboWnd__SetChoice;
	uintptr_t CComboWnd__SetColors;
	uintptr_t CContainerMgr__CloseContainer;
	uintptr_t CContainerMgr__OpenContainer;
	uintptr_t CContainerMgr__OpenExperimentContainer;
	uintptr_t CContainerWnd__HandleCombine;
	uintptr_t CContainerWnd__SetContainer;
	uintptr_t CContainerWnd__vftable;
	uintptr_t CContextMenu__AddMenuItem;
	uintptr_t CContextMenu__AddSeparator;
	uintptr_t CContextMenu__CContextMenu;
	uintptr_t CContextMenu__CheckMenuItem;
	uintptr_t CContextMenu__dCContextMenu;
	uintptr_t CContextMenu__RemoveAllMenuItems;
	uintptr_t CContextMenu__RemoveMenuItem;
	uintptr_t CContextMenu__SetMenuItem;
	uintptr_t CContextMenuManager__AddMenu;
	uintptr_t CContextMenuManager__CreateDefaultMenu;
	uintptr_t CContextMenuManager__Flush;
	uintptr_t CContextMenuManager__PopupMenu;
	uintptr_t CContextMenuManager__RemoveMenu;
	uintptr_t CCursorAttachment__AttachToCursor;
	uintptr_t CCursorAttachment__IsOkToActivate;
	uintptr_t CCursorAttachment__RemoveAttachment;
	uintptr_t CDBStr__GetString;
	uintptr_t CDisplay__cameraType;
	uintptr_t CDisplay__CleanGameUI;
	uintptr_t CDisplay__GetClickedActor;
	uintptr_t CDisplay__GetFloorHeight;
	uintptr_t CDisplay__GetUserDefinedColor;
	uintptr_t CDisplay__InitCharSelectUI;
	uintptr_t CDisplay__PreZoneMainUI;
	uintptr_t CDisplay__RealRender_World;
	uintptr_t CDisplay__ReloadUI;
	uintptr_t CDisplay__SetViewActor;
	uintptr_t CDisplay__ToggleScreenshotMode;
	uintptr_t CDisplay__TrueDistance;
	uintptr_t CDisplay__WriteTextHD2;
	uintptr_t CDisplay__ZoneMainUI;
#if defined(CDistillerInfo__Instance_x)
	uintptr_t CDistillerInfo__GetIDFromRecordNum;
	uintptr_t CDistillerInfo__Instance;
#endif
	uintptr_t CEditBaseWnd__SetSel;
	uintptr_t CEditWnd__DrawCaret;
	uintptr_t CEditWnd__EnsureCaretVisible;
	uintptr_t CEditWnd__GetCaretPt;
	uintptr_t CEditWnd__GetCharIndexPt;
	uintptr_t CEditWnd__GetDisplayString;
	uintptr_t CEditWnd__GetHorzOffset;
	uintptr_t CEditWnd__GetLineForPrintableChar;
	uintptr_t CEditWnd__GetSelStartPt;
	uintptr_t CEditWnd__GetSTMLSafeText;
	uintptr_t CEditWnd__PointFromPrintableChar;
	uintptr_t CEditWnd__ReplaceSelection;
	uintptr_t CEditWnd__SelectableCharFromPoint;
	uintptr_t CEditWnd__SetEditable;
	uintptr_t CEditWnd__SetWindowText;
	uintptr_t CEQSuiteTextureLoader__CreateTexture;
	uintptr_t CEQSuiteTextureLoader__GetDefaultUIPath;
	uintptr_t CEQSuiteTextureLoader__GetTexture;
	uintptr_t CEverQuest__ClickedPlayer;
	uintptr_t CEverQuest__CreateTargetIndicator;
	uintptr_t CEverQuest__DoPercentConvert;
	uintptr_t CEverQuest__DoTellWindow;
	uintptr_t CEverQuest__DropHeldItemOnGround;
	uintptr_t CEverQuest__dsp_chat;
	uintptr_t CEverQuest__Emote;
	uintptr_t CEverQuest__GetBodyTypeDesc;
	uintptr_t CEverQuest__GetClassDesc;
	uintptr_t CEverQuest__GetClassThreeLetterCode;
	uintptr_t CEverQuest__GetDeityDesc;
	uintptr_t CEverQuest__GetLangDesc;
	uintptr_t CEverQuest__GetRaceDesc;
	uintptr_t CEverQuest__InterpretCmd;
	uintptr_t CEverQuest__IssuePetCommand;
	uintptr_t CEverQuest__LeftClickedOnPlayer;
	uintptr_t CEverQuest__LMouseUp;
	uintptr_t CEverQuest__OutputTextToLog;
	uintptr_t CEverQuest__ReportSuccessfulHeal;
	uintptr_t CEverQuest__ReportSuccessfulHit;
	uintptr_t CEverQuest__RightClickedOnPlayer;
	uintptr_t CEverQuest__RMouseUp;
	uintptr_t CEverQuest__SetGameState;
	uintptr_t CEverQuest__trimName;
	uintptr_t CEverQuest__UPCNotificationFlush;
	uintptr_t CFindItemWnd__CFindItemWnd;
	uintptr_t CFindItemWnd__PickupSelectedItem;
	uintptr_t CFindItemWnd__Update;
	uintptr_t CFindItemWnd__WndNotification;
	uintptr_t CGaugeWnd__Draw;
	uintptr_t CGroupWnd__UpdateDisplay;
	uintptr_t CGroupWnd__WndNotification;
	uintptr_t CGuild__FindMemberByName;
	uintptr_t CGuild__GetGuildIndex;
	uintptr_t CGuild__GetGuildName;
	uintptr_t CharacterBase__GetItemByGlobalIndex;
	uintptr_t CharacterBase__GetItemByGlobalIndex1;
	uintptr_t CharacterBase__IsExpansionFlag;
	uintptr_t CharacterZoneClient__ApplyDamage;
	uintptr_t CharacterZoneClient__BardCastBard;
	uintptr_t CharacterZoneClient__CalcAffectChange;
	uintptr_t CharacterZoneClient__CalcAffectChangeGeneric;
	uintptr_t CharacterZoneClient__CanMedOnHorse;
	uintptr_t CharacterZoneClient__CanUseItem;
	uintptr_t CharacterZoneClient__CanUseMemorizedSpellSlot;
	uintptr_t CharacterZoneClient__CastSpell;
	uintptr_t CharacterZoneClient__CharacterZoneClient;
	uintptr_t CharacterZoneClient__Cur_HP;
	uintptr_t CharacterZoneClient__Cur_Mana;
	uintptr_t CharacterZoneClient__FindAffectSlot;
	uintptr_t CharacterZoneClient__GetAdjustedSkill;
	uintptr_t CharacterZoneClient__GetBaseSkill;
	uintptr_t CharacterZoneClient__GetCastingTimeModifier;
#if defined(CharacterZoneClient__GetCurrentMod_x)
	uintptr_t CharacterZoneClient__GetCurrentMod;
#endif
	uintptr_t CharacterZoneClient__GetCursorItemCount;
	uintptr_t CharacterZoneClient__GetEnduranceRegen;
	uintptr_t CharacterZoneClient__GetFirstEffectSlot;
	uintptr_t CharacterZoneClient__GetFocusCastingTimeModifier;
#if defined(CharacterZoneClient__GetFocusDurationMod_x)
	uintptr_t CharacterZoneClient__GetFocusDurationMod;
#endif
	uintptr_t CharacterZoneClient__GetFocusRangeModifier;
	uintptr_t CharacterZoneClient__GetFocusReuseMod;
	uintptr_t CharacterZoneClient__GetHPRegen;
	uintptr_t CharacterZoneClient__GetItemCountInInventory;
	uintptr_t CharacterZoneClient__GetItemCountWorn;
	uintptr_t CharacterZoneClient__GetLastEffectSlot;
	uintptr_t CharacterZoneClient__GetManaRegen;
	uintptr_t CharacterZoneClient__GetModCap;
	uintptr_t CharacterZoneClient__GetOpenEffectSlot;
#if defined(CharacterZoneClient__GetPctModAndMin_x)
	uintptr_t CharacterZoneClient__GetPctModAndMin;
#endif
	uintptr_t CharacterZoneClient__GetPCSpellAffect;
	uintptr_t CharacterZoneClient__HasSkill;
	uintptr_t CharacterZoneClient__IsStackBlocked;
	uintptr_t CharacterZoneClient__MakeMeVisible;
	uintptr_t CharacterZoneClient__Max_Endurance;
	uintptr_t CharacterZoneClient__Max_HP;
	uintptr_t CharacterZoneClient__Max_Mana;
	uintptr_t CharacterZoneClient__NotifyPCAffectChange;
	uintptr_t CharacterZoneClient__RemovePCAffectex;
	uintptr_t CharacterZoneClient__SpellDuration;
	uintptr_t CharacterZoneClient__TotalEffect;
	uintptr_t CharacterZoneClient__UseSkill;
	uintptr_t ChatManagerClient__Instance;
	uintptr_t CHelpWnd__SetFile;
	uintptr_t CHotButton__SetButtonSize;
	uintptr_t CHotButton__SetCheck;
	uintptr_t CHotButtonWnd__DoHotButton;
	uintptr_t CInvSlot__GetItemBase;
	uintptr_t CInvSlot__HandleRButtonUp;
	uintptr_t CInvSlot__SliderComplete;
	uintptr_t CInvSlot__UpdateItem;
	uintptr_t CInvSlotMgr__MoveItem;
	uintptr_t CInvSlotMgr__SelectSlot;
	uintptr_t CInvSlotWnd__CInvSlotWnd;
	uintptr_t CInvSlotWnd__HandleLButtonUp;
	uintptr_t CItemDisplayManager__CreateWindowInstance;
	uintptr_t CItemDisplayWnd__InsertAugmentRequest;
	uintptr_t CItemDisplayWnd__RemoveAugmentRequest;
	uintptr_t CItemDisplayWnd__RequestConvertItem;
	uintptr_t CItemDisplayWnd__SetItem;
	uintptr_t CItemDisplayWnd__UpdateStrings;
	uintptr_t CKeyRingWnd__ExecuteRightClick;
	uintptr_t CLabel__UpdateText;
	uintptr_t CLargeDialogWnd__Open;
	uintptr_t ClientSOIManager__GetSingleton;
	uintptr_t CListWnd__AddColumn;
	uintptr_t CListWnd__AddColumn1;
	uintptr_t CListWnd__AddLine;
	uintptr_t CListWnd__AddString;
	uintptr_t CListWnd__CalculateCustomWindowPositions;
	uintptr_t CListWnd__CalculateFirstVisibleLine;
	uintptr_t CListWnd__CalculateVSBRange;
	uintptr_t CListWnd__ClearAllSel;
	uintptr_t CListWnd__ClearSel;
	uintptr_t CListWnd__CListWnd;
	uintptr_t CListWnd__CloseAndUpdateEditWindow;
	uintptr_t CListWnd__Compare;
	uintptr_t CListWnd__dCListWnd;
	uintptr_t CListWnd__Draw;
	uintptr_t CListWnd__DrawColumnSeparators;
	uintptr_t CListWnd__DrawHeader;
	uintptr_t CListWnd__DrawItem;
	uintptr_t CListWnd__DrawLine;
	uintptr_t CListWnd__DrawSeparator;
	uintptr_t CListWnd__EnableLine;
	uintptr_t CListWnd__EnsureVisible;
	uintptr_t CListWnd__ExtendSel;
	uintptr_t CListWnd__GetColumnMinWidth;
	uintptr_t CListWnd__GetColumnWidth;
	uintptr_t CListWnd__GetCurSel;
	uintptr_t CListWnd__GetItemData;
	uintptr_t CListWnd__GetItemHeight;
	uintptr_t CListWnd__GetItemRect;
	uintptr_t CListWnd__GetItemText;
	uintptr_t CListWnd__GetItemWnd;
	uintptr_t CListWnd__GetSelList;
	uintptr_t CListWnd__GetSeparatorRect;
	uintptr_t CListWnd__InsertLine;
	uintptr_t CListWnd__RemoveLine;
	uintptr_t CListWnd__SetColors;
	uintptr_t CListWnd__SetColumnJustification;
	uintptr_t CListWnd__SetColumnLabel;
	uintptr_t CListWnd__SetColumnsSizable;
	uintptr_t CListWnd__SetColumnWidth;
	uintptr_t CListWnd__SetCurSel;
	uintptr_t CListWnd__SetItemColor;
	uintptr_t CListWnd__SetItemData;
	uintptr_t CListWnd__SetItemIcon;
	uintptr_t CListWnd__SetItemText;
	uintptr_t CListWnd__SetItemWnd;
	uintptr_t CListWnd__SetVScrollPos;
	uintptr_t CListWnd__Sort;
	uintptr_t CListWnd__ToggleSel;
	uintptr_t CListWnd__vftable;
	uintptr_t CLootWnd__LootAll;
	uintptr_t CLootWnd__RequestLootSlot;
	uintptr_t CMapViewWnd__CMapViewWnd;
	uintptr_t CMemoryMappedFile__SetFile;
	uintptr_t CMerchantWnd__DisplayBuyOrSellPrice;
	uintptr_t CMerchantWnd__PurchasePageHandler__RequestGetItem;
	uintptr_t CMerchantWnd__PurchasePageHandler__RequestPutItem;
	uintptr_t CMerchantWnd__PurchasePageHandler__UpdateList;
	uintptr_t CMerchantWnd__SelectBuySellSlot;
	uintptr_t COptionsWnd__FillChatFilterList;
	uintptr_t CPacketScrambler__hton;
	uintptr_t CPacketScrambler__ntoh;
	uintptr_t CPageWnd__FlashTab;
	uintptr_t CPageWnd__SetTabText;
	uintptr_t CQuantityWnd__Open;
	uintptr_t CResolutionHandler__GetWindowedStyle;
	uintptr_t CResolutionHandler__UpdateResolution;
	uintptr_t CScreenPieceTemplate__IsType;
	uintptr_t CSidlManager__CreateHotButtonWnd;
	uintptr_t CSidlManager__CreateXWnd;
	uintptr_t CSidlManagerBase__CreateXWnd;
	uintptr_t CSidlManagerBase__CreateXWndFromTemplate;
	uintptr_t CSidlManagerBase__CreateXWndFromTemplate1;
	uintptr_t CSidlManagerBase__FindAnimation1;
	uintptr_t CSidlManagerBase__FindButtonDrawTemplate;
	uintptr_t CSidlManagerBase__FindScreenPieceTemplate;
	uintptr_t CSidlManagerBase__FindScreenPieceTemplate1;
	uintptr_t CSidlScreenWnd__CalculateHSBRange;
	uintptr_t CSidlScreenWnd__CalculateVSBRange;
	uintptr_t CSidlScreenWnd__ConvertToRes;
	uintptr_t CSidlScreenWnd__CreateChildrenFromSidl;
	uintptr_t CSidlScreenWnd__CSidlScreenWnd1;
	uintptr_t CSidlScreenWnd__CSidlScreenWnd2;
	uintptr_t CSidlScreenWnd__dCSidlScreenWnd;
	uintptr_t CSidlScreenWnd__DrawSidlPiece;
	uintptr_t CSidlScreenWnd__EnableIniStorage;
	uintptr_t CSidlScreenWnd__GetChildItem;
	uintptr_t CSidlScreenWnd__GetSidlPiece;
	uintptr_t CSidlScreenWnd__HandleLButtonUp;
	uintptr_t CSidlScreenWnd__Init1;
	uintptr_t CSidlScreenWnd__LoadIniListWnd;
	uintptr_t CSidlScreenWnd__LoadSidlScreen;
	uintptr_t CSidlScreenWnd__m_layoutCopy;
	uintptr_t CSidlScreenWnd__StoreIniVis;
	uintptr_t CSidlScreenWnd__vftable;
	uintptr_t CSkillMgr__GetNameToken;
	uintptr_t CSkillMgr__GetSkillCap;
	uintptr_t CSkillMgr__IsActivatedSkill;
	uintptr_t CSkillMgr__IsAvailable;
	uintptr_t CSkillMgr__IsCombatSkill;
	uintptr_t CSkillMgr__GetSkillTimerDuration;
	uintptr_t CSkillMgr__GetSkillLastUsed;
	uintptr_t CSliderWnd__GetValue;
	uintptr_t CSliderWnd__SetNumTicks;
	uintptr_t CSliderWnd__SetValue;
	uintptr_t CSpellBookWnd__MemorizeSet;
	uintptr_t CSpellDisplayManager__ShowSpell;
	uintptr_t CSpellDisplayWnd__SetSpell;
	uintptr_t CSpellDisplayWnd__UpdateStrings;
	uintptr_t CSpellDisplayWnd__vftable;
	uintptr_t CStmlWnd__AppendSTML;
	uintptr_t CStmlWnd__CalculateHSBRange;
	uintptr_t CStmlWnd__CalculateVSBRange;
	uintptr_t CStmlWnd__FastForwardToEndOfTag;
	uintptr_t CStmlWnd__ForceParseNow;
	uintptr_t CStmlWnd__GetNextTagPiece;
	uintptr_t CStmlWnd__GetVisibleText;
	uintptr_t CStmlWnd__MakeStmlColorTag;
	uintptr_t CStmlWnd__MakeWndNotificationTag;
	uintptr_t CStmlWnd__SetSTMLText;
	uintptr_t CStmlWnd__StripFirstSTMLLines;
	uintptr_t CStmlWnd__UpdateHistoryString;
	uintptr_t CTabWnd__Draw;
	uintptr_t CTabWnd__DrawCurrentPage;
	uintptr_t CTabWnd__DrawTab;
	uintptr_t CTabWnd__GetTabRect;
	uintptr_t CTabWnd__InsertPage;
	uintptr_t CTabWnd__RemovePage;
	uintptr_t CTabWnd__SetPage;
	uintptr_t CTabWnd__UpdatePage;
	uintptr_t CTargetManager__Get;
	uintptr_t CTargetWnd__GetBuffCaster;
	uintptr_t CTargetWnd__HandleBuffRemoveRequest;
	uintptr_t CTargetWnd__RefreshTargetBuffs;
	uintptr_t CTargetWnd__WndNotification;
	uintptr_t CTaskManager__GetElementDescription;
	uintptr_t CTaskManager__GetEntry;
	uintptr_t CTaskManager__GetTaskStatus;
	uintptr_t CTaskWnd__ConfirmAbandonTask;
	uintptr_t CTaskWnd__UpdateTaskTimers;
	uintptr_t CTextOverlay__DisplayText;
	uintptr_t CTextureAnimation__Draw;
	uintptr_t CTextureAnimation__SetCurCell;
	uintptr_t CTextureFont__DrawWrappedText;
	uintptr_t CTextureFont__DrawWrappedText1;
	uintptr_t CTextureFont__DrawWrappedText2;
	uintptr_t CTextureFont__GetHeight;
	uintptr_t CTextureFont__GetTextExtent;
	uintptr_t CTribute__GetActiveFavorCost;
	uintptr_t CUnSerializeBuffer__GetString;
	uintptr_t CWndDisplayManager__FindWindow;
	uintptr_t CXMLDataManager__GetXMLData;
	uintptr_t CXMLSOMDocumentBase__XMLRead;
	uintptr_t CXStr__gCXStrAccess;
	uintptr_t CXStr__gFreeLists;
	uintptr_t CXWnd__BringToTop;
	uintptr_t CXWnd__ClrFocus;
	uintptr_t CXWnd__CXWnd;
	uintptr_t CXWnd__dCXWnd;
	uintptr_t CXWnd__Destroy;
	uintptr_t CXWnd__DoAllDrawing;
	uintptr_t CXWnd__DrawColoredRect;
	uintptr_t CXWnd__DrawTooltip;
	uintptr_t CXWnd__DrawTooltipAtPoint;
	uintptr_t CXWnd__GetChildItem;
	uintptr_t CXWnd__GetChildWndAt;
	uintptr_t CXWnd__GetClientClipRect;
	uintptr_t CXWnd__GetRelativeRect;
	uintptr_t CXWnd__GetScreenClipRect;
	uintptr_t CXWnd__GetScreenRect;
	uintptr_t CXWnd__GetTooltipRect;
	uintptr_t CXWnd__IsActive;
	uintptr_t CXWnd__IsDescendantOf;
	uintptr_t CXWnd__IsReallyVisible;
	uintptr_t CXWnd__IsType;
	uintptr_t CXWnd__Minimize;
	uintptr_t CXWnd__ProcessTransition;
	uintptr_t CXWnd__Refade;
	uintptr_t CXWnd__Resize;
	uintptr_t CXWnd__Right;
	uintptr_t CXWnd__SetFocus;
	uintptr_t CXWnd__SetFont;
	uintptr_t CXWnd__SetKeyTooltip;
	uintptr_t CXWnd__SetMouseOver;
	uintptr_t CXWnd__SetParent;
	uintptr_t CXWnd__StartFade;
	uintptr_t CXWnd__vftable;
	uintptr_t CXWndManager__DrawCursor;
	uintptr_t CXWndManager__DrawWindows;
	uintptr_t CXWndManager__GetKeyboardFlags;
	uintptr_t CXWndManager__HandleKeyboardMsg;
	uintptr_t CXWndManager__RemoveWnd;
	uintptr_t DrawNetStatus;
	uintptr_t EQ_LoadingS__Array;
	uintptr_t EQ_LoadingS__SetProgressBar;
#ifdef EQ_Spell__GetSpellAffectBySlot_x
	uintptr_t EQ_Spell__GetSpellAffectBySlot;
#endif
	uintptr_t EQ_Spell__GetSpellLevelNeeded;
	uintptr_t EQ_Spell__IsDegeneratingLevelMod;
	uintptr_t EQ_Spell__IsSPAIgnoredByStacking;
	uintptr_t EQ_Spell__IsSPAStacking;
	uintptr_t EQ_Spell__SpellAffectBase;
	uintptr_t EQ_Spell__SpellAffects;
	uintptr_t EQGroundItemListManager__Instance;
	uintptr_t EQItemList__add_item;
	uintptr_t EQItemList__delete_item;
	uintptr_t EQItemList__EQItemList;
	uintptr_t EQItemList__FreeItemList;
	uintptr_t EQPlacedItemManager__GetItemByGuid;
	uintptr_t EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds;
	uintptr_t EQPlacedItemManager__Instance;
	uintptr_t EqSoundManager__PlayScriptMp3;
	uintptr_t EqSoundManager__SoundAssistPlay;
	uintptr_t EqSoundManager__WaveInstancePlay;
	uintptr_t EqSoundManager__WavePlay;
#if defined(pinstEQSpellStrings_x)
	uintptr_t EQSpellStrings__GetString;
#endif
	uintptr_t EQSwitch__UseSwitch;
	uintptr_t FactionManagerClient__HandleFactionMessage;
	uintptr_t FactionManagerClient__Instance;
	uintptr_t FreeTargetTracker__CastSpell;
	uintptr_t FreeToPlayClient__Instance;
	uintptr_t FreeToPlayClient__RestrictionInfo;
	uintptr_t IconCache__GetIcon;
	uintptr_t ItemBase__CanGemFitInSlot;
	uintptr_t ItemBase__CreateItemTagString;
	uintptr_t ItemBase__GetImageNum;
	uintptr_t ItemBase__GetItemValue;
	uintptr_t ItemBase__IsEmpty;
	uintptr_t ItemBase__IsKeyRingItem;
	uintptr_t ItemBase__IsLore;
	uintptr_t ItemBase__IsLoreEquipped;
	uintptr_t ItemBase__ValueSellMerchant;
	uintptr_t ItemClient__CanDrop;
	uintptr_t ItemClient__CanGoInBag;
	uintptr_t ItemClient__CreateItemClient;
	uintptr_t ItemClient__dItemClient;
	uintptr_t KeyCombo__GetTextDescription;
	uintptr_t KeypressHandler__AttachAltKeyToEqCommand;
	uintptr_t KeypressHandler__AttachKeyToEqCommand;
	uintptr_t KeypressHandler__ClearCommandStateArray;
	uintptr_t KeypressHandler__Get;
	uintptr_t KeypressHandler__HandleKeyDown;
	uintptr_t KeypressHandler__HandleKeyUp;
	uintptr_t KeypressHandler__SaveKeymapping;
#if HAS_ADVANCED_LOOT
	uintptr_t LootFiltersManager__AddItemLootFilter;
	uintptr_t LootFiltersManager__GetItemFilterData;
	uintptr_t LootFiltersManager__RemoveItemLootFilter;
	uintptr_t LootFiltersManager__SetItemLootFilter;
#endif
	uintptr_t MapViewMap__Clear;
	uintptr_t MapViewMap__SetZoom;
	uintptr_t MapViewMap__vftable;
	uintptr_t MercenaryAlternateAdvancementManagerClient__BuyAbility;
	uintptr_t MercenaryAlternateAdvancementManagerClient__Instance;
	uintptr_t msg_new_text;
	uintptr_t msg_spell_worn_off;
	uintptr_t msgTokenText;
	uintptr_t MultipleItemMoveManager__ProcessMove;
	uintptr_t PcBase__GetAlternateAbilityId;
	uintptr_t PcBase__GetCombatAbility;
	uintptr_t PcBase__GetCombatAbilityTimer;
	uintptr_t PcBase__GetItemContainedRealEstateIds;
	uintptr_t PcBase__GetNonArchivedOwnedRealEstates;
	uintptr_t PcClient__AlertInventoryChanged;
	uintptr_t PcClient__GetConLevel;
	uintptr_t PcClient__HasLoreItem;
	uintptr_t PcZoneClient__BandolierSwap;
	uintptr_t PcZoneClient__CanEquipItem;
	uintptr_t PcZoneClient__DestroyHeldItemOrMoney;
	uintptr_t PcZoneClient__doCombatAbility;
	uintptr_t PcZoneClient__GetItemByID;
	uintptr_t PcZoneClient__GetItemRecastTimer;
	uintptr_t PcZoneClient__GetPcSkillLimit;
	uintptr_t PcZoneClient__HasAlternateAbility;
	uintptr_t PcZoneClient__HasCombatAbility;
	uintptr_t PcZoneClient__RemoveBuffEffect;
	uintptr_t PcZoneClient__RemoveMyAffect;
	uintptr_t PcZoneClient__RemovePetEffect;
#if HAS_ADVANCED_LOOT
	uintptr_t pinstLootFiltersManager;
#endif
	uintptr_t PlayerBase__CanSee;
	uintptr_t PlayerBase__CanSee1;
	uintptr_t PlayerBase__GetVisibilityLineSegment;
	uintptr_t PlayerBase__HasProperty;
	uintptr_t PlayerBase__IsTargetable;
	uintptr_t PlayerClient__ChangeBoneStringSprite;
	uintptr_t PlayerClient__GetPcClient;
	uintptr_t PlayerClient__PlayerClient;
	uintptr_t PlayerClient__SetNameSpriteState;
	uintptr_t PlayerClient__SetNameSpriteTint;
	uintptr_t PlayerManagerBase__DestroyAllPlayers;
	uintptr_t PlayerManagerBase__PrepForDestroyPlayer;
	uintptr_t PlayerManagerClient__CreatePlayer;
	uintptr_t PlayerManagerClient__GetPlayerFromPartialName;
	uintptr_t PlayerManagerClient__GetSpawnByID;
	uintptr_t PlayerManagerClient__GetSpawnByName;
	uintptr_t PlayerPointManager__GetAltCurrency;
	uintptr_t PlayerZoneClient__ChangeHeight;
	uintptr_t PlayerZoneClient__DoAttack;
	uintptr_t PlayerZoneClient__GetLevel;
	uintptr_t PlayerZoneClient__IsValidTeleport;
	uintptr_t ProfileManager__GetCurrentProfile;
	uintptr_t RealEstateManagerClient__Instance;
	uintptr_t SpellManager__GetSpellByGroupAndRank;
	uintptr_t Spellmanager__LoadTextSpells;
	uintptr_t StringTable__getString;
	uintptr_t Teleport_Table_Size;
	uintptr_t Teleport_Table;
	uintptr_t UdpConnection__GetStats;
	uintptr_t Util__FastTime;
	uintptr_t Window__getProgress;
	uintptr_t Window__getStatus;
	uintptr_t Window__getURI;
	uintptr_t ZoneGuideManagerClient__Instance;

	//----------------------------------------------------------------------------
	// Instance Pointers
	//----------------------------------------------------------------------------

	// Raw pointers (value types in eq)
	CMDLIST* EQADDR_CMDLIST;
	IDirectInputDevice8A** EQADDR_DIKEYBOARD;
	IDirectInputDevice8A** EQADDR_DIMOUSE;
	PPOINT                                     EQADDR_DIMOUSECHECK;
	MQMouseInfo* EQADDR_MOUSE;
	uintptr_t                                  EQADDR_HWND;
	char* EQADDR_SERVERHOST;
	char** EQMappableCommandList;
	BYTE* EQbCommandStates;
	HMODULE* ghEQMainInstance;
	DWORD* gpbCommandEvent;
	DWORD* gpMouseEventTime;
	CDynamicZone* pDynamicZone;
	CTribute* pTribute;
	CTribute* pEQMisc;
	CEQSuiteTextureLoader* pEQSuiteTextureLoader;
	INT* pgCurrentSocial;
	CGuild* pGuild;
	CGuild* pGuildList;
	CRaid* pRaid;
	DWORD* pScreenMode;

	SpellLoadout* pSpellSets;
	CTaskManager* pTaskManager;
	BYTE* pTributeActive;
	ZONEINFO* pZoneInfo;
	SoeUtil::String* pExceptionSubmissionEndpoint;
	EQLogin* pEQLogin;
	LabelCache* g_labelCache;

	// pEverQuestInfo pointers
	EverQuestinfo* pEverQuestInfo;
	int* pScreenX;
	int* pScreenY;
	int* pScreenXMax;
	int* pScreenYMax;
	uint8_t* pMouseLook;
	bool* gpbShowNetStatus;
	bool* gpShiftKeyDown;
	EQSOCIAL* pSocialList;
	BYTE* gpAutoFire;

	uint8_t* EQADDR_NOTINCHATMODE;
	uint8_t* EQADDR_RUNWALKSTATE;
	uint8_t* EQADDR_ZONETYPE;
	char* EQADDR_SERVERNAME;
	int* EQADDR_DOABILITYLIST;
	BYTE* EQADDR_ATTACK;


	// Spawn/Char related
	ForeignPointer<PcClient>                   pCharData /**/;
	ForeignPointer<PcClient>                   pPCData /**/;
	ForeignPointer<PcClient>                   pLocalPC;
	ForeignPointer<PlayerClient>               pActiveBanker;
	ForeignPointer<PlayerClient>               pActiveCorpse;
	ForeignPointer<PlayerClient>               pActiveGMaster;
	ForeignPointer<PlayerClient>               pActiveMerchant;
	ForeignPointer<PlayerClient>               pCharSpawn /**/;
	ForeignPointer<PlayerClient>               pControlledPlayer; // the controlled player (mount, eye of zomm, etc)
	ForeignPointer<PlayerClient>               pLocalPlayer;      // the local player
	ForeignPointer<PlayerClient>               pTarget;
	ForeignPointer<PlayerClient>               pTradeTarget;

	// Non-UI Foreign Pointers (pointer types in eq)
	ComputedPointer<AggroMeterManagerClient>   pAggroInfo;
	ForeignPointer<AltAdvManager>              pAltAdvManager;
	ComputedPointer<ClientAuraManager>         pAuraMgr;
	ForeignPointer<CChatWindowManager>         pChatManager;
	ComputedPointer<UniversalChatProxy>        pChatService;
	ForeignPointer<UdpLibrary::UdpConnection>  pConnection;
	ForeignPointer<CContainerMgr>              pContainerMgr;
	ForeignPointer<CContextMenuManager>        pContextMenuManager;
	ForeignPointer<DatabaseStringTable>        pDBStr;
	ForeignPointer<MAPLABEL>                   pCurrentMapLabel;
	ForeignPointer<CDisplay>                   pDisplay;
	ForeignPointer<DynamicZonePlayerInfo>      pDZMember;
	ForeignPointer<DynamicZoneClientTimerData> pDZTimerInfo;
	ForeignPointer<EqSoundManager>             pEqSoundManager;
#if defined(pinstEQSpellStrings_x)
	ForeignPointer<EQSpellStrings>             pEQSpellStrings;
#endif
	ForeignPointer<CEverQuest, EVERQUEST>      pEverQuest;
	ForeignPointer<SGraphicsEngine>            pGraphicsEngine;
	ForeignPointer<CInvSlotMgr>                pInvSlotMgr;
	ForeignPointer<CItemDisplayManager>        pItemDisplayManager;
	ForeignPointer<CSpellDisplayManager>       pSpellDisplayManager;
	ComputedPointer<EQGroundItemListManager>   pItemList;
#if defined(KeypressHandler__Get_x)
	ComputedPointer<KeypressHandler>           pKeypressHandler;
#elif defined (pinstKeypressHandler_x)
	ForeignPointer<KeypressHandler>            pKeypressHandler;
#endif
#if HAS_ADVANCED_LOOT
	ForeignPointer<LootFiltersManager>         pLootFiltersManager;
#endif
#if HAS_MERCENARY_AA
	ComputedPointer<MercenaryAlternateAdvancementManagerClient> pMercAltAbilities;
#endif
	ForeignPointer<CMercenaryManager>          pMercInfo;
	ForeignPointer<CMercenaryManager>          pMercManager;
	ComputedPointer<CPlayerPointManager>       pPlayerPointManager;
	ComputedPointer<RealEstateManagerClient>   pRealEstate;
	ForeignPointer<CResolutionHandler>         pResolutionHandler;
	ForeignPointer<CSidlManager>               pSidlMgr;
	ForeignPointer<SkillManager>               pSkillMgr;
	ForeignPointer<SkillManager>               pCSkillMgr;
	ComputedPointer<PlayerClient>              pSpawnList;
	ForeignPointer<PlayerManagerClient>        pSpawnManager;
	ForeignPointer<ClientSpellManager>         pSpellMgr;
	ForeignPointer<StringTable>                pStringTable;
	ForeignPointer<EqSwitchManager>            pSwitchMgr;
	ForeignPointer<SharedTaskPlayerInfo>       pTaskMember;
	ComputedPointer<CBroadcast>                pTextOverlay;
	ForeignPointer<CXWndManager>               pWndMgr;
	ForeignPointer<EQWorldData>                pWorldData;

#define pCResolutionHandler pResolutionHandler
#define pCDBStr pDBStr

	// UI Window Instance Pointers
	ForeignPointer<CAAWnd>                     pAAWnd;
	ForeignPointer<CAchievementsWnd>           pAchievementsWnd;
	ForeignPointer<CActionsWnd>                pActionsWnd;
	ForeignPointer<CSidlScreenWnd>             pAdvancedDisplayOptionsWnd;
	ForeignPointer<CAdvancedLootWnd>           pAdvancedLootWnd;
	ForeignPointer<CSidlScreenWnd>             pAdventureLeaderboardWnd;
	ForeignPointer<CSidlScreenWnd>             pAdventureRequestWnd;
	ForeignPointer<CSidlScreenWnd>             pAdventureStatsWnd;
	ForeignPointer<CSidlScreenWnd>             pAggroMeterWnd;
	ForeignPointer<CAlarmWnd>                  pAlarmWnd;
	ForeignPointer<CSidlScreenWnd>             pAlertHistoryWnd;
	ForeignPointer<CSidlScreenWnd>             pAlertStackWnd;
	ForeignPointer<CSidlScreenWnd>             pAlertWnd;
	ForeignPointer<CSidlScreenWnd>             pAltStorageWnd;
	ForeignPointer<CSidlScreenWnd>             pAudioTriggersWnd;
	ForeignPointer<CAuraWnd>                   pAuraWnd;
	ForeignPointer<CBandolierWnd>              pBandolierWnd;
	ForeignPointer<CBankWnd>                   pBankWnd;
	ForeignPointer<CBarterWnd>                 pBarterWnd;
	ForeignPointer<CSidlScreenWnd>             pBarterMerchantWnd;
	ForeignPointer<CBarterSearchWnd>           pBarterSearchWnd;
	ForeignPointer<CSidlScreenWnd>             pBazaarConfirmationWnd;
	ForeignPointer<CBazaarSearchWnd>           pBazaarSearchWnd;
	ForeignPointer<CBazaarWnd>                 pBazaarWnd;
	ForeignPointer<CSidlScreenWnd>             pBlockedBuffWnd;
	ForeignPointer<CSidlScreenWnd>             pBlockedPetBuffWnd;
	ForeignPointer<CBodyTintWnd>               pBodyTintWnd;
	ForeignPointer<CBookWnd>                   pBookWnd;
	ForeignPointer<CBreathWnd>                 pBreathWnd;
	ForeignPointer<CBuffWindow>                pBuffWnd;
	ForeignPointer<CBuffWindow>                pSongWnd;
	ForeignPointer<CBugReportWnd>              pBugReportWnd;
	ForeignPointer<CCastingWnd>                pCastingWnd;
	ForeignPointer<CCastSpellWnd>              pCastSpellWnd;
	ForeignPointer<CSidlScreenWnd>             pCharacterCreation;
	ForeignPointer<CCharacterListWnd>          pCharacterListWnd;
	ForeignPointer<ClaimWindow>                pClaimWnd;
	ForeignPointer<CColorPickerWnd>            pColorPickerWnd;
	ForeignPointer<CCombatAbilityWnd>          pCombatAbilityWnd;
	ForeignPointer<CCombatSkillsSelectWnd>     pCombatSkillsSelectWnd;
	ForeignPointer<CCompassWnd>                pCompassWnd;
	ForeignPointer<CSidlScreenWnd>             pContextTipWnd;
	ForeignPointer<CCursorAttachment>          pCursorAttachment;
	ForeignPointer<CSidlScreenWnd>             pDragonHoardWnd;
	ForeignPointer<CSidlScreenWnd>             pDynamicZoneWnd;
	ForeignPointer<CEditLabelWnd>              pEditLabelWnd;
	ForeignPointer<CEQMainWnd>                 pEQMainWnd;
	ForeignPointer<CSidlScreenWnd>             pEventCalendarWnd;
	ForeignPointer<CExtendedTargetWnd>         pExtendedTargetWnd;
	ForeignPointer<CFactionWnd>                pFactionWnd;
	ForeignPointer<CFeedbackWnd>               pFeedbackWnd;
	ForeignPointer<CSidlScreenWnd>             pFellowshipWnd;
	ForeignPointer<CFileSelectionWnd>          pFileSelectionWnd;
	ForeignPointer<CFindItemWnd>               pFindItemWnd;
	ForeignPointer<CFindLocationWnd>           pFindLocationWnd;
	ForeignPointer<CFriendsWnd>                pFriendsWnd;
	ForeignPointer<CGemsGameWnd>               pGemsGameWnd;
	ForeignPointer<CGiveWnd>                   pGiveWnd;
	ForeignPointer<CGroupSearchFiltersWnd>     pGroupSearchFiltersWnd;
	ForeignPointer<CGroupSearchWnd>            pGroupSearchWnd;
	ForeignPointer<CGroupWnd>                  pGroupWnd;
	ForeignPointer<CSidlScreenWnd>             pGuildBankWnd;
	ForeignPointer<CSidlScreenWnd>             pGuildCreationWnd;
	ForeignPointer<CGuildMgmtWnd>              pGuildMgmtWnd;
	ForeignPointer<CHelpWnd>                   pHelpWnd;
	ForeignPointer<CSidlScreenWnd>             pHeritageSelectionWnd;
	ForeignPointer<CHotButtonWnd>              pHotButtonWnd;  // Simply points to the first
	ForeignPointer<CHotButtonWnd>              pHotButtonWnds[MAX_HOTBUTTON_WNDS];
	ForeignPointer<CSidlScreenWnd>             pIconSelectionWnd;
	ForeignPointer<CInspectWnd>                pInspectWnd;
	ForeignPointer<CInventoryWnd>              pInventoryWnd;
	ForeignPointer<CSidlScreenWnd>             pItemExpTransferWnd;
	ForeignPointer<CSidlScreenWnd>             pItemFuseWnd;
	ForeignPointer<CSidlScreenWnd>             pItemOverflowWnd;
	ForeignPointer<CJournalCatWnd>             pJournalCatWnd;
	ForeignPointer<CJournalTextWnd>            pJournalTextWnd;
#if HAS_KEYRING_WINDOW
	ForeignPointer<CKeyRingWnd>                pKeyRingWnd;
#endif // HAS_KEYRING_WINDOW
	ForeignPointer<CLargeDialogWnd>            pLargeDialog;
	ForeignPointer<CSidlScreenWnd>             pLayoutCopyWnd;
	ForeignPointer<CSidlScreenWnd>             pLFGuildWnd;
	ForeignPointer<CLoadskinWnd>               pLoadskinWnd;
	ForeignPointer<CSidlScreenWnd>             pLootFiltersCopyWnd;
	ForeignPointer<CSidlScreenWnd>             pLootFiltersWnd;
	ForeignPointer<CSidlScreenWnd>             pLootSettingsWnd;
	ForeignPointer<CLootWnd>                   pLootWnd;
	ForeignPointer<CSidlScreenWnd>             pMailAddressBookWnd;
	ForeignPointer<CSidlScreenWnd>             pMailCompositionWnd;
	ForeignPointer<CSidlScreenWnd>             pMailIgnoreListWindow;
	ForeignPointer<CSidlScreenWnd>             pMailWnd;
	ForeignPointer<CSidlScreenWnd>             pManageLootWnd;
	ForeignPointer<CMapToolbarWnd>             pMapToolbarWnd;
	ForeignPointer<CMapViewWnd>                pMapViewWnd;
	ForeignPointer<CMarketplaceWnd>            pMarketplaceWnd;
	ForeignPointer<CMerchantWnd>               pMerchantWnd;
	ForeignPointer<CSidlScreenWnd>             pMIZoneSelectWnd;
	ForeignPointer<CMusicPlayerWnd>            pMusicPlayerWnd;
	ForeignPointer<CSidlScreenWnd>             pNameChangeMercWnd;
	ForeignPointer<CSidlScreenWnd>             pNameChangePetWnd;
	ForeignPointer<CSidlScreenWnd>             pNameChangeWnd;
	ForeignPointer<CNoteWnd>                   pNoteWnd;
	ForeignPointer<CSidlScreenWnd>             pObjectPreviewWnd;
	ForeignPointer<COptionsWnd>                pOptionsWnd;
	ForeignPointer<COverseerWnd>               pOverseerWnd;
	ForeignPointer<CPetInfoWnd>                pPetInfoWnd;
	ForeignPointer<CPetitionQWnd>              pPetitionQWnd;
	ForeignPointer<CPlayerCustomizationWnd>    pPlayerCustomizationWnd;
	ForeignPointer<CPlayerNotesWnd>            pPlayerNotesWnd;
	ForeignPointer<CPlayerWnd>                 pPlayerWnd;
	ForeignPointer<CSidlScreenWnd>             pProgressionSelectionWnd;
	ForeignPointer<CPurchaseGroupWnd>          pPurchaseGroupWnd;
	ForeignPointer<CSidlScreenWnd>             pPurchaseWnd;
	ForeignPointer<CSidlScreenWnd>             pPvPLeaderboardWnd;
	ForeignPointer<CSidlScreenWnd>             pPvPStatsWnd;
	ForeignPointer<CQuantityWnd>               pQuantityWnd;
	ForeignPointer<CSidlScreenWnd>             pRaceChangeWnd;
	ForeignPointer<CRaidOptionsWnd>            pRaidOptionsWnd;
	ForeignPointer<CRaidWnd>                   pRaidWnd;
	ForeignPointer<CRealEstateItemsWnd>        pRealEstateItemsWnd;
	ForeignPointer<CSidlScreenWnd>             pRealEstateLayoutDetailsWnd;
	ForeignPointer<CSidlScreenWnd>             pRealEstateManageWnd;
	ForeignPointer<CSidlScreenWnd>             pRealEstateNeighborhoodWnd;
	ForeignPointer<CSidlScreenWnd>             pRealEstatePlotSearchWnd;
	ForeignPointer<CSidlScreenWnd>             pRealEstatePurchaseWnd;
	ForeignPointer<CRespawnWnd>                pRespawnWnd;
	ForeignPointer<CRewardSelectionWnd>        pRewardSelectionWnd;
	ForeignPointer<CSelectorWnd>               pSelectorWnd;
	ForeignPointer<CSidlScreenWnd>             pSendMoneyWnd;
	ForeignPointer<CSidlScreenWnd>             pServerListWnd;
	ForeignPointer<CSkillsSelectWnd>           pSkillsSelectWnd;
	ForeignPointer<CSkillsWnd>                 pSkillsWnd;
	ForeignPointer<CSocialEditWnd>             pSocialEditWnd;
	ForeignPointer<CSidlScreenWnd>             pSocialWnd;
	ForeignPointer<CSpellBookWnd>              pSpellBookWnd;
	ForeignPointer<CStoryWnd>                  pStoryWnd;
	ForeignPointer<CSidlScreenWnd>             pTargetOfTargetWnd;
	ForeignPointer<CTargetWnd>                 pTargetWnd;
	ForeignPointer<CSidlScreenWnd>             pTaskOverlayWnd;
	ForeignPointer<CSidlScreenWnd>             pTaskSelectWnd;
	ForeignPointer<CSidlScreenWnd>             pTaskTemplateSelectWnd;
	ForeignPointer<CTaskWnd>                   pTaskWnd;
	ForeignPointer<CTextEntryWnd>              pTextEntryWnd;
	ForeignPointer<CTipWnd>                    pTipOfDayWnd;
	ForeignPointer<CSidlScreenWnd>             pTitleWnd;
	ForeignPointer<CTrackingWnd>               pTrackingWnd;
	ForeignPointer<CSidlScreenWnd>             pTradeskillWnd;
	ForeignPointer<CTradeWnd>                  pTradeWnd;
	ForeignPointer<CTrainWnd>                  pTrainWnd;
	ForeignPointer<CSidlScreenWnd>             pTributeBenefitWnd;
	ForeignPointer<CSidlScreenWnd>             pTributeMasterWnd;
	ForeignPointer<CSidlScreenWnd>             pTributeTrophyWnd;
	ForeignPointer<CVideoModesWnd>             pVideoModesWnd;
	ForeignPointer<CSidlScreenWnd>             pVoiceMacroWnd;
	ForeignPointer<CSidlScreenWnd>             pVoteResultsWnd;
	ForeignPointer<CSidlScreenWnd>             pVoteWnd;
	ForeignPointer<CZoneGuideWnd>              pZoneGuideWnd;
	ForeignPointer<CZonePathWnd>               pZonePathWnd;

	ForeignPointer<CRender>                    g_pDrawHandler;
	ForeignPointer<IDirectInputDevice8A>       g_pDIKeyboard;
	ForeignPointer<IDirectInputDevice8A>       g_pDIMouse;
	DIMOUSESTATE2* g_pDIMouseState;
	SDeviceInputProxy* g_pDeviceInputProxy;

	//----------------------------------------------------------------------------

#define ScreenMode (*pScreenMode)
#define ScreenX (*pScreenX)
#define ScreenXMax (*pScreenXMax)
#define ScreenY (*pScreenY)
#define ScreenYMax (*pScreenYMax)
#define bMouseLook (*pMouseLook)
#define gbCommandEvent (*gpbCommandEvent)
#define gbShowNetStatus (*gpbShowNetStatus)
#define gCurrentSocial (*pgCurrentSocial)
#define gMouseEventTime (*gpMouseEventTime)
#define gShiftKeyDown (*gpShiftKeyDown)

// TODO: These should just be turned into regular functions like the rest
	using fEQNewUIINI = char* (*)();
	using fEQProcGameEvts = int    (*)();
	using fGetLabelFromEQ = bool   (*)(int, CXStr*, bool*, COLORREF*);

	fEQNewUIINI          NewUIINI;
	fEQProcGameEvts      ProcessGameEvents;
	fGetLabelFromEQ      GetLabelFromEQ;

	uintptr_t __ModuleList;
	uintptr_t __ProcessList;

	const char* szExpansions[NUM_EXPANSIONS + 1];

	//============================================================================
	// EQGraphicsDX9.dll Offsets
	//============================================================================

	uintptr_t __eqgraphics_fopen;
	uintptr_t CEQGBitmap__GetFirstBitmap;
	uintptr_t CParticleSystem__Render;
	uintptr_t CParticleSystem__CreateSpellEmitter;
	uintptr_t CRender__RenderScene;
	uintptr_t CRender__RenderBlind;
	uintptr_t CRender__UpdateDisplay;
	uintptr_t CRender__ResetDevice;
	BOOL* g_bRenderSceneCalled;
	uintptr_t C2DPrimitiveManager__AddCachedText;
	uintptr_t C2DPrimitiveManager__Render;
	uintptr_t ObjectPreviewView__Render;

	//============================================================================
	// eqmain.dll Offsets
	//============================================================================

	uintptr_t EQMain__CEQSuiteTextureLoader__GetTexture;
	uintptr_t EQMain__CLoginViewManager__HandleLButtonUp;
	uintptr_t EQMain__CXWndManager__GetCursorToDisplay;
	uintptr_t EQMain__LoginController__GiveTime;
	uintptr_t EQMain__LoginController__ProcessKeyboardEvents;
	uintptr_t EQMain__LoginController__ProcessMouseEvents;
	uintptr_t EQMain__LoginController__Shutdown;
	uintptr_t EQMain__LoginServerAPI__JoinServer;
	uintptr_t EQMain__WndProc;

	uintptr_t EQMain__pinstCEQSuiteTextureLoader;
	uintptr_t EQMain__pinstCLoginViewManager;
	uintptr_t EQMain__pinstCSidlManager;
	uintptr_t EQMain__pinstCXWndManager;
	uintptr_t EQMain__pinstLoginController;
	uintptr_t EQMain__pinstLoginServerAPI;
	uintptr_t EQMain__pinstLoginClient;

	ForeignPointer<CLoginViewManager> g_pLoginViewManager;
	ForeignPointer<LoginClient> g_pLoginClient;
	ForeignPointer<LoginController> g_pLoginController;
	ForeignPointer<LoginServerAPI> g_pLoginServerAPI;

	//----------------------------------------------------------------------------
	// Global Functions

	/* OTHER IMPORTED FROM EQ */
	int CastRay(PlayerClient*, float y, float x, float z);
	int CastRayLoc(const CVector3& SourcePos, EQRace Race, float DestX, float DestY, float DestZ);
	float HeadingDiff(float h1, float h2, float* DiffOut);
	float FixHeading(float Heading);
	float get_bearing(float x1, float y1, float x2, float y2);
	unsigned long GetFastTime();
	void FlushDxKeyboard();
	DWORD EQGetTime();
	EQLIB_OBJECT CXStr STMLToText(const CXStr& In, bool bReplaceBrWithNewline = true);
	class IconCache* __cdecl GetAnimationCache(int index);
	void SaveColors(int, int, int, int);
	bool EQExecuteCmd(unsigned int command, bool keyDown, void* data, const KeyCombo* combo);
	bool CopyLayout(const CXStr& currlayout, const CXStr& newlayout, bool bHotbuttons, bool bLoadouts,
		bool bSocials, CXStr& ErrorOut, bool bForceReload = false);
	uint32_t GetBufferCRC(const char* szBuffer, size_t bufferLength, int baseValue = 0);
	uint32_t GetStringCRC(std::string_view);

	//----------------------------------------------------------------------------
	// FIXME: Remove these macros
	//#define indoor (((*EQADDR_ZONETYPE) == 0) || ((*EQADDR_ZONETYPE) == 3) || ((*EQADDR_ZONETYPE) == 4))
	//#define outdoor (((*EQADDR_ZONETYPE) == 1) || ((*EQADDR_ZONETYPE) == 2) || ((*EQADDR_ZONETYPE) == 5))
	//#define bindable (((*EQADDR_ZONETYPE) == 2) || ((*EQADDR_ZONETYPE) == 3) || ((*EQADDR_ZONETYPE) == 4))

	//----------------------------------------------------------------------------

	void InitializeGlobals();

	bool InitializeEQMainOffsets();
	void CleanupEQMainOffsets();

	void InitializeEQLib();
	void ShutdownEQLib();

	//----------------------------------------------------------------------------

} // namespace eqlib

#if __has_include("../private/EQLib-globals-private.h")
#include "../private/EQLib-globals-private.h"
#endif
