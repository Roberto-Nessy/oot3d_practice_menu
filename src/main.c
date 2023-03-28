/*
 * This file is a distant descendent of a file originally written by n3rdswithgame
 */

#include "menu.h"
#include "advance.h"
#include "draw.h"
#include "input.h"
#include "common.h"
#include "menus/cheats.h"
#include "menus/watches.h"
#include "menus/commands.h"
#include "menus/scene.h"
#include "menus/settings.h"
#include "3ds/extdata.h"
#include "3ds/services/irrst.h"
#include <string.h>

#include "z3D/z3D.h"
#include "actor.h"

#define NOCLIP_SLOW_SPEED 4
#define NOCLIP_FAST_SPEED 20

advance_ctx_t advance_ctx = {};
uint8_t practice_menu_init = 0;
static bool isAsleep = false;
u32 alertFrames = 0;
char* alertMessage = "";

GlobalContext* gGlobalContext;
u8 gInit = 0;

f32 sins(u16 angle) {
    // Taylor expansion up to x^7. Use symmetries for larger angles.
    if (angle <= 0x4000) {
        f32 theta = angle * 0.0000958737992429, theta2 = theta * theta, result = theta;
        theta *= theta2 * 0.166666666667;
        result -= theta;
        theta *= theta2 * 0.05;
        result += theta;
        theta *= theta2 * 0.0238095238095;
        result -= theta;
        return result;
    } else if (angle <= 0x8000) {
        return sins(0x8000 - angle);
    }
    return -sins(angle - 0x8000);
}

f32 coss(u16 angle) {
    return sins(angle + 0x4000);
}

void setGlobalContext(GlobalContext* globalContext) {
    gGlobalContext = globalContext;
}

void before_GlobalContext_Update(GlobalContext* globalCtx) {
    if (!gInit) {
        setGlobalContext(globalCtx);
        Actor_Init();
        irrstInit();
        gInit = 1;
    }
}

void after_GlobalContext_Update(GlobalContext* globalCtx) {}

static void toggle_advance(void) {
    if(pauseUnpause && advance_ctx.advance_state == NORMAL && !advance_ctx.latched){
        advance_ctx.advance_state = PAUSED;
        advance_ctx.latched = 1;
    } else if(pauseUnpause && advance_ctx.advance_state != NORMAL && !advance_ctx.latched) {
        advance_ctx.advance_state = NORMAL;
        advance_ctx.latched = 1;
    } else if (frameAdvance && advance_ctx.advance_state == NORMAL){
        advance_ctx.advance_state = LATCHED;
    } else if(!pauseUnpause) {
        advance_ctx.latched = 0;
    }
}

static void drawWatches(void) {
    for(u32 i = 0; i < WATCHES_MAX; ++i) {
        if (!watches[i].display) {
            continue;
        }

        // Skip attempting to draw the address if it would otherwise be an invalid read.
        // Attempting to read these locations would crash the game.
        const MemInfo address_info = query_memory_permissions((int)watches[i].addr);
        if (!is_valid_memory_read(&address_info)) {
            Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: Invalid address", watches[i].name);
            continue;
        }


        switch(watches[i].type) {
            case(S8): {
                s8 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %03d", watches[i].name, dst);
                break;
            }
            case(U8): {
                u8 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %03u", watches[i].name, dst);
                break;
            }
            case(X8): {
                u8 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %02X", watches[i].name, dst);
                break;
            }
            case(S16): {
                s16 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %05d", watches[i].name, dst);
                break;
            }
            case(U16): {
                u16 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %05u", watches[i].name, dst);
                break;
            }
            case(X16): {
                u16 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %04X", watches[i].name, dst);
                break;
            }
            case(S32): {
                s32 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %010d", watches[i].name, dst);
                break;
            }
            case(U32): {
                u32 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %010u", watches[i].name, dst);
                break;
            }
            case(X32): {
                u32 dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %08X", watches[i].name, dst);
                break;
            }
            case(F32): {
                float dst;
                memcpy(&dst, watches[i].addr, sizeof(dst));
                Draw_DrawFormattedString(70, 40 + i * SPACING_Y, COLOR_WHITE, "%s: %05.2F", watches[i].name, dst);
                break;
            }
        }
    }
    Draw_FlushFramebuffer();
}

