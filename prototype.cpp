// Prototype Pattern

/
/* 
Prototype is a creational pattern that creates new objects by copying an existing, 
fully initialized instance, instead of building a fresh one from raw inputs. 



We need to keep some points in mind when working of prototype: 

1. Deep copy vs Shallow  Copy
2. Writing copy constructors by our own for some important things that need to be copied down with manual code.
the default copy constructors can only work for integers, strings, vectors..... 

the things which stores values somewhere else, and copying them in usual way will result in  two owners pointing to same thing. 


*/


#include <iostream>
#include <string>
#include <memory>
#include <utility>

using namespace std;



// ---------- A polymorphic part that the enemy OWNS through a base pointer ----------



class Weapon{

    public:

    virtual ~Weapon() = default;
    virtual unique_ptr<Weapon> clone() const =0; 
    virtual string name() const=0;
};




class Sword : public Weapon{
    
    public:
    string name() const override{
        return "sword";
    }


    unique_ptr<Weapon> clone() const override{
       return make_unique<Sword>(*this); 
    }


};



class Bow: public Weapon{

    public:
    string name() const override{
        return "bow";
    }

    unique_ptr<Weapon> clone() const override{
        make_unique<Sword>(*this);
    }
}


// ---------- A plain value part: copies itself, no help needed ----------



struct Stats {
    int attack = 0;
    int defense = 0;
};




class Enemy{


    public:
    
}