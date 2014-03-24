#MongoDB plugin

##Description

This plugin contains backend for storage service of type `storage`.

##Configuration

It is installed with the Cocaine plugins and configured in section `storages` of Cocaine configuration file like follows:

```
"mongodb_backend": {
    "type": "mongodb",
    "args": {
        "uri": "MongoDB connection string"
    }
}
```

`uri` is a standard [MongoDB connection string](http://docs.mongodb.org/manual/reference/connection-string/).

[Back to Contents](contents.md)
