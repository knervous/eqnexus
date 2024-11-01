// eqgame.ixx

export module eqgame;
export import <cstdint>;

// eqgame.exe offsets
export constexpr uint32_t ClientDate = 20130510u;
export constexpr const char* ExpectedVersionDate = "May 10 2013";
export constexpr const char* ExpectedVersionTime = "23:30:08";
export constexpr uint32_t __ActualVersionDate_x = 0x9DD250;
export constexpr uint32_t __ActualVersionTime_x = 0x9DD25C;

// Memory Protection
export constexpr uint32_t __MemChecker0_x = 0x5643F0;
export constexpr uint32_t __MemChecker1_x = 0x808CB0;
export constexpr uint32_t __MemChecker2_x = 0x5F97E0;
export constexpr uint32_t __MemChecker3_x = 0x5F9730;
export constexpr uint32_t __EncryptPad0_x = 0xAE42D0;

// EverQuestInfo
export constexpr uint32_t pinstEverQuestInfo_x = 0xDDF618;
export constexpr uint32_t instEQZoneInfo_x = 0xDDF8F0;

export constexpr uint32_t __ScreenX_x = 0xDDF624;
export constexpr uint32_t __ScreenY_x = 0xDDF620;
export constexpr uint32_t __ScreenXMax_x = 0xDDF628;
export constexpr uint32_t __ScreenYMax_x = 0xDDF62C;
export constexpr uint32_t __InChatMode_x = 0xDDF66C;
export constexpr uint32_t __RunWalkState_x = 0xDDF670;
export constexpr uint32_t DI8__Mouse_Copy_x = 0xDDF684;
export constexpr uint32_t __MouseLook_x = 0xDDF702;
export constexpr uint32_t __NetStatusToggle_x = 0xDDF705;
export constexpr uint32_t __pulAutoRun_x = 0xDDF720;
export constexpr uint32_t __Clicks_x = 0xDDF734;
export constexpr uint32_t __RMouseHeldTime_x = 0xDDF79C;
export constexpr uint32_t __LMouseHeldTime_x = 0xDDF7A0;
export constexpr uint32_t __ZoneType_x = 0xDDFAF8;
export constexpr uint32_t __ShiftKeyDown_x = 0xDDFD00;
export constexpr uint32_t __RangeAttackReady_x = 0xDE0864;
export constexpr uint32_t __AttackOnAssist_x = 0xDE09DC;
export constexpr uint32_t __ShowNames_x = 0xDE0A70;
export constexpr uint32_t __PCNames_x = 0xDE0BC8;
export constexpr uint32_t __UseTellWindows_x = 0xDE0D58;
export constexpr uint32_t __pulForward_x = 0xDE0D90;
export constexpr uint32_t __pulBackward_x = 0xDE0D94;
export constexpr uint32_t __pulTurnRight_x = 0xDE0D98;
export constexpr uint32_t __pulTurnLeft_x = 0xDE0D9C;
export constexpr uint32_t __pulStrafeLeft_x = 0xDE0DA0;
export constexpr uint32_t __pulStrafeRight_x = 0xDE0DA4;
export constexpr uint32_t __LastTell_x = 0xDE1558;
export constexpr uint32_t __ServerName_x = 0xE15E10;
export constexpr uint32_t __DoAbilityList_x = 0xE15E50;
export constexpr uint32_t __SocialChangedList_x = 0xE15E98;
export constexpr uint32_t __Socials_x = 0xE15F10;
export constexpr uint32_t __Inviter_x = 0xE63528;
export constexpr uint32_t __Attack_x = 0xE635AA;
export constexpr uint32_t __Autofire_x = 0xE635AB;
export constexpr uint32_t EverQuest__Cameras_x = 0xDE0D64;
export constexpr uint32_t EverQuestinfo__IsItemPending_x = 0xE635AC;


// Misc Globals
export constexpr uint32_t __do_loot_x = 0x4E7880;
export constexpr uint32_t __gfMaxZoomCameraDistance_x = 0x9D0DE4;
export constexpr uint32_t __gfMaxCameraDistance_x = 0xA002C4;
export constexpr uint32_t __CurrentSocial_x = 0xAB6A28;
export constexpr uint32_t __BindList_x = 0xACBEE8;
export constexpr uint32_t g_eqCommandStates_x = 0xDCEF08;
export constexpr uint32_t __CommandList_x = 0xACD5A8;
export constexpr uint32_t __ScreenMode_x = 0xD1F3B8;
export constexpr uint32_t __gWorld_x = 0xDD25AC;
export constexpr uint32_t __gpbCommandEvent_x = 0xDD2670;
export constexpr uint32_t __ServerHost_x = 0xDD003C;
export constexpr uint32_t __Guilds_x = 0xDD5CF8;
export constexpr uint32_t __MouseEventTime_x = 0xE639CC;
export constexpr uint32_t DI8__MouseState_x = 0xE67884;
export constexpr uint32_t __heqmain_x = 0xE67CD0;
export constexpr uint32_t DI8__Mouse_x = 0xE67B50;
export constexpr uint32_t __HWnd_x = 0xE678A0;
export constexpr uint32_t __Mouse_x = 0xE67B54;
export constexpr uint32_t DI8__Keyboard_x = 0xE67B4C;
export constexpr uint32_t __LoginName_x = 0xE66478;
export constexpr uint32_t __CurrentMapLabel_x = 0xF73E00;
export constexpr uint32_t __LabelCache_x = 0xF74AB0;
export constexpr uint32_t Teleport_Table_Size_x = 0xDD25B4;
export constexpr uint32_t Teleport_Table_x = 0xDD02E0;
export constexpr uint32_t __ExceptionFilter_x = 0x8E3338;

////
//Section 1: Vital Offsets
////
export constexpr uint32_t instCRaid_x = 0xDD2690;
export constexpr uint32_t pinstActiveBanker_x = 0xDD2650;
export constexpr uint32_t pinstActiveCorpse_x = 0xDD2654;
export constexpr uint32_t pinstActiveGMaster_x = 0xDD2658;
export constexpr uint32_t pinstActiveMerchant_x = 0xDD264C;
export constexpr uint32_t pinstAltAdvManager_x = 0xD20358;
export constexpr uint32_t pinstCEverQuest_x = 0xE67CCC;
export constexpr uint32_t pinstCamActor_x = 0xD1FD8C;
export constexpr uint32_t pinstCDBStr_x = 0xD1F380;
export constexpr uint32_t pinstCDisplay_x = 0xDD2660;
export constexpr uint32_t pinstControlledPlayer_x = 0xDD2644;
export constexpr uint32_t pinstCResolutionHandler_x = 0x15D46CC;
export constexpr uint32_t pinstCSidlManager_x = 0x15D3D08;
export constexpr uint32_t pinstCXWndManager_x = 0x15D3D00;
export constexpr uint32_t instDynamicZone_x = 0xDDF4D0;
export constexpr uint32_t instExpeditionLeader_x = 0xDDF51A;
export constexpr uint32_t instExpeditionName_x = 0xDDF55A;
export constexpr uint32_t pinstDZMember_x = 0xDDF5E0;
export constexpr uint32_t pinstDZTimerInfo_x = 0xDDF5E4;
export constexpr uint32_t pinstEqLogin_x = 0xE65DC8;
export constexpr uint32_t instTribute_x = 0xABFCD0;
export constexpr uint32_t pinstEQSoundManager_x = 0xD20460;
//export constexpr uint32_t pinstEQSpellStrings_x = 0x0;
export constexpr uint32_t pinstSGraphicsEngine_x = 0x15D4698;
export constexpr uint32_t pinstDeviceInputProxy_x = 0xDDF684;
export constexpr uint32_t pinstLocalPC_x = 0xDD261C;
export constexpr uint32_t pinstLocalPlayer_x = 0xDD2630;
export constexpr uint32_t pinstCMercenaryClientManager_x = 0xE63E90;
//export constexpr uint32_t pinstMercenaryStats_x = 0x0;
export constexpr uint32_t pinstModelPlayer_x = 0xDD265C;
export constexpr uint32_t pinstRenderInterface_x = 0x15D46A4;
export constexpr uint32_t pinstSkillMgr_x = 0xE64610;
export constexpr uint32_t pinstSpawnManager_x = 0xE641D0;
export constexpr uint32_t pinstSpellManager_x = 0xE646B0;
export constexpr uint32_t pinstSpellSets_x = 0xE5C8B4;
export constexpr uint32_t pinstStringTable_x = 0xDD25C4;
export constexpr uint32_t pinstSwitchManager_x = 0xDCFD00;
export constexpr uint32_t pinstTarget_x = 0xDD2648;
export constexpr uint32_t pinstTaskMember_x = 0xD1F314;
export constexpr uint32_t pinstTrackTarget_x = 0xDD263C;
export constexpr uint32_t pinstTradeTarget_x = 0xDD262C;
export constexpr uint32_t instTributeActive_x = 0xABFCF5;
export constexpr uint32_t pinstViewActor_x = 0xD1FD88;
export constexpr uint32_t pinstWorldData_x = 0xDD2600;
export constexpr uint32_t pinstPlayerPath_x = 0xE641F8;
export constexpr uint32_t pinstTargetIndicator_x = 0xE64748;
export constexpr uint32_t EQObject_Top_x = 0xDD2610;

