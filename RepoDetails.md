# Project MU IPMI Feature

??? info "Git Details"
    Repository Url: {{mu_feature_ipmi.url}}
    Branch:         {{mu_feature_ipmi.branch}}
    Commit:         [{{mu_feature_ipmi.commit}}]({{mu_feature_ipmi.commitlink}})
    Commit Date:    {{mu_feature_ipmi.date}}

The repo contains the MU implementation for the IPMI feature package based on
TianoCore code. This code should be consumed as needed for IPMI feature support.

## Repository Philosophy

Unlike other Project MU repositories, the IPMI feature repo does not strictly
follow the EDKII releases, but rather has a continuous main branch which will
periodically receive cherry-picks of needed changes from EDKII. For stable
builds, release tags will be used instead to determine commit hashes at stable
points in development. Release branches may be created as needed to facilitate a
specific release with needed features, but this should be avoided.

## Consuming the IPMI Feature Package

Since this project does not follow the release fork model, the code should be
consumed from a release hash and should be consumed as a extdep in the platform
repo. To include, create a file named feature_ipmi_ext_dep.json desired release
tag hash. This could be in the root of the project or in a subdirectory as
desired.

    {
      "scope": "global",
      "type": "git",
      "name": "FEATURE_IPMI",
      "var_name": "FEATURE_IPMI_PATH",
      "source": "https://github.com/microsoft/mu_feature_ipmi.git",
      "version": "<RELEASE HASH>",
      "flags": ["set_build_var"]
    }

Setting the the var_name and the set_build_var flags will allow the build scripts
to reference the extdep location. To make sure that the package is discoverable
for the build, the following line should also be added to the build
configurations GetPackagesPath list.

    shell_environment.GetBuildVars().GetValue("FEATURE_IPMI_PATH", "")

_Note: If using pytool extensions older then version 0.17.0 you will need to
append the root path to the build variable string._

After this the package should be discoverable to can be used in the build like
any other dependency.

## More Info

Please see the Project Mu docs (<https://github.com/Microsoft/mu>) for more
information.  

This project has adopted the [Microsoft Open Source Code of
Conduct](https://opensource.microsoft.com/codeofconduct/).

For more information see the [Code of Conduct
FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact
[opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional
questions or comments.


## Issues

Please open any issues in the Project Mu GitHub tracker. [More
Details](https://microsoft.github.io/mu/How/contributing/)

## Contributing Code or Docs

Please follow the general Project Mu Pull Request process.  [More
Details](https://microsoft.github.io/mu/How/contributing/)

* [Code Requirements](https://microsoft.github.io/mu/CodeDevelopment/requirements/)
* [Doc Requirements](https://microsoft.github.io/mu/DeveloperDocs/requirements/)

## Builds

Please follow the steps in the Project Mu docs to build for CI and local
testing. [More Details](https://microsoft.github.io/mu/CodeDevelopment/compile/)

## Copyright & License

Copyright (C) Microsoft Corporation  
SPDX-License-Identifier: BSD-2-Clause-Patent
