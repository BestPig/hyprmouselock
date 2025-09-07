#include <hyprland/src/includes.hpp>
#include <sstream>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/managers/eventLoop/EventLoopManager.hpp>

#include "globals.hpp"


static bool g_confined      = false;
static SP<HOOK_CALLBACK_FN> g_mouseMoveHook;

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    // Ensure the plugin matches the running Hyprland build (like common examples)
    const std::string HASH = __hyprland_api_get_hash();

    if (HASH != GIT_COMMIT_HASH) {
        throw std::runtime_error("[hyprmouselock] Version mismatch");
    }

    // Dispatcher "toggle" : plugin:hyprmouselock:toggle
    bool success = true;
    success = success &&
        HyprlandAPI::addDispatcherV2(
            PHANDLE,
            "hyprmouselock:toggle",
            [&](std::string /*args*/) -> SDispatchResult {
                g_confined = !g_confined;

            return {};
        });

    g_mouseMoveHook = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseMove",
        [&](void*, SCallbackInfo& info, std::any data) {
            if (!g_confined)
                return;

            // extract coords
            Vector2D pos;
            try {
                pos = std::any_cast<Vector2D>(data);
            } catch (...) {
                return;
            }

            // get target window (active)
            auto pWindow = g_pCompositor->m_lastWindow.lock();
            if (!pWindow)
                return;

            // use public bbox of the main surface so we don't include borders
            const auto box = pWindow->getWindowMainSurfaceBox();
            const double minX = box.x;
            const double minY = box.y;
            const double maxX = box.x + box.w - 1.0;
            const double maxY = box.y + box.h - 1.0;

            const double clampedX = std::clamp(pos.x, minX, maxX);
            const double clampedY = std::clamp(pos.y, minY, maxY);

            if (clampedX == pos.x && clampedY == pos.y)
                return;

            // outside: warp inside and cancel further handling of this event
            g_pCompositor->warpCursorTo(Vector2D{clampedX, clampedY});
            info.cancelled = true;
        });

    if (!success) {
        throw std::runtime_error("[hyprmouselock] Dispatcher registration failed");
    }

    PLUGIN_DESCRIPTION_INFO info{
        .name = "hyprmouselock",
        .description = "Confine cursor to active window",
        .version = "1.0"
    };
    return info;
}
