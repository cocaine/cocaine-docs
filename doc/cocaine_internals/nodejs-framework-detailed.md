
## Nodejs Framework Detailed Guide

### Overview

To be a complying `Worker` or a `Client`, we need several things to
do. These things are done in layers, separated by concerns they are
dealing with.


#### Channel
The first is a transport layer, let's call it a channel. Its responsibility is
to deal with transport-level issues: message framing and
decoding. This is done in C++ binding. It connects to unix or tcp
socket, and sends/receives cocaine-framed messages.

This layer's task is to read arrived data from socket, decode it, and
when a complete message is received, pass it up to the upper
layer. The same thing for messages coming downward from upper layer:
it stores them to buffer, and feeds its buffer gradually to the socket.

#### Worker, Client

The next layer's purpose is session handling. Over each channel,
messages for many simultaneous sessions can flow, so this layer
handles multiplexing/demultiplexing messages into corresponding
sessions. This is done in a very similar fashion for client and server
sessions, with a few differences.

Client sessions have to create unique per-channel id, and message flow
in for client session is one message sent (RPC call), and one or
multiple received (response stream). So the API for Services [client]
streams is function-calls based, i.e. call of interface function
results in RPC message being sent, and when result stream's messages
are received, they are returned in one-by-one fashion, or as one
aggregated result.

Server sessions (or `Worker` sessions) have reversed flow -- one
incoming message (RPC call), and many outgoing messages, so the
natural interface for these sessions is EventEmitter, which emits
Stream instances. Write to Stream results in one outgoing `chunk`
stream message being sent.

So this layer is implemented separately for `Client` and `Worker`.

#### Client wrappers

Next layer provides interface to particular service. It wraps client
sessions into method calls. Method API can be callback-based,
promise-based, and other forms.

#### Worker wrappers

To facilitate http applications portability between cloud and regular
environments, there is a layer, that emulates a behavior of incoming
network connections. This layer's things can be passed to regular node.js http
server. For this layer to work, some support from http module is
needed, so it works with slightly patched standard node.js http
module.

### Detailed guide

### Channel: C++ binding

We won't go into much detail on low-level C++ binding in this guide,
what we do is just describe its provided interface and capabilities.

Currently, channel binding is weirdly accessed with
```
var binding = require("./Release/cocaine_framework").Communicator
```

#### Methods

##### Constructor
```
new binding("1.2.3.4", 12345)
new binding("::1", 12345)
new binding("path/to/unix/socket")
```
accepts either of the above forms; constructs and initializes a
binding class. Initiates a connection to the specified endpoint.

When connection is ready, `on_connect` callback is called; on any
error during connection `on_socket_error` callback is called with
errno as an argument.

##### send
```
handle.send(msgpack.pack([methodId, sessionId, [1, 2, "any", ["arguments"]]]))
```
Accepts a buffer, and sends it raw into connected socket. Buffer is
expected to be a msgpack-packed tuple of one of the supported forms: a
method call, one of the stream messages, or one of lifecycle messages,
with sessionId and with the appropriately formed arguments.

It doesn't perform any checks, however some checks are performed by
cocaine-runtime, and sending any other form data may result in
an undefined behavior.

##### close
```
handle.close()
```
Closes underlying socket.

#### Callbacks

##### Socket callbacks
`handle.on_connect()` called after connection is established.
`handle.on_socket_error(errno)` called on any connection error: either
at the time of establishing a new connection or due to some network
issues.

##### Message callbacks, lifecycle

`handle.on_heartbeat()` called when heartbeat message is received from
cocaine-runtime.