////
//Section 2:  UI Related Offsets
////
export constexpr uint32_t pinstCChatWindowManager_x = 0xF71070;
export constexpr uint32_t pinstCContainerMgr_x = 0xD1FD84;
export constexpr uint32_t pinstCContextMenuManager_x = 0x15D391C;
export constexpr uint32_t pinstCInvSlotMgr_x = 0xD1FD80;
export constexpr uint32_t pinstCItemDisplayManager_x = 0xF73A28;
export constexpr uint32_t pinstCPopupWndManager_x = 0xF74338;
export constexpr uint32_t pinstCSpellDisplayMgr_x = 0xF749F0;
export constexpr uint32_t pinstCTaskManager_x = 0xB82C80;
export constexpr uint32_t pinstEQSuiteTextureLoader_x = 0xB64CEC;
export constexpr uint32_t pinstItemIconCache_x = 0xF71398;
//export constexpr uint32_t pinstLootFiltersManager_x = 0x0;

////
// Section 3: Miscellaneous Offsets
////
export constexpr uint32_t __CastRay_x = 0x4FD1D0;
export constexpr uint32_t __CastRay2_x = 0x4FCC20;
export constexpr uint32_t __compress_block_x = 0x7AEBD0;
export constexpr uint32_t __ConvertItemTags_x = 0x4ED110;
export constexpr uint32_t __CleanItemTags_x = 0x4ED580;
export constexpr uint32_t __CreateCascadeMenuItems_x = 0x494BD0;
export constexpr uint32_t __decompress_block_x = 0x7AEC80;
export constexpr uint32_t __DoesFileExist_x = 0x80BDD0;
export constexpr uint32_t __EQGetTime_x = 0x809810;
export constexpr uint32_t __ExecuteCmd_x = 0x4D7230;
export constexpr uint32_t __FixHeading_x = 0x8D1F50;
export constexpr uint32_t __FlushDxKeyboard_x = 0x5F9A70;
export constexpr uint32_t __get_bearing_x = 0x4EF250;
export constexpr uint32_t __get_melee_range_x = 0x4DDEB0;
export constexpr uint32_t __GetAnimationCache_x = 0x665430;
export constexpr uint32_t __GetGaugeValueFromEQ_x = 0x762410;
export constexpr uint32_t __GetLabelFromEQ_x = 0x763640;
export constexpr uint32_t __GetXTargetType_x = 0x8D3BE0;
export constexpr uint32_t __HandleMouseWheel_x = 0x5F9C90;
export constexpr uint32_t __HeadingDiff_x = 0x8D1FC0;
export constexpr uint32_t __HelpPath_x = 0xE63870;
export constexpr uint32_t __InitMouse_x = 0x5FC850;
export constexpr uint32_t __NewUIINI_x = 0x761D40;
export constexpr uint32_t __ProcessGameEvents_x = 0x53A6C0;
export constexpr uint32_t __ProcessKeyboardEvents_x = 0x5FDA30;
export constexpr uint32_t __ProcessMouseEvents_x = 0x539E60;
export constexpr uint32_t __ProcessDeviceEvents_x = 0xDDF684;
export constexpr uint32_t __SaveColors_x = 0x489FB0;
export constexpr uint32_t __STMLToText_x = 0x860620;
export constexpr uint32_t __WndProc_x = 0x5FCD80;
export constexpr uint32_t CMemoryMappedFile__SetFile_x = 0x8FC1B0;
export constexpr uint32_t DrawNetStatus_x = 0x573620;
export constexpr uint32_t Util__FastTime_x = 0x808B50;
export constexpr uint32_t __eq_delete_x = 0x8DB146;
export constexpr uint32_t __eq_new_x = 0x8DBB3B;
export constexpr uint32_t __CopyLayout_x = 0x55BE50;
export constexpr uint32_t __ThrottleFrameRate_x = 0x517EE0; // SleepWell

////
// Section 4: Function Offsets
////

// CAAwnd
export constexpr uint32_t CAAWnd__ShowAbility_x = 0x60A7A0;
export constexpr uint32_t CAAWnd__Update_x = 0x609F90;
export constexpr uint32_t CAAWnd__UpdateSelected_x = 0x608650;

// CAdvancedLootWnd
// export constexpr uint32_t CAdvancedLootWnd__DoAdvLootAction_x = 0x0;
// export constexpr uint32_t CAdvancedLootWnd__DoSharedAdvLootAction_x = 0x0;
// export constexpr uint32_t CAdvancedLootWnd__AddPlayerToList_x = 0x0;
// export constexpr uint32_t CAdvancedLootWnd__UpdateMasterLooter_x = 0x0;

// AltAdvManager
export constexpr uint32_t AltAdvManager__GetCalculatedTimer_x = 0x4A3840;
export constexpr uint32_t AltAdvManager__IsAbilityReady_x = 0x4A3880;
export constexpr uint32_t AltAdvManager__GetAAById_x = 0x4A3C60;
export constexpr uint32_t AltAdvManager__CanTrainAbility_x = 0x4A44C0;
export constexpr uint32_t AltAdvManager__CanSeeAbility_x = 0x4A40F0;

// CharacterZoneClient
export constexpr uint32_t CharacterZoneClient__BardCastBard_x = 0x432960;
export constexpr uint32_t CharacterZoneClient__CalcAffectChange_x = 0x436670;
export constexpr uint32_t CharacterZoneClient__CalcAffectChangeGeneric_x = 0x434DF0;
export constexpr uint32_t CharacterZoneClient__CanMedOnHorse_x = 0x4434B0;
export constexpr uint32_t CharacterZoneClient__CanUseItem_x = 0x44C430;
export constexpr uint32_t CharacterZoneClient__CanUseMemorizedSpellSlot_x = 0x433BC0;
export constexpr uint32_t CharacterZoneClient__CastSpell_x = 0x43A830;
export constexpr uint32_t CharacterZoneClient__CharacterZoneClient_x = 0x45AB40;
export constexpr uint32_t CharacterZoneClient__Cur_HP_x = 0x449E00;
export constexpr uint32_t CharacterZoneClient__Cur_Mana_x = 0x4442E0;
export constexpr uint32_t CharacterZoneClient__FindAffectSlot_x = 0x43D300;
export constexpr uint32_t CharacterZoneClient__GetAdjustedSkill_x = 0x449FE0;
export constexpr uint32_t CharacterZoneClient__GetBaseSkill_x = 0x449C80;
export constexpr uint32_t CharacterZoneClient__GetCastingTimeModifier_x = 0x4336D0;
export constexpr uint32_t CharacterZoneClient__GetCursorItemCount_x = 0x451FD0;
export constexpr uint32_t CharacterZoneClient__GetEnduranceRegen_x = 0x448EC0;
export constexpr uint32_t CharacterZoneClient__GetFirstEffectSlot_x = 0x432AD0;
export constexpr uint32_t CharacterZoneClient__GetFocusCastingTimeModifier_x = 0x42C9A0;
export constexpr uint32_t CharacterZoneClient__GetFocusRangeModifier_x = 0x42CB80;
export constexpr uint32_t CharacterZoneClient__GetFocusReuseMod_x = 0x42CEC0;
export constexpr uint32_t CharacterZoneClient__GetHPRegen_x = 0x4527F0;
export constexpr uint32_t CharacterZoneClient__GetItemCountInInventory_x = 0x451ED0;
export constexpr uint32_t CharacterZoneClient__GetItemCountWorn_x = 0x451DE0;
export constexpr uint32_t CharacterZoneClient__GetLastEffectSlot_x = 0x438EE0;
export constexpr uint32_t CharacterZoneClient__GetManaRegen_x = 0x452DF0;
export constexpr uint32_t CharacterZoneClient__GetModCap_x = 0x57F550;
export constexpr uint32_t CharacterZoneClient__GetOpenEffectSlot_x = 0x43A6D0;
export constexpr uint32_t CharacterZoneClient__GetPCSpellAffect_x = 0x433FD0;
export constexpr uint32_t CharacterZoneClient__HasSkill_x = 0x44A1B0;
export constexpr uint32_t CharacterZoneClient__IsStackBlocked_x = 0x4341E0;
export constexpr uint32_t CharacterZoneClient__MakeMeVisible_x = 0x4592B0;
export constexpr uint32_t CharacterZoneClient__Max_Endurance_x = 0x582020;
export constexpr uint32_t CharacterZoneClient__Max_HP_x = 0x443FA0;
export constexpr uint32_t CharacterZoneClient__Max_Mana_x = 0x581E60;
export constexpr uint32_t CharacterZoneClient__NotifyPCAffectChange_x = 0x4591B0;
export constexpr uint32_t CharacterZoneClient__RemovePCAffectex_x = 0x43A7A0;
export constexpr uint32_t CharacterZoneClient__SpellDuration_x = 0x434420;
export constexpr uint32_t CharacterZoneClient__TotalEffect_x = 0x438990;
export constexpr uint32_t CharacterZoneClient__UseSkill_x = 0x45B2D0;


