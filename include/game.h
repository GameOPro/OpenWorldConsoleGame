#ifndef GAME_H
#define GAME_H

#include <gameobject.h>
#include <iostream>
#include <string>

enum GameState{
    GAME_ACTIVE,
    GAME_MENU
};

enum UIState{
    INGAME,
    WRITING
};

class Game{
    public:
        GameState state;
        UIState UIstate;
        unsigned int Width, Height;

        //const/dest
        Game(unsigned int width, unsigned int height);
        ~Game();

        void Init();

        // Game Loop
        void ProcessInput();
        void Update();
        void Render(bool drawMap);
        bool DoCollisions();

        float Time();
        void GetPlayerPos();

        void RespawnPlayer();

        bool PlaceObject(Direction direction);
        std::string WriteToString(ObjectType objType);
        std::string WriteToString(bool savingFile);

        bool ReadSaveFile(std::string filename);
        bool WriteSaveFile(std::string filename);

        void GetAllBlocks();
};  

#endif