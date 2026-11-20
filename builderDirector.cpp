#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

enum class Size {Large, Medium, Small};



class Pizza {

    public:
    void setSize(Size s){
        size_ = s;
    }
    void setCrust(string c){
        crust_ = c;
    }

    void addToppings(string t){
        toppings_.push_back(t);
    }
    
    
    
    void describe() const {
        cout << sizeName() << " pizza, " << crust_ << " crust, toppings:";
        if (toppings_.empty()) {
            cout << " none";
        }
        for (const auto& topping : toppings_) {
            cout << ' ' << topping;
        }
        cout << '\n';
    }


    private:

    Size size_ = Size::Medium;
    string crust_ ="regular";
    vector<string> toppings_;



    const char* sizeName() const
    {
        switch(size_){
            case Size::Small : return "small";
            case Size::Medium : return "medium";
            case Size::Large : return "large";
        }

        return "unknown";
    }


};


class PizzaBuilder{
    
    public:
    virtual ~PizzaBuilder() = default;
    virtual void buildSize()  =0;
    virtual void buildCrust()  =0;
    virtual void buildToppings() =0;
    virtual Pizza getResult()=0;
};


class MargeritaBuilder : public PizzaBuilder{

    public:
    void buildSize() override{
        pizza_.setSize(Size::Large);
    }

    void buildCrust() override{
        pizza_.setCrust("regular");
    }

    void buildToppings() override{
        pizza_.addToppings("cheese");
        pizza_.addToppings("basil");
    }


    Pizza getResult() override{
        return move(pizza_);
    }



    private:
    Pizza pizza_;

};




class VeggieBuilder : public PizzaBuilder{


    public:

    void buildSize() override{
        pizza_.setSize(Size::Medium);
    }

    void buildCrust() override{
        pizza_.setCrust("thin");
    }
    

    void buildToppings() override{
        pizza_.addToppings("onion");
        pizza_.addToppings("tomato");
        pizza_.addToppings("basil");
    }


    private:
    Pizza pizza_;


};



class Director{


    public:
    
    Pizza makefull(PizzaBuilder& builder) const {
        builder.buildSize();
        builder.buildCrust();
        builder.buildToppings();
        return builder.getResult(); 
        
    }

    Pizza makePlain(PizzaBuilder& builder) const{
        builder.buildSize();
        builder.buildCrust();
        return builder.getResult();
    }


};


int main()
{


    MargeritaBuilder margerita;
    Director director;

    director.makefull(margerita).describe();
    director.makePlain(margerita).describe();

};