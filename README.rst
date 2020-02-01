|travis| |appveyor|

Simple Logging Library
======================
Known Issues:
 - Fails on Windows/VC - `init_seg` problem (used to work)

Advantages:
 - Has variable and constexpr activation level checks. The constexpr check
   compares to the compiled in activation level. It is therefor less flexible
   but will be fully optimized away in case the level is not high enough to
   trigger log output.
 - Makes sure that static initialization happens very early. The logger can
   safely be used in other classes' constructors and destructors even if those
   classes are used in a static storage context.
 - Uses log-ids that enable to relate between log output and code location.
   (Uniqueness is not enforced by this code. A tool might be added in the
   future)
 - Has extra log macros (`EXT_DEV`, `EXT_DEV_IF`, ...) for development. This
   allows for efficient removal of development artifacts.
 - Has optional support to output locations in formats understood by gdb and
   vim. (Has to be requested at compile time.)


Disadvantages:
 - Works only with supported compilers (gcc, clang, msvc).
 - Setup **MUST** happen in single threaded part of application (may change).

.. |travis| image:: https://travis-ci.org/extcpp/logging.svg?branch=master
   :target: https://travis-ci.org/extcpp/logging

.. |appveyor| image:: https://ci.appveyor.com/api/projects/status/7evseehi5ejvbgpc/branch/master?svg=true
   :target: https://ci.appveyor.com/project/obiwahn/logging