// CBankWnd
export constexpr uint32_t CBankWnd__WndNotification_x = 0x6293C0;

// CBarterWnd
export constexpr uint32_t CBarterWnd__WndNotification_x = 0x631A10;

// CBarterSearchWnd
export constexpr uint32_t CBarterSearchWnd__WndNotification_x = 0x62D810;
export constexpr uint32_t CBarterSearchWnd__UpdateInventoryList_x = 0x62C960;

// CBazaarSearchWnd
export constexpr uint32_t CBazaarSearchWnd__HandleSearchResults_x = 0x635530;

// CBuffWindow
export constexpr uint32_t CBuffWindow__WndNotification_x = 0x641810;

// CButtonWnd
export constexpr uint32_t CButtonWnd__vftable_x = 0xA1B41C;

// CChatWindowManager
export constexpr uint32_t CChatWindowManager__GetRGBAFromIndex_x = 0x6508A0;
export constexpr uint32_t CChatWindowManager__InitContextMenu_x = 0x6513C0;
export constexpr uint32_t CChatWindowManager__FreeChatWindow_x = 0x650420;
export constexpr uint32_t CChatWindowManager__SetLockedActiveChatWindow_x = 0x650870;
export constexpr uint32_t CChatWindowManager__CreateChatWindow_x = 0x6549B0;

// ChatManagerClient
//export constexpr uint32_t ChatManagerClient__Instance_x = 0x0;

// CChatWindow
export constexpr uint32_t CChatWindow__CChatWindow_x = 0x656100;
export constexpr uint32_t CChatWindow__Clear_x = 0x655CB0;
export constexpr uint32_t CChatWindow__WndNotification_x = 0x656640;
export constexpr uint32_t CChatWindow__AddHistory_x = 0x655AC0;

// CContextMenu
export constexpr uint32_t CContextMenu__AddMenuItem_x = 0x85B7B0;
export constexpr uint32_t CContextMenu__RemoveMenuItem_x = 0x85BAA0;
export constexpr uint32_t CContextMenu__RemoveAllMenuItems_x = 0x85BAC0;
export constexpr uint32_t CContextMenu__CheckMenuItem_x = 0x85BB40;
export constexpr uint32_t CContextMenu__SetMenuItem_x = 0x85B9C0;
export constexpr uint32_t CContextMenu__AddSeparator_x = 0x85B930;

// CContextMenuManager
export constexpr uint32_t CContextMenuManager__AddMenu_x = 0x861A90;
export constexpr uint32_t CContextMenuManager__RemoveMenu_x = 0x861DD0;
export constexpr uint32_t CContextMenuManager__PopupMenu_x = 0x862220;
export constexpr uint32_t CContextMenuManager__Flush_x = 0x861B00;
export constexpr uint32_t CContextMenuManager__CreateDefaultMenu_x = 0x660480;

// CChatService
export constexpr uint32_t CChatService__GetNumberOfFriends_x = 0x7EFA80;
export constexpr uint32_t CChatService__GetFriendName_x = 0x7EFA90;

// CComboWnd
export constexpr uint32_t CComboWnd__DeleteAll_x = 0x86A960;
export constexpr uint32_t CComboWnd__Draw_x = 0x86AB40;
export constexpr uint32_t CComboWnd__GetCurChoice_x = 0x86A780;
export constexpr uint32_t CComboWnd__GetListRect_x = 0x86ADE0;
export constexpr uint32_t CComboWnd__GetTextRect_x = 0x86A9C0;
export constexpr uint32_t CComboWnd__InsertChoice_x = 0x86AE50;
export constexpr uint32_t CComboWnd__SetColors_x = 0x86A710;
export constexpr uint32_t CComboWnd__SetChoice_x = 0x86A740;
export constexpr uint32_t CComboWnd__GetItemCount_x = 0x86A770;
export constexpr uint32_t CComboWnd__GetCurChoiceText_x = 0x86A790;
export constexpr uint32_t CComboWnd__GetChoiceText_x = 0x86A800;

// CContainerWnd
export constexpr uint32_t CContainerWnd__HandleCombine_x = 0x65F7B0;
export constexpr uint32_t CContainerWnd__SetContainer_x = 0x65E1F0;
export constexpr uint32_t CContainerWnd__vftable_x = 0x9E6758;

// CDisplay
export constexpr uint32_t CDisplay__cameraType_x = 0xD1FD9C;
export constexpr uint32_t CDisplay__ZoneMainUI_x = 0x4891A0;
export constexpr uint32_t CDisplay__PreZoneMainUI_x = 0x489190;
export constexpr uint32_t CDisplay__CleanGameUI_x = 0x4936E0;
export constexpr uint32_t CDisplay__GetClickedActor_x = 0x48B6B0;
export constexpr uint32_t CDisplay__GetUserDefinedColor_x = 0x489F70;
export constexpr uint32_t CDisplay__InitCharSelectUI_x = 0x4967B0;
export constexpr uint32_t CDisplay__ReloadUI_x = 0x49D6B0;
export constexpr uint32_t CDisplay__WriteTextHD2_x = 0x48EB40;
export constexpr uint32_t CDisplay__TrueDistance_x = 0x48C2C0;
export constexpr uint32_t CDisplay__SetViewActor_x = 0x48F030;
export constexpr uint32_t CDisplay__GetFloorHeight_x = 0x48C5B0;
export constexpr uint32_t CDisplay__SetRenderWindow_x = 0x494080;
export constexpr uint32_t CDisplay__ToggleScreenshotMode_x = 0x48D030;
export constexpr uint32_t CDisplay__RealRender_World_x = 0x49CA10;

// CEditWnd
export constexpr uint32_t CEditBaseWnd__SetSel_x = 0x88C0C0;
export constexpr uint32_t CEditWnd__DrawCaret_x = 0x87B230;
export constexpr uint32_t CEditWnd__EnsureCaretVisible_x = 0x87D240;
export constexpr uint32_t CEditWnd__GetCaretPt_x = 0x87CCF0;
export constexpr uint32_t CEditWnd__GetCharIndexPt_x = 0x87C260;
export constexpr uint32_t CEditWnd__GetDisplayString_x = 0x87B3E0;
export constexpr uint32_t CEditWnd__GetHorzOffset_x = 0x87B720;
export constexpr uint32_t CEditWnd__GetLineForPrintableChar_x = 0x87BD20;
export constexpr uint32_t CEditWnd__GetSelStartPt_x = 0x87C520;
export constexpr uint32_t CEditWnd__GetSTMLSafeText_x = 0x87B8C0;
export constexpr uint32_t CEditWnd__PointFromPrintableChar_x = 0x87BE10;
export constexpr uint32_t CEditWnd__ReplaceSelection_x = 0x87CD30;
export constexpr uint32_t CEditWnd__SelectableCharFromPoint_x = 0x87BF90;
export constexpr uint32_t CEditWnd__SetEditable_x = 0x87B890;
export constexpr uint32_t CEditWnd__SetWindowText_x = 0x87DD00;

