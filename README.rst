####
 vgl
####
----
 "Verbatim GL" - mock STL for intros
----

.. contents::
    :depth: 2

Abstract
====

Originally, when writing **Junamatkailuintro** [ref1]_, I was thinking there was necessarily no need to do everything the absolutely hardest way possible even if trying to write small intros. In the source folder of that project, there are files in the form ``verbatim_xyz.hpp`` - these are a primitive attempt at bringing some of the conveniences of C++ programming without having to worry about C++ linkage.

This project is simply an evolution of those attempts, but in a separate repository as opposed to ``C-c`` and ``C-v`` of source. It aims to provide some programming convenience while only linking against **libc**, and tries to be as small as possible - at the expense of error checking of course.

There's some threading and graphics stuff, which is where the name comes from. They require **GL** and **SDL** respectively, but if you're interested about those, it's better to take a look at **dnload** [ref2]_.

Requirements
====

The compiler must support C++17. This means:

* Clang 5 or later
* GCC 7 or later
* MSVC 19.14 (Visual Studio 2017 with updates should be fine).

Usage
====

Clone the repository under your source folder as a subrepository so it becomes a folder named ``vgl/``.

Then, instead of, saying for example:

    #include <optional>

    using std::optional;
    using std::nullopt;

Instead say:

    #include "vgl/vgl_optional.hpp"

    using vgl::optional;
    using vgl::nullopt;

Everything else works pretty much the same way. There's also ``test.cpp`` that can be taken a look at.

Implemented concepts
----

**TODO:** write this instead of writing the code

TODO
====

* Finish importing stuff from older projects and modernize it.
* [DOCS] Write actual documentation.

References
====

.. [ref1] http://faemiyah.fi/demoscene/junamatkailuintro
.. [ref2] http://faemiyah.fi/demoscene/dnload
