# Ipmi Feature

This package implements core-functionality for IPMI as well as several services
built on IPMI. See [IPMI Specification 2nd Generation v2.0](https://www.intel.com/content/dam/www/public/us/en/documents/product-briefs/ipmi-second-gen-interface-spec-v2-rev1-1.pdf)
for more information.

## Purpose

The IPMI feature provides firmware functionality that implements behavior
described in the IPMI specification. IPMI enables out-of-band and monitoring
capabilities independent of the host system's CPU, firmware, and operating
system by communicating with the Baseboard Management Controller (BMC).

## Structure

The IPMI stack consists of several layers of functionality.

__Transport Library__ - This consists of the KCS and SSIF IPMI transport libraries.
These are responsible for implementing the necessary physical transport logic
for the platform. This layer does not need to understand the structure of IPMI
messages.

__Generic IPMI__ - Implements the generic IPMI in a protocol, PPI, or MM driver.
This layer is responsible for taking a high level IPMI request and building the
IPMI request and parsing the response. Generic IPMI will rely on the transport
library to handle the hardware specifics.

__IPMI Base Library__ - Supplies a library abstraction for the basic
functionality provided by the Generic IPMI component.

__IPMI Command Library__ - Implements wrappers for specific IPMI functions. This
calls into the IPMI base library.

__Functional IPMI Drivers__ - Implements functionality around IPMI. Consists of
the drivers such as BmcElog which rely on the IPMI command library.

![IPMI Stack](./Images/IpmiStack_mu.jpg)

## Extending the IPMI Command Set

Platforms may implement custom or specialized IPMI commands and functionality
that are not implemented in this repo. In this case, the platform should leverage
the IPMI base library to abstract the interface into a single command which they
can provide an opaque command and response buffer. Platforms should avoid directly
calling into the protocol/PPI as this is considered internal and may be subject
to change.
