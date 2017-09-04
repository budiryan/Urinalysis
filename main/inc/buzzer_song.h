#ifndef	__BUZZER_SONG_H
#define	__BUZZER_SONG_H

#include "buzzer.h"

extern const MUSIC_NOTE START_UP[];
extern const MUSIC_NOTE START_UP2[];
extern const MUSIC_NOTE SUCCESSFUL_SOUND[];
extern const MUSIC_NOTE FAIL_SOUND[];
extern const MUSIC_NOTE BIRTHDAY_SONG[];
extern const MUSIC_NOTE CLICK[];

extern const MUSIC_NOTE MARIO_BEGIN[];
extern const MUSIC_NOTE MARIO[];

#define START_UP_play                 buzzer_play_song(START_UP,40,10)
#define	SUCCESSFUL_MUSIC			buzzer_play_song(SUCCESSFUL_SOUND, 100, 0)
#define	FAIL_MUSIC 						buzzer_play_song(FAIL_SOUND, 120, 100)
#define	CLICK_MUSIC						buzzer_play_song(CLICK, 20, 0)
#define MARIO_START                buzzer_play_song(MARIO_BEGIN, 30, 2)
#define MARIO_SONG                      buzzer_play_song(MARIO, 15, 0)
#define BIRTHDAY_SONG_GO                    buzzer_play_song(BIRTHDAY_SONG, 50, 0)

#endif /* __BUZZER_SONG_H */
