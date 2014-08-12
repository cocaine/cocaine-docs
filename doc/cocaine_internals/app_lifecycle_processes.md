
# App lifecycle processes

This section describes the processes that occur when app works oк someone works with app.

## Regular Deployment Process

Native way of app deploy.
User builds the app manually and the output is the app tarball. After that user upload app with the manifest into Cocaine storage. When app starts isolate process is used. Nodes of cloud get app tarball, unfold it to the spool directory and start app invocation service.

Let's consider each operation with prepared app in details.

### upload
```
cocaine-tool app upload \
  --name <app-name> \
  --package path/to/app.tgz \
  --manifest manifest.json
```

While uploading app to the storage:

1. `cocaine-tool` packs manifest with the `msgpack` and send it to `storage`-service by the path `manifests::<app-name>`, where `manifests` - collection, and
`<app-name>` - key).
1. `cocaine-tool` sends app tarball to the storage by the path `apps::<app-name>`.

### start
```
cocaine-tool app start \
  --name <app-name> \
  --profile <profile-name>
  --host <locator-host-name|localhost>
  --port <locator-port|10053>
```

Despite command called `app start` it doesn't start app workers, it just app deploy to nodes. `cocaine-tool` finds `node`-service and calls its method `start_app`.

```
[start_app, <sid>, [{<app-name>:<profile-name>}]]
```

`node` reads app profile from the storage by the key `profiles::<profile-name>`, gets isolate type and installs app locally with the `isolate::spool` method. `process` is a default isolate its 'spool' method reads app from the storage and unfold it into `<spool>/<app-name>/` (`<spool>` configured in cocaine configuration file). 

Additionally this node starts app invocation service and app engine with empty input queue and no started workers.

Local Locator announce started app invocation service to all `synchronize`-client. From this moment app considered to be running.

If we want to start app at several nodes, it is required to send `app start` to each of them.

To start automatically app should be added to local runlist. If configuration file of node contain `{ "default-runlist": "<runlist-name>" }`,
`cocaine-runtime` will read `runlists::<runlist-name>` at runtime. Runlist should contain msgpack map of the next form:

```
{ "appname1":"profile1", "appname2":"profile2", ... }
```

`cocaine-runtime` will start each of app from this list with the corresponding profile.

### request handling
App invocation service gets input messages from client in the form of

```
[enqueue, <sid>, ["<event-name>", <binary-chunk>]]
```

Invocation service converts this message into 3:

```
[invoke, <sid>, ["<event-name>"]]
[chunk, <sid>, [<binary-chunk>]]
[choke, <sid>, []]
```

and writes them to the app engine message queue.

Engine thread starts at this time and execute `balance`-method which start some workers. Each worker start app instance with the `isolate::start`. `process::start` use environment from the app manifest, working directory is `<spool>/<app-name>` and the next arguments of command line:

```
  --app <app-name> \
  --endpoint <unix-socket> \
  --locator <locator-endpoint> \
  --uuid <worker-uuid>
```

Worker should connect to engine's `<unix-socket>` and send handshake message. Then worker should send heartbeat message. Only after heartbeat client can start communicate with app.

### stop
```
cocaine-tool app stop \
  --app <app-name> \
  --host <locator-host> \ 
  --port <locator-port>
```

This command sends `pause_app` message to the `node`-service which stops all the app workers and invocation service and announce about this all `sychronize` clients.

After `terminate`-message from engine app instance should finish all its processes in `terminate-timeout`, send `terminate` to engine and come to stop.

To stop app on several nodes `pause_app` should be sent to each node separately.

### remove

Removes app from storage (`apps::<app-name>`), removes app manifest (`manifests::<app-name>`). 

No started app check.

### Docker isolate plugin

When Docker used as isolation mechanism app with it environment are stored in Docker repository. Manifest and profile of app play the role as with another isolate type.

#### upload

When `cocaine-tool` used there is no difference in comparison with the described above procedure, just app container is stored in Docker repository.

#### start

The peculiarity of this process is in `docker_isolate::spool`. Instead of using `storage` `spool` calls `docker::pull <registry>/<app-name>` where `registry` is got from profile.

#### handle requests

Differences from earlier described `isolate::process` are not big. `docker_isolate::spawn` calls `docker::start`, mounts `unix-socket` directory into the container, sets environment variables from manifest and resources limits from the app profile.

#### stop, remove
This operations has no differences with described above.



