#Elasticsearch service plugin
##Description
This service is a Cocaine frontend to [elasticsearch](http://www.elasticsearch.org/), it doesn't extends or modify interface or an original engine of elastisearch. This plugin can't be used as a backend for Cocaine storage service.

##Handles

```
get(index, type, id)
index(data, index, type, id)
search(index, type, query, size)
delete_index(index, type, id)
```

You can find description of elasticsearch methods in the original documentation on project. Handles of service corresponds to the elasticsearch APIs as follows.

  * **get** corresponds to [Get API](http://www.elasticsearch.org/guide/en/elasticsearch/reference/current/docs-get.html). Returns `status` and `response` string.
  * **index** corresponds to [Index API](http://www.elasticsearch.org/guide/en/elasticsearch/reference/current/docs-index_.html). Returns `status` and `id` string.
  * **search** corresponds to [search apis](http://www.elasticsearch.org/guide/en/elasticsearch/reference/current/search.html). Returns `status`, `count` and `id` string.
  * **delete_index** corresponds to [Delete API](http://www.elasticsearch.org/guide/en/elasticsearch/reference/current/docs-delete.html). Returns `status`. 

##Configuration
Plugin configured in [`services` section](../maintenance_server_configuration.md#services) of the Cocaine configuration file. By default it has the next look:

```javascript
"elasticsearch": {
    "type": "elasticsearch",
    "args": {
        "host": "127.0.0.1",
        "port": 9200
    }
}
```

`host` and `port` are the parameters of entry point to the Elasticsearch.

[Back to Contents](../contents.md)
