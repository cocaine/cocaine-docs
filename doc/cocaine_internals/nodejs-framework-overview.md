# Overview

## C++ binding

TBD later; see [JS layer](#js-wrappers-adapters-and-classes) for now

* `include/`
  * `nodejs/`
    * `worker/`
      * `io/`
        * `socket.hpp`
        * `async_connector.hpp`
        * `readable_stream.hpp`
        * `writable_stream.hpp`
        * `channel.hpp`
        * `channel_interface.hpp`
        * `app_loop.hpp`
        * `error_handler.hpp`
        * `uv_event.hpp`
        * `worker.hpp`
  * `src/`
    * `node_addon.cpp`
    * `worker.cpp`

## JS wrappers, adapters and classes

  * `lib/`
    * `./` two important things below:
      * `fsm.js` `FSM` metaclass, implements handlers switching, state
        invariants checks, etc.
      * `session.js` is a Stream wrapper for Cocaine sessions.
         * `session1.js` srteams1 (pre- node 0.10 streams) implementation
            for the above wrapper.
         * `session2.js` streams2 (node 0.10 streams) sessions
           implementation.
    * `./` and other useful, but not that crucial stuff:
      * `util.js` -- some helpers here; `errno.js` -- big errno dict adapted
        from `<sys/errno.h>`; `protocol.js` -- protocol tables, adapted
        from `include/cocaine/messages.hpp`.
    * `client/` everything you need to implement a cloud client: a
      service client, an app client, reconnecting, various promise wrappers
      * `./`
         * `base_service.js` basic FSM for any service client
         * `logger.js` hand-tailored client for `logging` service, which
           implements a few custom methods and a custom connect
           behavior; a good example of how to do your custom service-client
         * `service.js` universal parameterized service builder
         * `client.js` `Client` class, that maintains
           connection with `Locator` service, uses it to resolve
           service names, and provides means to handle connection and
           other errors
      * `methods/` method implementations for service wrappers, that
        are used by `Service` builder to construct client-to-service classes
         * `callback.js` old simple callback-based method
           implementations for remote calls.
         * `promises.js` any-promise methods implementations for RPC. 
         * `promises_shim.js` two adaptors for promises methods: Q and
           Vow. You can implement your own likewise.
         * `streaming_method.js` one, streaming, method, used in all
           wrappers as it is implemented here.
         * `fibers.js` experimental, wraps RPC in fibers, providing
           almost drop-in replacement for services, which used to have
           synchronous API.
      * `services/` stubs for some services. Currently, a big part of
         services has only unpacking methods, and so constructed
         clients do by default. Here are stubs for a few other
         services, that have some non-default mix of methods.
         * `app.js` needs stub, because `info` is unpacking, and `enqueue` -- streaming
         * `storage.js` `read` method has to return binary chunk
           instead of utf8 encoded string
         * `urlfetch.js` `get` has binary last tuple member, whereas
          other, like headers, are strings
    * `worker/` all the behaviors an app needs to play along with Cocaine requirements
       * `worker.js` `Worker` class, which exposes request to
         app's "handle" as events in `EventEmitter` with an incoming
         stream as an argument.
       * `http.js` is wrapper around the above `Worker`, that provides
         NodeJS' "http"-compatible API to Cocaine streams.
       * `handles.js` contains `StreamHandle` and `ListenHandle`
         classes, who thoroughly mimic the behavior of `tcp_binding`, so
         that when http-server consumes them, they taste like a regular
         tcp guy to her.

For further details, see [detailed guide](nodejs-framework-detailed.md)