// CEverQuest
export constexpr uint32_t CEverQuest__DoPercentConvert_x = 0x51B600;
export constexpr uint32_t CEverQuest__ClickedPlayer_x = 0x5190A0;
export constexpr uint32_t CEverQuest__CreateTargetIndicator_x = 0x519D00;
export constexpr uint32_t CEverQuest__DeleteTargetIndicator_x = 0x519D80;
export constexpr uint32_t CEverQuest__DoTellWindow_x = 0x51DA00;
export constexpr uint32_t CEverQuest__OutputTextToLog_x = 0x5130C0;
export constexpr uint32_t CEverQuest__DropHeldItemOnGround_x = 0x526290;
export constexpr uint32_t CEverQuest__dsp_chat_x = 0x51F1A0;
export constexpr uint32_t CEverQuest__trimName_x = 0x5172D0;
export constexpr uint32_t CEverQuest__Emote_x = 0x51F400;
export constexpr uint32_t CEverQuest__EnterZone_x = 0x531F90;
export constexpr uint32_t CEverQuest__GetBodyTypeDesc_x = 0x515660;
export constexpr uint32_t CEverQuest__GetClassDesc_x = 0x514DC0;
export constexpr uint32_t CEverQuest__GetClassThreeLetterCode_x = 0x5153C0;
export constexpr uint32_t CEverQuest__GetDeityDesc_x = 0x515F70;
export constexpr uint32_t CEverQuest__GetLangDesc_x = 0x5159F0;
export constexpr uint32_t CEverQuest__GetRaceDesc_x = 0x515CE0;
export constexpr uint32_t CEverQuest__InterpretCmd_x = 0x51FCE0;
export constexpr uint32_t CEverQuest__LeftClickedOnPlayer_x = 0x536310;
export constexpr uint32_t CEverQuest__LMouseUp_x = 0x5384D0;
export constexpr uint32_t CEverQuest__RightClickedOnPlayer_x = 0x536D80;
export constexpr uint32_t CEverQuest__RMouseUp_x = 0x537D30;
export constexpr uint32_t CEverQuest__SetGameState_x = 0x519750;
export constexpr uint32_t CEverQuest__UPCNotificationFlush_x = 0x520C70;
export constexpr uint32_t CEverQuest__IssuePetCommand_x = 0x5209B0;
//export constexpr uint32_t CEverQuest__ReportSuccessfulHeal_x = 0x0;
export constexpr uint32_t CEverQuest__ReportSuccessfulHit_x = 0x52EE40;

// CGaugeWnd
export constexpr uint32_t CGaugeWnd__CalcFillRect_x = 0x66F7A0;
export constexpr uint32_t CGaugeWnd__CalcLinesFillRect_x = 0x66F800;
export constexpr uint32_t CGaugeWnd__Draw_x = 0x66FC30;

// CGuild
export constexpr uint32_t CGuild__FindMemberByName_x = 0x423D70;
export constexpr uint32_t CGuild__GetGuildName_x = 0x425670;
export constexpr uint32_t CGuild__GetGuildIndex_x = 0x425860;

// CHotButtonWnd
export constexpr uint32_t CHotButtonWnd__DoHotButton_x = 0x68C8B0;

//CHotButton
export constexpr uint32_t CHotButton__SetButtonSize_x = 0x550120;
export constexpr uint32_t CHotButton__SetCheck_x = 0x54FBE0;

// CInvSlotMgr
export constexpr uint32_t CInvSlotMgr__MoveItem_x = 0x698D80;
export constexpr uint32_t CInvSlotMgr__SelectSlot_x = 0x697F00;

// CInvSLot
export constexpr uint32_t CInvSlot__HandleRButtonUp_x = 0x697250;
export constexpr uint32_t CInvSlot__SliderComplete_x = 0x695270;
export constexpr uint32_t CInvSlot__GetItemBase_x = 0x694780;
export constexpr uint32_t CInvSlot__UpdateItem_x = 0x694D20;

// CInvSlotWnd
export constexpr uint32_t CInvSlotWnd__CInvSlotWnd_x = 0x69AC10;
export constexpr uint32_t CInvSlotWnd__HandleLButtonUp_x = 0x69A5D0;

// CItemDisplayWnd
export constexpr uint32_t CItemDisplayWnd__SetItem_x = 0x6AE100;
export constexpr uint32_t CItemDisplayWnd__UpdateStrings_x = 0x69AE30;
export constexpr uint32_t CItemDisplayWnd__InsertAugmentRequest_x = 0x6A8190;
export constexpr uint32_t CItemDisplayWnd__RemoveAugmentRequest_x = 0x6A86B0;
//export constexpr uint32_t CItemDisplayWnd__RequestConvertItem_x = 0x0;

// CSpellDisplayWnd
export constexpr uint32_t CSpellDisplayWnd__SetSpell_x = 0x75F060;
export constexpr uint32_t CSpellDisplayWnd__UpdateStrings_x = 0x75EAC0;
export constexpr uint32_t CSpellDisplayWnd__vftable_x = 0x9FB100;

// CWndDisplayManager
export constexpr uint32_t CWndDisplayManager__FindWindow_x = 0x796420;

// CItemDisplayManager
export constexpr uint32_t CItemDisplayManager__CreateWindowInstance_x = 0x6A1AE0;

// CSpellDisplayManager
export constexpr uint32_t CSpellDisplayManager__ShowSpell_x = 0x75FD80;

// CKeyRingWnd
//export constexpr uint32_t CKeyRingWnd__ExecuteRightClick_x = 0x0;

// CLabel
export constexpr uint32_t CLabel__UpdateText_x = 0x6B4D40;

// CListWnd
export constexpr uint32_t CListWnd__CListWnd_x = 0x857C00;
export constexpr uint32_t CListWnd__dCListWnd_x = 0x858C80;
export constexpr uint32_t CListWnd__vftable_x = 0xA1927C;
export constexpr uint32_t CListWnd__AddColumn_x = 0x858C20;
export constexpr uint32_t CListWnd__AddColumn1_x = 0x858400;
export constexpr uint32_t CListWnd__AddLine_x = 0x857EB0;
export constexpr uint32_t CListWnd__AddString_x = 0x8580C0;
export constexpr uint32_t CListWnd__CalculateFirstVisibleLine_x = 0x854F70;
export constexpr uint32_t CListWnd__CalculateVSBRange_x = 0x8570B0;
export constexpr uint32_t CListWnd__ClearSel_x = 0x854270;
export constexpr uint32_t CListWnd__ClearAllSel_x = 0x854220;
export constexpr uint32_t CListWnd__CloseAndUpdateEditWindow_x = 0x857A90;
export constexpr uint32_t CListWnd__Compare_x = 0x855A80;
export constexpr uint32_t CListWnd__Draw_x = 0x856D10;
export constexpr uint32_t CListWnd__DrawColumnSeparators_x = 0x856BA0;
export constexpr uint32_t CListWnd__DrawHeader_x = 0x854530;
export constexpr uint32_t CListWnd__DrawItem_x = 0x8560D0;
export constexpr uint32_t CListWnd__DrawLine_x = 0x856840;
export constexpr uint32_t CListWnd__DrawSeparator_x = 0x856C40;
export constexpr uint32_t CListWnd__EnableLine_x = 0x854440;
export constexpr uint32_t CListWnd__EnsureVisible_x = 0x8550D0;
export constexpr uint32_t CListWnd__ExtendSel_x = 0x855FE0;
export constexpr uint32_t CListWnd__GetColumnTooltip_x = 0x853C40;
export constexpr uint32_t CListWnd__GetColumnMinWidth_x = 0x853D70;
export constexpr uint32_t CListWnd__GetColumnWidth_x = 0x853B60;
export constexpr uint32_t CListWnd__GetCurSel_x = 0x853430;
export constexpr uint32_t CListWnd__GetItemAtPoint_x = 0x855370;
export constexpr uint32_t CListWnd__GetItemAtPoint1_x = 0x8553E0;
export constexpr uint32_t CListWnd__GetItemData_x = 0x853680;
export constexpr uint32_t CListWnd__GetItemHeight_x = 0x854BC0;
export constexpr uint32_t CListWnd__GetItemIcon_x = 0x853860;
export constexpr uint32_t CListWnd__GetItemRect_x = 0x8551B0;
export constexpr uint32_t CListWnd__GetItemText_x = 0x853710;
export constexpr uint32_t CListWnd__GetSelList_x = 0x8582F0;
export constexpr uint32_t CListWnd__GetSeparatorRect_x = 0x8559A0;
export constexpr uint32_t CListWnd__InsertLine_x = 0x858290;
export constexpr uint32_t CListWnd__RemoveLine_x = 0x8582A0;
export constexpr uint32_t CListWnd__SetColors_x = 0x853490;
export constexpr uint32_t CListWnd__SetColumnJustification_x = 0x853F10;
export constexpr uint32_t CListWnd__SetColumnLabel_x = 0x8544D0;
export constexpr uint32_t CListWnd__SetColumnWidth_x = 0x853BF0;
export constexpr uint32_t CListWnd__SetCurSel_x = 0x853470;
export constexpr uint32_t CListWnd__SetItemColor_x = 0x857770;
export constexpr uint32_t CListWnd__SetItemData_x = 0x8542D0;
export constexpr uint32_t CListWnd__SetItemText_x = 0x8575B0;
export constexpr uint32_t CListWnd__ShiftColumnSeparator_x = 0x857440;
export constexpr uint32_t CListWnd__Sort_x = 0x858D80;
export constexpr uint32_t CListWnd__ToggleSel_x = 0x8541A0;
export constexpr uint32_t CListWnd__SetColumnsSizable_x = 0x855F20;
export constexpr uint32_t CListWnd__SetItemWnd_x = 0x857630;
export constexpr uint32_t CListWnd__GetItemWnd_x = 0x8538B0;
export constexpr uint32_t CListWnd__SetItemIcon_x = 0x857540;
export constexpr uint32_t CListWnd__CalculateCustomWindowPositions_x = 0x8571E0;
export constexpr uint32_t CListWnd__SetVScrollPos_x = 0x855C00;

