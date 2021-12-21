#ifndef _Z3DCUTSCENE_H_
#define _Z3DCUTSCENE_H_

#include "z3Dvec.h"

typedef struct {
    /* 0x00 */ u16 action; // "dousa"
    /* 0x02 */ u16 startFrame;
    /* 0x04 */ u16 endFrame;
    /* 0x06 */ Vec3s rot;
    /* 0x0C */ Vec3i startPos;
    /* 0x18 */ Vec3i endPos;
    /* 0x24 */ Vec3i normal;
} CsCmdActorAction; // size = 0x30

typedef enum {
    /* 0x00 */ OCARINA_ACTION_UNK_0,
    /* 0x01 */ OCARINA_ACTION_FREE_PLAY_OCARINA,
    /* 0x02 */ OCARINA_ACTION_MINUET,
    /* 0x03 */ OCARINA_ACTION_BOLERO,
    /* 0x04 */ OCARINA_ACTION_SERENADE,
    /* 0x05 */ OCARINA_ACTION_REQUIEM,
    /* 0x06 */ OCARINA_ACTION_NOCTURNE,
    /* 0x07 */ OCARINA_ACTION_PRELUDE,
    /* 0x08 */ OCARINA_ACTION_SARIAS_SONG,
    /* 0x09 */ OCARINA_ACTION_EPONAS_SONG,
    /* 0x0A */ OCARINA_ACTION_ZELDAS_LULLABY,
    /* 0x0B */ OCARINA_ACTION_SUNS_SONG,
    /* 0x0C */ OCARINA_ACTION_SONG_OF_TIME,
    /* 0x0D */ OCARINA_ACTION_SONG_OF_STORMS,
    /* 0x0E */ OCARINA_ACTION_OCARINA_ACTION_UNK_E,
    /* 0x0F */ OCARINA_ACTION_MINUET_PLAYBACK,
    /* 0x10 */ OCARINA_ACTION_BOLERO_PLAYBACK,
    /* 0x11 */ OCARINA_ACTION_SERENADE_PLAYBACK,
    /* 0x12 */ OCARINA_ACTION_REQUIEM_PLAYBACK,
    /* 0013 */ OCARINA_ACTION_NOCTURNE_PLAYBACK,
    /* 0x14 */ OCARINA_ACTION_PRELUDE_PLAYBACK,
    /* 0x15 */ OCARINA_ACTION_SARIAS_SONG_PLAYBACK,
    /* 0x16 */ OCARINA_ACTION_EPONAS_SONG_PLAYBACK,
    /* 0x17 */ OCARINA_ACTION_ZELDAS_LULLABY_PLAYBACK,
    /* 0x18 */ OCARINA_ACTION_SUNS_SONG_PLAYBACK,
    /* 0x19 */ OCARINA_ACTION_SONG_OF_TIME_PLAYBACK,
    /* 0x1A */ OCARINA_ACTION_SONG_OF_STORMS_PLAYBACK
} OcarinaSongActionIDs;

#endif //_Z3DCUTSCENE_H_
