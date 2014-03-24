#Urlfetch service

##Description

Service allows send HTTP requests with GET and POST methods.

##Handles

```
get(url, timeout, cookies, headers, follow_location)
post(url, body, timeout, cookies, headers, follow_location)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|url|URL. Required for all handles.|
|timeout|Time to wait answer from server in milliseconds. 5000 by default. Optional.|
|cookies|Cookies. Optional.|
|headers|Headers. Optional.|
|follow_location|If this parameter set to `True` (by default), `urlfetch` will follow all server redirects. Optional.|
|body|Content of the packet. Required for `post`.|


All handles return the following set of fields:

|**Parameter**|**Description**|
|-------------|---------------|
|success|`True` if server answers with 1xx, 2xx or 3xx code.|
|data|Body of answer packet without headers.|
|code|HTTP code of answer.|
|headers|HTTP headers of answer.|

On errors urlfetch closes stream to the client.

##Configuration

Service configured in [`services` section](maintenance_server_configuration.md#services) of configuration files as follows

```
"urlfetch": {
    "type": "urlfetch",
    "args":{
    	"connections-limit": 10
	}
}
```

`connections-limit` regulates quantity of requests processed simultaneously. It can be set up to hundreds depending on the processing power of the host running Cocaine.

[Back to Contents](#contents.md)
