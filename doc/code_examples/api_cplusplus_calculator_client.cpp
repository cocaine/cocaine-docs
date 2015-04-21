#include <cocaine/framework/services/app.hpp>
#include <iostream>

namespace cf = cocaine::framework;

int
main(int argc, char *argv[]) {
    auto manager = cf::service_manager_t::create(cf::service_manager_t::endpoint_t("127.0.0.1", 10053));

    auto app = manager->get_service<cf::app_service_t>("calculator");

    auto get = app->enqueue("get_value", 0);

    try {
        // g.next() wait answer from app in blocking mode.
        std::cout << "Value: " << cf::unpack<int>(get.next()) << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


    auto add = app->enqueue("add", 10);

    try{
        add.next();
    } catch(cf::future_error const &e){
        if( e.code()==cf::future_errc::stream_closed ){
            std::cout<<"Data transmitted\n";
        }
    }

    get = app->enqueue("get_value", 0);

    try {
        // g.next() wait answer from app in blocking mode.
        std::cout << "Value: " << cf::unpack<int>(get.next()) << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    
    return 0;
}
