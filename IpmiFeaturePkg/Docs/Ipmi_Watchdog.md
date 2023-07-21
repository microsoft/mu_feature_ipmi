# IPMI Watchdog Timers

The IPMI watchdog timers that are typically used are the FRB2 and OS timers. The
FRB2 timer is used for UEFI boot and is typically configured during PEI and torn
down at _ready to boot_ in the BDS phase. The OS watchdog timer will be configured
at _exit boot services_ and will be handed off to the OS to maintain.

The IPMI feature package provides two methods for configuring and controlling the
watchdog timers. The [watchdog lib](../Include/Library/IpmiWatchdogLib.h) and
the [Watchdog modules](../IpmiWatchdog/). The library provides the ability to
generically configure, stop, and start a given watchdog timer while the module
will do this automatically based on a platform provided policy. Platforms that need
strong control over the watchdog configuration, either for a particular timer
or all may use the library directly. The library and module may be used in tandem,
but it may not be desirable to use both the library and module in the same phase
to avoid unexpected behavior or timing issues.

The PEI implementation of the watchdog timer will configure the FRB2 timer, if
enabled, at its entry during PEI. The DXE implementation will check the timer
state on entry and set up a callback for _ReadyToBoot_ to disable the FRB2 timer
and _ExitBootServices_ to enable the OS watchdog timer. Because the OS watchdog
timer is based on dynamic policy, this configuration can be dynamically updated at
boot time by any platform component so long as it's done before _ExitBootServices_.

The configuration policy for watchdog can be found in the [Watchdog Policy Header](../Include/Guid/IpmiWatchdogPolicy.h).
The platform will be responsible for publishing their appropriate policy in either
PEI or DXE depending on the timers and phases used. For details on using the policy
service, see the [Policy Service Readme](https://github.com/microsoft/mu_basecore/blob/release/202302/PolicyServicePkg/README.md).
An example of publishing such a policy is provided below.

```C
  IPMI_WATCHDOG_POLICY  Policy;
  UINT64                Attributes;
  EFI_STATUS            Status;

  ZeroMem (&Policy, sizeof (Policy));

  Policy.Frb2Enabled        = TRUE;
  Policy.Frb2TimeoutAction  = 0;
  Policy.Frb2TimeoutSeconds = 600;
  Policy.OsWatchdogEnabled  = TRUE;
  Policy.OsTimeoutAction    = 0;
  Policy.OsTimeoutSeconds   = 600;
  Attributes = POLICY_ATTRIBUTE_FINALIZED;

  Status = SetPolicy (&gIpmiWatchdogPolicyGuid, Attributes, &Policy, sizeof (Policy));
```
