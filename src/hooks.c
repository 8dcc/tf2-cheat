
#include <stdbool.h>
#include <stdio.h>
#include "include/hooks.h"
#include "include/globals.h"
#include "include/sdk.h"
#include "include/math.h"
#include "include/menu.h"
#include "features/features.h"

#define MAX_CHOKE 16

DECL_HOOK(LevelShutdown);
DECL_HOOK(LevelInitPostEntity);
DECL_HOOK(FrameStageNotify);
DECL_HOOK(CreateMove);
DECL_HOOK(OverrideView);
DECL_HOOK(Paint);
DECL_HOOK(PaintTraverse);
DECL_HOOK(DrawModelExecute);
DECL_HOOK(RunCommand);
DECL_HOOK(GetUserCmd);
DECL_HOOK(OnScreenSizeChanged);

SwapWindow_t ho_SwapWindow = NULL;
PollEvent_t ho_PollEvent   = NULL;

/*----------------------------------------------------------------------------*/

bool hooks_init(void) {
    VMT_HOOK(i_baseclient, LevelShutdown);
    VMT_HOOK(i_baseclient, LevelInitPostEntity);
    VMT_HOOK(i_baseclient, FrameStageNotify);
    VMT_HOOK(i_clientmode, CreateMove);
    VMT_HOOK(i_clientmode, OverrideView);
    VMT_HOOK(i_enginevgui, Paint);
    VMT_HOOK(i_panel, PaintTraverse);
    VMT_HOOK(i_modelrender, DrawModelExecute);
    VMT_HOOK(i_prediction, RunCommand);
    VMT_HOOK(i_input, GetUserCmd);
    VMT_HOOK(i_surface, OnScreenSizeChanged);

    HOOK_SDL(SwapWindow);
    HOOK_SDL(PollEvent);

    return true;
}

bool hooks_restore(void) {
    /* We don't worry about VMT hooks since we restore the whole VMT.
     * Here we restore SDL pointers */
    *SwapWindowPtr = ho_SwapWindow;
    *PollEventPtr  = ho_PollEvent;

    return true;
}

/*----------------------------------------------------------------------------*/

void h_LevelShutdown(BaseClient* thisptr) {
    ORIGINAL(LevelShutdown, thisptr);

    /* Clear localplayer idx */
    g.localidx = 0;

    /* Clear FrameStageNotify cache */
    cache_reset();

    /* Clear model index cache */
    for (int i = 0; i < MDLIDX_ARR_SZ; i++)
        g.mdl_idx[i] = -1;

    /* Reset cvars to their default values */
    cache_reset_cvars();
}

void h_LevelInitPostEntity(BaseClient* thisptr) {
    ORIGINAL(LevelInitPostEntity, thisptr);

    /* Get once on LevelInit */
    g.localidx = METHOD(i_engine, GetLocalPlayer);

    /* Get model indexes that we might need in the game (e.g. for entity esp) */
    cache_get_model_idx();

    /* Cache cvars we are going to use */
    cache_store_cvars();
}

