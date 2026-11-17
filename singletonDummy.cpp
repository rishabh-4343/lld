

/*

Singleton Pattern: Creational


1. It refers to two part of how the class handles the creation
    -> the class is the sole responsible to create its instance. no one from outside can create the class instance by calling "new" 
    -> whoever ask for the instance gets the same one. (meaning, an instance will be created when it is first asked, then that one will be shared globally)

2. This above method is simply referred to as "Lazy Initialization" of the instance.
but this leads to some issues: It is not thread safe. when no instance is created, and multiple threads ask for it at the same time, 
problems will happen leading to unexpected behaviour.

Solution for this: 

a) Thread safe Singleton

-> in this approach, whoever comes asking for the instance, acquires a lock, which makes other threads wait there. 
-> now, either new instance will be created or instance variable will be returned depending on whether its a first request or not. then the lock will be released



Problem with this approach: 

->  the subsequent request which does not require the initialization of the instance still acquired lock and then release it after getting the intance. 
-> this is a additional overhead which slows down things 
-> solution is approach (b)

b) Double-Checked Locking

below is the process for this approach

-> on the request to get the instance, first it is checked whether the instance already exists ( check 1 ) without acquiring any lock.
-> if it does, it is returned.
-> otherwise, a lock is acquired, then it is again checked  if the instance exists ( check 1 ). if it does not, then only the instance is created and returned. 

-> so the two layer of checks helps in two ways : skipping the lock acquiring if the instance already exists. thread safety to provide duplicate instance initialization


c) Eager Initialization

-> in this approach, Singleton instance is initialized as soon as the class loads, before any thread can access it. 
-> a static final field stores the single shared instance. 
-> No synchronization or locks are required. ( might be applicable for java only because JVM ensures that static fields are initialized once)

-> If the singleton is lightweight and always needed, then eager initialization is the simplest.


*/



/*

Implementation of Singleton Pattern 


for C++, only important implemenation is Meyers' Singleton.

Meyer's Singleton is a thread-safe and lazy-initialized implementation of the Singleton pattern in C++.
It leverages the fact that static local variables are initialized only once, even in the presence of multiple threads.

Example:
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance; // Meyer's Singleton
        return instance;
    }
private:
    Singleton() {}
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};

*/



// Code Example for a Logger using Singleton pattern



#include <iostream>
#include <mutex>
#include  <string> 
using namespace std;


// Logger as a Meyers Singleton: one shared logger for the whole program


class Logger {
    public: 


    enum class Level { Info, Warning, Error};


    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }




    // Block copying so nobody can create a second logger 

    Logger(const Logger&) = delete;
    Logger& operator =(const Logger&) = delete;

    ~Logger(){
        std::cout<< " [Logger] shutting down after " << messageCount_ << "message(s)\n";
    }

    void setMinlevel(Level level)
    {
        lock_guard<mutex> lock(mutex_);
        minlevel_ = level;
    }

    void log(Level level, const string& message)
    {
        lock_quard<mutex> lock(mutex_);
        if(level < min_level_)
        {
            return;
        }

        cout<< "[" << toString(level)<<"]"<<message<<'\n';
        ++messageCount_;
    }



    void info(const std::string& message)    { log(Level::Info, message); }
    void warning(const std::string& message) { log(Level::Warning, message); }
    void error(const std::string& message)   { log(Level::Error, message); }
    
    int messageCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return messageCount_;
    }


    private:

    Logger()
    {
        cout<<"[Logger] created\n";
    }


    static const char* toString(Level level) {
        switch (level) {
            case Level::Info:    return "INFO";
            case Level::Warning: return "WARN";
            case Level::Error:   return "ERROR";
        }
        return "UNKNOWN";
    }

    mutable mutex mutex_;
    Level minLevel_ = Level::Info;
    int messageCount_ = 0;



};

// Different parts of the program use the same logger without passing it around.
void connectToDatabase() {
    Logger::getInstance().info("Connecting to database...");
    Logger::getInstance().info("Connected.");
}

void processOrder(int orderId, int quantity) {
    Logger& logger = Logger::getInstance();
    logger.info("Processing order " + std::to_string(orderId));
    if (quantity <= 0) {
        logger.error("Order " + std::to_string(orderId) + " has invalid quantity");
        return;
    }
    if (quantity > 100) {
        logger.warning("Order " + std::to_string(orderId) + " is unusually large");
    }
}


int main() {

    cout <<"Main started \n";
    Logger& Logger  = Logger :: getInstance();
    logger.info("Application started");



    connectToDatabase();
    processOrder(1, 5);
    processOrder(2, 500);
    processOrder(3, 0);

     // Every call to getInstance() returns the very same object.
    Logger& other = Logger::getInstance();
    std::cout << "Same object? " << std::boolalpha << (&logger == &other) << '\n';

    // These would not compile because copying is deleted:
    // Logger copy = Logger::getInstance();
    // Logger another;


        // Only warnings and errors from now on.
    logger.setMinLevel(Logger::Level::Warning);
    logger.info("This info message is ignored");
    logger.warning("Disk space is low");

    std::cout << "Messages logged: " << logger.messageCount() << '\n';
    std::cout << "main ending\n";
    return 0;




}