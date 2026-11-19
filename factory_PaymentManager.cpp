#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <functional>

using namespace std;


class Payment{
    public:
    virtual ~Payment() = default;
    virtual void processPayment() const=0;

};


class creditCardPayment : public Payment{

    public:
    void processPayment() const override 
    {
        cout<<"Payment is processed using Credit card\n";
    }
};


class upiPayment : public Payment{

    public:
     void processPayment() const override{
        cout<<"payment is processed using upi\n";
     } 
};



class PaymentCreator{

    public:

    virtual ~PaymentCreator()=default;
    virtual unique_ptr<Payment> createPayment() const =0;

    void process()
    {
        unique_ptr<Payment> payment  =  createPayment();
        payment->processPayment();
    }
};


class CreditCardCreator : public PaymentCreator{

    public:

    unique_ptr<Payment> createPayment() const override{
        return make_unique<creditCardPayment>();
    }
};

class UpiCreator: public PaymentCreator{
    public:
    unique_ptr<Payment> createPayment() const override{
        return make_unique<upiPayment>();
    }
};



void checkout (PaymentCreator& creator)
{
    creator.process();
}


unique_ptr<PaymentCreator> makeCreator( const string& type){
    
    static const map<string, function<unique_ptr<PaymentCreator>()>> registry = {
        {
            "card",
            [] { return make_unique<CreditCardCreator>();}
        },
        {
            "upi",
            [] { return make_unique<UpiCreator>(); }
        }
    }; 


    auto it  =registry.find( type);
    if(it == registry.end())
    {
        return nullptr;
    }

    return it->second();
};

int main()
{

    // Approach 1 to use:  when you kn
    // ow which sub class to use. 

    unique_ptr<PaymentCreator> creator = make_unique<CreditCardCreator>();
    creator->process();


    // Approach 2 to use:  the main function/client does not bother about that Parent p = make_unique<Child class>()


    UpiCreator upi;
    checkout(upi);





    // Approach 3 to use: Decision of which sub class/implementation to use is made at runtime. 



   creator = makeCreator("upi");
   if(creator)
   {
     creator->process();

   }
   




}


