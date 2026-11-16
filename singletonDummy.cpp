

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