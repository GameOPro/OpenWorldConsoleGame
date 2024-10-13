#include "binaryfile.h"
#include "gamevectors.h"
#include "gameobject.h"

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>

BinaryFile::BinaryFile(std::string filename) : Filename(filename){};

bool BinaryFile::WriteGameObject(std::vector<GameObject*> GoP){
    if(!std::filesystem::exists("Saves")){
        std::filesystem::create_directory("Saves");
    }
    
    File.open(Filename, std::ios::binary | std::ios::out);

    if(!File.is_open()){
        std::cerr << "File Writing Error <" << Filename << ">" << std::endl;
        return false;
    }
    
    size_t ArrSize = GoP.size();
    File.write((char*)&ArrSize, sizeof(std::size_t));

    for(int i = 0; i < GoP.size(); i++){
        switch(GoP[i]->Type){
            case SIGN:
                {
                    Sign signObj = *GoP[i]->Cast<Sign>();
                    signObj.write(File);
                    continue;
                }
            case PORTAL:
                {
                    Portal portalObj = *GoP[i]->Cast<Portal>();
                    portalObj.write(File);
                    continue;
                }
            default:
                {
                    GameObject obj = *GoP[i];
                    obj.write(File);
                    continue;
                }
        }
    }
    File.close();
    return true;
}

std::vector<GameObject*> BinaryFile::ReadGameObject(){
    File.open(Filename, std::ios::binary | std::ios::in);
    std::ifstream bin(Filename, std::ios::binary | std::ios::in);

    if(!File){
        std::cerr << "File Reading Error <" << Filename << ">" << std::endl;
        return {};
    }
    std::vector<GameObject*> objects;

    size_t ObjAmount;
    File.read((char*)&ObjAmount, sizeof(std::size_t));

    int objectCount = 0;
    while(!File.eof() && objectCount < ObjAmount){
        ObjectType type;
        File.read((char*)&type, sizeof(type));

        switch(type){
            case SIGN:
                {
                    Sign sign;
                    sign.read(File);
                    objects.push_back(new Sign(sign));
                    break;
                }
            case PORTAL:
                {
                    Portal portal;
                    portal.read(File);
                    objects.push_back(new Portal(portal));
                    break;
                }
            default:
                {
                    GameObject obj;
                    obj.read(File);
                    objects.push_back(new GameObject(GameObject::getTypeAsString(obj.Type), obj.Position, obj.Type));
                    break;
                }
        }
        objectCount++;
    }

    File.close();
    return objects;
}