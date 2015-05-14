# Service Graphite
## Description
Service aggregates metrics for [Graphite](http://graphite.wikidot.com/) and sends it via text protocol.

## Handles
| Handle | Description |
|--------|-------------|
|send_one|Send one metric.|
|send_bulk|Send pack of metrics.|

```
send_one(string metric_name, double metric_value, optional uint metric_timestamp)
send_bulk( array of (string metric_name, double metric_value, optional uint metric_timestamp) )
```

### Parameters
|Parameter|Type|Description|
|---------|----|-----------|
|metric_name|String|Metric name. Cocaine adds a prefix to the name. Set prefix in service configuration.|
|metric_value|Double|Metric value.|
|metric_timestamp|Integer|Metric timestamp. Cocaine uses current time if parameter omitted.|

## Configuration
Service should be configured in [services section](../maintenance_server_configuration.md#services) of Cocaine configuration file as follows:  

```
"graphite": {
    "type": "graphite",
    "args":{
        "host": "127.0.0.1",
        "port": 2003,
        "flush_interval_ms": 1000,
        "prefix": "cocaine",
        "max_queue_size": "1000"
    }
}
```

###Arguments

|Argument|Description|
|--------|-----------|
|host|Graphite host. "127.0.0.1" by default.|
|port|Graphite port. "2003" by default.|
|flush_interval_ms|Interval in milliseconds between sending to Graphite.|
|prefix|Prefix. Cocaine automatically add prefix to metric names. "cocaine" by default.|
|max_queue_size|Maximum number of elements in queue. Service sends metrics to Graphite by interval (``flush_interval_ms``) or when size of queue amounts to ``max_queue_size``. "1000" by default.|

[Back to Contents](../contents.md)
