#include "game.h"
#include "map.h"
#include "gameobject.h"
#include <binaryfile.h>

#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <windows.h>
#include <vector>
#include <string>
#include <filesystem>

Map *map;
GameObject *plr;
std::string SavePathFile = "Saves/";

ObjectType selectedBlock = WALL;
int selectedIdx = 1;
Direction playerMove = UP;
float lastAction = 0.f;
float lastUIAction = 0.f;
float blockAction = 0.f;

bool paused = false;
bool allowPlacing = true;
bool useGravity = false;

bool grounded = true;
bool jumped = false;
bool teleported = false;

Game::Game(unsigned int width, unsigned int height) : state(GAME_ACTIVE), UIstate(INGAME), Width(width), Height(height){
    
}

Game::~Game(){
    delete map;
    delete plr;
}

void DrawGame(int Width, int Height);
void SelectBlock(int idx);


void Game::Init(){    

    //map
    std::vector<GameObject*> objects;
    GameObject* obj1 = new GameObject("SpawnPoint", Vector2I(0, 0), PLAYER_SPAWN);
    GameObject* obj2 = new GameObject("Wall", Vector2I(0, -1), WALL);
    objects.push_back(obj1);
    objects.push_back(obj2);
    map = new Map(objects);
    //vars
    plr = new GameObject("Player", map->objects[0]->Position, PLAYER);

    //Create a default save
    if(!std::filesystem::exists(SavePathFile + "default.save")){
        WriteSaveFile(SavePathFile + "default");
    }

    //Game Screen
    std::cout << std::endl;
    DrawGame(this->Width, this->Height);

}

void Game::Update(){

    ObjectType ot = map->getObjectTypeAtMapPoint(plr->Position);

    if( ot == CONVEYOR_UP || ot == CONVEYOR_RIGHT || ot == CONVEYOR_LEFT || ot == CONVEYOR_DOWN){
        this->DoCollisions();
        this->Render(true);
    }

    if(jumped){
        grounded = false;
        this->ProcessInput();
        plr->Position.y++;
        if(this->DoCollisions()){
            plr->Position.y--;
        }
        jumped = false;
        this->Render(true);
    }
    else if(useGravity && Time()+.1f > lastAction){
        do{
            ProcessInput();
            playerMove = DOWN;
            this->DoCollisions();   
            
            if(!grounded){
                plr->Position.y--;
                this->Render(true);
            }else{jumped = false;}
        }while(!grounded && Time()+.1f > lastAction && !useGravity);
    }
}

void Game::Render(bool drawMap){
    if(this->state == GAME_ACTIVE && this->UIstate == INGAME){
       
        if(drawMap){
            printf("\033[%d;%dH%c", 3 , 3, NULL);
        int halfHeight = (int)this->Height/2;
        int halfWidth = (int)this->Width/2;
        
        // Draw the map objects based on the player's location
        for(int y = plr->Position.y + halfHeight; y >= plr->Position.y - halfHeight; y--){
            for(int x = plr->Position.x - halfWidth + 1; x < plr->Position.x + halfWidth + 1; x++){
                ObjectType obj = map->getObjectTypeAtMapPoint(Vector2I(x, y));
                if(obj != NONE){
                    std::cout << obj;
                }
                else{
                    std::cout << ' ';
                }
            }
            std::cout << std::endl << " *";
        }

        //Draw Player after the map so he gets drawn ontop
        printf("\033[%d;%dH%c", halfHeight + 3, halfWidth + 2, plr->Type);
        }

        //Set Cursor to the end of the map
        printf("\033[%d;%dH%c", (int)this->Height + 5, 0, NULL);

        std::cout << "Selected Block: " << selectedBlock << " (" << GameObject::getTypeAsString(selectedBlock) << ")             " << std::endl;
        std::cout << "Player Pos: " << plr->Position << "         " << std::endl;
        std::cout << "Inside of: " << GameObject::getTypeAsString(map->getObjectTypeAtMapPoint(plr->Position)) << "              " << std::endl;
        std::cout << "Above of: " << GameObject::getTypeAsString(map->getObjectTypeAtMapPoint(Vector2I(plr->Position.x, plr->Position.y-1))) << "         " << std::endl;
        std::cout << "Object Count: " << map->objectAmount() << "    " << std::endl;
        if(useGravity){std::cout << "Gravity: On   " << std::endl;} else{std::cout << "Gravity: Off   " << std::endl;}
        if(allowPlacing){std::cout << "Placing: Enabled   " << std::endl;} else{std::cout << "Placing: Disabled     " << std::endl;}

        switch(map->getObjectTypeAtMapPoint(plr->Position)){
            case SIGN:
                {
                    std::cout << std::endl << "Sign Says: " << std::endl;
                    GameObject* obj = map->getObjectAtMapPoint(plr->Position);
                    std::string text = obj->Cast<Sign>()->GetText();;
                    std::cout << "\"" << text << "\"                                  ";
                }
                break;
            case PORTAL:
                {
                    std::cout << "Portal Amount: " << map->GetAllObjectsOfType(PORTAL).size() << std::endl;
                    std::cout << "PortalID: " << map->getObjectAtMapPoint(plr->Position)->Cast<Portal>()->GetPortalID();
                    for(auto obj : map->GetAllObjectsOfType(PORTAL)){
                        Portal* portal = obj->Cast<Portal>();
                        if(!teleported && portal->Position != plr->Position && portal->GetPortalID() == map->getObjectAtMapPoint(plr->Position)->Cast<Portal>()->GetPortalID()){
                            teleported = true;
                            plr->Position = portal->Position;
                            this->Render(true);
                            break;
                        }
                    }
                }
                break;
            default:
                for(int i = 0; i < 4; i++){
                    for(int j = 0; j < 2; j++){
                        std::cout << "                                                                                                                                                     ";
                    }
                    std::cout << std::endl;
                }
                break;

        }
    }   
}

