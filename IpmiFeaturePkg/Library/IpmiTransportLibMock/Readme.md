# Mock IPMI Transport Library

The mock IPMI transport library is intended to be used to leverage the IPMI stack
but without a real BMC providing the IPMI services. Currently, this means that
the mock library will attempt to return a good but meaningless reply for most
messages. This is useful for platform bring-up and testing new features in the
IPMI feature package that may not have hardware support yet.

In the future this can be leveraged further for testing various failure
scenarios.