// CMapViewWnd
export constexpr uint32_t CMapViewWnd__CMapViewWnd_x = 0x6CFB70;

// MapViewMap
export constexpr uint32_t MapViewMap__MapViewMap_x = 0x6C7E60;
export constexpr uint32_t MapViewMap__dMapViewMap_x = 0x6CC580;
export constexpr uint32_t MapViewMap__vftable_x = 0x9EE580;
export constexpr uint32_t MapViewMap__Clear_x = 0x6C7FD0;
export constexpr uint32_t MapViewMap__SetZoom_x = 0x6C7C40;
export constexpr uint32_t MapViewMap__HandleLButtonDown_x = 0x6C9430;
export constexpr uint32_t MapViewMap__GetWorldCoordinates_x = 0x6C88A0;

// CMerchantWnd
export constexpr uint32_t CMerchantWnd__DisplayBuyOrSellPrice_x = 0x6EF750;
export constexpr uint32_t CMerchantWnd__PurchasePageHandler__RequestGetItem_x = 0x6F5BE0;
export constexpr uint32_t CMerchantWnd__PurchasePageHandler__RequestPutItem_x = 0x6F6650;
export constexpr uint32_t CMerchantWnd__MerchantPageHandler__SelectBuySellSlot_x = 0x6EEC30;
export constexpr uint32_t CMerchantWnd__SelectBuySellSlot_x = 0x6F31A0;
export constexpr uint32_t CMerchantWnd__PurchasePageHandler__UpdateList_x = 0x6F0BB0;

// CPacketScrambler
export constexpr uint32_t CPacketScrambler__ntoh_x = 0x7D03B0;
export constexpr uint32_t CPacketScrambler__hton_x = 0x7D03C0;

// CSidlManagerBase
export constexpr uint32_t CSidlManagerBase__CreateTabWnd_x = 0x86C160;
export constexpr uint32_t CSidlManagerBase__CreatePageWnd_x = 0x86BDB0;
export constexpr uint32_t CSidlManagerBase__FindButtonDrawTemplate_x = 0x86CF40;
export constexpr uint32_t CSidlManagerBase__FindButtonDrawTemplate1_x = 0x86CED0;
export constexpr uint32_t CSidlManagerBase__FindAnimation1_x = 0x86E010;
export constexpr uint32_t CSidlManagerBase__FindScreenPieceTemplate_x = 0x870040;
export constexpr uint32_t CSidlManagerBase__FindScreenPieceTemplate1_x = 0x870120;
export constexpr uint32_t CSidlManagerBase__CreateLabel_x = 0x7556F0;
export constexpr uint32_t CSidlManagerBase__CreateXWndFromTemplate_x = 0x86E5E0;
export constexpr uint32_t CSidlManagerBase__CreateXWndFromTemplate1_x = 0x870400;
export constexpr uint32_t CSidlManagerBase__CreateXWnd_x = 0x870440;

// CSidlManager
export constexpr uint32_t CSidlManager__CreateHotButtonWnd_x = 0x755DE0;
export constexpr uint32_t CSidlManager__CreateXWnd_x = 0x755F20;

// CSidlScreenWnd
export constexpr uint32_t CSidlScreenWnd__CalculateHSBRange_x = 0x61EE40;
export constexpr uint32_t CSidlScreenWnd__CalculateVSBRange_x = 0x61ED60;
export constexpr uint32_t CSidlScreenWnd__ConvertToRes_x = 0x88BFA0;
export constexpr uint32_t CSidlScreenWnd__CreateChildrenFromSidl_x = 0x85DCF0;
export constexpr uint32_t CSidlScreenWnd__CSidlScreenWnd1_x = 0x85F070;
export constexpr uint32_t CSidlScreenWnd__CSidlScreenWnd2_x = 0x85F120;
export constexpr uint32_t CSidlScreenWnd__dCSidlScreenWnd_x = 0x85E600;
export constexpr uint32_t CSidlScreenWnd__DrawSidlPiece_x = 0x85D7D0;
export constexpr uint32_t CSidlScreenWnd__EnableIniStorage_x = 0x85CF00;
export constexpr uint32_t CSidlScreenWnd__GetChildItem_x = 0x85CFD0;
export constexpr uint32_t CSidlScreenWnd__GetSidlPiece_x = 0x85D9C0;
export constexpr uint32_t CSidlScreenWnd__HandleLButtonUp_x = 0x863A30;
export constexpr uint32_t CSidlScreenWnd__Init1_x = 0x85ED50;
export constexpr uint32_t CSidlScreenWnd__LoadIniInfo_x = 0x85DDA0;
export constexpr uint32_t CSidlScreenWnd__LoadIniListWnd_x = 0x85D0C0;
export constexpr uint32_t CSidlScreenWnd__LoadSidlScreen_x = 0x85E850;
export constexpr uint32_t CSidlScreenWnd__m_layoutCopy_x = 0x15D38E0;
export constexpr uint32_t CSidlScreenWnd__StoreIniInfo_x = 0x85CA70;
export constexpr uint32_t CSidlScreenWnd__StoreIniVis_x = 0x85CE90;
export constexpr uint32_t CSidlScreenWnd__vftable_x = 0xA1982C;
export constexpr uint32_t CSidlScreenWnd__WndNotification_x = 0x85EAD0;

// CSkillMgr   a.k.a. SkillManager
export constexpr uint32_t CSkillMgr__IsAvailable_x = 0x5BAE40;
export constexpr uint32_t CSkillMgr__GetSkillCap_x = 0x5BAEC0;
export constexpr uint32_t CSkillMgr__GetNameToken_x = 0x5BAAB0;
export constexpr uint32_t CSkillMgr__IsActivatedSkill_x = 0x5BABD0;
export constexpr uint32_t CSkillMgr__IsCombatSkill_x = 0x5BAB10;
export constexpr uint32_t CSkillMgr__GetSkillTimerDuration_x = 0x5BAD40;
export constexpr uint32_t CSkillMgr__GetSkillLastUsed_x = 0x5BACA0;