void Game::ProcessInput(){
    if(this->state == GAME_ACTIVE && this->UIstate == INGAME){
        if(paused){
            system("cls");
            std::cout << std::endl;
            DrawGame(this->Width, this->Height);
            paused = false;
            this->Render(true);
        }

        if(GetAsyncKeyState('W')  & 0x8000){
            teleported = false;
            if(!useGravity){
                plr->Position.y++;
                if(this->DoCollisions()){
                    plr->Position.y--;
                }else{
                    this->Render(true);
                }
                playerMove = UP;
            }
            else if(useGravity && grounded && !jumped && Time() > lastAction){
                lastAction = Time() + .25f;

                if(Time() < lastAction){
                    plr->Position.y++;
                    if(this->DoCollisions()){
                        plr->Position.y--;
                    }
                    else{
                        playerMove = UP;
                        grounded = false;
                        jumped = true;
                        this->Render(true); 
                    }
                }

                lastAction = Time() + .35f;
            }
        }

        if(GetAsyncKeyState('D') & 0x8000){
            playerMove = RIGHT;
            teleported = false;
            plr->Position.x++;
            if(DoCollisions()){
                plr->Position.x--;
            }else{
                this->Render(true);
            }
        }

        if(GetAsyncKeyState('S') & 0x8000){
            if(!useGravity){
                playerMove = DOWN;
                teleported = false;
                plr->Position.y--;
                if(DoCollisions()){
                    plr->Position.y++;
                }else{
                    this->Render(true);
                }
            }
        }

        if(GetAsyncKeyState('A') & 0x8000){
            playerMove = LEFT;
            teleported = false;
            plr->Position.x--;
            if(DoCollisions()){
                plr->Position.x++;
            }else{
                this->Render(true);
            }
        }

        if(GetAsyncKeyState(VK_UP) & 0x8000){
            if(allowPlacing && PlaceObject(UP)){
                this->Render(true);
            }
        }

        if(GetAsyncKeyState(VK_RIGHT) & 0x8000){
            if(allowPlacing && PlaceObject(RIGHT)){
                this->Render(true);
            }
        }

        if(GetAsyncKeyState(VK_DOWN) & 0x8000){
            if(allowPlacing && PlaceObject(DOWN)){
                this->Render(true);
            }
        }

        if(GetAsyncKeyState(VK_LEFT) & 0x8000){
            if(allowPlacing && PlaceObject(LEFT)){
                this->Render(true);
            }
        }



        if(GetAsyncKeyState('Q') & 0x8000){
            if(Time() > lastUIAction + 0.125f){
                selectedIdx--;
                SelectBlock(selectedIdx);
                this->Render(false);
                lastUIAction = Time();
            }
        }

        if(GetAsyncKeyState('E') & 0x8000){
            if(Time() > lastUIAction + 0.125f){
                selectedIdx++;
                SelectBlock(selectedIdx);
                this->Render(false);
                lastUIAction = Time();
            }
        }
        
        if(GetAsyncKeyState(VK_ESCAPE) & 0x8000){
            if(Time() > lastUIAction + 0.3f){
                this->state = GAME_MENU;
                lastUIAction = Time();
                this->Render(false);
            }
        }
    
        if(GetAsyncKeyState('G') & 0x8000){
            if(Time() > lastUIAction + 0.1f){
                useGravity = !useGravity;
                lastUIAction = Time();
                grounded = false;
                this->Render(false);
            }
        }
    
        if(GetAsyncKeyState('P') & 0x8000){
            if(Time() > lastUIAction + 0.15f){
                allowPlacing = !allowPlacing;
                lastUIAction = Time();
                grounded = false;
                this->Render(false);
            }
        }
    }

    if(this->state == GAME_MENU && this->UIstate == INGAME){
        
        if(!paused){
            system("cls");
            std::cout << "Paused" << std::endl;
            std::cout << "Press [Esc] to Return" << std::endl;
            std::cout << "Press R to respawn" << std::endl;
            std::cout << "Press S to save the map" << std::endl;
            std::cout << "Press L to load a map" << std::endl;
            std::cout << "Press O to show every object" << std::endl;
            paused = true;
        }
        
        if(GetKeyState(VK_ESCAPE) & 0x8000){
            if(Time() > lastUIAction + 0.3f){
                this->state = GAME_ACTIVE;
                lastUIAction = Time();
                this->Render(true);
            }
        }

        if(GetKeyState('R') & 0x8000){
            RespawnPlayer();
            this->state = GAME_ACTIVE;
            this->Render(true);
        }

        if(GetKeyState('S') & 0x8000){
            if(Time() > lastUIAction + 0.5f){
                this->UIstate = WRITING;
                std::string filename;
                filename = WriteToString(true);
                
                std::cout << std::endl;
                
                if(WriteSaveFile(SavePathFile + filename)){
                    std::cout << "File " << filename << " Saved" << std::endl;
                }else{
                    std::cout << "File " << filename << " Could not be saved" << std::endl;
                }
                std::cout << "Press [esc] to continue";
                lastUIAction = Time();
            }
        }

        if(GetKeyState('L') & 0x8000){
            if(Time() > lastUIAction + 0.5f){
                this->UIstate = WRITING;
                std::string filename;
                filename = WriteToString(false);

                std::cout << std::endl;
                if(ReadSaveFile(SavePathFile + filename)){
                    std::cout << "File " << filename << " Loaded" << std::endl;
                }else{
                    std::cout << "File " << filename << " Could not be loaded, loading default" << std::endl;
                    ReadSaveFile(SavePathFile + "default");
                }
                std::cout << "Press [esc] to continue";
                
                lastUIAction = Time();
            }
        }

        if(GetKeyState('O') & 0x8000){
            if(Time() > lastUIAction + 0.5f){
                GetAllBlocks();
                
                lastUIAction = Time();
            }
        }
    }
}

