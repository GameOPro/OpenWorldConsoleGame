#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <gamevectors.h>
#include <gameobject.h>


class BinaryFile{
    private:
        std::string Filename;
        std::fstream File;

    public:
        BinaryFile(std::string filename);
        bool WriteGameObject(std::vector<GameObject*> GoP);
        std::vector<GameObject*> ReadGameObject();
};