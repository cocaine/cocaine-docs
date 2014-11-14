#Cocaine RPC protocol

Cocaine RPC protocol uses [cocaine messages](cocaine_messages.md).

Each service has a table of callable methods. Table for `storage`-service for example:

| id | method name |
|----|-------------|
| 0 | read |
| 1 | write |
| 2 | find |
| 3 | remove |

To call `read` on the key `keyB` in `collectionA` caller required to get service endpoint, connect to it and send the next message:

```
[0,   // read method id 
 123, // client-generated random id
 ["collectionA", "keyB"]] // arguments to read method
```

Subsequently in our examples we will use name of method instead of its number. For example `[read, 123, ["collectionA", "keyB"]]`.

Service replay:

```
[chunk, 123, [<binary-buffer1>]]
[chunk, 123, [<binary-buffer2>]]
[chunk, 123, [<binary-buffer3>]]
[choke, 123, []]
```

In case of some error client gets:

```
[chunk, ...]
[chunk, ...]
[error, session_id, [<code>, <message>]]
```

RPC identifiers:

| id | name |
|----|------|
| 0 | handshake |
| 1 | heartbeat |
| 2 | terminate |
| 3 | invoke |
| 4 | chunk |
| 5 | error |
| 6 | choke |

Each connection to service can serve for multiple sessions simultaneously. Identifier of closed session can be reused, but it doesn't look useful because `session_id` is a 64bit integer.

As a recap.
Client creates (or uses existing) connection to the service. It sends message with the: 
  * `method id` from the services table;
  * `session_id` unique for connection;
  * parameters of the call.
As a response service sends some or none chunks of data and terminates it with `choke` or `error` messages.
