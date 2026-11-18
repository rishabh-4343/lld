// #include <iostream>
// #include <string>
// #include <mutex>

// using namespace std;



// class configManager
// {
//     public:

//     static configManager& getInstance()
//     {
//         static configManager instance;
//         return instance;
//     }

//     configManager(const configManager& ) = delete;
//     configManager& operator=(const configManager&) = delete;

//     ~configManager(){
//         cout <<"[config manager] shutting down\n";
//     }


//     void setKey(const string& key, const string& value)
//     {
//         this->mp[key] = value;
//         cout <<"[App config] " << key << " = "<< this->mp[key]<<"\n";
//     }

//     string getKey(const string& key, const string& defaultValue)
//     {
//         if(this->mp.find(key) != this->mp.end())
//         {
//             return this->mp[key];
//         }
//         return defaultValue;

//     }

//     bool has(const string& key)
//     {
//         if(this->mp.find(key) != this->mp.end())
//         {
//             return true;
//         }
//         return false;
        
//     }

//     private:

//     configManager()
//     {   
//         this->mp[max_users] = "100";
//         this->mp[debug] = "false";
//         this->mp[app_name] = "OrderService";
//         cout<<"config manager created\n with default values intialized";
//     }

//     map<string, string> mp;




// }


//  class Server
//  {
//     public:

//     configManager& configManager :: getInstance();
    


//     void setProcess()
//     {
//         if(configManager.has("process"))
//         {
//             string value = configManager.getKey("process", "server_1");
//             cout<< "key already present: process -> " << value<<"\n";
//         }
//         else
//         {
//             configManager.setKey("process", "server_1");
//             string value = configManager.getKey("process", "server_1");

//             cout<<"key value set: process -> " << value<<"\n";
//         }
//     }


// }


//  class database 
//  {
//     public:

//     configManager& configManager :: getInstance();

//     void configureDatabase()
//     {
//         this->max_user = configManager.getKey("max_user");
//         this->app_name = configManager.getKey("app_name");
//         cout<<"database configured\n";
//     }


//     private:
//     string max_users;
//     string app_name;

//  }


// int main(){

//     cout<<"main created\n";
    
    
//     configManager& manager::getInstance();

//     Server s = new Server();
//     database d = new Database();
//     s.setProcess();
//     d.configureDatabase();


    


// }






// above code was my own, triad, had some compile errors and some logical flaws. so below is the correct version of the same code







#include <iostream>
#include <map>
#include <mutex>
#include <string>

// Application-wide configuration: one shared object (Meyers singleton).
class ConfigManager {
public:
    // Built on the first call, reused afterwards. Thread-safe since C++11.
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    // Block copying so nobody can create a second ConfigManager.
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    ~ConfigManager() {
        std::cout << "[ConfigManager] shutting down\n";
    }

    void setKey(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        settings_[key] = value;
        std::cout << "[ConfigManager] " << key << " = " << value << '\n';
    }

    // Returns the stored value, or defaultValue if the key does not exist.
    std::string getKey(const std::string& key, const std::string& defaultValue) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = settings_.find(key);          // find() does not insert a missing key
        if (it == settings_.end()) {
            return defaultValue;
        }
        return it->second;
    }

    // Returns the stored value as an int, or defaultValue if the key is
    // missing or the value is not a number.
    int getInt(const std::string& key, int defaultValue) const {
        std::string value = getKey(key, "");    // getKey does the locking
        if (value.empty()) {
            return defaultValue;
        }
        try {
            return std::stoi(value);
        } catch (const std::exception&) {
            return defaultValue;
        }
    }

    bool has(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return settings_.find(key) != settings_.end();
    }

private:
    // Private constructor: only getInstance() can create the object.
    ConfigManager() {
        settings_["app_name"] = "OrderService";
        settings_["max_users"] = "100";
        settings_["debug"] = "false";
        std::cout << "[ConfigManager] created with default values\n";
    }

    // mutable so that const methods can still lock it.
    mutable std::mutex mutex_;
    std::map<std::string, std::string> settings_;
};

class Server {
public:
    void start() {
        ConfigManager& config = ConfigManager::getInstance();
        std::string name = config.getKey("app_name", "UnknownApp");
        int maxUsers = config.getInt("max_users", 10);
        std::cout << "[Server] starting " << name
                  << " with max users = " << maxUsers << '\n';
    }

    void setMaxUsers(int users) {
        ConfigManager::getInstance().setKey("max_users", std::to_string(users));
    }
};

class Database {
public:
    void configure() {
        ConfigManager& config = ConfigManager::getInstance();
        appName_ = config.getKey("app_name", "UnknownApp");
        maxConnections_ = config.getInt("max_users", 10);
        // "db_timeout" was never set, so the fallback value is used.
        timeoutSeconds_ = config.getInt("db_timeout", 30);
        std::cout << "[Database] configured for " << appName_
                  << ", connections = " << maxConnections_
                  << ", timeout = " << timeoutSeconds_ << "s\n";
    }

private:
    std::string appName_;
    int maxConnections_ = 0;
    int timeoutSeconds_ = 0;
};

int main() {
    std::cout << "main started\n";            // ConfigManager does not exist yet (lazy)

    ConfigManager& config = ConfigManager::getInstance();   // created here

    // Every getInstance() call returns the very same object.
    ConfigManager& other = ConfigManager::getInstance();
    std::cout << "Same object? " << std::boolalpha << (&config == &other) << '\n';

    // These would not compile because copying is deleted:
    // ConfigManager copy = ConfigManager::getInstance();
    // ConfigManager another;

    Server server;
    Database database;

    server.start();
    database.configure();

    // A change made through one class is seen by the other.
    server.setMaxUsers(250);
    database.configure();

    std::cout << "has(\"debug\")   = " << config.has("debug") << '\n';
    std::cout << "has(\"missing\") = " << config.has("missing") << '\n';

    std::cout << "main ending\n";
    return 0;
}
