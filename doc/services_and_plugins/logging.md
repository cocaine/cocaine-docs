#Logging service

##Description

Service ``logging`` provides interface for logging through the Cocaine. It uses [Blackhole logger](https://github.com/3Hren/blackhole/blob/master/doc/contents.md).

##Handles

```
emit(level, issuer, message, attributes)
```

Emits message to the log.

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|level|Level of logging. Number from 0 to 3, INFO=0, WARNING=1, ERROR=3, DEBUG=4.|
|issuer|Name of message author. Any string that identifies issuer of the message.|
|message|Log message.|
|attributes| Optional parameter which can be omitted. It is a JSON object of an arbitrary form. It is not usefull for the most cases but if you use for example Elasticsearch as a backend, you can get additional abilities with this parameter.|

##Configuration

Cocaine configuration file should contain the sections like following:

```
"services": {
    "logging": {
        "type": "logging"
    ]
},

...

"logging": {
    "core" : {
        "loggers": [
            {
                "formatter": {
                    "type": "string",
                    "pattern": "[%(timestamp)s] [%(severity)s]: %(message)s %(...:[:])s"
                },
                "sink": {
                    "type": "syslog",
                    "identity": "cocaine"
                }
            }
        ],
        "timestamp": "%Y-%m-%d %H:%M:%S.%f",
        "verbosity": "info"
    }
},

...

```

Read the documentation of [Blackhole](https://github.com/3Hren/blackhole/blob/master/doc/contents.md) to learn the the complete list of possible ``loggers`` configuration parameters. Read the [Main concepts](https://github.com/3Hren/blackhole/blob/master/doc/manual/concepts.rmd) section to understand what is it and then read the documentation for each ``sink`` and ``formatter`` to learn theirs configuration.

[Back to Contents](../contents.md)
