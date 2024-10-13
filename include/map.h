#pragma once

#include <gamevectors.h>
#include <gameobject.h>
#include <vector>

class Map{
    public:
        std::vector<GameObject*> objects;
        
        Map();
        Map(std::vector<GameObject*> objects);

        void addObj(GameObject* obj);
        void deleteObj(GameObject* obj);
        GameObject* getObjectAtMapPoint(Vector2I pos);
        ObjectType getObjectTypeAtMapPoint(Vector2I pos);
        GameObject* findFirstObjectOfType(ObjectType type);
        std::vector<GameObject*> GetAllObjectsOfType(ObjectType type);
        void deleteObjectAtMapPoint(Vector2I pos);
        int objectAmount();
    private:
        
};
