#Logstash plugin

##Description

This plugin can be used as a backend for [logging](services-and-plugins-logging.md) service. It is designed as a lightweight proxy to [logstash](http://logstash.net/), and doesn't extends or modify it functionality.

##Configuration

It is installed with the Cocaine plugins and configured in [`loggers` section](maintenance_server_configuration.md#loggers) of Cocaine configuration file like follows:

```
"logstash_backend": {
    "type": "logstash",
    "args": {
        "host": "url of logstash service",
        "port": "port"
    }
}
```

[Back to Contents](contenst.md)
