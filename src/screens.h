/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Screens Functions Declarations (Init, Update, Draw, Unload)
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty.
 *In no event will the authors be held liable for any damages arising from the
 *use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *including commercial applications, and to alter it and redistribute it freely,
 *subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *claim that you wrote the original software. If you use this software in a
 *product, an acknowledgment in the product documentation would be appreciated
 *but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *distribution.
 *
 *   Modifications:
 *   1. Luis Torres in December 2024.
 *
 **********************************************************************************************/

#ifndef SCREENS_H
#define SCREENS_H

#include "helpers.c"

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen
{
    START = 0,
    TITLE,
    INSTRUCTIONS,
    GAMEPLAY,
    ENDING
} GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;
extern Font font;
extern Music music;
extern Sound fxCoin;
extern int counter_1, counter_2;

#ifdef __cplusplus
extern "C"
{ // Prevents name mangling of functions
#endif

    //----------------------------------------------------------------------------------
    // Start Screen Functions Declaration
    //----------------------------------------------------------------------------------
    void InitStartScreen(void);
    void UpdateStartScreen(void);
    void DrawStartScreen(void);
    void UnloadStartScreen(void);
    int FinishStartScreen(void);

    //----------------------------------------------------------------------------------
    // Title Screen Functions Declaration
    //----------------------------------------------------------------------------------
    void InitTitleScreen(void);
    void UpdateTitleScreen(void);
    void DrawTitleScreen(void);
    void UnloadTitleScreen(void);
    int FinishTitleScreen(void);

    //----------------------------------------------------------------------------------
    // Instructions Screen Functions Declaration
    //----------------------------------------------------------------------------------
    void InitInstructionsScreen(void);
    void UpdateInstructionsScreen(void);
    void DrawInstructionsScreen(void);
    void UnloadInstructionsScreen(void);
    int FinishInstructionsScreen(void);

    //----------------------------------------------------------------------------------
    // Gameplay Screen Functions Declaration
    //----------------------------------------------------------------------------------
    void InitGameplayScreen(void);
    void UpdateGameplayScreen(void);
    void DrawGameplayScreen(void);
    void UnloadGameplayScreen(void);
    int FinishGameplayScreen(void);

    //----------------------------------------------------------------------------------
    // Ending Screen Functions Declaration
    //----------------------------------------------------------------------------------
    void InitEndingScreen(void);
    void UpdateEndingScreen(void);
    void DrawEndingScreen(void);
    void UnloadEndingScreen(void);
    int FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H