// CSliderWnd
export constexpr uint32_t CSliderWnd__GetValue_x = 0x895FE0;
export constexpr uint32_t CSliderWnd__SetValue_x = 0x8961B0;
export constexpr uint32_t CSliderWnd__SetNumTicks_x = 0x896930;

// CSpellBookWnd
export constexpr uint32_t CSpellBookWnd__MemorizeSet_x = 0x75CAC0;

// CStmlWnd
export constexpr uint32_t CStmlWnd__AppendSTML_x = 0x886720;
export constexpr uint32_t CStmlWnd__CalculateHSBRange_x = 0x87EE80;
export constexpr uint32_t CStmlWnd__CalculateVSBRange_x = 0x87EF10;
export constexpr uint32_t CStmlWnd__CanBreakAtCharacter_x = 0x87F090;
export constexpr uint32_t CStmlWnd__FastForwardToEndOfTag_x = 0x87F970;
export constexpr uint32_t CStmlWnd__ForceParseNow_x = 0x887070;
export constexpr uint32_t CStmlWnd__GetNextTagPiece_x = 0x87F870;
export constexpr uint32_t CStmlWnd__GetVisibleText_x = 0x8803C0;
export constexpr uint32_t CStmlWnd__InitializeWindowVariables_x = 0x8825A0;
export constexpr uint32_t CStmlWnd__MakeStmlColorTag_x = 0x87E0D0;
export constexpr uint32_t CStmlWnd__MakeWndNotificationTag_x = 0x87E130;
export constexpr uint32_t CStmlWnd__SetSTMLText_x = 0x883E10;
export constexpr uint32_t CStmlWnd__StripFirstSTMLLines_x = 0x8863C0;
export constexpr uint32_t CStmlWnd__UpdateHistoryString_x = 0x881300;

// CTabWnd
export constexpr uint32_t CTabWnd__Draw_x = 0x88D870;
export constexpr uint32_t CTabWnd__DrawCurrentPage_x = 0x88D0E0;
export constexpr uint32_t CTabWnd__DrawTab_x = 0x88CEA0;
export constexpr uint32_t CTabWnd__GetCurrentPage_x = 0x88D4B0;
export constexpr uint32_t CTabWnd__GetCurrentTabIndex_x = 0x88CB20;
export constexpr uint32_t CTabWnd__GetPageFromTabIndex_x = 0x88CDF0;
export constexpr uint32_t CTabWnd__GetPageInnerRect_x = 0x88CB90;
export constexpr uint32_t CTabWnd__GetTabInnerRect_x = 0x88CD90;
export constexpr uint32_t CTabWnd__GetTabRect_x = 0x88CC60;
export constexpr uint32_t CTabWnd__InsertPage_x = 0x88DB60;
export constexpr uint32_t CTabWnd__RemovePage_x = 0x88D650;
export constexpr uint32_t CTabWnd__SetPage_x = 0x88D4E0;
export constexpr uint32_t CTabWnd__SetPageRect_x = 0x88D7B0;
export constexpr uint32_t CTabWnd__UpdatePage_x = 0x88DB00;

// CPageWnd
//export constexpr uint32_t CPageWnd__FlashTab_x = 0x0;
export constexpr uint32_t CPageWnd__SetTabText_x = 0x89B9A0;

// CTextOverlay
export constexpr uint32_t CTextOverlay__DisplayText_x = 0x41F840;
export constexpr uint32_t CBroadcast__Get_x = 0x41F3A0;


// CTextureFont
export constexpr uint32_t CTextureFont__DrawWrappedText_x = 0x889B70;
export constexpr uint32_t CTextureFont__DrawWrappedText1_x = 0x889950;
export constexpr uint32_t CTextureFont__DrawWrappedText2_x = 0x889A50;
export constexpr uint32_t CTextureFont__GetTextExtent_x = 0x8898D0;
export constexpr uint32_t CTextureFont__GetHeight_x = 0x889910;

// CXMLDataManager
export constexpr uint32_t CXMLDataManager__GetXMLData_x = 0x8929A0;

// CXMLSOMDocumentBase
export constexpr uint32_t CXMLSOMDocumentBase__XMLRead_x = 0x851540;

// CXStr
export constexpr uint32_t CXStr__gFreeLists_x = 0xB618A0;
export constexpr uint32_t CXStr__gCXStrAccess_x = 0x15D35E4;

// CXWnd
export constexpr uint32_t CXWnd__BringToTop_x = 0x862C50;
export constexpr uint32_t CXWnd__ClrFocus_x = 0x862820;
export constexpr uint32_t CXWnd__Destroy_x = 0x869760;
export constexpr uint32_t CXWnd__DoAllDrawing_x = 0x868A10;
export constexpr uint32_t CXWnd__DrawChildren_x = 0x868BA0;
export constexpr uint32_t CXWnd__DrawColoredRect_x = 0x863010;
export constexpr uint32_t CXWnd__DrawTooltip_x = 0x8632C0;
export constexpr uint32_t CXWnd__DrawTooltipAtPoint_x = 0x8679B0;
export constexpr uint32_t CXWnd__GetBorderFrame_x = 0x863690;
export constexpr uint32_t CXWnd__GetChildItem_x = 0x868330;
export constexpr uint32_t CXWnd__GetChildWndAt_x = 0x866980;
export constexpr uint32_t CXWnd__GetClientClipRect_x = 0x8634D0;
export constexpr uint32_t CXWnd__GetRelativeRect_x = 0x8637C0;
export constexpr uint32_t CXWnd__GetScreenClipRect_x = 0x8680B0;
export constexpr uint32_t CXWnd__GetScreenRect_x = 0x8638D0;
export constexpr uint32_t CXWnd__GetTooltipRect_x = 0x863150;
export constexpr uint32_t CXWnd__IsActive_x = 0x8760A0;
export constexpr uint32_t CXWnd__IsDescendantOf_x = 0x863640;
export constexpr uint32_t CXWnd__IsReallyVisible_x = 0x866960;
export constexpr uint32_t CXWnd__IsType_x = 0x868E10;
export constexpr uint32_t CXWnd__Minimize_x = 0x8666D0;
export constexpr uint32_t CXWnd__ProcessTransition_x = 0x862C00;
export constexpr uint32_t CXWnd__Refade_x = 0x8629D0;
export constexpr uint32_t CXWnd__Resize_x = 0x863990;
export constexpr uint32_t CXWnd__Right_x = 0x867EB0;
export constexpr uint32_t CXWnd__SetFocus_x = 0x865250;
export constexpr uint32_t CXWnd__SetFont_x = 0x862870;
export constexpr uint32_t CXWnd__SetKeyTooltip_x = 0x863C00;
export constexpr uint32_t CXWnd__SetMouseOver_x = 0x8683F0;
export constexpr uint32_t CXWnd__SetParent_x = 0x869710;
export constexpr uint32_t CXWnd__StartFade_x = 0x862CA0;
export constexpr uint32_t CXWnd__vftable_x = 0xA19C74;
export constexpr uint32_t CXWnd__CXWnd_x = 0x869AC0;
export constexpr uint32_t CXWnd__dCXWnd_x = 0x869E50;

// CScreenPieceTemplate
export constexpr uint32_t CScreenPieceTemplate__IsType_x = 0x88F1E0;

// CXWndManager
export constexpr uint32_t CXWndManager__DrawCursor_x = 0x8779C0;
export constexpr uint32_t CXWndManager__DrawWindows_x = 0x877600;
export constexpr uint32_t CXWndManager__GetKeyboardFlags_x = 0x875BD0;
export constexpr uint32_t CXWndManager__HandleKeyboardMsg_x = 0x876530;
export constexpr uint32_t CXWndManager__RemoveWnd_x = 0x876230;
export constexpr uint32_t CXWndManager__RemovePendingDeletionWnd_x = 0x876C80;

// CDBStr
export constexpr uint32_t CDBStr__GetString_x = 0x4866C0;

// ProfileManager
export constexpr uint32_t ProfileManager__GetCurrentProfile_x = 0x7DB210;

// CCharacterListWnd
export constexpr uint32_t CCharacterListWnd__SelectCharacter_x = 0x42FEC0;
export constexpr uint32_t CCharacterListWnd__EnterWorld_x = 0x42FD20;
export constexpr uint32_t CCharacterListWnd__Quit_x = 0x42EA50;
export constexpr uint32_t CCharacterListWnd__UpdateList_x = 0x430760;