void h_FrameStageNotify(BaseClient* thisptr, ClientFrameStage_t curStage) {
    /* On FRAME_RENDER_START, if we want to use network angles for thirdperson,
     * set them before calling the original FrameStageNotify. */
    if (curStage == FRAME_RENDER_START) {
        if (g.localplayer && settings.thirdperson &&
            settings.thirdperson_network && !vec_is_zero(g.render_angles))
            g.localplayer->v_angle = g.render_angles;
    }

    ORIGINAL(FrameStageNotify, thisptr, curStage);

    switch (curStage) {
        case FRAME_NET_UPDATE_START:
            /* Reset because we don't want to cache a removed pointer */
            cache_reset();
            break;
        case FRAME_NET_UPDATE_END:
            /* Cache information for the rest of the cheat */
            cache_update();
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------------------------*/

bool h_CreateMove(ClientMode* thisptr, float flInputSampleTime,
                  usercmd_t* cmd) {
    /* Reset each tick */
    g.psilent = false;

    const vec3_t old_angles  = cmd->viewangles;
    const float old_sidemove = cmd->sidemove;
    const float old_forward  = cmd->forwardmove;

    bool ret = ORIGINAL(CreateMove, thisptr, flInputSampleTime, cmd);

    /* On dummy calls, return */
    if (!cmd || cmd->command_number == 0)
        return ret;

    /* If original returned true, update engine viewangles to cmd viewangles */
    if (ret)
        METHOD_ARGS(i_engine, SetViewAngles, &cmd->viewangles);

    if (g.localplayer) {
        GetFlags(g.localplayer);
    }

    nopush();
    thirdperson();
    bhop(cmd);
    autobackstab(cmd);

    pred_start(cmd);
    {
        spinbot(cmd);
        meleebot(cmd);
        aimbot(cmd);
        automedigun(cmd);
        autorocketjump(cmd);
        auto_detonate_stickies(cmd);
    }
    pred_end();

    antiafk(cmd);
    store_velocity();

    ang_clamp(&cmd->viewangles);
    correct_movement(cmd, old_angles);

    /* Did I choke in the last tick? */
    static bool did_choke = false;

    if (g.psilent) {
        *bSendPacket = false;
        did_choke    = true;
    } else if (did_choke) {
        /* Only restore if we chocked on the tick before this one */
        *bSendPacket = true;
        did_choke    = false;

        cmd->viewangles  = old_angles;
        cmd->sidemove    = old_sidemove;
        cmd->forwardmove = old_forward;
    }

    /* Make sure we aren't choking too many packets */
    if (c_clientstate->chokedcommands >= MAX_CHOKE) {
        *bSendPacket = true;
        did_choke    = false;
    }

    /* Save real angles for renderning in FrameStageNotify(RENDER_START) */
    if (*bSendPacket)
        g.render_angles = cmd->viewangles;

    return false;
}

/*----------------------------------------------------------------------------*/

void h_OverrideView(ClientMode* thisptr, ViewSetup* pSetup) {
    custom_fov(pSetup);

    ORIGINAL(OverrideView, thisptr, pSetup);
}

/*----------------------------------------------------------------------------*/

static void update_w2s_viewmatrix(void) {
    /* Get player view and viewmatrix */
    static ViewSetup player_view;
    if (!METHOD_ARGS(i_baseclient, GetPlayerView, &player_view))
        return;

    static VMatrix w2v, v2pr, w2px; /* Unused */
    METHOD_ARGS(i_renderview, GetMatricesForView, &player_view, &w2v, &v2pr,
                &g.w2s_vmatrix, &w2px);
}

void h_Paint(EngineVGui* thisptr, uint32_t mode) {
    ORIGINAL(Paint, thisptr, mode);

    if (mode & PAINT_UIPANELS) {
        StartDrawing(i_surface);
        {
            /* Update viewmatrix used by world_to_screen each frame */
            update_w2s_viewmatrix();

            esp();
            draw_aim_fov();
            spectator_list();
            draw_velocity();
        }
        FinishDrawing(i_surface);
    }
}

/*----------------------------------------------------------------------------*/

void h_PaintTraverse(IPanel* thisptr, VPanel panel, bool forcerepaint,
                     bool allowforce) {
    if (settings.clean_screenshots && METHOD(i_engine, IsTakingScreenshot)) {
        ORIGINAL(PaintTraverse, thisptr, panel, forcerepaint, allowforce);
        return;
    }

    const char* panel_name = METHOD_ARGS(i_panel, GetName, panel);
    if (!panel_name) {
        ORIGINAL(PaintTraverse, thisptr, panel, forcerepaint, allowforce);
        return;
    }

    switch (hash_str(panel_name)) {
        case 0x47DE1CB6: /* hash_str("HudScope") */
            if (!settings.remove_scope)
                break;

            int sw = 0, sh = 0;
            METHOD_ARGS(i_engine, GetScreenSize, &sw, &sh);

            /* Draw lines */
            METHOD_ARGS(i_surface, SetColor, 5, 5, 5, 255);
            METHOD_ARGS(i_surface, DrawLine, 0, sh / 2, sw, sh / 2); /* - */
            METHOD_ARGS(i_surface, DrawLine, sw / 2, 0, sw / 2, sh); /* | */

            return; /* Don't draw this panel (scope) */
        default:
            break;
    }

    ORIGINAL(PaintTraverse, thisptr, panel, forcerepaint, allowforce);
}

/*----------------------------------------------------------------------------*/

void h_DrawModelExecute(ModelRender* thisptr, DrawModelState_t* state,
                        ModelRenderInfo_t* pInfo,
                        matrix3x4_t* pCustomBoneToWorld) {
    if (settings.ps1_visuals)
        state->m_lod = 5;

    if (settings.clean_screenshots && METHOD(i_engine, IsTakingScreenshot)) {
        ORIGINAL(DrawModelExecute, thisptr, state, pInfo, pCustomBoneToWorld);
        return;
    }

    /* Store original renderview colors */
    static float_rgba_t orig_col;
    METHOD_ARGS(i_renderview, GetColorModulation, &orig_col);
    orig_col.a = METHOD(i_renderview, GetBlend);

    /* Override renderview color, material, etc. if needed */
    chams(thisptr, state, pInfo, pCustomBoneToWorld);

    /* Call original */
    ORIGINAL(DrawModelExecute, thisptr, state, pInfo, pCustomBoneToWorld);

    /* Reset old renderview color */
    METHOD_ARGS(i_renderview, SetColorModulation, &orig_col);
    METHOD_ARGS(i_renderview, SetBlend, orig_col.a);

    /* Reset to defaut materials */
    METHOD_ARGS(i_modelrender, ForcedMaterialOverride, NULL, OVERRIDE_NORMAL);
}

/*----------------------------------------------------------------------------*/

void h_RunCommand(IPrediction* thisptr, Entity* player, usercmd_t* cmd,
                  MoveHelper* move_helper) {
    /* We only hook this to get i_movehelper for prediction */
    if (!i_movehelper)
        i_movehelper = move_helper;

    ORIGINAL(RunCommand, thisptr, player, cmd, move_helper);
}

/*----------------------------------------------------------------------------*/

usercmd_t* h_GetUserCmd(CInput* thisptr, int sequence_number) {
    /*
     * To get CInput.m_pCommands, you can look at the first line of CInput's
     * CreateMove(). From IDA's decompiler:
     *
     *   v5 = thisptr[64] + 68 * (sequence_number % 90);
     *
     * Where `thisptr` is the first argument of the function and
     * `sequence_number` is the second argument.
     *
     * You can probably tell that it's accessing an index in an array at
     * `thisptr[64]`, where each item of the array is 68 bytes. This is the
     * location of m_pCommands, but keep in mind that `thisptr[64]` is the same
     * as:
     *
     *   thisptr + (sizeof(void*) * 64)
     *
     * So the offset is not 64 but 0x100.
     */
    usercmd_t* usercmd =
      &thisptr->m_pCommands[sequence_number % MULTIPLAYER_BACKUP];

    /* We need to hook this to remove the original check, for crithack:
     * https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/game/client/in_main.cpp#L1405-L1408
     */
    return usercmd;
}

/*----------------------------------------------------------------------------*/

void h_OnScreenSizeChanged(MatSurface* thisptr, int nOldWidth, int nOldHeight) {
    ORIGINAL(OnScreenSizeChanged, thisptr, nOldWidth, nOldHeight);

    /* Reload fonts */
    fonts_init();
}

/*----------------------------------------------------------------------------*/

static inline void toggle_keybinds(void) {
    /* Toggle menu and cursor */
    if (nk_input_is_key_released(&ctx->input, MENU_KEY)) {
        menu_open = !menu_open;
        METHOD_ARGS(i_surface, SetCursorAlwaysVisible, menu_open);
    }

    /* Feature keybinds. See features.h */
    aimbot_key_down = nk_input_is_key_down(&ctx->input, settings.aim_keycode);
    meleebot_key_down =
      nk_input_is_key_down(&ctx->input, settings.melee_keycode);
}

void h_SwapWindow(SDL_Window* window) {
    /* Initialize once */
    if (!ctx)
        if (!menu_init(window))
            return ORIGINAL(SwapWindow, window);

    /* Switch to our gl context */
    SDL_GL_MakeCurrent(window, gl_ctx);

    /* Check if we need to toggle menu or keybinds */
    toggle_keybinds();

    /* Render the watermark and menu */
    if (settings.watermark || menu_open) {
        if (settings.watermark)
            watermark_render();

        if (menu_open)
            menu_render();

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY,
                      MAX_ELEMENT_MEMORY);
    }

    /* Switch to original gl context */
    SDL_GL_MakeCurrent(window, orig_gl_ctx);

    /* Call original */
    nk_input_begin(ctx);
    ORIGINAL(SwapWindow, window);
    nk_input_end(ctx);
}

int h_PollEvent(SDL_Event* event) {
    int ret = ORIGINAL(PollEvent, event);

    /*
     * Credits:
     * https://github.com/deboogerxyz/ah4/blob/93e135135716e4d840547eea3c1ad922714ef9b7/hooks.c#L65
     */
    if (ret && nk_sdl_handle_event(event) && menu_open)
        event->type = 0;

    return ret;
}
