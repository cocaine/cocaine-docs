#Server configuration

The default configuration file of the Cocaine runtime after the installation is [/etc/cocaine/cocaine-runtime.conf](https://github.com/cocaine/cocaine-core/blob/v0.12/debian/cocaine-runtime.conf). It has JSON format. The most parameters that you can specify there just replace default values.

Let's consider questions of network communications in Cocaine.

![Cocaine network](../images/network_configuration.png)

Cocaine cloud consists of multiply nodes combined to cluster wich can be configured as a multicast group. Gateways are the entry points of a cloud and they are the balancers of load. Any node of a cloud can be a gateway, even all of them. If you want to create just one node there is no need to configure gateway. 

How nodes learn about each other and how do they know which services are run on each? There is a Locator core service runs on each node for this purpose.

You should use [locator](#locator) and [network](#network) sections of configuration file to manipulate with aspects of Cocaine network.

Other configuration options regulate such parameters of node like [running services](#services), [storage](#storages) and [logging](#logging) components description and a [paths](#paths) settings.

The minimalistic working configuration looks like follows:

```json
{
    "version": 3,
    "services": {
        "logging": {
            "type": "logging"
        },
        "storage": {
            "type": "storage"
        },
    },
    "storages": {
        "core": {
            "type": "files",
            "args": {
                "path": "/var/lib/cocaine"
            }
        },
    },
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
    }
}
```

It's enough for Lonely Cloud node start and serve. 

You need to **remember** that the [paths](#paths) used by default should exist and be available for changes by user who runs node.

The first parameter of the file is `"version": 3` it should be used as is, no changes.

Sections of the configuration file:

| Section | Description |
|---------|-------------|
| [paths](#paths)   | Required paths.  |
| [locator](#locator) | Configuration of the Locator service. |
| [network](#network) | Parameters, that makes a cloud from nodes. |
| [services](#services) | Declaration of used services, custom and core. |
| [storages](#storages) | Configuration of backends for Cocaine storage service. |
| [logging](#logging) | Logger services configuration. |

##Paths

Defaults:

```
"paths": {
    "plugins": "/usr/lib/cocaine",
    "runtime": "/var/run/cocaine"
}
```

| Parameter | Description |
|-----------|-------------|
| plugins | Path to plugin libs (services, storages, loggers etc.). |
| runtime | Path to the pid-file of the process and UNIX-domain-sockets to communicate with workers |


##Locator

Possible fields:

```
"locator": {
    "type": "locator",
    "hostname" : "localhost",
    "endpoint" : "127.0.0.1",
    "port" : "10054",
    "port-range" : "",
    "args": {
        "cluster": {
            "type": "unicorn",
            "args": {
                "endpoints": [{"host":"localhost","port":2181}, ...],
                "retry_interval": 1,
                "check_interval": 60
            }
        },
        "gateway": {
            "type": "adhoc"
        }
    }
}
```

| Parameter | Description |
|-----------|-------------|
| hostname | Local hostname which will be used for Locator publishing. Resolved with the system calls by default. |
| endpoint | Bind address. Can be specified as a hostname or an address. Locator is bound to all interfaces by default.|
| port | Port of the service. |
| port-range | Range of ports available for services binding. If not specified, services are bound to any free port of operating system choice. |
| args | Additional parameters of configuration. [Cluster configuration](#cluster-configuration). [Gateway configuration](#gateway-configuration)|

###Cluster configuration

Cluster is a set of ``Cocaine`` instances in the cloud. Each instance has a ``Locator`` service. All of the ``Locators`` should know about each other. To provide it ``Locator`` can use three types of communication:
  * Multicast groups;
  * Predefined set of ``Locators``;
  * [Unicorn service](../services_and_plugins/unicorn.md).

**Multicast groups**

If network supports multicast groups, you should configure them and then configure ``Locator`` in the following way:

```
"locator": {
    ...
    "args": {
        "cluster": {
            "type": "multicast",
            "args": {
                "group": "",
                "port": 1234,
                "interval": 60
            }
        }
    }
}
```

|Agrument|Description|
|--------|-----------|
| group | IP address of configured multicast group. |
| port | Port number. |
| interval | The period of sending messages to the group. |

**Predefined set**

The configuration of ``Locator`` should have the following form:

```
"locator": {
    ...
    "args": {
        "cluster": {
            "type": "predefined",
            "args": {
                "nodes":{uuid:"host:port", ... }
            }
        }
    }
}
```

|Agrument|Description|
|--------|-----------|
|nodes| Set of all ``Locators`` in the cloud. ``uuid`` is the identifier that ``Locator`` gets. |


**Unicorn service**

How to configure ``Locator`` to use ``Unicorn`` service you can read in [service description](../services_and_plugins/unicorn.md#cocaine_internals).

###Gateway configuration

Gateway configuration consists only of type that can be "adhoc" or "ipvs".

Defaults:

```
"gateway": {
    "type": "ipvs",
    "args": {
        "scheduler": "wlc",
        "weight": "1",
        "port-range": ""
    }
}
```

| Parameter | Description |
|-----------|-------------|
| scheduler | It is the name of ipvs scheduler. You can get the list of them with the `man ipvsamd` command.|
| weight | Weight of the node. By default all Cocaine nodes have weight 1.|
| port-range | Range of ports available for services binding. If not specified, services are bound to any free port of operating system choice.|

When using `ipvs`-gateway, you should be sure that `ip_vs` module is loaded into your Linux kernel. Also `port-range` option must be non empty. So an empty port range is prohibited by a configuration check in `ipvs` plugin. Creating an LVS on `0` port makes a host not accept any network connections.

##Network

Possible subsections:

```
"network": {
    "pinned": {},
    "endpoint" : "localhost:1234",
    "pool" : 10 ,
    "hostname" : "unique_name"
    "shared": [32700, 33000]
}
```

| Parameter | Description |
|-----------|-------------|
| pinned    | You can use this parameter to define ports for core services. For example, ``"pinned": {"locator": 10053}`` forces ``locator`` to listen the port number 10053.|
| endpoint  | Forces Cocaine runtime to listen specified ``host`` and ``port``.|
| pool      | Number of processing threads. |
| hostname  | The name that Cocaine uses for self identification. If ``hostname`` is not specified Cocaine tries to resolve hostname by system call. |
| shared | Range of ports which are used by services to bind. Otherwise OS selects a port |


##Services

Defaults:

```
"services": {
    "logging": {
        "type": "logging"
    },
    "storage": {
        "type": "storage"
    },
    "node": {
        "type": "node",
        "args": {
            "runlist": "default"
        }
    }
}
```

Services are apps, that start with the Cocaine runtime instead of custom apps, that run by the `node` service.

Each service in this section must have unique name.
Cocaine has the next services by default that can be configured in this section:

  *  [logging](#logging-configuration) - logger service. It can be used by custom apps and by the client to write logs.
  *  [storage](#storage-configuration) - backend. It can be used by custom apps and by the clients to store data.
  *  [node](#node-configuration) - this service is designed to run custom apps.
  
You can write you own service. How you can do this described in corresponding part of the documentation.

###Logging configuration

Service [logging]() is configured in [logging](#logging) section.

```
"logging": {
    "type": "logging",
},
```

###Storage configuration
Service `storage` can use any backend configured in [storages](#storages) section. Name of backend is designated in `backend` argument like follows:

```
"storage": {
    "type": "storage",
    "args": {
        "backend": "core"
    }
},
```
By default (when `backend` is not configured) service use `core` backend.

###Node configuration
Service `node` has the only parameter `runlist` that designates name of a list. 

Runlist is a JSON file that contains the names of apps with their profiles in such a manner of:

```
{
    "app_1": "profile",
    "app_2": "profile",
    "app_3": "another_profile",
    ...
}
```

Runlists are uploaded to the server with the command

```
cocaine-tool runlist upload --name list_name --runlist path_to_JSON
```

The commands which used to work with runlist:
  * `cocaine-tool runlist list` - shows uploaded runlists;
  * `cocaine-tool runlist view --name list_name` - shows configuration context for runlist;
  * `cocaine-tool runlist create --name list_name` - create runlist and upload it into the storage;
  * `cocaine-tool runlist remove --name list_name` - remove runlist from the storage;
  * `cocaine-tool runlist add-app --name list_name --app application_name --profile app_profile_name` - add specified application with profile to the runlist (existence of application or profile is not checked).

##Storages
This section describes parameters of backends for `storage` type of services. The Cocaine cache is also configured in this section.

Cocaine `core` is a backend, that will be used by default. After installation it configured with the type "files":

```
"core": {
    "type": "files",
    "args": {
        "path": "/var/lib/cocaine"
    }
}
```

It should present in any Cocaine configuration because it used by Cocaine core to store apps, manifests, profiles and other service information. Cocaine does not use this storage as a service. This means that if we omit `core` description in `storages` Cocaine runtime will not start, but if we omit description of `storage` in `services` section only apps than use `storage` service will fail. For example, `cocaine-tool` will not upload custom app to the server if `storage` service is not configured in `services` section.

One more Cocaine storage is `cache`. It cann't be used with the `storage` services. It configuration looks like:

```
"cache": {
    "type": "files",
    "args": {
        "path": "/var/cache/cocaine"
    }
}
```

The only parameter of `cache` is `path` to the cache data. Cache is used to store apps, manifests, profiles and other service data, and Cocaine runtime read all of this info from cache. This means that if we will try to upload some app iteratively it will not be updated until we clean the cache.

In case when you just start experiments with the Cocaine, delete this section from the configuration file if it is there. When it is not configured Cocaine doesn't use cache.

##Logging
This section contains description of logger configuration. Cocaine provides logging with ``logging`` service. Read the [description of this service](services_and_plugins/logging.md) to learn how to configure it.

Default configuration:

```
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
}
```

[Back to Contents](contents.md)
