#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <exception>

#include "gamevectors.h"

enum ObjectType : char{
    NONE = ' ',
    WALL = '#',
    PLATFORM_UP = '^',
    PLATFORM_RIGHT = '>',
    PLATFORM_LEFT = '<',
    PLAYER = 'x',
    PLAYER_SPAWN = 'S',
    DEADLY = '!',
    CONVEYOR_UP = '~',
    CONVEYOR_RIGHT = ')',
    CONVEYOR_LEFT = '(',
    CONVEYOR_DOWN = 'U',
    SIGN = '@',
    PORTAL = '0'
};

enum Direction{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

class GameObject{
    public:
        const char* Name;
        Vector2I Position = Vector2I(0, 0);
        ObjectType Type = NONE;

        //CONSTRUCTOR
        GameObject();
        //GameObject(const GameObject&);
        GameObject(const char* name, Vector2I pos, ObjectType objectType);
        GameObject(const char* name, int posX, int posY, ObjectType objectType);
        
        //DESTRUCTOR
        //~GameObject();
        
        // FUNCTIONS
        int GetID();
        virtual std::string GetText();
        virtual std::string GetPortalID();

        // OPERATOR OVERLOAD
        bool operator==(const GameObject &) const;
        void operator=(const GameObject &);


        // STATIC
        static const char* getTypeAsString(ObjectType type);
        static const char* getDirectionAsString(Direction direction);
        
        //SAVE FILE
        void write(std::ostream& f);
        void read(std::istream& f);


        // TEMPLATE
        template<class T>
        T* Cast(){
            try{
                return static_cast<T*>(this);
            }
            catch(std::exception ex){
                return NULL;
            }
        };
    private:
        const int ID;
};

class Sign : public GameObject{
    public:
        std::string Text;

        Sign();
        Sign(Vector2I pos, std::string text);
        std::string GetText();

        //SAVE FILE
        void write(std::ostream& f);
        void read(std::istream& f);
};

class Portal : public GameObject{
    public:
        std::string PortalID;

        Portal();
        Portal(Vector2I pos, std::string portalID);
        std::string GetPortalID();

        //SAVE FILE
        void write(std::ostream& f);
        void read(std::istream& f);
};

#endif