#Logging service

##Description

Service Logging provides interface for logging through the Cocaine.

##Handles

```
emit(level, issuer, message)
```

Emits message to the log.

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|level|Level of logging. Number from 0 to 3, INFO=0, WARNING=1, ERROR=3, DEBUG=4.|
|issuer|Name of message author. Any string that identifies issuer of the message.|
|message|Log message.|

##Configuration

Logging can use any backend which supports its interface. Cocaine provides three backends:

  * `files`.
  * `syslog`.
  * `logstash` as a separate [plugin](logstash.md).

Cocaine configuration file should contain the sections looks like following:

```
"services": {
    "syslog_logger": {
        "type": "logging",
        "args": {
            "backend": "syslog_backend"
        }
    },
},

...

"loggers": {
    "files_backend": {
        "type": "files",
        "args": {
            "path": "/var/lib/cocaine"
        }
    },
    "syslog_backend": {
        "type": "syslog",
        "args": {
            "identity": "cocaine",
            "verbosity": "info"
        }
    },
    "logstash_backend": {
        "type": "logstash",
        "args": {
            "port": 50030
        }
    }
}
```

By default Cocaine use service `logging` with the syslog, configured as `core` backend. Example of it can be found in [configuration guide](../maintenance_server_configuration.md).

[Back to Contents](../contents.md)
