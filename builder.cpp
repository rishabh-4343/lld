//  Builder Pattern


// Usage: useful when objects have many optional fields, when constructors are getting out of hand
//  or  when an object needs to be assembled in a specific order before it is ready to use


/*
** Ideas that define  the Builder pattern

1. Step by Step construction:  instead of passing everything to a constructor at once, you set each feild through individual method calls
2. Fluent interface: each method returns the builder itself so you can chain calls into one clean readable expression that ends with build()



*/

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>

using namespace std;
enum class Size{ Small, Medium, Large};





class Pizza{


    public:

    void describe() const 
    {
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
     
    Pizza() = default;
    friend class PizzaBuilder;

    Size size_ = Size::Medium;
    string crust_ = "regular";
    vector<string> toppings_;

    const char* sizeName() const {
        switch (size_) {
            case Size::Small:  return "small";
            case Size::Medium: return "medium";
            case Size::Large:  return "large";
        }
        return "unknown";
    }



};


class PizzaBuilder{

    public:

        PizzaBuilder& size(Size s){
            size_=s;
            return *this;
        }

        PizzaBuilder& crust(string c)
        {
            crust_ = move(c);
            return *this;
        }

        PizzaBuilder& addTopping(string t) {
            toppings_.push_back(move(t));
            return *this;
        }


        Pizza build() const{
            if(toppings_.size()>4)
            {
                throw invalid_argument("a pizza cannot have more than 4 toppings");
            }
            Pizza pizza;
            pizza.size_=size_;
            pizza.crust_ = crust_;
            pizza.toppings_ = toppings_;
            return pizza;
        }
    private:

    Size size_ = Size::Medium;
    string crust_ = "regular";
    vector<string> toppings_;
};




int main(){


    Pizza margerita = PizzaBuilder()
                        .size(Size :: Large)
                        .crust("thin")
                        .addTopping("cheese")
                        .addTopping("onion")
                        .build();


    margerita.describe();
}