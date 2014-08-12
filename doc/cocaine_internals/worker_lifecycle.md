#Worker lifecycle
At some moment of time engine::balance decide to start a new worker. `Slave_t` reads `slave`-field from the manifest and starts slave as follows:

```
relative/path/to/slave --app app-name --locator 1.2.3.4:10053 --endpoint /path/to/unix.sock --uuid aa-bb-bb-cc-cc-cc-dd-dd
```

Engine waits incoming connection to the endpoint socket until `startup-timeout`. Worker should establish connection to Engine and send handshake message. When worker is ready to process incoming requests it should send heartbeat message to Engine. After the heartbeat Engine starts to send messages to worker instance.

When worker wants to finalize it should send `terminate` message with `ok` code to Engine. After message sent worker can come to a stop, if worker is not stop itself, Engine send SIGTERM to it.

In case of Engine should stop worker it send `terminate` message. After worker receive this message in should come to stop in `terminate-timeout` by the way described above.

