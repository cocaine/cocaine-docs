Unfortunately possibilities of apps monitoring are not so big.

You can test if an app is running with the [`cocaine-tool`](http://cocaine-tools.readthedocs.org/en/latest/tools.html). To do it use the command

```
cocaine-tool info
```

There you will see a list of app-services ready to take a load. State of app shoud be "running". If state field has another value it is a signal that something goes wrong. What is wrong you will see in Cocaine logs. To understand where you can find the logs read the Cocaine [logging configuration](https://github.com/cocaine/cocaine-docs/blob/v0.11/doc/maintenance_server_configuration.md#logging).

In case of app is terminated correctly with an error, you can work with its crashlog using a set of commands

```
cocaine-tool crashlog <cmd>
```

Also app can write logs. To do this it should use `logging` service. App logs will be near the Cocaine logs.