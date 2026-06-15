// F4Online - F4SE plugin entry point.
//
// This is the minimal-but-real skeleton: it opens an F4SE-style log, registers
// for F4SE's lifecycle messages, and reports each one. It uses no hardcoded
// game addresses and no game structures yet, so it is safe to load on any
// Next-Gen runtime. As real multiplayer hooks are added, see the note on
// F4SEPlugin_Version below about declaring version/address compatibility.

#include "common/IPrefix.h"          // UInt32/UInt64, gLog + _MESSAGE, winsock2/Windows
#include <shlobj.h>                  // CSIDL_MYDOCUMENTS

#include "f4se_common/f4se_version.h" // RUNTIME_VERSION_*, PACKED_F4SE_VERSION
#include "f4se/PluginAPI.h"           // F4SEInterface, F4SEPluginVersionData, messaging

namespace {
    constexpr UInt32 kPluginVersion = 1;

    PluginHandle                     g_pluginHandle = kPluginHandle_Invalid;
    const F4SEMessagingInterface*    g_messaging    = nullptr;

    // Called by F4SE for each lifecycle event we registered for (sender "F4SE").
    void OnF4SEMessage(F4SEMessagingInterface::Message* msg)
    {
        switch (msg->type) {
        case F4SEMessagingInterface::kMessage_PostLoad:
            _MESSAGE("message: PostLoad (all plugins loaded)");
            break;
        case F4SEMessagingInterface::kMessage_PostPostLoad:
            _MESSAGE("message: PostPostLoad");
            break;
        case F4SEMessagingInterface::kMessage_InputLoaded:
            _MESSAGE("message: InputLoaded");
            break;
        case F4SEMessagingInterface::kMessage_GameDataReady:
            _MESSAGE("message: GameDataReady (data handler %s)",
                     msg->data ? "ready" : "unloading");
            break;
        case F4SEMessagingInterface::kMessage_NewGame:
            _MESSAGE("message: NewGame");
            break;
        case F4SEMessagingInterface::kMessage_PostLoadGame:
            _MESSAGE("message: PostLoadGame (%s)",
                     msg->data ? "loaded ok" : "load failed");
            break;
        case F4SEMessagingInterface::kMessage_GameLoaded:
            _MESSAGE("message: GameLoaded (one-time, fully initialised)");
            break;
        default:
            _MESSAGE("message: unhandled type %u", msg->type);
            break;
        }
    }
}

extern "C" {

// F4SE reads this exported struct to decide whether to load the plugin.
//
// We currently declare Address Library independence for both Next-Gen lines so
// F4SE will load us regardless of the exact 1.10.980+/1.11.x build, and
// NoStructs because we touch no game structures yet. The moment real hooks are
// added you must EITHER keep using the Address Library (and resolve every
// address through it) OR switch to an explicit `compatibleVersions` list and
// drop these independence flags - otherwise the declaration becomes a lie and
// will crash on mismatched runtimes.
__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
{
    F4SEPluginVersionData::kVersion,

    kPluginVersion,
    "F4Online",
    "bagglebrain",

    F4SEPluginVersionData::kAddressIndependence_AddressLibrary_1_10_980 |
        F4SEPluginVersionData::kAddressIndependence_AddressLibrary_1_11_137,
    F4SEPluginVersionData::kStructureIndependence_NoStructs,
    { 0 },  // compatibleVersions: empty - relying on address independence above

    0,      // seVersionRequired: any
    0, 0,   // reservedNonBreaking / reservedBreaking
    { 0 }   // reserved
};

__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
{
    gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\F4Online.log");
    gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
    gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

    _MESSAGE("F4Online v%u loading - F4SE %08X, runtime %08X%s",
             kPluginVersion, f4se->f4seVersion, f4se->runtimeVersion,
             f4se->isEditor ? " (editor)" : "");

    g_pluginHandle = f4se->GetPluginHandle();

    g_messaging = static_cast<const F4SEMessagingInterface*>(
        f4se->QueryInterface(kInterface_Messaging));
    if (g_messaging) {
        // Listen to F4SE itself for the game lifecycle events above.
        g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);
        _MESSAGE("registered F4SE messaging listener");
    } else {
        _MESSAGE("ERROR: messaging interface unavailable");
    }

    _MESSAGE("F4Online load complete");
    return true;
}

}; // extern "C"
