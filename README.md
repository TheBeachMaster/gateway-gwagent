#Gateway Agent

The Gateway Agent (GA) is the element designed to make a bridge between the 
local Alljoyn bus and the Internet. It has a high level design that can be
found at the [Allseen Alliance page](
https://wiki.allseenalliance.org/gateway/gatewayagent).

This repository is organized with the following structure. There are 3 main
languages in use. One for computers, in C++, and the other two for mobiles,
specifically, iOS and Android (java).

The mobiles devices just have the Control App (using the name in the HLD),
named GatewayController in the source. On the other side, the C++ part, the
one to be working as the Gateway Agent, has the implementations for:

 - GatewayController -> Control App in the docs
 - GatewayConnector -> Connector App in the docs
 - GatewayMgmtApp -> Gateway Management App in the docs

As well as the code to create implementations of them, there are sample
implementations.

## Setting up the environment

To set up the environment, you just need to follow the Alljoyn environment
setup instructions. Taken directly from the [Allseen Alliance wiki](
https://wiki.allseenalliance.org/develop/contributing_source_code) you might
want to clone all the alljoyn tree:

```
mkdir aj-tree && cd aj-tree
repo init -u https://git.allseenalliance.org/gerrit/devtools/manifest.git
```

Once that is done, you can get into gateway/gwagent repository and run
scons.