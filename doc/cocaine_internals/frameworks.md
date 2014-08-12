
# Framework

## What it does and helps to do

So one can interact with cocaine environment and all of it's
components by exchange of messages. To help developers develop their
apps quickly and be compliant with requirements of cocaine
environment, there are frameworks for languages. Basically, framework
gives the developer means to interact as a client with any service, or
helps to run her code in the cloud. It helps to interact with various
conditions such as session handling and lifetime events like termination.

## Client

Client component of a framework allows developer to interact with any
cocaine service. It resolves certain service, provides means to
conveniently call it methods, and means to handle response stream,
means to interpret streams according to developer's needs.

## Worker

Worker component of a framework gives a developer means to turn her
code into scaling app, and to run it compliantly under the management of
cocaine-runtime.

## Worker requirements

Here are the requirements for any code that wants to implement behaving
worker under cocaine-runtime management.

1. Overview
When spawned, slave process receives command-line arguments. It should
connect to the right socket and to present itself with a certain
handshake. Afterwards it should send a heartbeat message every time
the interval passes. When the first heartbeat arrives to
cocaine-runtime, it passes incoming messages to worker, keeping count
of pending non-ended sessions. Worker replies to incoming messages
with stream. On some condition it may signal termination, after which
no new sessions will be passed to it.
When cocaine-runtime decides to stop worker instance, it sends
terminate message, stops sending in new sessions. When worker is done
processing its current sessions, it sends terminate message back to
cocaine runtime, and cocaine-runtime sends worker process a TERM
signal. 

1. Start-up
On start-up, worker is called with the following command-line
arguments:
`--app <name_of_app>` name of app as it is known to cocaine-runtime
from runlist or start_app message.
`--uuid <uuid>` 512-bit uuid in string form. Used in handshake.
`--locator <locator_endpoint>` tcp socket or path to unix socket, that worker
should use to communicate with locator to resolve services.
`--endpoint <app_endpoint>` tcp socket or path to unix socket, that worker
should use to communicate with cocaine-runtime and through which it
will receive incoming sessions.
Worker should connect to `app_endpoint`, send a handshake message that
looks like `[<handshake-id>, 0, [<uuid-as-string>]]`.
After this is done, and when ready to process incoming requests,
worker should start sending `heartbeat` messages.
`[<heartbeat-id>, 0, []]`
When cocaine-runtime doesn't receive first heartbeat within configured
`startup-timeout`, it considers worker as failed to initialize, and
sends it TERM signal. After the first heartbeat is received,
cocaine-runtime changes timeout for next `heartbeat` to
`heartbeat-timeout`. When there's no next heartbeat within that time,
cocaine-runtime considers worker to be stuck, and sends it TERM signal.

1. Handling sessions
After the cocaine-runtime received first heartbeat from worker, it
starts sending in user sessions. The incoming sessions looks like
this: `[invoke, session_id, ["event-name"]]`, chunks, `<choke>`.
Currently, when app incoming sessions originate from the app's dedicated
invocation service, there will be at most one chunk in incoming
stream.

##### example

In case of http requests encoded, binary payload of the incoming chunk
looks like this:
```
[method, version, uri, [[header, value], [header, value]], <binary-body]
```
response chunk stream payloads would look like this:
```
[200, [[header:value], [header:value]...]]
<binary-body-slice>
<binary-body-slice>
```

1. Handling heartbeats
Heartbeats used to let cocaine-runtime and worker know to each other that they are
online and operating normally.
First heartbeat message to runtime should be sent no earlier than
worker is ready to process incoming sessions.
When worker fails to send heartbeat within currently active timeout,
it is considered as stuck or hang-up, so cocaine-runtime sends it TERM
signal and drop all sessions the worker started processing.
When worker fails to receive heartbeat message from cocaine-runtime
within certain interval, it should consider itself abandoned for some
reason, it should complete any transactions that it can complete on
its own, and terminate.

1. Handling and signaling termination
When cocaine-runtime decides to shut down worker instance for this or
that reason, it sends worker a terminate message. The reason and code
are specified within the message. Worker, given such a message on
input, should complete all of its transactions, send terminate
message back, and exit with normal status.
When worker decides to exit for some reason, it should send terminate
message to cocaine-runtime with code and message explaining why it
terminates, and exit with normal or abnormal status.

Any session received by the worker, under normal conditions, should be
responded with stream, possibly containing only error message.


