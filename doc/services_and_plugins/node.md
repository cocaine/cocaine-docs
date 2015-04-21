#Node service

##Description
`Node` service is a core Cocaine service that is designed to manage apps running. It starts and stops apps.

One instance of `Node` is started by `cocaine-runtime`. `Node` get name of runlist from the configuration file or use "default". Then it start all apps in runlist as separate [app services](app.md). If "default" runlist doesn't exist or empty service doesn't start anything.

##Handles

| Handle | Description |
|--------|-------------|
|[start_app](#start_app)|Start apps.|
|[pause_app](#pause_app)|Stop apps.|
|[list](#list)|Return list of started apps.|


###start_app
Starts apps in runlist which passed as a parameter of call.

```
start_app(runlist)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|runlist|List of pairs `"app_name","profile"`.|

**Return value**

`Node` tries to start each app in runlist and return list of apps with resolution on each app:

  * "the app is already running";
  * "the app has been started";
  * result of exception.what().

###pause_app
Stops apps in list which passed as a parameter of call.

```
pause_app(applist)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|applist|List of app names.|

**Return value**

`Node` tries to stop each app in runlist and return list of apps with resolution on each app:

  * "the app is not running";
  * "the app has been stopped".

###list
Returns list of started apps.

```
list()
```

**Return value**

List of apps. This list includes all apps in all states, sleeping and working.

##Configuration
Configuration parameters for `Node`-service are gathered in [node section](../maintenance_server_configuration.md#node-configuration) of configuration guide.

[Back to Contents](../contents.md)
