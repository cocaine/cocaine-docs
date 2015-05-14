# Unicorn service plugin
## Description
This plugin is a key-value storage. It uses [ZooKeeper](http://zookeeper.apache.org/) as a backend thus do not use this service as a database. 

Service allows to:
  * Store any data, that can be serialized with the [msgpack](http://msgpack.org/) and compatible with ZooKeeper applicability. For example, you can use it to store the app configuration parameters.
  * Use publish/subscribe way to be consistent with the data changes.
  * Create a global distributed lock.

Read the [ZooKeeper overview](http://zookeeper.apache.org/doc/trunk/zookeeperOver.html) to learn the basic terms such as a ``node``.

Service does not support some features of ZooKeeper. For example, you cannot store values in nodes that have children.

## Handles

|Handle|Description|
|------|-----------|
| [children_subscribe](#children_subscribe) | Subscribe for node children. |
| [close](#close) | Stop communication with ``Unicorn`` service. |
| [create](#create) | Create node. |
| [del](#del) | Delete a node. |
| [get](#get) | Get value of a node. |
| [increment](#increment) | Increment a node's data by the specified value.|
| [lock](#lock) | Create a lock on a specified node. |
| [put](#put) | Put a value to a specified node. |
| [subscribe](#subscribe) | Subscribe for a specified node. |

### children_subscribe

Subscribes for the node children creation/deletion.

```
children_subscribe(path)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Path to the node with the children.|

**Return value**

Messages:
  * Stream of "value" with vector of deleted/created node names. ``Unicorn`` sends this vector for each new of deleted child.
  * "error" with an error description. Errors are unrecoverable and communication with ``Unicorn`` will be stopped.


### close

Stops communication with ``Unicorn`` service. It stops subscriptions and releases locks. Also ``Unicorn`` will not answer to any of your messages after it.

```
close()
```

### create

Creates node with the specified value of the version 0.

```
create(string path, variant value)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Node path.|
|value|variant|Data to store.|

**Return value**

Messages:
  * "value" with True.
  * "error" with an error description.

### del

Deletes a node if version matches.

```
del(path, version)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Node path.|
|version|unsigned int|Version of node.|

**Return value**

Messages:
  * "value" with True.
  * "error" with an error description.

### get

Gets value of the specified node.

```
get(path)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Path to the node that has no children.|

**Return value**

Messages:
  * "value" with the tuple(value, version).
  * "error" with an error description. Errors are unrecoverable and communication with ``Unicorn`` will be stopped.

### increment
 
 Increments node's data by the specified value. If one of operand is non-numeric returns error, if one operand is floating point returns floating point, otherwise returns int. Value can be negative. 

```
increment(string path, variant value)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Node path.|
|value|variant|Value can have integer or floating point type.|

**Return value**

Messages:
  * "value" with the tuple(value, version) contains new values.
  * "error" with an error description.
   
### lock

Creates a lock on the specified node. Do not use the specified path for other purposes. If lock is acquired other app blocks until lock is released. Automatically released on disconnection.

```
lock(path)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Node path.|

**Return value**

Messages:
  * "value" with True.
  * "error" with an error description.

### put

Puts the value to the specified node. 

```     
put(path, value, version)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Node path.|
|value|variant|Data to store.|
|version|unsigned int|Version of a data.|

**Return value**

Messages:
  * "value" with the tuple(bool, value, version). 
    * ``Bool`` equals ``True`` if value was stored or ``False`` if ``Unicorn`` contains value of upper version. 
    * ``Value`` contains currently stored data. ``Version`` contains currently stored version.
  * "error" with an error description.

### subscribe

Subscribes for specified node.

```
subscribe(path)
```

**Parameters**

|Parameter|Type|Description|
|---------|----|-----------|
|path|string|Path to the node that has no children.|

**Return value**

Messages:
  * Stream of "value" with the tuple(value, version). ``Unicorn`` will send updates of value as the version upped.
  * "error" with an error description. Errors are unrecoverable and communication with ``Unicorn`` will be stopped.

## Configuration
Service should be configured in [services section](../maintenance_server_configuration.md#services) of Cocaine configuration file as follows:  

```
"unicorn": {
    "type": "unicorn",
    "args":{
        "endpoints": [{"host":"localhost","port":2181}, ...]
    }
}
```

###Arguments

|Argument|Description|
|--------|-----------|
|endpoints|Array of ZooKeeper endpoints. ``Unicorn`` use "localhost:2181" by default.|
    
##Cocaine internals
``Unicorn`` plugin also can be used for internal Cocaine needs:
  * Cocaine can store any management data.
  * Cocaine can use ``Unicorn`` as a discovery service in ``Locator``.

### Locator discovery

Plugin provides ``cocaine::cluster::unicorn_cluster_t`` class.  ``Locator`` can use it as a cluster interface for discovery functionality.
Discovery works in a following way:
  * On ``Locator`` announce ``Unicorn`` puts ``$prefix/$uuid`` node with serialized endpoints as a value. ``Unicorn`` checks announce every ``check_interval`` seconds.  If something goes wrong plugin retries to announce every ``retry_interval`` seconds.
  * Each ``Locator`` also subscribes for children on ``prefix`` node. For each node addition/deletion ``Unicorn`` calls ``drop_node/link_node``. It checks that list is valid every ``check_interval`` seconds and retries in case of failure every ``retry_interval`` seconds.

###Configuration

To use ``Unicorn`` service for ``Locator`` discovery purpose administrator should configure ``Locator`` as follows:

```
"locator": {
    "type": "locator",
    "hostname" : "localhost",
    "endpoint" : "127.0.0.1",
    "port" : "10054",
    "args": {
        "cluster": {
            "type": "unicorn",
            "args": {
                "endpoints": [{"host":"localhost","port":2181}, ...]
                "retry_interval": 1,
                "check_interval": 60
            }
        }
    }
}
```

``Unicorn`` configuration contains three arguments:

|Argument|Description|
|--------|-----------|
|endpoints|Array of ZooKeeper endpoints. ``Unicorn`` use localhost:2181 by default.|
|retry_interval|Interval to retry in case of failure. 1 second by default.|
|check_interval|Check interval of regular order. 60 seconds by default.|

[Back to Contents](../contents.md)
