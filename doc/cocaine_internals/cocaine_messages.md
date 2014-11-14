#Cocaine messages

Messages are formed using [msgpack](http://msgpack.org/). A message is a tuple (array in msgpack terms) of: message_type_id (sometimes called method_id), session_id, and a tuple (array)  of arguments.

`method_id`'s are specified in corresponding protocol's.

`find`-method of `storage`-service will be called with the message:

```
[ 3, // id of find method
  123, // id of session
  [ "collection name", // collection name to look items in
    [ "tag1", "tag2" ] ] ] // tags
```
