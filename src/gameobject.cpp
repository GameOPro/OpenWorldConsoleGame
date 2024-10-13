#include <gameobject.h>
#include <string>
#include <iostream>


// GAMEOBJECT

static int ObjectCount = 0;

GameObject::GameObject() : Name("GameObject"), ID(ObjectCount){
    ObjectCount++;
}

GameObject::GameObject(const char* name, Vector2I pos, ObjectType objectType) : Name(name), Position(pos), Type(objectType), ID(ObjectCount){
    if(objectType == PLAYER){objectType = NONE;}
    ObjectCount++;
}

GameObject::GameObject(const char* name, int posX, int posY, ObjectType objectType) : Name(name), Position(Vector2I(posX, posY)), Type(objectType), ID(ObjectCount){
    if(objectType == PLAYER){objectType = NONE;} 
    ObjectCount++;
}

const char* GameObject::getTypeAsString(ObjectType objectType){
    switch(objectType){
        case NONE:
            return "None";
        case WALL:
            return "Wall";
        case PLATFORM_UP:
            return "Up One-Way";
        case PLATFORM_RIGHT:
            return "Right One-Way";
        case PLATFORM_LEFT:
            return "Left One-Way";
        case PLAYER:
            return "Player";
        case PLAYER_SPAWN: 
            return "Spawnpoint";
        case DEADLY:
            return "Deadly";
        case CONVEYOR_UP:
            return "Conveyor UP";
        case CONVEYOR_RIGHT:
            return "Conveyor RIGHT";
        case CONVEYOR_LEFT:
            return "Conveyor LEFT";
        case CONVEYOR_DOWN:
            return "Conveyor DOWN";
        case SIGN:
            return "Sign";
        case PORTAL:
            return "Portal";
        default:
            return "NULL";
    }
}

const char* GameObject::getDirectionAsString(Direction direction){
    switch(direction){
        case UP:
            return "UP";
        case RIGHT:
            return "RIGHT";
        case DOWN:
            return "DOWN";
        case LEFT:
            return "LEFT";
    }
    return "";
}

int GameObject::GetID(){
    return this->ID;
}

std::string GameObject::GetText(){
    return "";
}
std::string GameObject::GetPortalID(){
    return "";
}


bool GameObject::operator==(const GameObject & other) const{
    if(this->ID == other.ID){
        return true;
    }
    return false;
}

void GameObject::operator=(const GameObject & other){
    this->Name = other.Name;
    this->Position = other.Position;
    this->Type = other.Type;
}


void GameObject::write(std::ostream& f){
    f.write((char*)&Type, sizeof(Type));
    f.write((char*)&Name, sizeof(Name));
    Position.write(f);
    f.write((char*)&Type, sizeof(Type));
}
void GameObject::read(std::istream& f){
    f.read((char*)&Name, sizeof(Name));
    Position.read(f);
    f.read((char*)&Type, sizeof(Type));
}


// SIGN OBJECT

Sign::Sign() : GameObject("Sign", Vector2I(0, 0), SIGN), Text("Null"){};

Sign::Sign(Vector2I pos, std::string text) : GameObject("Sign", pos, SIGN), Text(text){};

std::string Sign::GetText(){
    return this->Text;
}

void Sign::write(std::ostream& f){
    GameObject::write(f);

    size_t size = Text.size();
    f.write((char*)&size, sizeof(size_t));
    f.write(Text.c_str(), size);
}
void Sign::read(std::istream& f){
    GameObject::read(f);

    size_t size;
    f.read((char*)&size, sizeof(size));
    const char* text_str;
    text_str = new char[size];
    f.read((char*)text_str, size);
    Text = text_str;
    Text.resize(size);
    delete text_str;
}



// PORTAL OBJECT

Portal::Portal() : GameObject("Portal", Vector2I(0, 0), PORTAL), PortalID(""){};

Portal::Portal(Vector2I pos, std::string portalID) : GameObject("Portal", pos, PORTAL), PortalID(portalID){};

std::string Portal::GetPortalID(){
    return this->PortalID;
}

void Portal::write(std::ostream& f){
    GameObject::write(f);

    size_t size = PortalID.size();
    f.write((char*)&size, sizeof(size_t));
    f.write(PortalID.c_str(), size);
}
void Portal::read(std::istream& f){
    GameObject::read(f);

    size_t size;
    f.read((char*)&size, sizeof(size));
    char* portalID_str;
    portalID_str = new char[size+1];
    f.read(portalID_str, size);
    PortalID = portalID_str;
    PortalID.resize(size);
    delete portalID_str;
}