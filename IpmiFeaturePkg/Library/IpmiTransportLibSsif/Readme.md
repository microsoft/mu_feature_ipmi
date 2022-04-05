# SSIF Transport Library

This library implements the SMBus System Interface (SSIF) logic for
communicating IPMI messages.

## Prerequisites

For this transport library to operate, it assumes the platform will provide and
implementation of the Simple SMBus library. This library is responsible for
implementing the device specifics for the BMC and for accessing the I2C device.