`handle.on_terminate(code, reason)` called when terminate message is
received from cocaine-runtime, which indicates some terminate
condition, usually it says "this instance of worker has to be shut
down". `code` is either `normal` or `abnormal`, `message` is some
human-readable string relevant to condition of termination. See
[worker-lifecycle](technical-details.md#worker-lifecycle) for details
on termination conditions.

##### Message callbacks, RPC
`handle.on_invoke(sid, eventName)` is called on invoke message, which
is sent to indicate the beginning of an incoming stream. `eventName`
is handle name to be used. It's an event name used by `Worker` class
to emit event.


`handle.on_chunk(sid, buffer)` called on chunk message, which carries
actual data payload in incoming stream.

`handle.on_choke(sid)` called on choke message, which indicates normal
termination of incoming stream.

`handle.on_error(sid)` called on error message, which indicates some
error condition at the time of stream being produced.


### JS layer

## `lib/fsm.js`

`FSM` is a quite simple builder, used to implement `BaseService` and
`Worker`. Its main parts are `FSMPrototype` object and `define`
function.


There is a certain pattern to implement NodeJS bindings in
C++. For example, the following picture approximately depicts
participating entities of functioning net.Socket instance. (Not
everything here is exactly correct, but it demonstrates overall
picture quite well)

```
                             +-Handle-+              +-[proto]-+
      +-Socket-+             |        |              |         |
      |        |             |    __proto__--------->|     listen ---+
      |    _handle---------->|        |              |     connect --+
      |        |             |    on_connect         |     write ----+
    connect    |        +--->|    on_something2      |         |     |
    method2    |        |    |        |              +---------+     |
      |        |        |    |        |                              |
      |        |<-------+--owner      |                              |
      |        |        |    |        |                              |
      +--------+        |    |    hidden_value[0]---+                |
                        |    +--------+             |                |
                        |                           |                |
                        +--------------------+      |                |
                                             |      |                |
                             +-ObjectWrap-+  |      |                |
                           t |            |<-+------+                |
                           c |    _handle----+                       |
                           p |            |                          |
                           _ |    on_uv_connect                      |
                           b |    on_uv_data                         |
                           i |    on_uv_something_else               |
                           n |            |                          |
                           d |            |                          |
                           i |        Listen <------+                |
                           n |        Connect <-----+----------------+
                           g |        WriteBuffer <-+
                             |            |
                             |        AfterConnect
                             |            |
                             +------------+
```

As we see, it has three (named) things: a `Socket`, a `Handle` and an
`ObjectWrap`. And, apparently, there is a `proto` object, which is pointed
by `Handle.__proto__` property.

So the `Socket` object is an instance of pure JavaScript `Socket`
class. It has methods and properties, all JavaScript, everything as
usual. But the one outstanding thing about it is it's `_handle` property,
which points to `Handle` object.

`Handle` object is a regular JavaScript object, too. But it's special
array's (called `hidden_values` here) zeroth element is pointing to
`ObjectWrap` thing, and its proto is linked to that `[proto]` object,
whose methods point to some static functions of the `ObjectWrap`
class. By the way, `Handle` here is an instance of the
`process.bindings("tcp_wrap").TCP` class.

Also, `Handle` has callback fields, that are called `on_*`, and whose
meaning we will see later.

`ObjectWrap` is a regular C++ guy, it has a `_handle` property,
which points to aforementioned `Handle` object. Actually, what is
pictured here is `ObjectWrap`'s descendant implementing specific
methods like `Listen` and `Connect`, but we won't go in this detail
here, and call it `ObjectWrap`, since it _is_ an `ObjectWrap`.


Now, how all of this works? There are two cases of what can happen
here.

Firstly, one can call `Socket.connect`, which will call
`this._handle.connect` which is nothing other then
`ObjectWrap::Connect`. It will set up needed socket watcher via
uv_connect with callback like `on_uv_connect` and return.

Secondly, when socket connects, it calls static `on_uv_connect`, which
gets a pointer to the `ObjectWrap`, calls `ow->AfterConnect`, which, in
its turn, makes a call equivalent of JavaScript call of
`_handle.on_connect.apply(_handle,[args])`.

So, it's assumed that `Socket` has put some of function into
`_handle.on_connect`. That function can access `Socket` instance via
`this.owner`, and it does something like `this.owner.emit("connect")`.

So how FSM is helpful here? Imagine that we have multiple methods
of `Socket` class, we have multiple callbacks in `ObjectWrap`, and
every method call can change the state of the system. If there are
many states and many state transitions are allowed, it becomes very
quickly hard to track which flags correspond to which states, what
combinations of flags are allowed and what combinations are
invalid. So what FSM does -- it allows to manage states, methods and
handle callbacks, corresponding to each state.

Implementing cocaine client and server is very similar to what is
described above about `Socket`. Let's take client as an example here.
We have `BaseService` (it is like `Socket` from above pic), it has a `_handle`
property with callbacks managed by FSM, and there is underlying
binding that is pointed to with `_handle` property.

Consider an excerpt from `BaseService`:
```
var BaseService = FSM.define({

  // these are global methods, always available on BaseService instance:
  methods:{
    _send: function(message){ ... },
    _setHandle: function(handle){ ... },
    _closeHandle: function(){ ... },
    _resetSessions: function(errno){ ... }
  },

  // state to start from
  startState: 'closed',

  // states description
  states:{

    // describes "closed" state
    closed:{
      invariant: function(){
         // evaluated each time any of
         // state methods is called, and exception is thrown if it returns false
      },

      // when 'closed' state is entered, all these methods become available
      // on BaseService instance. The only method that is available at
      // state "closed" is connect method.
      methods:{
        connect: function(endpoint){
          // ..
          this._setState('connecting') // triggers instance to enter
                                       // 'connecting' state
        }
      }
    },

    // "connecting" state
    connecting:{
      invariant: function(){ ... },

      // the only method in "connecting" state is .close method
      methods:{
        close:function(){
          // ...
          this._setState('closed') // enter "closed" state, if called
        }
      },

      // handlers are callbacks that are set on "Handle" class from
      // the picture above. In "connecting" state there can be two
      // callbacks called -- on_socket_error, and on_connect, which
      // trigger corresponding state transitions.
      handlers:{
        on_socket_error:function(errno){
          // ...
          _this._setState('error') // enter "error" state
          _this.emit('error', err) // emit "error" event
        },
        on_connect:function(){
          // ...
          _this._setState('connected') // enter "connected" state
          _this._emit('connect') // emit "connect" event
        }
      }
    },

    connected:{
      invariant: function(){},
      methods:{
        send:function(msg){},

        close:function(){
          // ...
          this._setState('closed')
        }
      },

      // a lot of callbacks can be called in "connected" state
      handlers:{
        on_socket_error:function(errno){
          // ...
          _this._setState('closed')
        },

        on_chunk: function(sid, chunk){
        },

        on_choke: function(sid){
        },

        on_error: function(sid, code, message){
        }
      }
    }
  }
})
```

What we see here, is that BaseService is defined via `FSM.define`. It
provides `methods` field, which contains "global" methods, that is
available in every state.

Then, it provides `states` field, which contains per-state definitions
of states and callbacks. Every state's field contains `methods`,
`handlers` and `invariant` fields. When the corresponding state is
entered, first the corresponding `invariant` (if present) is checked,
then callbacks specified in `handlers` are attached to `_handle`
object (named `Handle` in scheme [1]), (of course, old state's
callbacks deregistered), and, lastly, methods, specified, naturally,
in `methods` field, become available on `BaseServer` instance.

If an invariant fails, or absent method is called, or absent callback
is called, a very clear error message is emitted, which simplifies
diagnosing unexpected conditions greatly. Also, it removes a lot of
state-related checks in methods, like when `close` has to do different
things when `connecting` and `connected`.

With FSM, it becomes if not easy, then at least comprehensible and
obvious what state transitions are expected and what is the assumed
workflow for specific class.

So, there are two small things in `fsm.js`. One is `define` function,
which interprets the definition like the one above, and the other is
`FSMPrototype`, that is used as a `[[proto]]` for every class created
by `define`.

`FSMPrototype` implements the only method, which is called
`_setState`. It is used to trigger state transitions, and is sparsely
depicted below:
```js
  _setState: function(state){

    if(this._state === state)
      return // don't do anything if we
             // already are in target state

    var stateDef0 = this.__fsmdef[this._state] // state definition for current state
    var stateDef1 = this.__fsmdef[state] // state definition for target state

    if(!this._handle){
      // if this._handle is not set, assure that we don't have
      // callbacks for this state, and enter state
    } else {
      this._state = state // enter state
      util.unsetHandlers(this._handle, stateDef0.handlers) // unset old state handlers
      util.setHandlers(this._handle, stateDef1.handlers) // set new state handlers
    }
  }
```

(TODO) And here are detailed explanations of the `define` function.


## Worker layer

### Session class

Session class is `ReadableStream` and a `WritableStream`. To maintain
compatibility with node versions both 0.8 and 0.10, there are two
implementations of this class, `Session1` and `Session2`. `Worker`
class expects them to have the following API.
```
var SessionInterface = {
  pushChunk: function(buffer) {},
  pushChoke: function() {}, 
  pushError: function(code, message) {}
}
```
Since they both implement it, they can be easily swapped dependent on
version of node being in use.

#### Session1

NodeJS 0.8 agrees on very formal stream interface -- whoever wants to
be a ReadableStream, has to emit `data`, `end` and `error`
events. Session1 implements pause and resume methods to facilitate
backpressure handling. 

After `pause` method is called, no events will be emitted, and all
incoming (from lower level) messages will be buffered, until `resume`
is called, which triggers all pending events to be fired (N.B. it
happens at the time of `resume` call, and probably should happen in
the next tick instead).

Writable part of interface are `write`, `end`, and `error` methods,
which send correspondingly `chunk`, `choke`, and `error` messages in
for session. Naturally, it's asserted that to writable calls will be
called after either of `end` and `error` calls, which both terminate
stream.

#### Session2

NodeJS 0.10 provides more elaborate streams classes. Session2 supports
Streams2 interface. It supports both Streams1 interface and Sreams2
interface.

### `lib/worker/worker.js`

`Worker` is implemented as FSM with a following state diagram:
```
closed
    connect -> connecting

connecting
    on_socket_error -> error
    on_connect -> connected

connected
    terminate -> selfTerminated
    on_terminate -> engineTerminated
    close -> closed
     
    on_chunk, on_choke, on_error, on_heartbeat -> connected

error
    close -> closed
     
engineTerminated
    terminate -> closed
     
selfTerminated
    on_terminate -> closed
```

Here, states are represented as
```
 state
   method -> new_state
   on_event -> new_state
```
where method is an instance method of resulting FSM in particular
state, and on_event is a callback, attached to _handle in that
particular state.


Public API of `Worker` class is quite simple.
```
var Worker = FSM.define({
  events: <EventEmitter>

  methods:{
    __init__: function(options) {},
    Session: function(event){},
    listen: function(){},

    connect: function(endpoint){}
    terminate: function(normal, reason){},
    close: function(){}
  },

  handlers:{
    on_socket_error: function(errno){},

    on_heartbeat: function(){},
    on_terminate: function(sid, code, reason){},

    on_invoke: function(sid, event){},
    on_chunk: function(sid, data){},
    on_choke: function(sid){},
    on_error: function(sid, code, message){}
  }
})
```

`Worker` is a descentant of EventEmitter, it emits streams that receives
in `invoke` and `chunk` messages. Socket errors and other events are
emitted on Worker.events emitter, to distinguish them from incoming
streams.

So, the very basic usage would be like this:

```
var W = new Worker({app:"appname",
                    locator:"localhost:10053",
                    uuid:"aa-bb-aaaaaaa-cccc",
                    endpoint: "/path/to/unix.sock"})

W.connect()

W.events.on("connect", function(){
    console.log("connected")
})

W.on("http", function(S){
    S.on("data", function(chunk){
        console.log("received data chunk", chunk)
    })

    S.on("end", function(){
        console.log("stream ended")
    })

    S.on("error", function(err){
        console.log("stream error", err)
    })
})

```


