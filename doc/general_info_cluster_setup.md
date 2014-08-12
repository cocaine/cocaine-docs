# Cluster setup

## Big Picture of What Is Where

Typical cluster:

[TODO](scheme)

**Node** hosts are generally used for `cocaine-runtime` and `docker`.


**LoadBalancer** hosts contain `nginx`, `cocaine-http-proxy`, `cocaine-runtime` with the IPVS-gateway plugin.


**Elliptics** storage is a cloud itself and it works in separate cluster.


**DockerRepository** stores apps images in Docker repository with Elliptics backend.


**Dockerbuild** hosts build engine for apps. In contains Docker server. 

When someone need to create app image he should pass app to Dockerbuild with `git push` command under `ape` user account. When app image is ready it pushed into the **DockerRepository**.


**ElasticSearch** cluster is used for logging of all Cocaine cluster activity, logs from apps, services, cocaine-runtime and other.

## Node

Node hosts `cocaine-runtime` with services configured. Required services are `node`, `logging`, `storage`.


By `app_start(app_name, profile_name)` command `node`-service  takes app profile and manifest from the storage and starts app. 

Profile contains description of app isolate. `node` calls `isolate->spool()` which performs `docker pull` in case of Docker isolation. 

After getting app image `node` creates invocation service for app. This service has the same name as app it serves and is published on randomly selected port. Local Locator pushes information on started app to all interested Locators (synchronize method), particularly LoadBalancer Locator.

When invocation service gets `enqueue` requests it pushes messages into the local app queue. Engine runs app's workers and pushes messages to them through the pipe. Backward messages from workers are proxied to the client through the Engine.

## LoadBalancer

LoadBalancer hosts `cocaine-runtime` with IPVS. Locator of this node listens announces of running nodes in multicast group. For each new node it calls `synchronize` method to get information about running apps at this node. When LoadBalancer gets info about new app, it adds virtual service named as app into IPVS and adds routes of working app services to IPVS-table.

When client requests service LoadBalancer sends address of IPVS virtual service. Client should connect to it and send messages to service. IPVS virtual service will redirect clients messages to one of real workers added before.

Cocaine-http-proxy at LoadBalancer gets client http-requests, converts them and sends to corresponding virtual service. Proxy can establish several connections to each virtual service for purpose of cross-versions balancing for one app. At connection opening proxy resolves app in Locator.

Locator apps resolving uses routing groups. Routing groups contains app version weights. These weights are used to balance load. For example we can tell Locator to send 5% of total load to v.1 and 95% to v.2 of the same app.
To provide this mechanism proxy should have roughly 20-60 connections to app. Proxy should periodically refresh this pool using LoadBalancer.
