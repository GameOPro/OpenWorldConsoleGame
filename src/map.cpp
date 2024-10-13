#include "map.h"
#include "gamevectors.h"

#include <vector>
#include <algorithm>

Map::Map() : objects(){}

Map::Map(std::vector<GameObject*> gameObjects) : objects(gameObjects){}


GameObject* Map::getObjectAtMapPoint(Vector2I pos){
    auto it = find_if(this->objects.begin(), this->objects.end(), [&pos](const GameObject* obj) {return obj->Position == pos;});

    if(it != this->objects.end()){
        auto idx = std::distance(this->objects.begin(), it);

        return this->objects.at(idx);
    }

    return nullptr;
}

ObjectType Map::getObjectTypeAtMapPoint(Vector2I pos){
    auto it = find_if(this->objects.begin(), this->objects.end(), [&pos](const GameObject* obj) {return obj->Position == pos;});

    if(it != this->objects.end()){
        auto idx = std::distance(this->objects.begin(), it);

        return this->objects.at(idx)->Type;
    }

    return NONE;
}

GameObject* Map::findFirstObjectOfType(ObjectType type){
    auto it = find_if(this->objects.begin(), this->objects.end(), [&type](const GameObject* obj) {return obj->Type == type;});

    if(it != this->objects.end()){
        auto idx = std::distance(this->objects.begin(), it);

        return this->objects.at(idx);
    }

    return nullptr;
}

std::vector<GameObject*> Map::GetAllObjectsOfType(ObjectType type){
    std::vector<GameObject*> allObjects;

    for (std::vector<GameObject*>::iterator iter = std::find_if(this->objects.begin(), this->objects.end(), [&type](const GameObject* obj) {return obj->Type == type;}); 
        iter != this->objects.end(); 
        iter = std::find_if(++iter, this->objects.end(), [&type](const GameObject* obj) {return obj->Type == type;}))
    {
        allObjects.push_back(*iter);
    }

    if(allObjects.size() > 0){
        return allObjects;
    }

    // auto it = find_if(this->objects.begin(), this->objects.end(), [&type](const GameObject* obj) {return obj->Type == type;});

    // if(it != this->objects.end()){
    //     auto idx = std::distance(this->objects.begin(), it);

    //     allObjects->push_back(this->objects.at(idx));
    // }
    // if(!!allObjects->size() != 0){
    //     return allObjects;
    // }

    return std::vector<GameObject*>();
}


void Map::addObj(GameObject* obj){
    this->objects.push_back(obj);
}

void Map::deleteObj(GameObject* obj){
    auto it = find(this->objects.begin(), this->objects.end(), obj);

    if(it != this->objects.end()){
        this->objects.erase(it);
    }
}

int Map::objectAmount(){
    return this->objects.size();
}

void Map::deleteObjectAtMapPoint(Vector2I pos){
    auto it = find_if(this->objects.begin(), this->objects.end(), [&pos](const GameObject* obj) {return obj->Position == pos;});

    if(it != this->objects.end()){
        //auto idx = std::distance(this->objects.begin(), it);

        this->objects.erase(it);
    }
}