// ItemClient
export constexpr uint32_t ItemBase__CanGemFitInSlot_x = 0x7BCDB0;
export constexpr uint32_t ItemBase__CreateItemTagString_x = 0x7BBC50;
export constexpr uint32_t ItemBase__GetImageNum_x = 0x7B2FB0;
export constexpr uint32_t ItemBase__GetItemValue_x = 0x7BA920;
export constexpr uint32_t ItemBase__IsEmpty_x = 0x7B22A0;
//export constexpr uint32_t ItemBase__IsKeyRingItem_x = 0x0;
export constexpr uint32_t ItemBase__ValueSellMerchant_x = 0x7BCF80;
export constexpr uint32_t ItemClient__CanDrop_x = 0x554E50;
export constexpr uint32_t ItemClient__CanGoInBag_x = 0x554FD0;
export constexpr uint32_t ItemClient__CreateItemClient_x = 0x5559A0;
export constexpr uint32_t ItemClient__dItemClient_x = 0x5552F0;

// EQ_LoadingS
export constexpr uint32_t EQ_LoadingS__SetProgressBar_x = 0x4A59A0;
export constexpr uint32_t EQ_LoadingS__Array_x = 0xAC6330;

// PcClient
export constexpr uint32_t PcBase__GetAlternateAbilityId_x = 0x7C4460;
export constexpr uint32_t PcBase__GetCombatAbility_x = 0x7C44F0;
export constexpr uint32_t PcBase__GetCombatAbilityTimer_x = 0x7C45A0;
export constexpr uint32_t PcBase__GetItemContainedRealEstateIds_x = 0x7D0160;
export constexpr uint32_t PcBase__GetNonArchivedOwnedRealEstates_x = 0x7C95F0;
export constexpr uint32_t PcClient__AlertInventoryChanged_x = 0x577C80;
export constexpr uint32_t PcClient__GetConLevel_x = 0x577CB0;
export constexpr uint32_t PcClient__HasLoreItem_x = 0x57AE30;
export constexpr uint32_t PcZoneClient__GetItemRecastTimer_x = 0x580600;
export constexpr uint32_t PcZoneClient__RemoveMyAffect_x = 0x57FA50;

// EQGroundItemListManager
export constexpr uint32_t EQItemList__EQItemList_x = 0x4DBBD0;
export constexpr uint32_t EQItemList__add_item_x = 0x4DBB10;
export constexpr uint32_t EQItemList__delete_item_x = 0x4DBFD0;
export constexpr uint32_t EQItemList__FreeItemList_x = 0x4DBED0;

export constexpr uint32_t EQItemList__add_object_x = 0x506030;

// CTribute
export constexpr uint32_t CTribute__GetActiveFavorCost_x = 0x485970;

// PlayerClient
export constexpr uint32_t PlayerBase__CanSee_x = 0x8D0520;
export constexpr uint32_t PlayerBase__CanSee1_x = 0x8D05E0;
export constexpr uint32_t PlayerBase__GetVisibilityLineSegment_x = 0x8D0300;
export constexpr uint32_t PlayerBase__HasProperty_x = 0x8CFDC0;
export constexpr uint32_t PlayerBase__IsTargetable_x = 0x8D0090;
export constexpr uint32_t PlayerClient__ChangeBoneStringSprite_x = 0x58BE90;
export constexpr uint32_t PlayerClient__dPlayerClient_x = 0x590A60;
export constexpr uint32_t PlayerClient__GetPcClient_x = 0x58C9A0;
export constexpr uint32_t PlayerClient__PlayerClient_x = 0x590880;
export constexpr uint32_t PlayerClient__SetNameSpriteState_x = 0x58E2D0;
export constexpr uint32_t PlayerClient__SetNameSpriteTint_x = 0x58BF00;
export constexpr uint32_t PlayerZoneClient__ChangeHeight_x = 0x5A4EC0;
export constexpr uint32_t PlayerZoneClient__DoAttack_x = 0x5A5000;
export constexpr uint32_t PlayerZoneClient__GetLevel_x = 0x59FC90;
export constexpr uint32_t PlayerZoneClient__IsValidTeleport_x = 0x506A20;

// PlayerManagerClient
export constexpr uint32_t PlayerManagerClient__GetSpawnByID_x = 0x5996E0;
export constexpr uint32_t PlayerManagerClient__GetSpawnByName_x = 0x599B40;
export constexpr uint32_t PlayerManagerClient__GetPlayerFromPartialName_x = 0x599710;
export constexpr uint32_t PlayerManagerClient__CreatePlayer_x = 0x599BD0;
export constexpr uint32_t PlayerManagerBase__PrepForDestroyPlayer_x = 0x8CF790;
export constexpr uint32_t PlayerManagerBase__DestroyAllPlayers_x = 0x8CF800;

// KeyPressHandler
export constexpr uint32_t pinstKeypressHandler_x = 0xE639B0;
export constexpr uint32_t KeypressHandler__AttachAltKeyToEqCommand_x = 0x55AFA0;
export constexpr uint32_t KeypressHandler__AttachKeyToEqCommand_x = 0x55AFE0;
export constexpr uint32_t KeypressHandler__ClearCommandStateArray_x = 0x55AB80;
export constexpr uint32_t KeypressHandler__HandleKeyDown_x = 0x5594E0;
export constexpr uint32_t KeypressHandler__HandleKeyUp_x = 0x559800;
export constexpr uint32_t KeypressHandler__SaveKeymapping_x = 0x55AC50;

// PlayerPointManager
export constexpr uint32_t PlayerPointManager__GetAltCurrency_x = 0x7D5350;

// StringTable
export constexpr uint32_t StringTable__getString_x = 0x7D0660;

// PcZoneClient
export constexpr uint32_t PcZoneClient__doCombatAbility_x = 0x5808C0;
export constexpr uint32_t PcZoneClient__DestroyHeldItemOrMoney_x = 0x5862F0;
export constexpr uint32_t PcZoneClient__GetPcSkillLimit_x = 0x584BE0;
export constexpr uint32_t PcZoneClient__RemovePetEffect_x = 0x580CA0;
export constexpr uint32_t PcZoneClient__HasAlternateAbility_x = 0x582670;
export constexpr uint32_t PcZoneClient__CanEquipItem_x = 0x583550;
export constexpr uint32_t PcZoneClient__GetItemByID_x = 0x585A10;
export constexpr uint32_t PcZoneClient__RemoveBuffEffect_x = 0x57FCE0;
export constexpr uint32_t PcZoneClient__BandolierSwap_x = 0x585AA0;

// Doors
export constexpr uint32_t EQSwitch__UseSwitch_x = 0x511D70;

// IconCache
export constexpr uint32_t IconCache__GetIcon_x = 0x665A60;

// CContainerMgr
export constexpr uint32_t CContainerMgr__OpenContainer_x = 0x65CAE0;
export constexpr uint32_t CContainerMgr__CloseContainer_x = 0x65D2F0;
export constexpr uint32_t CContainerMgr__OpenExperimentContainer_x = 0x65CAB0;

// CQuantityWnd
export constexpr uint32_t CQuantityWnd__Open_x = 0x725B00;

// CHelpWnd
export constexpr uint32_t CHelpWnd__SetFile_x = 0x54E670;

// CLootWnd
export constexpr uint32_t CLootWnd__LootAll_x = 0x6BDBA0;
export constexpr uint32_t CLootWnd__RequestLootSlot_x = 0x6BE3A0;

// EQ_Spell
export constexpr uint32_t EQ_Spell__GetSpellLevelNeeded_x = 0x4AF700;
export constexpr uint32_t EQ_Spell__SpellAffects_x = 0x4AF770;
export constexpr uint32_t EQ_Spell__SpellAffectBase_x = 0x4AF7A0;
export constexpr uint32_t EQ_Spell__IsSPAStacking_x = 0x4ACE40;
export constexpr uint32_t EQ_Spell__IsSPAIgnoredByStacking_x = 0x4ACA70;
//export constexpr uint32_t EQ_Spell__IsDegeneratingLevelMod_x = 0x0;
export constexpr uint32_t __IsResEffectSpell_x = 0x432590;

// EQSpellStrings
//export constexpr uint32_t EQSpellStrings__GetString_x = 0x0;

