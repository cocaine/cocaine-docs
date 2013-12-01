Let's create a simple C++ cocaine service and simple C++ client for it.

Before you start, read the [Cocaine installation tutorial](tutorial_cocaine_install.md).

We will name our service as `superapp`. It will have just one method - `get`, which intended for extracting of a data from Cocaine storage.

Cocaine stores its data in key-value storage by default. All data in the storage should be placed into the namespaces (it looks like a filesystem with one level of hierarchy). 

Place "Data for supertest" by the "superkey" into  "supernamespace" with the command:

```
cocaine-tool call storage write "'supernamespace', 'superkey', 'Data for supertest'"
```

You should get `None` as a result.

#Our cocaine service app
Start your program with the next code:

```
#include <cocaine/framework/dispatch.hpp>
#include <cocaine/framework/services/storage.hpp>
```

Declare app class

```
class superapp;
```

The next step is implementing of `get` method handler.
We will read the file from storage through the service "storage". We could use the internal Cocaine context to do this, but we will use service for greater versatility. After the file is read it will be sent to the client.
Native Framework uses asynchronous protocol. This mean that program is not blocked while request is processed but call the handler (`on_get::send`) on event from service.

```
struct on_get :
    public cocaine::framework::handler<superapp>,
    public std::enable_shared_from_this<on_get>
{
    on_get(superapp &a) : cocaine::framework::handler<superapp>(a)
    { }

    /*  Actually, the request handler. 
    `chunk` is a packed (`msgpack`) key ("superkey" in our case) that used for data reading, it is a parameter of our `get` method.
    `on_chunk` is a method that will be called on client `get`. This is a fixed name for such call.
    */
    void on_chunk(const char *chunk, size_t size);

    /* `send` will be called on event from storage (`gen.then(send)`, read the `on_chunk` inmplementation). It gets read result and send it to the caller of our method `get`
    This is a custom method that can implement some other logic and it can has a different name.
    */
    void send(cocaine::framework::generator<std::string>& g) {
        response()->write(g.next());
    }
};
```

App class.

The basic requirement is to use `dispatch_t` object as a parameter of constructor of this class,  `dispatch_t` object is a gateway to Cocaine services.
Here we get Cocaine services `storage` and `logger` and set handler for client's `get`.

```
class superapp {
public:
    superapp(cocaine::framework::dispatch_t &d) {
        log = d.service_manager()->get_system_logger();
        storage = d.service_manager()->get_service<cocaine::framework::storage_service_t>("storage");

        //`get`-handler registering
        d.on<on_get>("get", *this);

        COCAINE_LOG_INFO(log, "Yet another worker is running!");
    }

    std::shared_ptr<cocaine::framework::logger_t> log;
    std::shared_ptr<cocaine::framework::storage_service_t> storage;
};
```

Implementation of `on_chunk`-callback:

```
void on_get::on_chunk(const char *chunk, size_t size) {
    // Read operation returns an object that can be used for subscription to the asynchronous operation result
    cocaine::framework::generator<std::string> gen = parent().storage->read(
        "supernamespace",
        cocaine::framework::unpack<std::string>(chunk, size)
    );

    //This call will wait event from storage and call `send` method defined before.
    gen.then(std::bind(&on_get::send, shared_from_this(), std::placeholders::_1));
}
```

The `main` function:

```
int
main(int argc, char *argv[]) {
    return cocaine::framework::run<superapp>(argc, argv);
}
```

Compile app with the next command

```
g++ service_main.cpp -o superapp -std=c++0x -lcocaine-framework -lmsgpack -lev -lboost_program_options -lboost_thread-mt -lboost_system
```

Now we are ready to deploy our app into the cloud and use it there.

#Deployment
To deploy service into the cloud we should create tarball, write manifest and upload both of them to the Cocaine server.

Tarball.

```
tar -czf superapp.tar.gz superapp
```

Manifest.

```
{
    "slave": "superapp"
}
```

Save it to the `manifest.json`, or you can use another name.

Profile.

Apps are started in Cocaine with some profile. You can get list of profiles with the command

```
cocaine-tool profile list
```

If there is no default profile we should create it.

```
{}
```

Save above code to `default.profile` file, and upload it to the server. Other apps can be run with this profile.

```
cocaine-tool profile upload --profile=default.profile --name=default
```

Upload and run.

We will use the `cocaine-tool` to upload and start our app into the cloud.

```
cocaine-tool app upload --name superapp --package superapp.tar.gz --manifest manifest.json
cocaine-tool app start --name superapp --profile default
```

Let's check our application at the server

```
cocaine-tool info
```

If all that we did before was correctly finished we will see something like:

```
{
    "apps": {
        "superapp": {
            "load-median": 0, 
            "profile": "superapp", 
            "sessions": {
                "pending": 0
            }, 
            "queue": {
                "depth": 0, 
                "capacity": 100
            }, 
            "state": "running", 
            "slaves": {
                "active": 0, 
                "idle": 0, 
                "capacity": 10
            }
        }
    }
}
```
Here we can see that our `superapp` has `"state":"running"`.

#Client app
We have two ways to call `get` method of our service: `cocaine-tool` and client-side API.

##cocaine-tool
The next command call `get`-method with the parameter. Parameter as it was mentioned above should be packed with the `msgpack`.

```
x=$(python -c "import msgpack; print msgpack.packb('superkey')")
cocaine-tool call superapp enqueue "'get', '$x'"
```

##Client-side API
At the client side we should create the `service_manager` object which is used to communicate with services in Cocaine cloud.

```
#include <cocaine/framework/services/app.hpp>
#include <iostream>

namespace cf = cocaine::framework;

int
main(int argc, char *argv[]) {
    auto manager = cf::service_manager_t::create(cf::service_manager_t::endpoint_t("127.0.0.1", 10053));

    auto app = manager->get_service<cf::app_service_t>("superapp");

    auto g = app->enqueue("get", "superkey");

    try {
        // g.next() wait answer from app in blocking mode.
        std::cout << "Value: " << g.next() << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

`g.next()` returns packed data, that we need to deserialize with the `msgpack`.

Let's compile app and start it

```
g++ client_main.cpp -o superapp_client -std=c++0x -lcocaine-framework -lmsgpack -lev -lboost_program_options -lboost_thread-mt -lboost_system

./superapp_client
```

[Back to Contents](contents.md)