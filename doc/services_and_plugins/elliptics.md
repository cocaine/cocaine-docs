#Elliptics service and backend plugin

##Description
`Elliptics` plugin contains two components: service and backend. Backend can be used with the [core `storage` service](storage.md) but it has more functions then the core storage service provides for clients. If you want to use all of the resources of `elliptics` backend `elliptics` service should be used.

This plugin is installed with [Elliptics](http://reverbrain.com).

##Handles

Elliptics plugin adds the next handles to [storage](storage.md) interface:

| Handle | Description |
|--------|-------------|
|cache_read|Read the key from the Elliptics cache.|
|cache_write|Write the data to the Elliptics cache and set its lifetime to timeout.|
|bulk_read|Read several keys.|

```
cache_read(collection, key)
cache_write(collection, key, blob, timeout)
bulk_read(collection, keys)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|collection|Name of collection.|
|key|Data identification string.|
|keys|List of keys.|
|blob|Data.|
|timeout|Time after which the object could be removed from the Elliptics cache. Optional parameter.|

**Return value**

Result is programming language specific. Read the corresponding APIs descriptions.

##Configuration
[Cocaine configuration file](../maintenance_server_configuration.md) should contain the next sections:

```javascript
"services": {
    "storage_elliptics": {
        "type": "elliptics",
        "args": {
            "backend": "backend_elliptics"
        }
    }
}

"storages": {
    "backend_elliptics": {
        "type": "elliptics",
        "args": {
            "nodes": [
                "host:port:2",
                "host:port:2"
            ]
            "groups": [2],
        }
    }
}
```

If you want Cocaine to use elliptics by default `storage_elliptics` should be renamed to `storage` and `backend_elliptics` should be renamed to `core`.

**Arguments of backed_elliptics**

As an **args** for backend the next fields can be specified: `wait-timeout`, `check-timeout`, `io-thread-num`, `net-thread-num`, `flags`, `groups`, `remotes`, `nodes`.

`nodes` describes endpoints where elliptics client could connect. If Elliptics **2.25** is used section is array of strings (i.e. ```"host:port:address_family"```), but in case of version **2.24**  map (i.e. ```{"host": port}```) has to be used.

`flags` corresponds to flag parameter from [elliptics config core section](http://doc.reverbrain.com/elliptics:configuration#elliptics_core_section).

`remotes` corresponds to `remote` parameter of [client node configuration section](http://doc.reverbrain.com/elliptics:configuration#client_node_configuration).

`groups` is a list of groups in the following format: "[group_id_1,group_id_2,...]".

All other parameters can be found in the [client node configuration section](http://doc.reverbrain.com/elliptics:configuration#client_node_configuration) under original names.

[Back to Contents](../contents.md)
