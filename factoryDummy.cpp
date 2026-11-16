#include <stdio>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <mutex>

using namespace std;


/*
 C++ has no keyword "abstract" or "interface"
 A class which has atleast one pure virtual function is Abstract.
 A class is abstract or is it a Interface depends on the what's inside it  => 
 if it contains data members or normal functions with implementations, then it is abstract class 
 otherwise it's a Interface
*/

class Notification
{
    public:
    
        // this asks compiler to generate the normal default constructor. 

        Notification() = default;
    
        /* 
        a virtual destructor 
        
        Rule of Thumb: Any class meant to be used as a base class with polymorphism
        should have a Virtual Destructor
        
        The Factory returns a pointer to the base class, but the object it points to is really 
        an EmailNotification, SMSNotification, etc.
        When that pointer is destroyed, c+ must call the destructor of the real object. 

        */
        virtual ~Notification() = default;



        /*
        Object Slicing : occurs when a derived class object is assigned by value to a base class object
        during this copy operation, the c++ compiler copies only the base class portion of the object, 
        effectively "slicing off" behaviours and properties of the derived class        
        */
        

        /*
        Copy Constructor is used to initialize a new object as an exact duplicate of an existing object 
        of the same class.

        these two lines below prevent the Object slicing that can be done in C++ using two things: 
         (i) Copy Constructor (ii) copy assignment operator

         example of (i): Notification n2 = n1;
         example of (ii): n2 = n1 (here, assignment is happened because both were already created )
        */

        Notification (const Notification&) = delete;
         
        Notification& operator=(const Notification&) = delete;


        /*
        
        Three ways to take a string parameter
        1. void send (string message) ------------ copies  the whole text every call
        2. void send (const string& message) ----------- no copy, but must be a real std::string
        3. void send (string_view message) ------------ no copy, accepts almost any test source

        */


        /*
        Notification is the Rulebook : "Anyone doing the send job must not change the machine while doing it. "
        Effect of using "const" : inside this function you can't change the object's data members
        */
        
        /*
        "=0"  makes it pure virtual
        */
        virtual void send (string_view message) const=0
 
}
 

class NotificationFactory
{
public:
        /*
        1. we are making a shorthand name "Creator" for something big. 
        2. we are defining that it will be a function which will return something and takes something. 
        3. it will take a Notification because that is commmon among all notificataions (sms, email, etc.) - they all are notification
        4. it will return a Unique_ptr, this will delete memory automatically when object goes out of scope or ownership changes. 
            using this ensures that no-one has to keep remembering to free the allocating the memory after using. 
        5. this "Creator" contains some runnable code that can be used anytime.     
        */

        using creator = function<unique_ptr<Notification>()>;



        /*
        1. Singleton instance
        2. "Only one factory exists in the whole program"
        3. "factory" created the first time instance() is called, then reused on every later call. 
        4. it uses a reference (&), so everyone shares same object, no copies


        5. Why not use :Global Variable. 
        -> Static initialization order problem : as Registrar is also global variable, 
        we are not sure who gets created first when spread across files, so we need to ensure that
        factory exists before registrar dumps data into it. ]

        6. the "static" at the function name is different then "static" inside the function
        this one means that you can call the function without already having a factory object. 

        so I can do => NotificationFactory::instance() //  ask the class itself, not the object 
        */


        static NotificationFactory& instance()
        {
            static NotificationFactory factory;
            return factory;
        }




        bool registerType(string name, Creator creator)
        {
            if(!creator)
            {
                throw invalid_argument("Creator must not be empty");
            }

            name = normalize(name);
            unique_lock lock(mutex);
            return creators_.emplace(move(name), move(creator)).second;
        }

        /*
        1. [[nodiscard]] => if the caller throws the result away, the compiler warns them.
        */

        [[nodiscard]] unique_ptr<Notification> tryCreate(string_view kind) const{

            Creator creator;
            {
                shared_lock lock(mutex_);
                auto it = creators_.find?(normalize(kind));
                if(it==creators_.end()) return nullptr;
                creator = it->second;
            }

            return creator();
        }

        [[nodiscard]] unique_ptr<Notification> create(string_view kind) const{

            if(auto p = tryCreate(kind)) return p;
            throw invalid_argument("Unknown notification type : " + string(kind));
        } 



        [[nodiscard]] vector<string> registeredTypes() const{
            shared_lock locl(mutex_);
            vector<string> names;
            names.reserve(creators_.size());
            for(const auto& [name, _] : creators_) names.push_back(name);
            return names;
        }

    
private:
    NotificationFactory() = default;

    static string normalize(string_view s)
    {
        string out(s);
        transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });
        return out;
    }
    mutable shared_mutex mutex_;
    map<string, Creator> creators_;
        
        
        
};




template <typename T> 
class Registrar {
    public:


    /*
    1. when you have a class with a construtor which accept a one input, you need to use explicit
    because, let's say you have a function : doSomething( Registrar<EmailNotification> r);
    and someone writes it as  : doSomething(string ("email")), C++ quietly thinks that I need a registrar, I 
    have a string, I know how to make one, so it will make one. 

    explicit will prevent this. it will tell, only create a Registrar if the user specifically calls for it. 

    2.  Notification::instance() ->  gives the one shared book.  
        .registerType() -> writes a page in it. 

        registerType() ->  takes two input. 
        (i) move (name) -> the dish name, handed over directly instead of copied
        (ii) the recipe card : [] { return make_unique <T> (); }
            
        -> the recipe card is a lambda function 
        -> make_unique<T>() : builds one new T and return it with an ownership ticket
    
    */
        explicit Registrar (string name){
            NotificationFactory::instance().registerType(
                move(name), [] {return make_unique<T>();}
            );
        }
};





/* 
1. using the Registrar
2. these are global variable, it is created before "main" begins.
3. creating it, rings the doorbell, which runs the constructor, which writes the "email" page in the book. 
4. the "namespace" means "keep these private to this file"
*/

namespace {

const Registrar<EmailNotification> registerEmail{"email"};
const Registrar<SMSNotification> registerSMS{"sms"};
const Registrar<PushNotification> registerPush{"push"};

}





// ---------- Concrete products ----------
class EmailNotification final : public Notification {
public:
    void send(string_view message) const override {
        cout << "Sending EMAIL: " << message << '\n';
    }
};

class SMSNotification final : public Notification {
public:
    void send(string_view message) const override {
        cout << "Sending SMS: " << message << '\n';
    }
};

class PushNotification final : public Notification {
public:
    void send(string_view message) const override {
        cout << "Sending PUSH: " << message << '\n';
    }
};


int main()
{

    // factory becomes the handle to the book
    auto& factory  = NotificationFactory :: instance();


    cout << "Registered types: ";
    for(const auto& name  : factory.registeredTypes()) cout << " " << name;
    cout << "\n\n";


    for(string_view kind: {"email", "SMS", "push"})
    {
        factory.create(kind) -> send("Your order has shipped");
    }


    if(auto n = factory.tryCreate("pigeon"); !n)
    {
       cout << "\n'pigeon' is not supported\n";
    }


    try {
    factory.create("pigeon");
    } catch (const invalid_argument& e) {
        cerr << "Error: " << e.what() << '\n';
    }



}
