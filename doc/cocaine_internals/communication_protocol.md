#Cocaine communicaton protocol

Cocaine use this protocol for all of its communication processes between all services and between clients and services.

Cocaine use TCP/IP as a transport. Any Cocaine service can connect to another service and send/receive messages generated with ``msgpack``.

Each communication transaction has ID, that we named ``channel ID``.

Each message has it's type and message specific rules for one channel (transaction).

Protocol of each service contain description of all it's messages and all transactions with them.

##Message structure
Cocaine uses ``msgpack``.

Cocaine's ``msgpack`` message has the following structure:

```
[channel ID (int), message type ID (int), [message specific arguments], message specific data... ]
```

|Field|Description|
|-----------|-----------|
|``channel ID``|  Each physical connection provides multiple logical channels. Use this ID to split these channels. Each communication transaction should be processed in a separate channel. |
|``message type ID``| Each message type has its name (``chunk``, ``write`` ...) which has corresponding ID in RPC table. RPC table for the service you can get from ``Locator``. |
|``message specific arguments``| Any message specific agruments.|
|``message specific data``| Any message data.|

Developer of a service may specify any arguments and data for each message type.

##Communication trasactions

Communication transactions of Cocaine protocol has the following form:

```
{ message_type_1:[transaction description], message_type_2:[transaction description], ...}
```

It's like a JSON object. ``Locator`` returns description of any service protocol in this form.

Keys are the numbers of methods in service dispatch table.

Transaction description consists of 3 parts:
  * Message name. It is just a human readable name of message with message_type_ID that service waits at the beginning of transaction;
  * Messages that service waits after the first message;
  * Messages that service sends.

When you develop a protocol for your service you should write it in described form. If you cann't do this Cocaine will not support your communications.

Let's consider some examples of protocol descriptions to understand how to write the protocol.

### Reading the protocol of ``Storage`` service

!!!!!!!!!!!!!!! Вставить сюда реальный пример для Storage. Команду в локатор и полную строку.!!!!!!!

For example, when you resolve ``Storage`` service ``Locator`` returns object of the following form:

```
{0:["read",{},{0:["value",{}],1:["error",{}]}], 1:["another handle", ...], ...}
```

We see that transaction for message 0 ("read") is the following array:

```
["read",{},{0:["value",{}],1:["error",{}]}]
```

  * ``read`` is the name of the first message in transaction.
  * ``{}`` is a section of messages that ``Storage`` waits after ``read``. We see no messages but the braces shows us that we can use the same syntax as whole transaction uses. ``{}`` is an empty subtransaction with the same ``channel ID`` as the main one.
  * ``{0:["value",{}],1:["error",{}]}`` is the subtransaction object that describes messages which ``Storage`` will send as an answer on ``read``. ``Storage`` should use one of these messages obligatory and we see that it will not wait answer for any of them. These messages, ``value`` and ``error``, are terminating.

We can simplify above description with the following words: app that sends ``read`` message to ``Storage`` waits for ``value`` or ``error`` messages as a reply. 

All of the messages from one transaction description should have the same ``channel ID``. If app sends ``read`` to ``Storage`` it waits ``value`` or ``error`` at the same channel. 

Each ``read`` should be in a separate channel.

### Reading the protocol of ``App`` service.

Let's look at more complicated example. Consider ``enqueue`` transaction of ``App``:

```
{0:["enqueue", {0:["write",None],1:["error",{}],2:["close",{}]}, {0:["write",None],1:["error",{}],2:["close",{}]}]}
```

In this example we see a new type of definition ``0:["write",None]``. This is a cycle. Service or an app can send ``write`` as much times as it need. It is a good way to implement streaming of data.

Rules for request and for answer messages are the same ``{0:["write",None],1:["error",{}],2:["close",{}]}``. ``App`` client can send ``write`` multiple times and stop streaming with ``error`` or ``close``. ``App`` service can do the same.

The whole picture of ``enqueue`` protocol is follows:

If ``App`` gets ``enqueue`` message it waits for any number of ``write`` messages and can send any number of ``write`` messages itself in the same channel. If ``App`` wants to stop its streaming it should use "error" or ``close`` messages. Application from opposite side of connection should do the same if it wants to stop streaming.


##Differences from the v0.11 version
Protocol in v0.11 and earlier versions of Cocaine provided only 3 types of messages: ``chunk``, ``choke`` and ``error``. And it had strictly rules for these messages. 

In v0.12 protocol of services and other Cocain components can contain any type of messages. Protocol allows to describe rules of message communications and provides clear and strict description of these rules. With this new protocol you can provide streaming of a data between services and their clients in a natural way. 

Messages structure are differ between versions and they are not compatible.
