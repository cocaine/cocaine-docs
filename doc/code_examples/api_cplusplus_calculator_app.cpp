#include <cocaine/framework/dispatch.hpp>

class calculator;

//Handlers
//========
struct add_handler :
					public cocaine::framework::handler<calculator>, 
					public std::enable_shared_from_this<add_handler>
{
	add_handler(calculator &calc) : cocaine::framework::handler<calculator>(calc)
    {}

    void on_chunk(const char *chunk, size_t size);
};

struct subtract_handler :
					public cocaine::framework::handler<calculator>, 
					public std::enable_shared_from_this<subtract_handler>
{
	subtract_handler(calculator &calc) : cocaine::framework::handler<calculator>(calc)
    {}

    void on_chunk(const char *chunk, size_t size);
};

struct get_value_handler :
					public cocaine::framework::handler<calculator>, 
					public std::enable_shared_from_this<get_value_handler>
{
	get_value_handler(calculator &calc) : cocaine::framework::handler<calculator>(calc)
    {}

    void on_chunk(const char *chunk, size_t size);
};


//App class
//========
class calculator {
public:
    calculator(cocaine::framework::dispatch_t &d) {
    	value = 0;
        d.on<add_handler>("add", *this);
        d.on<subtract_handler>("subtract", *this);
        d.on<get_value_handler>("get_value", *this);
    }

	int value;
};

//App
//========
int main(int argc, char *argv[]) {
    return cocaine::framework::run<calculator>(argc, argv);
}

//Handler methods implementation
//========
void add_handler::on_chunk(const char *chunk, size_t size){
	parent().value += cocaine::framework::unpack<int>(chunk, size);
}

void subtract_handler::on_chunk(const char *chunk, size_t size){
	parent().value -= cocaine::framework::unpack<int>(chunk, size);
}

void get_value_handler::on_chunk(const char *chunk, size_t size){
	response()->write(parent().value);
}
