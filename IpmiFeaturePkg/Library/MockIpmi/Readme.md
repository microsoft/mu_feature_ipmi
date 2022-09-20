# Mock IPMI Library

The mock IPMI library is intended to be used to leverage the IPMI stack but
without a real BMC providing the IPMI services. Currently, this means that the
mock library will attempt to return a valid but often useless reply for most
messages. This is useful for platform bring-up and testing new features in the
IPMI feature package that may not have hardware support yet. The base library
implementation of Mock IPMI is intended for use in host based unit tests of
higher level functionality by allowing IPMI functionality without requiring the
generic IPMI UEFI modules.

To add new functionality to the mock IPMI library, handlers should be created
and added to the MockHandlers array at the top of [MockIpmi.c](./MockIpmi.c).
The mock handlers will be provided in command data and the response data buffers.
Failures should be reflected in the returned CompletionCode and the response
size should always be set to the size of the returned structure.
