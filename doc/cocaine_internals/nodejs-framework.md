
# NodeJS framework

## Requirements and design goals

To communicate with services, framework has to provide means to
connect to a service and to pack and unpack messages. It should have
robust means to handle errors. 

To facilitate worker functionality, framework has to provide means to
connect to app data socket, to control termination and heartbeat
messages.

Overall, framework has to provide means to effectively proxy messages
to application with minimal overhead.

Framework has to provide compatibility layer for http applications,
i.e. means to run standard Node.js http workers with minimal or no
code changes.

Also, to simplify migration and adoption, framework has to provide
means to conveniently use services. Any given project should have a
way to customize services wrapping layer.

## Design decisions

Any communication requires to decode a limited set of message types on
worker or client side, i.e. RPC ones, and two control-flow
`heartbeat`, `terminate`. Since decoding in js-land imposes overhead
at a Buffer-allocation time, it was decided that decoding of incoming
messages is done in C code, and js-land is notified with special
callback with a specific signature for every type of message received
in channel.

Outgoing messages, in case of service client, can be of any type, so
encoding of outgoing messages was left to js-land.

## Channel

Channel class provides a low-level interface to communicate with
a service or with an app engine.

Firstly, it has `connect` method coupled with `on_connect`
callback. You can use it to connect to tcp or unix socket cocaine
endpoint. Until the connection has been established, first error
happened will be reported through the `on_socket_error` callback.

Then, there is a `send` method, which assumes a `(sid, buffer)` as
its arguments. Use it to send any type of message to any of session
within the channel.

Lastly, there's a family of callbacks(with their signatures):
```
on_chunk(session_id, buffer)
on_choke(session_id)
on_terminate(session_id, code, reason)
on_error(session_id, code, message)
on_heartbeat()
```
When incoming message is completely decoded, the corresponding
callback is called.

The `on_socket_error` callback is called with system `errno` as it's
only argument in case of any error on channel socket.

`close` method is used to close socket descriptor and detach event
loop listeners.

## Services

There's a class `BaseService`. It manages communication with generic
service: connecting to a service, handling connection errors. 
As its crucial part it includes _handle member, referencing the
instance of Channel class. `BaseService` manages callbacks attached to
_handle, using `FSM` module.

States of Service instance are: `closed`, `connecting`, `connected`,
and `error`.

Any additional states, like dns-resolving stage, is implemented using
flags over the `closed` state in the `Service` class.

## Any Service


## Client

`Client`'s purpose is to provide convenient API to communicate with
cocaine cloud. It gives a developer means to create Service-clients,
Loggers. It communicates with Locator, resolves services and creates
instances of Service client classes.

`Client`'s methods are:

### Service(service_name, definition)

`service_name` is a string name of a service to connect to.
`definition` is an object, describing custom methods implementation
for created Service client. For details on how to use it, refer to
[customizing service client methods]()

### Logger(prefix)

Creates an instance of `Logger` class with specified `prefix`


## Worker

`Worker` is used to implement all the required worker-process
behavior. It is an `EventEmitter`. 


### Events

Events emitted on the instance of Worker are events arrived through
`invoke` message. Argument of the emitted event is an instance of
`Session`, which provides an interface to cocaine-stream.

`Stream`'s events `data`, `end`, and `error` correspond to
`chunk`, `choke`, and `error` cocaine messages. `Sessions`'s methods
`write`, `end`, `error` allow to send corresponding cocaine messages.

### Worker.events
`Worker.events` is an `EventEmitter` instance used for error and
control messages notifications. Events emitted on it are:

`connect` connection with engine is established
`error` socket error happened
`terminate` `terminate` message is received in channel
`disown` emitted, when heartbeat from app engine hasn't been received
within the configured timeout.

### connect
Initiate connection to the app engine: connect, handshake, start
heartbeat.

### close
Close connection to the app engine. Discard any data unsent to channel.

### listen
is a helper method used from `ListenHandle`. It calls connect if the
state is closed.


## Logger

Logger is a customized descendant of `Service` class. As a service it
provides emit method, which is used to emit logger messages. For
developer’s convenience, logger provides methods `error`, `info` and
other corresponding logging levels. On connection, it is supposed to
get its logging level with  `verbosity` method, and not send messages
with verbosity greater than returned level.



