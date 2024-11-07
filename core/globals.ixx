export module globals;

// Import dependencies from other modules or standard library headers.
import common;
import eqgame;
import cxstr;
import eqgraphics;
import eqmain;
import eq;
import offset_utils;
import login_frontend;
import <dinput.h>;
import <cstdint>;
import <type_traits>;
import <iostream>;

namespace eqlib {

    // Define constants based on the target architecture.
#if defined(_M_AMD64)
    export constexpr uintptr_t EQGamePreferredAddress = 0x140000000;
    export constexpr uintptr_t EQLibraryPreferredAddress = 0x180000000;
#else
    export constexpr uintptr_t EQGamePreferredAddress = 0x400000;
    export constexpr uintptr_t EQLibraryPreferredAddress = 0x10000000;
#endif

    export uintptr_t EQGameBaseAddress = (uintptr_t)GetModuleHandle(nullptr);
    export uintptr_t EQGraphicsBaseAddress = NULL;
    export uintptr_t EQMainBaseAddress = NULL;

    // Define utility functions to calculate dynamic offsets.
    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
    inline uintptr_t FixEQGameOffset(T nOffset) {
        return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQGamePreferredAddress) + EQGameBaseAddress;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, void>>
    inline uintptr_t FixEQMainOffset(T nOffset) {
        return static_cast<uintptr_t>(nOffset) - static_cast<uintptr_t>(EQLibraryPreferredAddress) + EQMainBaseAddress;
    }

    // Define instance pointers and offset data
    export uintptr_t pinstCEverQuest = FixEQGameOffset(pinstCEverQuest_x);
    export uintptr_t pinstEverQuestInfo = FixEQGameOffset(pinstEverQuestInfo_x);
    export ForeignPointer<CEverQuest> pEverQuest;
    export EverQuestinfo* pEverQuestInfo = nullptr;

    export HMODULE* ghEQMainInstance;

    export uintptr_t EQMain__CEQSuiteTextureLoader__GetTexture = 0;
    export uintptr_t EQMain__CLoginViewManager__HandleLButtonUp = 0;
    export uintptr_t EQMain__CXWndManager__GetCursorToDisplay = 0;
    export uintptr_t EQMain__LoginController__GiveTime = 0;
    export uintptr_t EQMain__LoginController__ProcessKeyboardEvents = 0;
    export uintptr_t EQMain__LoginController__ProcessMouseEvents = 0;
    export uintptr_t EQMain__LoginController__Shutdown = 0;
    export uintptr_t EQMain__LoginServerAPI__JoinServer = 0;
    export uintptr_t EQMain__WndProc = 0;

    export uintptr_t EQMain__pinstCEQSuiteTextureLoader = 0;
    export uintptr_t EQMain__pinstCLoginViewManager = 0;
    export uintptr_t EQMain__pinstCSidlManager = 0;
    export uintptr_t EQMain__pinstCXWndManager = 0;
    export uintptr_t EQMain__pinstLoginController = 0;
    export uintptr_t EQMain__pinstLoginServerAPI = 0;
    export uintptr_t EQMain__pinstLoginClient = 0;

    export ForeignPointer<LoginClient> g_pLoginClient;
    export ForeignPointer<LoginController> g_pLoginController;
    export ForeignPointer<LoginServerAPI> g_pLoginServerAPI;


    // Define initialization functions
    export bool InitializeEQMainOffsets() {
        EQMainBaseAddress = (uintptr_t)GetModuleHandle(L"eqmain.dll");
        if (!EQMainBaseAddress) {
            return false;
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

        if (EQMain__LoginController__GiveTime) {
#if defined(_M_AMD64)
            EQMain__LoginController__ProcessKeyboardEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 9, 1, 4);
            EQMain__LoginController__ProcessMouseEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 22, 1, 4);
#else
            //EQMain__LoginController__ProcessKeyboardEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 3, 1, 4);
            //EQMain__LoginController__ProcessMouseEvents = GetFunctionAddressAt(EQMain__LoginController__GiveTime + 11, 1, 4);
#endif
        }

        g_pLoginController = EQMain__pinstLoginController;
        g_pLoginServerAPI = EQMain__pinstLoginServerAPI;
        g_pLoginClient = EQMain__pinstLoginClient;

        return true;
    }

    export void CleanupEQMainOffsets() {
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

    export void ShutdownEQLib() {
        CleanupEQMainOffsets();
    }

    // Initialize function to setup offsets and globals
    export void InitializeGlobals() {
        if (!EQGameBaseAddress) {
            return;
        }
        
        pEverQuestInfo = (EverQuestinfo*)pinstEverQuestInfo;
        pEverQuest = pinstCEverQuest;

        ghEQMainInstance = (HINSTANCE*)FixEQGameOffset(__heqmain_x);

        std::cout << "Set main instance" << *ghEQMainInstance << std::endl;
    }

}
