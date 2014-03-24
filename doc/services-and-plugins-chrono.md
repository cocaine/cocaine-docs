#Service Chrono

##Description

This service allows apps to get timer events.

##Handles

|**Handle**|**Description**|
|----------|---------------|
|[notify_after](#notify_after)|Set notification after specified time.|
|[notify_every](#notify_every)|Set periodical notifications.|
|[cancel](#cancel)|Cancel active timer.|
|[restart](#restart)|Restart active timer time counter.|

###notify_after
Notifies you after time you specified.

```
notify_after(time_to_wait, id_flag)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|time_to_wait|Time in seconds to wait.|
|id_flag|If not specified or false, timer_id will not be returned immediately in the first chunk. It will be passed when event occurs.|

**Return value**

Timer identificator on each event call.

###notify_every
Notifies you every time period you specified.

```
notify_every(period, id_flag)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|period|Period in seconds.|
|id_flag|If not specified or false, timer_id will not be returned immediately in the first chunk. It will be passed when event occurs.|

**Return value**

Timer identificator on each event call.

###cancel
Cancels active timer.

```
cancel(timer_id)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|timer_id|Timer identificator.|

###restart
Restarts active timer time counter.

```
restart(timer_id)
```

**Parameters**

|**Parameter**|**Description**|
|-------------|---------------|
|timer_id|Timer identificator.|

##Configuration
Service should be configured in [services section](maintenance_server_configuration.md#services) of Cocaine configuration file as follows:

```javascript
"chrono" : {
	"type" : "chrono"
}
```
