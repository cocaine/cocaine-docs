
# NodeJS Quick Start

## using a Client
To use a Client to speak to Cocaine services you need to know the
metalocator address. If you do it from one of nodes on the cluster,
it's usually 127.0.0.1:10053. If you access services from outside of
the cloud, it usually well be the load-balancer's locator. Let's say
it's `front.ape.host`.

```
var cocaine = require("cocaine")
var cli = new cocaine.Client(["front.ape.host", 10053])
```

Here, `cli` is an instance of a Client class, that is not yet
connected. It can construct various Service classes for you.

### Creating individual Service-clients using Client

```
var Storage = cli.Service("storage")
```

Here, Storage is an instance of future ready-to-use service. It still
lacks storage's methods through -- they are implemented when Client
receives service's metadata from locator.

`"storage"` service will be resolved using the locator endpoint specified in
Client constructor. Resolution happens when you call Storage's connect
method: it resolves service's metadata, implements it's methods, and
connects to service's endpoint. Then emits `"connect"` event.

So, after `connect` event, the `Storage` is ready to use:

```
Storage.connect()
Storage.on("connect", function(){
  Storage.find("manifests", ["apps"])
})
```

(See [services.md] for full description of storage's methods)

As a developer of a robust applications, you have to know what error
conditions can happen at various points of time in Client's life. See
reference [TODO] and examples [examples/client.js] for info on
handling error conditions.

### Creating multiple Services-clients at once

To save developer's efforts, there's a simplified API to create
multiple Service-clients. You can use it like this:

```
cli.getServices(["storage", "logging", "geobase"], function(err, Storage, Logging, Geo) {
  if(!err){
    // here, Storage, Logging, Geo all are instances of
    // corresponding Service-clients, already resolved and connected
  }
})
```

The drawback of such approach is that it's harder to handle
errors. `err` here is the error object that has info on what services
were failed to resolve.

## running basic cocaine app with Worker

To run your app scalable on nodes of the cluster, you have to
implement all mechanics of interactions that are expected from such
app. For all details what is required, see [frameworks.md]. All this
behavior is implemented in Worker class of the framework.

To initialize, the Worker class needs to know about command-line keys,
that cocaine-runtime has passed to spawned process, in form of
```
{ "app": "appname", 
  "endpoint": "/path/to/unix.sock" }
```

Luckily, there's npm `optimist` module that parses command-line
arguments exactly like that. 

```
var argv = require("optimist").argv
var Worker = new cocaine.Worker(argv)
```

Here, `Worker` is an instance of `cocaine.Worker` class. `Worker` is not
yet connected to cocaine-runtime. Since runtime will send us incoming
requests as soon as connect method's messages reach it, we have to
specify handlers for incoming events first, to not to loose any of the
incoming sessions:

```
var mp = require("msgpack")
Worker.on("ping", function(stream) {
  stream.on("data", function(chunk){
    console.log(chunk)
  })
  stream.on("end", function(){
    stream.end(mp.pack({ pong: "hello" }))
  })
  stream.on("error", function(err){
    console.log("error in incoming stream", err)
  })
})
```

After all handlers are there, we call `Worker`'s connect method:

```
Worker.connect()
Worker.events.on("connect", function(){
  console.log("start handling incoming sessions")
})
```

Event identifiers, emitted on Worker instance are those, that are sent in
`invoke` messages, and event argument is a `Session` instance, that
represents Cocaine stream -- it's chunks, choke and error
messages. Namely, `data`, `end`, and `error` events on a `Session`
instance correspond to `chunk`, `choke`, and `error` messages in
Cocaine stream.

For full details on `Worker` and `Session` classes see [nodejs-framework.md]


## tweaking your existing http app

Worker class supports an interface that mimics listening tcp socket of
the standard `net` node-js module. To be more specific, it mimics tcp
socket's "lower" part, that in node-js is called handle.

To get such a handle from Worker, you have to specify incoming event
name. And such listening handle will behave exactly like tcp listening
handle, emitting tcp handles, that, in their turn, mimic all the
behavior of connected tcp handles from `net` module.

Consequently, you can use this all this mechanics to accept connections
with nodeJs standard `http.Server`.

Here is a simple example of what such usage would look like.

```
var http = require("http")

var handle = Worker.getListenHandle("some-incoming-event")

var S = http.Server()

S.on("request", function(req, res){
  var body = ""
  req.setEncoding("utf8")
  req.on("data", function(data){
    body += data
  })
  req.on("end", function(){
    res.end("body length is "+ body.length)
  })
  req.on("error", function(err){
    console.log("error in request: ", req, err)
  })
})

S.listen(handle)
```

Although, there's a small [trick, difficulty] with this scenario.

The one part of the problem is that standard nodejs http server manages to effectively
pack http header and first body chunk in first `write` call it makes
on network socket

The other part of the problem is that cocaine-http-proxy expects reply
in the form of
```
```

So to provide cocaine-proxy with the response we need, we would have
to parse node's http reply on socket level, pack into corresponding
structure and only then send it back to proxy.

See again: nodejs http server packs a buffer out of `writeHead` and `write`
and `end` calls, then we should parse it as a http response, convert
it on-the-fly into expected by cocaine-proxy form, pack into a buffer
again, and only then send it to proxy.

To work around this double pack-unpack way, there's http server in
cocaine, that does exactly one thing differently from standard node's
http server. On `writeHead` call it constructs first `meta-chunk`, and
all of `write`s and `end` are sent as `chunk` messages.

And the other circumstance around this collision is that frameworks
such as connect expect `http` to be the only module they use and
extend to handle http requests, so if we want the express or connect
to use and extend our http, we have to replace the original http
module with ours.

Here's how it's done from the developer's point of view:

```
var http = cocaine.http // this is implemented as a getter
```

The getter patches standard http module, and returns what was the
result of the patch. It touches only server-side, so all of the
standard http client functions remain still what they are.

Then, developer does what was shown above:

```
var handle = Worker.getListenHandle("some-incoming-event")

// use createServer, if you need
var S = new http.Server(function(rq,rs){
  // ...
}) 

S.listen(handle)
```

## tweaking your existing express app

Using the mechanics shown above, you can tweak your already
working express apps to make them interoperable and compliant to
cocaine infrastructure.

```
var http = cocaine.http // this has to be before express require

var app = require("express")

// ... app initialization

var S = new http.Server(app)

Server.listen(handle)

```

# Uploading your app to the cloud

See [nodejs-running-app.md] for instructions.
