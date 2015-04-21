# cocaine-http-proxy

Cocaine http proxy is a program that listens for incoming http
requests, translates them to a specific predetermined structure, and
proxies them as RPC method calls to corresponding app. It is a usual
http server from one side, and it is a usual Cocaine client from the
other side.

## requirements

The structure to pack http request is as follows.

Proxy has to pack it with msgpack and provide the resulting binary
buffer as the only argument to the `enqueue` method of the destination
app's invocation service. So, the cocaine request looks like this.

It should handle the situation when the corresponding app is not found
among the running apps, in which condition it replies with 504 http
status.

The expected request from cocaine app should be expected in form of
stream, where first chunk's data has to be a binary buffer resulting
from msgpack-encoding of the following structure. And other chunks
should be slices of body to be responded to http.

Proxy may stream body chunks as they arrive from the app, or it may
buffer them in any way it needs.

## tornado proxy

There is a complying proxy embedded into cocaine-tool. It is
pure-python, easy to run, and handy to use for development. Also it is
being used in production for certain use-cases.
Use it like this.

## native proxy

Is performing higher than python proxy. You can use and configure it
like this.