bool Game::DoCollisions(){
    Vector2I playerPos = plr->Position;
    if(useGravity){
        switch(map->getObjectTypeAtMapPoint(Vector2I(plr->Position.x, plr->Position.y-1))){
            case DEADLY:
                grounded = false;
                break;
            case WALL:
            case PLATFORM_UP:
                if(playerMove == DOWN){
                    grounded = true;
                    return false;
                }
                break;
            case CONVEYOR_UP:
                if(playerMove == DOWN){
                    grounded = true;
                }
                break;
            default:
                grounded = false;
                break;
        }
    }

    switch(map->getObjectTypeAtMapPoint(playerPos)){
        case DEADLY:
            grounded = false;
            RespawnPlayer();
            return false;
        case WALL:
            return true;
        case PLATFORM_UP:
            if(playerMove == DOWN){
                return true;
            }
            break;
        case PLATFORM_LEFT:
            if(playerMove == RIGHT ){
                return true;
            }
            break;
        case PLATFORM_RIGHT:
            if(playerMove == LEFT){
                return true;
            }
            break;
        case CONVEYOR_UP:
            if(playerMove != DOWN){
                blockAction = Time() + 0.1f;
                playerMove = UP;
            }
            if(Time() > blockAction + 0.0025f){
                playerMove = UP;
                blockAction = Time();
                plr->Position.y++;
            }
            break;
        case CONVEYOR_RIGHT:
            if(playerMove != LEFT){
                blockAction = Time() + 0.1f;
                playerMove = RIGHT;
            }
            if(Time() > blockAction + 0.0025f){
                playerMove = RIGHT;
                blockAction = Time();
                plr->Position.x++;
            }
            break;
        case CONVEYOR_LEFT:
            if(playerMove != RIGHT){
                blockAction = Time() + 0.1f;
                playerMove = LEFT;
            }
            if(Time() > blockAction + 0.0025f){
                playerMove = LEFT;
                blockAction = Time();
                plr->Position.x--;
            }
            break;
        case CONVEYOR_DOWN:
            if(playerMove != UP){
                blockAction = Time() + 0.1f;
                playerMove = DOWN;
            }
            if(Time() > blockAction + 0.0025f){
                playerMove = DOWN;
                blockAction = Time();
                plr->Position.y--;
            }
            break;
        default:
            break;
    }
    return false;
}


