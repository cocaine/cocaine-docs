# Actor Model in Cocaine

Actor model assumes that every item of a system is an actor.

According to the model, actors have following properties.

1. Actor has an address.
1. Actor can spawn another actor
1. Actor can send and receive messages. Actor sends messages only to actors, whose address it knows.

More about Actor model you can read in [wiki](http://en.wikipedia.org/wiki/Actor_model).

## Cocaine Actors

Cocaine doesn't bring all of Actors properties into life. In Cocaine terms, service is an Actor.

Service has an address. It is called an endpoint and it can be a tcp address or a unix socket pathname.

Service can spawn another service.

Service can receive messages. It happens as follows.

Client connects to Service's endpoint. Client and Service exchange framed messages using the socket.

Apps are actors too. Any running app has its certain endpoint and anyone can send messages to it.

In the ["Services and Apps"](general_info_services_and_apps.md) part of the document you can read about difference between them.



[Back to Contents](contents.md)
