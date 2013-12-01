#Cocaine installation
In this tutorial we will consider basic installation of the Cocaine server and its tools under Ubuntu 12.04. After completing it you will have basic environment for custom app development and running developed apps under Cocaine.

The first thing that you need is to get sources from the github.

```
#This repository contains cocaine-runtime server
git clone https://github.com/cocaine/cocaine-core.git -b v0.11
git submodule update --init

#Here is the console command that required to install custom applications into the cloud
git clone https://github.com/cocaine/cocaine-tools.git -b v0.11

#These repositories contain frameworks that you will use to create apps
#install only that what you need
#C++
git clone https://github.com/cocaine/cocaine-framework-native.git -b v0.11
#Python
git clone https://github.com/cocaine/cocaine-framework-python.git -b v0.11
#Java
git clone https://github.com/cocaine/cocaine-framework-java.git -b v0.11
```

You don't need to get sources of `Go` and `Node.js` frameworks locally due to the platform's specific.

The next step is to compile projects and install them.

```
sudo aptitude install equivs devscripts
sudo aptitude install python-pip
pip install setuptools

cd cocaine-core
sudo mk-build-deps -ir
sudo aptitude install libssl-dev
debuild -sa
cd ..
sudo dpkg -i cocaine-dbg_*_amd64.deb cocaine-runtime_*_amd64.deb libcocaine-core2_*_amd64.deb libcocaine-dev_*_amd64.deb

cd cocaine-tools
python setup.py install

cd cocaine-framework-native
debuild -sa
cd ..
sudo dpkg -i cocaine-framework-native_*_amd64.deb cocaine-framework-native-dbg_*_amd64.deb
cocaine-framework-native-dev_*_amd64.deb

cd cocaine-framework-python
python setup.py install
cd ..

cd cocaine-framework-java
mvn package
cocaine-core/target/cocaine-core-0.10.5-1.jar
cocaine-services/target/cocaine-services-0.10.5-1.jar
 
mvn source:jar
cocaine-core/target/cocaine-core-0.10.5-1-sources.jar
cocaine-services/target/cocaine-services-0.10.5-1-sources.jar
```

Cocaine-runtime installed as a service and can be managed with the `service` command for start/stop operations. For example, you can check status of service with the command

```
$service cocaine-runtime status
```

If you want to start it manually use the command

```
$cocaine-runtime -c /etc/cocaine/cocaine-default.cfg
```

If you need to build Cocaine components from sourced use corresponding [guide](maintanace_cocaine_deployment.md).

Description of `cocaine-runtime` configuration parameters you can find in [maintenance section](maintanance_server_configuration.md).

While experimenting with Cocain it is usefull to toggle off caching. It can be done by deleting of `cache` configuration from the `storages` section of the configuration file.

Now you can create your applications and run them under Cocaine. To do this, check tutorials:
    * [Python](https://github.com/cocaine/cocaine-framework-python/wiki/Echo-application-example)
    * [Node.js](https://github.com/cocaine/cocaine-framework-nodejs/blob/development/README.md)
    * [C++](tutorial_cplusplus.md)
    * Go
    * Java

[Back to Contents](contents.md)
