libprojectM Public API Headers
==============================

This subproject contains (and generates) all public API headers installed by the core library. It is defined as a
header-only interface library target, so it can be used in optional components without directly linking/including
anything from the core library itself and ensure the optional libraries only use the public API functions with the
library import call specs.