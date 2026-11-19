


// Abstract Factory 


// This pattern provides an interface for creating a Family of related objects without naming their concrete classes.





#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>

using namespace std;



class Table{

    public:
    virtual ~Table()= default;
    
    virtual void describe() const =0;


};


class Chair{
    public:
    virtual ~Chair()=default;

    virtual void describe() const =0;
};



class ModernChair: public Chair{
    public:
    
    void describe() const override{
        cout<<"modern chair purchased\n";

    }

 

};

class ClassicChair: public Chair{
    public:
    void describe () const override{
        cout<<"classic chair purchased\n";

    }
};



class ModernTable: public Table{
    public:
    void describe() const override{
        cout<<"modern table purchased\n";
    }
};


class ClassicTable : public Table{
    public:
    void describe() const override{
        cout<<"classic table purchased\n";
    }
};


class FurnitureFactory {

    public:
    virtual ~FurnitureFactory()=default;
    virtual unique_ptr<Chair> createChair() const=0;
    virtual unique_ptr<Table> createTable() const=0;


};

class ModernFurnitureFactory : public FurnitureFactory{

    public:
    unique_ptr<Chair> createChair()  const override{
        return make_unique<ModernChair>();
    }


    unique_ptr<Table> createTable() const override{
        return make_unique<ModernTable>();
    }

};


class ClassicFurnitureFactory : public FurnitureFactory{
    public:

    unique_ptr<Chair> createChair()  const override{
        return make_unique<ClassicChair>();

    }


    unique_ptr<Table> createTable() const override{
        return make_unique<ClassicTable>();
    }
};



class Room{
    public:
    explicit Room(const FurnitureFactory& factory)
    : chair_(factory.createChair()),
    table_(factory.createTable()) {}

     void show() const {
            chair_->describe();
            table_->describe();     
     }






    private:
    unique_ptr<Chair> chair_;
    unique_ptr<Table> table_;

};

unique_ptr<FurnitureFactory> makeCreator(const string& type){


    static const map <string, function<unique_ptr<FurnitureFactory>()>> registry ={

        {
            "modern",
            [] { return make_unique<ModernFurnitureFactory>();}
        },
        {
            "classic",
            [] { return make_unique<ClassicFurnitureFactory>();}
        }



    };




    auto it = registry.find(type);
    if(it == registry.end())
    {
        return nullptr;
    }


    return it->second();

};

int main ()
{

    unique_ptr<FurnitureFactory> creator = makeCreator("classic");

    if(creator)
    {
         Room room (*creator);
         room.show();
    }
    return 0;


}