bool Game::PlaceObject(Direction direction){
    short addX = 0;
    short addY = 0;
    bool shouldRender = false;

    switch(direction){
        case UP:
            addY = 1;
            break;
        case RIGHT:
            addX = 1;
            break;
        case DOWN:
            addY = -1;
            break;
        case LEFT:
            addX = -1;
            break;
    }

    Vector2I posToAdd = Vector2I(plr->Position.x + addX, plr->Position.y + addY);

    if(selectedBlock == PLAYER_SPAWN && map->getObjectTypeAtMapPoint(posToAdd) != PLAYER_SPAWN){
        map->deleteObj(map->findFirstObjectOfType(PLAYER_SPAWN));
        shouldRender = true;
    }

    if(!!map->getObjectAtMapPoint(posToAdd) && map->getObjectTypeAtMapPoint(posToAdd) != PLAYER_SPAWN && map->getObjectTypeAtMapPoint(posToAdd) != selectedBlock){
        map->deleteObjectAtMapPoint(posToAdd);
        shouldRender = true;

        if(selectedBlock == NONE && direction == DOWN){
            grounded = false;
        }
    }
    
    if(selectedBlock != NONE && map->getObjectTypeAtMapPoint(posToAdd) != PLAYER_SPAWN && map->getObjectTypeAtMapPoint(posToAdd) != selectedBlock){
        switch(selectedBlock){
            case WALL:
            case PLATFORM_UP:
            case PLATFORM_RIGHT:
            case PLATFORM_LEFT:
            case PLAYER_SPAWN:
            case DEADLY:
            case CONVEYOR_UP:
            case CONVEYOR_RIGHT:
            case CONVEYOR_LEFT:
            case CONVEYOR_DOWN:
            {
                map->addObj(new GameObject(GameObject::getTypeAsString(selectedBlock), posToAdd, selectedBlock));
                shouldRender = true;
                break;
            }
            case SIGN:
            {
                this->UIstate = WRITING;
                std::string text = WriteToString(SIGN);
                std::cout << text << std::endl;
                Sign* ObjToAdd = new Sign(posToAdd, text);
                map->addObj(ObjToAdd);
                shouldRender = true;
                break;
            }
            case PORTAL:
            {
                this->UIstate = WRITING;
                std::string portalID = WriteToString(PORTAL);
                
                if(portalID.length() != 0){
                    Portal* objToAdd = new Portal(posToAdd, portalID);
                    map->addObj(objToAdd);
                    shouldRender = true;
                }
                break;
            }
            default:
                break;
        }
    }
    return shouldRender;
}

void Game::GetPlayerPos(){
    std::cout << "(" << plr->Position.x << " " << plr->Position.y << ")";
}

void Game::RespawnPlayer(){
    plr->Position = map->findFirstObjectOfType(PLAYER_SPAWN)->Position;
    grounded = true;
    this->Render(true);
}

float Game::Time(){
    return clock() / 1000.000;
}


std::string Game::WriteToString(ObjectType objType){
    paused = true;
    system("cls");
    
    switch(objType){
        case SIGN:
            std::cout << "Write to sign (ESC to clear text): " << std::endl;
            break;
        case PORTAL:
            std::cout << "Write the Portal ID (ESC to clear text): " << std::endl;
            break;
        default:
            break;
    }

    while(_kbhit()) {_getch();}
    std::string text;
    if(this->UIstate == WRITING){
        std::getline(std::cin, text);
    }
    this->UIstate = INGAME;
    return text;
}

