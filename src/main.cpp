#include "game.h"
#include <time.h>
#include <iostream>
#include <thread>
#include <windows.h>

#include <fstream>
#include <map.h>

Game game = Game(50, 20);

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

int main(){
    HWND Hwnd = GetForegroundWindow();
    ShowWindow(Hwnd, SW_MAXIMIZE);  

    std::cout << "Maximize the window!";
    Sleep(3000);
    system("cls");

    ShowConsoleCursor(false);
    game.Init();
    game.Render(true);

    // std::ifstream file_input("C:\\Users\\Game\\Downloads\\cpp test\\nav4\\src\\Map.json");
    // Json::Reader reader;
    // Json::Value root;
    // reader.parse(file_input, root);

    while(true){
        game.ProcessInput();
        game.Update();
        
        //load map from json (to make)
    }

    return 1;
}
