#include <cocaine/framework/dispatch.hpp>
#include <cocaine/framework/services/storage.hpp>

class superapp;

struct on_get :
    public cocaine::framework::handler<superapp>,
    public std::enable_shared_from_this<on_get>
{
    on_get(superapp &a) :
        cocaine::framework::handler<superapp>(a)
    { }

    // Actually, the request handler. 
    //   `chunk` is a msgpacked key ("superkey" in our case) that used for data reading,
    //   it is a parameter of our `get` method.
    //   `on_chunk` is a method that will be called on client `get`. This is a fixed name.
    void on_chunk(const char *chunk, size_t size);

    //Will be called on event from storage (`gen.then(send)`).
    //It gets read result from generator and send it to the caller of our method `get`
    //This is a custom method that can implement some other logic and it can has a different name
    void send(cocaine::framework::generator<std::string>& g) {
        response()->write(g.next());
    }
};

class superapp {
public:
    superapp(cocaine::framework::dispatch_t &d) {
        log = d.service_manager()->get_system_logger();
        // Client to `storage` service
        storage = d.service_manager()->get_service<cocaine::framework::storage_service_t>("storage");

        //Setting `get`-handler
        d.on<on_get>("get", *this);

        COCAINE_LOG_INFO(log, "Yet another worker is running!");
    }

    std::shared_ptr<cocaine::framework::logger_t> log;
    std::shared_ptr<cocaine::framework::storage_service_t> storage;
};

void on_get::on_chunk(const char *chunk, size_t size) {
    // Read operation returns an object that can be used for subscription to the asynchronous operation result
    cocaine::framework::generator<std::string> gen = parent().storage->read(
        "supernamespace",
        cocaine::framework::unpack<std::string>(chunk, size)
    );

    //This call will wait event from storage and call `send` method defined upper.
    gen.then(std::bind(&on_get::send, shared_from_this(), std::placeholders::_1));
}


int
main(int argc, char *argv[]) {
    return cocaine::framework::run<superapp>(argc, argv);
}
