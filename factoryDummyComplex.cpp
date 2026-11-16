#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

// ---------- Product interface ----------
class Notification {
public:
    Notification() = default;
    virtual ~Notification() = default;

    // Polymorphic objects shouldn't be copied (prevents slicing)
    Notification(const Notification&) = delete;
    Notification& operator=(const Notification&) = delete;

    virtual void send(string_view message) const = 0;
};

// ---------- Factory ----------
class NotificationFactory {
public:
    using Creator = function<unique_ptr<Notification>()>;

    static NotificationFactory& instance() {
        static NotificationFactory factory;  // thread-safe since C++11
        return factory;
    }

    // Returns false if the name was already registered
    bool registerType(string name, Creator creator) {
        if (!creator) {
            throw invalid_argument("Creator must not be empty");
        }
        name = normalize(name);
        unique_lock lock(mutex_);
        return creators_.emplace(move(name), move(creator)).second;
    }

    // Returns nullptr if the type is unknown
    [[nodiscard]] unique_ptr<Notification> tryCreate(string_view kind) const {
        Creator creator;
        {
            shared_lock lock(mutex_);
            auto it = creators_.find(normalize(kind));
            if (it == creators_.end()) return nullptr;
            creator = it->second;  // copy, so we can call it outside the lock
        }
        return creator();
    }

    // Throws if the type is unknown
    [[nodiscard]] unique_ptr<Notification> create(string_view kind) const {
        if (auto p = tryCreate(kind)) return p;
        throw invalid_argument("Unknown notification type: " + string(kind));
    }

    [[nodiscard]] vector<string> registeredTypes() const {
        shared_lock lock(mutex_);
        vector<string> names;
        names.reserve(creators_.size());
        for (const auto& [name, _] : creators_) names.push_back(name);
        return names;
    }

private:
    NotificationFactory() = default;

    static string normalize(string_view s) {
        string out(s);
        transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });
        return out;
    }

    mutable shared_mutex mutex_;
    map<string, Creator> creators_;
};

// ---------- Self-registration helper ----------
template <typename T>
class Registrar {
public:
    explicit Registrar(string name) {
        NotificationFactory::instance().registerType(
            move(name), [] { return make_unique<T>(); });
    }
};

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

// Each class registers itself at startup. Adding a new type touches
// only its own code, never the factory.
namespace {
const Registrar<EmailNotification> registerEmail{"email"};
const Registrar<SMSNotification>   registerSms{"sms"};
const Registrar<PushNotification>  registerPush{"push"};
}  // namespace

// ---------- Client code ----------
int main() {
    auto& factory = NotificationFactory::instance();

    cout << "Registered types:";
    for (const auto& name : factory.registeredTypes()) cout << ' ' << name;
    cout << "\n\n";

    for (string_view kind : {"email", "SMS", "push"}) {
        factory.create(kind)->send("Your order has shipped!");
    }

    // Recoverable path: no exception
    if (auto n = factory.tryCreate("pigeon"); !n) {
        cout << "\n'pigeon' is not supported\n";
    }

    // Exception path
    try {
        factory.create("pigeon");
    } catch (const invalid_argument& e) {
        cerr << "Error: " << e.what() << '\n';
    }
}