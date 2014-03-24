#Locator

##Description

Locator is a router and balancer in one face. When client connects to some service, Locator is the entry point, it knows all the services in the cloud and helps the client to be connected where it need.

As a router Locator acts the next manner. It collects information about all services in the cloud, where they are located and so on, and provides this information to the client. Decision of endpoint for client connection includes balancer data.

Balancer knows about load on all services in the cloud. If there are some versions of any app, balancer also knows how much load (in percent) it should provide to each version.

##Handles

|**Handle**|**Description**|
|----------|---------------|
|[resolve](#resolve)|Resolve service.|
|[synchronize](#synchronize)|Synchronize with other Locators.|
|[reports](#reports)|Get different statistics of services functioning.|
|[refresh](#refresh)|Update info on routing groups.|

###resolve
Resolves service. Get info where it run.

```
resolve(service_name)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|service_name|Name of service to resolve.|


**Return value**

Function returns set of fields.

|**Field**|**Description**|
|---------|---------------|
|endpoint|Endpoint for the client to connect to in order to use the service.|
|protocol_version|Service protocol version. If the client wishes to use the service, the protocol versions must match.|
|names_mapping|A mapping between method slot numbers and names for use in dynamic languages like Python or Ruby.|

###synchronize
Synchronizes with other Locators. When you call this handle, other Locators start to push info of their services on each change. This means that synchronized Locator has results of `resolve` for whole cluster.

```
synchronize()
```

**Return value**

A full dump of all available services on this node.

###reports
Returns different statistics of services functioning, how much clients connected, how much data transferred and so on.

```
reports(endpoint_mapping)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|endpoint_mapping|Maps client remote endpoint to the amount of memory used for that client.|

**Return value**

Service I/O usage counters: number of concurrent connections and memory footprints.

###refresh
Updates info on routing groups.

```
refresh(group_name)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|group_name|Name of the group to refresh.|

##Configuration
Service is configured as described in [Locator section](maintenance_server_configuration.md#locator) of the configuration guide.

[Back to Contents](contents.md)
