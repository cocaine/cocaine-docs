#Worker protocol

This protocol provides communication between Cocaine ``App`` service and a worker runs a custom app.

Worker protocol consists of two parts:
  * The control protocol;
  * The communication protocol.

##The control protocol

This protocol provides messages for start/stop and stay alive purposes. The control protocol requires the first channel (channel_id = 1). When worker starts it should send "handshake" message to Cocaine.

**Messages**

|(ID) Message  |Channel|args                         |Description|
|--------------|-------|-----------------------------|-----------|
|(0) handshake |1      |(string) uuid                |The first message that worker sends to Cocaine after start. ``uuid`` is that which used in worker start command line.|
|(1) heartbeat |1      |                             |Worker should send this message to Cocaine periodically. Cocaine should answer to this message immediately with the same message. If there is no answer it means some problems with the core. If Cocaine does not answer for "heartbeat" messages for some time worker should stop. Time to wait for Cocaine answer is not specified anywhere, good practice is to use 60 seconds.|
|(2) terminate |1      |(int) errno, (string) reason | Cocaine sends this message to the worker to stop it. Worker should answer with any message immediately and stop. As an answer the worker can use the same message. In its turn the worker sends this message to Cocaine if it stops.|

Interval of "heartbeat" sending is specified in app profile, but the worker can not get it. Developer of framwork should implement "heartbeat"s nore often, then 30 seconds in default Cocaine configuration.

##The communication protocol

This protocol provides messages for data exchange and for custom app handles call.

The first message of communication transaction should be "invoke" it starts data exchange it the channel. Each "invoke" message should be sent in a new channel in ascending order of channel numbers. After "invoke" Cocaine and worker can send other protocol messages in both directions.

**Messages**

|(ID) Message  |Channel|args                          |Description|
|--------------|-------|------------------------------|-----------|
|(3) invoke    |Any    | (string) event               | ``event`` is an app handle. Cocaine sends it to the worker to start communication transaction. Worker should be ready to receive "chunk", "error" and "choke" in the started channel, the worker also can send these message to Cocaine.|
|(4) chunk     |Any    | (byte string) data           |Data message. Can contain any data suitable for ``msgpack``.|
|(5) error     |Any    | (int) errno, (string) reason |Error message. Contains identifier of error and its description.|
|(6) choke     |Any    |                              |Close channel message. Can be sent from both ends of channel. Both directions of a channel are treated as independent, thus if Cocaine and worker wants to stop all communications "choke" should be sent by both sides.|

[Back to Contents](../contents.md)