std::string Game::WriteToString(bool savingFile){
    paused = true;
    system("cls");
    
    if(savingFile){
        std::cout << "Save File as:";
    }else{
        std::cout << "Load File:";
    }

    std::string text;
    if(this->UIstate == WRITING){
        while(_kbhit()) {_getch();}
        std::getline(std::cin, text);
    }
    this->UIstate = INGAME;
    return text;
}


void SelectBlock(int idx){
    const short MAX_ITEMS = 12;

    if(idx < 0){
        selectedIdx = MAX_ITEMS;
    }else if (idx > MAX_ITEMS){
        selectedIdx = 0;
    }

    switch(selectedIdx){
        case 0:
            selectedBlock = NONE;
            break;
        case 1:
            selectedBlock = WALL;
            break;
        case 2:
            selectedBlock = PLATFORM_UP;
            break;
        case 3:
            selectedBlock = PLATFORM_RIGHT;
            break;
        case 4:
            selectedBlock = PLATFORM_LEFT;
            break;
        case 5:
            selectedBlock = PLAYER_SPAWN;
            break;
        case 6:
            selectedBlock = DEADLY;
            break;
        case 7:
            selectedBlock = CONVEYOR_UP;
            break;
        case 8:
            selectedBlock = CONVEYOR_RIGHT;
            break;
        case 9:
            selectedBlock = CONVEYOR_LEFT;
            break;
        case 10:
            selectedBlock = CONVEYOR_DOWN;
            break;
        case 11:
            selectedBlock = SIGN;
            break;
        case 12:
            selectedBlock = PORTAL;
            break;
    }
}

void DrawBar(int width){
    width = width+2;
    std::cout << ' ';
    for(int i = 0; i < width; i++){
        std::cout << '*';
    }
    std::cout << std::endl;
}

void DrawGame(int Width, int Height){
    DrawBar(Width);
    for(int i = 0; i < Height+1; i++){
        std::cout << " *";
        for(int j = 0; j < Width; j++){
            std::cout << ' '; 
        }
        std::cout << "* ";

        //display 
        switch(i){
            case 2:
                std::cout << "  Movements: ";
                break;
            case 3: 
                std::cout << "   W: UP";
                break;
            case 4:
                std::cout << "   A: LEFT";
                break;
            case 5:
                std::cout << "   S: DOWN";
                break;
            case 6:
                std::cout << "   D: RIGHT";
                break;
            case 8:
                std::cout << "  Placement: ";
                break;
            case 9:
                std::cout << "   Q | E: Switch Blocks";
                break;
            case 10:
                std::cout << "   Arrow Keys: Place Block";
                break;
            case 12:
                std::cout << "  Other:";
                break;
            case 13:
                std::cout << "    G: Toggle Gravity";
                break;
            case 14:
                std::cout << "    P: Toggle Placing";
        }

        std::cout << std::endl;
    }
    DrawBar(Width);
    std::cout << std::endl;
}




bool Game::ReadSaveFile(std::string f){
    map->objects.clear();
    
    BinaryFile bin(f+".save");
    std::vector<GameObject*> fileObjects = bin.ReadGameObject();
    if(fileObjects.size() == 0){
        return false;
    }

    for(auto obj : fileObjects){
        switch(obj->Type){
            case SIGN:
                {
                    map->addObj(new Sign(obj->Position, obj->Cast<Sign>()->Text));
                    break;
                }
            case PORTAL:
                {
                    map->addObj(new Portal(obj->Position, obj->Cast<Portal>()->PortalID));
                    break;
                }
            default:
                {
                    map->addObj(new GameObject(obj->Name, obj->Position, obj->Type));
                    break;
                }
        }
    }
    return true;
}

bool Game::WriteSaveFile(std::string f){
    BinaryFile bin(f+".save");

    return bin.WriteGameObject(map->objects);
}

void Game::GetAllBlocks(){
    std::cout << std::endl << "Objects: " << map->objects.size() << std::endl;
    for(auto &obj : map->objects){
        std::cout << obj->Name << " [" << obj->Position << ", " << obj->Type << "]" << std::endl;
    }
}