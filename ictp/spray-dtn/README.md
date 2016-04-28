DTN for Contiki
===============

This is a Delay-Tolerant Networking implementation for the Contiki operating system.

Summary
-------
This is a Contiki module available to use in wireless device applications. It provides an interface for application to easily establish a Delay-Tolerant Network and send messages from one node to another.

Install
-------
- Download and copy the `dtn.h` and `dtn.c` into the source code directory of your Contiki application. You can put them in a sub-folder under your souce code directory, e.g. `myapp/src/dtn/`, if you like to.
- Include the module's header file `dtn.h` in your source code where you want to use Delay-Tolerant Networking, i.e. `#include "dtn.h"`. If you put the module under a sub-folder, change the path accordingly.
- Add `dtn.c` into the variable `CONTIKI_SOURCEFILES` in your application's Makefile: `CONTIKI_SOURCEFILES += dtn.c`. Change the path if you use a different one.
- If you have multiple projects using the module, you can consider putting the module in the Contiki's source directory, and add `dtn.c` into the variable `CONTIKI_SOURCEFILES` in Contiki's Makefile `$(CONTIKI)/Makefile.include`.
- Use the module in your application.

Usage
-----
- Refer to the [example application](example-dtn.c) about how to use the module.
- Refer to the [header file](dtn.h) about details of the module's interface.
