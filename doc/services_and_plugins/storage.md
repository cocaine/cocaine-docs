#Storage

##Description
`Storage` service is a core Cocaine service, it provides basic interface to storage operations. `Storage` just a lightweight proxy that directs user's requests to backends.

`Storage` provides interface for key-value processing. Data in storage organized into `collections` in manner of one level filesystem structure. Keys has additional attributes - `tags`. `Tags` are the strings that allows to flexible mark data keys inside each `collection`.

##Handles

| Handle | Description |
|--------|-------------|
|find|Find keys by tags. Returns list of keys.|
|read|Get data from the storage by the key.|
|remove|Remove data from storage by the key.|
|write|Put data into the storage by the key.|

```
read(collection, key)
write(collection, key, blob, tags)
remove(collection, key)
find(collection, tags)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|collection|Name of collection.|
|key|Data identification string.|
|blob|Data.|
|tags|Set of attribute strings.|

##Configuration

Service is configured in `services` section of the configuration file. Type of service is `storage`.

```javascript
"storage": {
    "type": "storage",
    "args":{
        "backend": "storage_backend"
    }
}
```

As a backend (`storage_backend`) for this service any type of storages can be used, that support it internal interface. Nowdays it is a file storage and [`MongoDB` plugin](mongodb.md).

File storage is configured like:

```
"files_backend": {
    "type": "files",
    "args": {
        "path": "path to storage"
    }
}
```

`path to storage` should exists and user running Cocaine should have enough rights to read/write to that `path`.

[Elliptics plugin](elliptics.md) can be used with `storage` also but it has more functions and if you want to use all of them you should configure it with the original storage service with `elliptics` type.

[Back to Contents](../contents.md)