// CTargetWnd
export constexpr uint32_t CTargetWnd__GetBuffCaster_x = 0x769FD0;
export constexpr uint32_t CTargetWnd__WndNotification_x = 0x769AC0;
export constexpr uint32_t CTargetWnd__RefreshTargetBuffs_x = 0x76A1B0;
export constexpr uint32_t CTargetWnd__HandleBuffRemoveRequest_x = 0x7693F0;

// CTaskWnd
export constexpr uint32_t CTaskWnd__UpdateTaskTimers_x = 0x76EF90;
export constexpr uint32_t CTaskWnd__ConfirmAbandonTask_x = 0x76FD50;

// CTaskManager
export constexpr uint32_t CTaskManager__GetEntry_x = 0x47FFD0;
export constexpr uint32_t CTaskManager__HandleMessage_x = 0x482080;
export constexpr uint32_t CTaskManager__GetTaskStatus_x = 0x481100;
export constexpr uint32_t CTaskManager__GetElementDescription_x = 0x4800C0;

// EqSoundManager
export constexpr uint32_t EqSoundManager__WavePlay_x = 0x4A9140;
export constexpr uint32_t EqSoundManager__PlayScriptMp3_x = 0x4AA7D0;
export constexpr uint32_t EqSoundManager__SoundAssistPlay_x = 0x5BFBD0;
export constexpr uint32_t EqSoundManager__WaveInstancePlay_x = 0x5BF9A0;

// CCombatSkillsSelectWnd
export constexpr uint32_t CCombatSkillsSelectWnd__ShouldDisplayThisSkill_x = 0x4772A0;

// CTextureAnimation
export constexpr uint32_t CTextureAnimation__Draw_x = 0x87A4A0;
export constexpr uint32_t CTextureAnimation__SetCurCell_x = 0x87A860;

// FreeTargetTracker
export constexpr uint32_t FreeTargetTracker__CastSpell_x = 0x54C010;

// CharacterBase
export constexpr uint32_t CharacterBase__GetItemByGlobalIndex_x = 0x7E9AE0;
export constexpr uint32_t CharacterBase__GetItemByGlobalIndex1_x = 0x7E9B40;
export constexpr uint32_t CharacterBase__IsExpansionFlag_x = 0x4106C0;

// CCastSpellWnd
export constexpr uint32_t CCastSpellWnd__ForgetMemorizedSpell_x = 0x647D50;
export constexpr uint32_t CCastSpellWnd__IsBardSongPlaying_x = 0x646EE0;
export constexpr uint32_t CCastSpellWnd__RefreshSpellGemButtons_x = 0x647EA0;

// messages
export constexpr uint32_t msg_spell_worn_off_x = 0x4B79F0;
export constexpr uint32_t msg_new_text_x = 0x4C0080;
export constexpr uint32_t __msgTokenTextParam_x = 0x4BDBE0;
export constexpr uint32_t msgTokenText_x = 0x4BDD00;

// SpellManager
export constexpr uint32_t Spellmanager__LoadTextSpells_x = 0x5C1C30;
export constexpr uint32_t SpellManager__GetSpellByGroupAndRank_x = 0x5C1AE0;

// CCollisionInfoTargetVisibility
export constexpr uint32_t CCollisionInfoTargetVisibility__CCollisionInfoTargetVisibility_x = 0x8D4570;

// CLargeDialogWnd
export constexpr uint32_t CLargeDialogWnd__Open_x = 0x6B4EE0;

// CCursorAttachment
export constexpr uint32_t CCursorAttachment__AttachToCursor_x = 0x661820;
export constexpr uint32_t CCursorAttachment__IsOkToActivate_x = 0x660F30;
export constexpr uint32_t CCursorAttachment__RemoveAttachment_x = 0x660F30;

// CEQSuiteTextureLoader
export constexpr uint32_t CEQSuiteTextureLoader__CreateTexture_x = 0x88AEE0;
export constexpr uint32_t CEQSuiteTextureLoader__GetDefaultUIPath_x = 0x88A8A0;
export constexpr uint32_t CEQSuiteTextureLoader__GetTexture_x = 0x88AA20;

// CFindItemWnd
//export constexpr uint32_t CFindItemWnd__WndNotification_x = 0x0;
//export constexpr uint32_t CFindItemWnd__Update_x = 0x0;
//export constexpr uint32_t CFindItemWnd__PickupSelectedItem_x = 0x0;

// LootFiltersManager
//export constexpr uint32_t LootFiltersManager__AddItemLootFilter_x = 0x0;
//export constexpr uint32_t LootFiltersManager__GetItemFilterData_x = 0x0;
//export constexpr uint32_t LootFiltersManager__RemoveItemLootFilter_x = 0x0;
//export constexpr uint32_t LootFiltersManager__SetItemLootFilter_x = 0x0;

// COptionsWnd
export constexpr uint32_t COptionsWnd__FillChatFilterList_x = 0x700C80;

// CResolutionHandler
export constexpr uint32_t CResolutionHandler__UpdateResolution_x = 0x8D9C50;
export constexpr uint32_t CResolutionHandler__GetWindowedStyle_x = 0x5B9420;

// CColorPickerWnd
export constexpr uint32_t CColorPickerWnd__Open_x = 0x659AF0;

// CGroupWnd
export constexpr uint32_t CGroupWnd__WndNotification_x = 0x67C620;
export constexpr uint32_t CGroupWnd__UpdateDisplay_x = 0x67B470;

// ItemBase
export constexpr uint32_t ItemBase__IsLore_x = 0x7B4B70;
//export constexpr uint32_t ItemBase__IsLoreEquipped_x = 0x0;

export constexpr uint32_t MultipleItemMoveManager__ProcessMove_x = 0x572300;

// EQPlacedItemManager
export constexpr uint32_t EQPlacedItemManager__Instance_x = 0x5061E0;
export constexpr uint32_t EQPlacedItemManager__GetItemByGuid_x = 0x5060C0;
export constexpr uint32_t EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x = 0x506120;

// RealEstateManagerClient
export constexpr uint32_t RealEstateManagerClient__Instance_x = 0x5B3540;

// FactionManagerClient
//export constexpr uint32_t FactionManagerClient__Instance_x = 0x0;
//export constexpr uint32_t FactionManagerClient__HandleFactionMessage_x = 0x0;
//export constexpr uint32_t FactionManagerClient__GetFactionStanding_x = 0x0;
//export constexpr uint32_t FactionManagerClient__GetMaxFaction_x = 0x0;
//export constexpr uint32_t FactionManagerClient__GetMinFaction_x = 0x0;

// AggroMeterManagerClient
export constexpr uint32_t AggroMeterManagerClient__Instance_x = 0x4110E0;

// ClientSOIManager
export constexpr uint32_t ClientSOIManager__GetSingleton_x = 0x466CC0;

// MercenaryAlternateAdvancementManagerClient
//export constexpr uint32_t MercenaryAlternateAdvancementManagerClient__Instance_x = 0x0;
//export constexpr uint32_t MercenaryAlternateAdvancementManagerClient__BuyAbility_x = 0x0;
//export constexpr uint32_t CAltAbilityData__GetMercCurrentRank_x = 0x0;
//export constexpr uint32_t CAltAbilityData__GetMercMaxRank_x = 0x0;

// CTargetManager
export constexpr uint32_t CTargetManager__Get_x = 0x5C6380;

// RealEstateManagerClient

// EQGroundItemListManager
export constexpr uint32_t EQGroundItemListManager__Instance_x = 0x4DC1B0;

// KeyCombo
export constexpr uint32_t KeyCombo__GetTextDescription_x = 0x88B690;

// CUnSerializeBuffer
export constexpr uint32_t CUnSerializeBuffer__GetString_x = 0x4048D0;

// ZoneGuideManagerClient
export constexpr uint32_t ZoneGuideManagerClient__Instance_x = 0x600B10;

// AchievementManager
export constexpr uint32_t AchievementManager__Instance_x = 0x404060;

// FreeToPlayClient
export constexpr uint32_t FreeToPlayClient__Instance_x = 0x7DBA30;
export constexpr uint32_t FreeToPlayClient__RestrictionInfo_x = 0xA0AA28;

// UdpConnection
export constexpr uint32_t UdpConnection__GetStats_x = 0x8C2FD0;
