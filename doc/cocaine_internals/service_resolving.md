#Service resolving
Cocaine have a special `Locator`-service that knows about all other service (including app services). This service assigns parameters of service's endpoints, controls running services and their load, disseminates collected information over the cloud nodes and also provides clients with information about services location.

`Locator` is always available at specified in configuration file address.

From the client point of view main function of Locator is service resolving. This procedure is performed as follows.

Client which searches some service should call `resolve` method of Locator with the Cocaine RPC. Let's find `storage`-service for example. For this purpose we should use the next message:

```
[resolve, <session-id>, ["storage"]]
```

Remember, that you should use numeric identifier of method `resolve` instead of word "resolve".

`Locator` answers as follows:

```
[chunk, <session-id>,
  [msgpack([
    ["host", 12345], // endpoint
    1,               // protocol version
    {0: "read",      // method table
     1: "write",
     2: "delete",
     3: "find"}])]]
[choke, <session-id>, []]
```

As we can see, answer from `Locator` contains network address of service and its methods table.
