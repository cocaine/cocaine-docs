# Services


## What is a Service

So, a Service is a piece of c++ code that is run in its own thread,
that implements a bunch of methods available through RPC
protocol. Current implementation of Service class supports the
following message exchange.

Service expects incoming messages in the form of

`[<method_id>, <session_id>, [<arguments>]]`

Here, `method_id` is a numeric id of a particular service method in
service methods table (as defined in protocol<protocol_tag>::type
typedef in messages.hpp),
`session_id`, as usual, is a client-generated numeric id of a session
unique within a channel, and <arguments> is a tuple of a predetermined
structure specified in corresponding protocol::tuple_type typedef.

The response to the call is conveyed in form of a stream, i.e. zero or
more chunk messages, followed by choke or error
message. Interpretation of data found in chunks is up to calling
client. Usually, it is some kind of a msgpack-packed structure.

## How is it run

Service is configured according to corresponding section in
cocaine-runtime configuration file. For every key of "services"
section the corresponding service is attempts to start. In case of
failure of any of configured services, the cocaine-runtime fails to
start. Every service is registered within Locator service, and
published on its designated endpoint.

##Cocaine services
List of services and their description you can found in ["Services and Plugins"](../services_and_plugins/services-and-plugins.md) part of documentation.


# Apps

There are implementation differences between App and Service.

Since app is a user-supplied piece of code, that is accounted to be
faulty, apps are run in isolation from the core part of
cocaine-runtime. At least, app instances are run in separate
processes, called slaves, workers, or slave worker processes. The
level of isolation and how these processes are started and handled is
described in a specific isolate plugin.

Isolate plugin interface has the two main methods: `spool` and
`spawn`. Spool is used to fetch application's code or image to local
computing node, and spawn is used to launch one worker instance.

## Start-up procedure

Let's consider a start-up procedure for some app `testapp`. 

A certain cluster node, specifically its service `node`, receives
message `[start_app, sid, [{"testapp":"someprofile"}]]`. We won't go
into details of specific profile here, or isolate plugin, or cache
storage interactions. We assume that there's no cache storage on
cluster nodes, and describe generic start-up procedure.

So when `node` service receives `app_start` message, it creates an
instance of `slave_t` class, which starts an instance of `engine_t`
class, which has `queue` member. `app_t` constructor calls
`isolate_t::spool` method, which is supposed to prefetch and unpack
application code from shared network storage to local storage (spool
directory in case of plain process isolate).

Also, a special service is started. It's called invocation service, it
has two methods `enqueue` and `info`, and it's published to locator
under the name of application being started, here `testapp`.
Consequently, anyone who is resolving service named `tetsapp`,
receives the endpoint of this invocation service, and method table
which consists of two methods: `enqueue` and `info`.

So, by far, there is an app code deployed on local node, and there is
the published invocation service named `testapp` listening on certain
endpoint.

When client connects to this `testapp` service, and sends in messages,
using `enqueue` method of this service, the following things
happen. Service calls method `enqueue` of its connected `slave`
instance. Then it calls `wake` method, which makes slave's thread to
wake up. Slave's engine calls pump method. Since there's no active running
workers that are able to process newly queued messages, no messages
are fetched from queue. Then it calls balance method. Balance method
does balancing as it is described in [overview.md#Balancing_slave]. So
it finds that there are unprocessed messages in queue, and uses
it's isolate's method spawn to spawn more workers. 

In case of plain-process isolate, worker is run in the cwd of it's
spool directory, with the exec path of .slave field from
manifest. Process plugin passes various options (as argv) like worker
uuid, app name, endpoint that worker has to connect to communicate
with cocaine-runtime. Worker process should, after it's
initialization, connect to its app's socket, send handshake message
(which is handled by `engine_t`), and a first heartbeat message, when
worker is ready to process incoming messages.

After it happens, slave becomes active, and when `pump` method is
called once again, active slave receives messages, which are passed
through slave's socket to the slave worker process.

## Available Isolates

There are two basic isolate plugins currently in use. The process
isolate, which operations are as described above, and docker
isolate. 

For basic understanding how Docker works in general, refer to
[docker.md]. The docker isolate's `spool` method instead of fetching
app's tar, does basically a `docker pull` call through local docker daemon's
REST api. Docker isolate's `start` method does `docker run` call with
resource constraints found in profile.

## App configuration

Unlike Services, which are configured in cocaine-runtime configuration
file on nodes of cluster, apps are configured through the following
entities stored in shared storage. 

_manifest_ currently it specifies the path to slave worker process
that cocaine-runtime is supposed to run in worker isolate. It used to
contain different other options, but currently they got out of use,
and usually manifest boils down to something like
```
{"slave": "./path/to/worker"}
```
path to worker is specified relatively to tgz root, in case of
tar-packed app, and to container root in case of docker container.

_profile_ specifies various balancing, isolate and other properties
related to application runtime configuration. Usually, profile to run
app in docker container looks like this:
```
{
  "queue-limit":1000,
  "concurrency":100,
  "spawn-threshold": 10,
  "isolate":{
    "type":"docker",
    "args":{
      "registry":"http://dockerregistry.tst.ape.yandex.net"
      "repository":"unstable"
    }
  }
}
```
Refer to [TODO] to see the complete specification of available profile parameters.


_runlist_ is a map from app-name to profile. Cocaine-runtime uses it
to start apps on a start of a particular node start, as described in
Services.md#node_service.
Typical runlist would look like this:
```
{
  "app-some-1":"docker-local",
  "app-other":"run-from-tgz",
  "app-another": "queue-10000"
}
```
Here, again, app-some-1, app-other, app-another are apps names, and
docker-local, run-from-tgz and queue-1000 are profile names, with
which corresponding app should be run.

