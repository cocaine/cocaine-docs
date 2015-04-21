# Graphite
## Description
 It's a service for sending metrics to graphite (http://graphite.wikidot.com/). It provides metrics aggregation and bulk send via text protocol.

## API
  * `send_one(string metric_name, double metric_value, optional uint metric_timestamp) -> void` - sends one metric
  * `send_bulk( array of (string metric_name, double metric_value, optional uint metric_timestamp) ) -> void` - sends pack of metrics.
 Where:
  * metric_name - string, metric name, prefixed by configurable value in cocaine.
  * metric_value - double, metric value.
  * metric_timestamp - metric timestamp. If omitted - current timestamp will be used in cocaine.

## Configuration
  * host - graphite host. Default 127.0.0.1
  * port - graphite port. Default 2003
  * flush_interval_ms - How often metrics are flushed to graphite (in ms).
  * prefix - prefix added to metrics. Default "cocaine"
  * max_queue_size - Maximum number of elements in queue after which metrics are flushed. Default 1000