void drawAlert() {
    if (ToggleSettingsMenu.items[TOGGLESETTINGS_PAUSE_AND_COMMANDS_DISPLAY].on == 0)
        alertFrames = 0;

    if (alertFrames > 0) {
        Draw_DrawFormattedStringTop(280, 220, COLOR_WHITE, alertMessage);
        Draw_FlushFramebufferTop();
        alertFrames--;
    }
}

static void titleScreenDisplay(void){
    Draw_DrawFormattedStringTop(150, 20, COLOR_WHITE, "OoT3D Practice Patch");
    Draw_FlushFramebufferTop();

    char menuComboString[COMMAND_COMBO_MAX + 1] = {0};
    Commands_ComboToString(menuComboString, 0);
    Draw_DrawFormattedString(150, 0, COLOR_WHITE, menuComboString);
    Draw_FlushFramebuffer();
}

void pauseDisplay(void) {
    if (ToggleSettingsMenu.items[TOGGLESETTINGS_PAUSE_AND_COMMANDS_DISPLAY].on == 0)
        return;

    Draw_DrawFormattedStringTop(20, 20, COLOR_WHITE, "Paused");
    Draw_FlushFramebufferTop();
}

void advance_main(void) {
    if(practice_menu_init == 0){
        Draw_SetupFramebuffer();
        extDataInit();
        Settings_LoadExtSaveData();
        practice_menu_init = 1;
    }

    if(gSaveContext.entranceIndex == 0x0629 && gSaveContext.cutsceneIndex == 0xFFF3 && gSaveContext.gameMode != 2){
        titleScreenDisplay();
    }

    if(shouldDrawWatches){
        drawWatches();
    }
    drawAlert();
    Input_Update();
    Command_UpdateCommands(rInputCtx.cur.val);

    if(menuOpen) {
        menuShow();

        Draw_Lock();
        Draw_ClearFramebuffer();
        Draw_FlushFramebuffer();
        Draw_Unlock();
        rInputCtx.cur.val = 0;
    }
    applyCheats();

    toggle_advance();

    if(advance_ctx.advance_state == STEP) {
        if(frameAdvance) {
            advance_ctx.advance_state = LATCHED;
        } else {
            advance_ctx.advance_state = PAUSED;
        }
    }
    pauseUnpause = 0;
    frameAdvance = 0;

    while(!isAsleep &&(advance_ctx.advance_state == PAUSED || advance_ctx.advance_state == LATCHED)) {
        pauseDisplay();
        drawAlert();
        Input_Update();
        Command_UpdateCommands(rInputCtx.cur.val);
        if(menuOpen) {
            menuShow();
            Draw_Lock();
            Draw_ClearFramebuffer();
            Draw_FlushFramebuffer();
            Draw_Unlock();
        }
        applyCheats();
        toggle_advance();
        if(advance_ctx.advance_state == LATCHED && !frameAdvance) {
            advance_ctx.advance_state = PAUSED;
        }
        if(advance_ctx.advance_state == PAUSED && frameAdvance) {
            advance_ctx.advance_state = STEP;
        }
        pauseUnpause = 0;
        frameAdvance = 0;
        svcSleepThread(16E6);
        rInputCtx.cur.val = 0;
    }
    isAsleep = false;

    if(noClip && releasedABbuttons) {
        u32 in = rInputCtx.cur.val;
        f32 amount = (in & BUTTON_R1) ? NOCLIP_FAST_SPEED : NOCLIP_SLOW_SPEED;
        if(in & BUTTON_L1) {
            if(in & (BUTTON_DOWN)) {
                PLAYER->actor.world.pos.y -= amount;
            }
            if(in & (BUTTON_UP)) {
                PLAYER->actor.world.pos.y += amount;
            }
        }
        else {
            if(in & (BUTTON_DOWN)) {
                PLAYER->actor.world.pos.z += amount;
            }
            if(in & (BUTTON_UP)) {
                PLAYER->actor.world.pos.z -= amount;
            }
            if(in & (gSaveContext.masterQuestFlag ? BUTTON_RIGHT : BUTTON_LEFT)) {
                PLAYER->actor.world.pos.x -= amount;
            }
            if(in & (gSaveContext.masterQuestFlag ? BUTTON_LEFT : BUTTON_RIGHT)) {
                PLAYER->actor.world.pos.x += amount;
            }
        }

        s16 yaw = gGlobalContext->cameraPtrs[gGlobalContext->activeCamera]->camDir.y;
        if(ControlStick_X * ControlStick_X + ControlStick_Y * ControlStick_Y > 100) {
            PLAYER->actor.world.pos.x += 0.02 * amount * (ControlStick_Y * sins(yaw) - ControlStick_X * coss(yaw));
            PLAYER->actor.world.pos.z += 0.02 * amount * (ControlStick_Y * coss(yaw) + ControlStick_X * sins(yaw));
        }

        if(in & BUTTON_X) {
            PLAYER->stateFlags2 |= 0x08000000; //freeze actors (ocarina state)
        }
        else if(in & BUTTON_Y) {
            PLAYER->stateFlags2 &= ~0x08000000; //unfreeze actors
        }

        if(in & BUTTON_A) { //confirm new position
            PLAYER->actor.home.pos = PLAYER->actor.world.pos;
            Scene_NoClipToggle();
        }
        else if(in & BUTTON_B) { //cancel movement
            PLAYER->actor.world.pos = PLAYER->actor.home.pos;
            Scene_NoClipToggle();
        }
    }
    else if(freeCam && releasedABbuttons) {
        u32 in = rInputCtx.cur.val;
        circlePosition cStick = rInputCtx.cStick;
        f32 posMult = (in & BUTTON_R1) ? 0.08 : 0.4;
        u16 rotMult = (in & BUTTON_R1) ? 2 : 5;

        if(in & BUTTON_L1 && cStick.dx * cStick.dx + cStick.dy * cStick.dy <= 100) {
            if(ControlStick_X * ControlStick_X + ControlStick_Y * ControlStick_Y > 100) {
                freeCamView.rot.x += ControlStick_Y * rotMult * 2;

                if(freeCamView.rot.x > 0x3F00) {
                    freeCamView.rot.x = 0x3F00;
                }
                else if(freeCamView.rot.x < -0x3F00) {
                    freeCamView.rot.x = -0x3F00;
                }

                freeCamView.rot.y -= ControlStick_X * rotMult * 2;
            }
        }
        else {
            if(ControlStick_X * ControlStick_X + ControlStick_Y * ControlStick_Y > 100) {
                freeCamView.pos.x += posMult * (ControlStick_Y * sins(freeCamView.rot.y) - ControlStick_X * coss(freeCamView.rot.y));
                freeCamView.pos.z += posMult * (ControlStick_Y * coss(freeCamView.rot.y) + ControlStick_X * sins(freeCamView.rot.y));
            }
        }

        if(cStick.dx * cStick.dx + cStick.dy * cStick.dy > 100) {
            freeCamView.rot.x += cStick.dy * rotMult;

            if(freeCamView.rot.x > 0x3F00) {
                freeCamView.rot.x = 0x3F00;
            }
            else if(freeCamView.rot.x < -0x3F00) {
                freeCamView.rot.x = -0x3F00;
            }

            freeCamView.rot.y -= cStick.dx * rotMult * ((gSaveContext.masterQuestFlag) ? -1 : 1);
        }

        if(in & BUTTON_UP) {
            freeCamView.pos.y += 50 * posMult;
        }
        if(in & BUTTON_DOWN) {
            freeCamView.pos.y -= 50 * posMult;
        }

        if(in & BUTTON_B) {
            Scene_FreeCamToggle();
        }
        else {
            View* view = &gGlobalContext->view;

            view->eye = view->at = freeCamView.pos;

            view->at.x += coss(freeCamView.rot.x) * sins(freeCamView.rot.y);
            view->at.y += sins(freeCamView.rot.x);
            view->at.z += coss(freeCamView.rot.x) * coss(freeCamView.rot.y);

            view->up.x = 0;
            view->up.y = 1;
            view->up.z = 0;

            view->distortionOrientation.x = 0;
            view->distortionOrientation.y = 0;
            view->distortionOrientation.z = 0;

            view->distortionScale.x = 1;
            view->distortionScale.y = 1;
            view->distortionScale.z = 1;
        }
    }
    else {
        releasedABbuttons = !(rInputCtx.cur.val & (BUTTON_A | BUTTON_B));
    }
}

void Gfx_SleepQueryCallback(void) {
    menuOpen = false;
    isAsleep = true;
}
