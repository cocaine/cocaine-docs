
# Running NodeJS Application in Cocaine

This tutorial shows how run your nodejs application using vagrant
environment.

## Tweak your app as needed

Let's consider a simple example. For full details what actions are
needed and why see [nodejs-quick-start.md]

Let's say we want to run a simple http app like this:
```
var http = require("http")

var Server = new http.Server(function(rq, rs){
  rq.on("data", function(data){
    console.log(data)
  })
  rq.on("end", function(){
    rs.end("hi")
  })
  rq.on("error", function(err){
    console.log("error in request", rq, err)
  })
})
Server.listen(8080)
```

We add the following strings:
```
var cocaine = ...
```

## Install vagrant
```
git clone ...
cd cocaine-vagrant
vagrant up
```

## Push app to the image

```
git remote add ssh://ape@localhost:2222/app-name ape
```

The next string will push your code to vagrant image and build docker
image.
```
git push ape master
```

You see the output of the build process. If there are any errors, correct
them, and push again.

Finally, when the build succeds, you see something like 


## Start and test your app

```
ssh ape@localhost -p 2222 start ape-name master
```

```
ssh ape@localhost -p 2222 status ape-name master
```

The output should look like this

Now, since the port 8080 of vagrant image maps to 48080 on the
localhost, we can reach our app like this:

```
curl -v http://localhost:48080/app-name/incoming-event-name/
>
>
>
<
<
<
Hi
```

