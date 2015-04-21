# Unicorn
## Description
This plugin provides a service, which can be used:
  * To store applications configurations
  * To store any management data used by cocaine
  * To provide consistent publish-subscribe mechanism
  * To be used as distributed locking service
  * To be used as discovery in locator.

This plugin is NOT intended to be used as database to store any non-service data.
Currently it is implemented via zookeeper.

## API
Service defines following methods:

  * `create(string path, variant value) -> error || bool`
   Creates node with specified value if it does not exist. Returns error otherwise.
  * `put(string path, variant value, uint version) -> error || tuple(bool, variant value, uint version)`
   Puts value to specified node path. Returns tuple of bool(indicates if put was performed by this call), value(current value), version(current version).
  * `subscribe(string path) -> stream of (tuple(variant value, uint version) || error)`
   Subscribes for specified path. Streams updates(including initial value) to rx. Updates are guaranteed to be sent in version ascending order.
   Errors are unrecoverable -> RX is closed after error.
  * `children_subscribe(string path) -> stream of ((vector of string) || error)`
   Subscription for node children. Streams list of node names(including starting list) - children of specified node.
   Errors are unrecoverable -> RX is closed after error.
  * `del(string path, uint version) -> error || bool`
   Deletes a node if version matches. Error otherwise.
  * `increment(string path, variant value) -> error || tuple(variant value, uint version)`
   Increments node's value by specified value and returns resulting value. If one of operand is non-numeric returns error, if one operand is floating point returns floating point, otherwise returns int. Value can be negative.
  * `lock(string path) -> void`
   Creates lock on specified path. Path SHOULD NOT be used for other purposes. If lock is  aquired by other app blocks until lock is released. Automatically released on disconnection.

After any call close method is defined, which can be used to cancel subscription or release lock or just close channel.

Also this plugin provides cocaine::cluster::unicorn_cluster_t class which can be used as cluster interface in locator for discovery functionality.
Discovery works in a following way:
  * On locator announce it puts a node $prefix/$uuid with serialized endpoints as value. It checks announce every $check_interval seconds.
  If something goes wrong it retries to announce every $retry_interval seconds.
  * Each locator also subscribes for childs on $prefix. On each node addition/deletion it calls drop_node/link_node. It checks that list is valid every $check_interval seconds and retries in case of failure every $retry_interval seconds.

## Configuration
args section of service config in cocaine config has following parameters:

  * endpoints -> array of objects with fields(non-optional)
    * host -> zookeeper's host
    * port -> zookeeper's port
    * If endpoints is empty or not present default ZK host port used (localhost:2181)
    
args section of cluster config in cocaine config has following parameters:

  * endpoints - the same as in service
  * retry_interval - interval to retry in case of failure. Default 1s
  * check_interval - check interval in case everything goes smooth. Default 60s
