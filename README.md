# RetroFit

----

## What is all this stuff?

It's a game engine, some tools, and a general assortment of coding bits you need to make a game.

----

## But why though?

Firstly, it saves a lot of time and headache to point at something in a professional setting and say "Look: this can be done, here's what it looks like, and here's how it works". It's safer to attempt a new technique or coding pattern after you've already seen it applied somewhere else successfully.

Secondly, sometimes you just want to make a certain game! But there aren't many good options for high-quality C++ game engine pieces.

----

## Can I make a game with this stuff?

You *can*, but not out-of-box. You're going to have to already be an experienced gameplay or engine programmer to tie together the parts into the architecture you want. It helps to also have AAA experience so you can recognize some common patterns and machinery.

If that doesn't sound like what you're looking for, here's some more approachable game-making packages you may be interested in (in increasing complexity):

* Zelda Classic
* RPG Maker
* Unity
* Unreal Engine

----

## So what's even in this thing?

Pretty standard AAA machinery. Often with some extra improvements for common problems.
A few of the notable pieces:

### STL Wrapper

* Entire codebase uses a wrapper namespace instead of using the STL directly
* Wrapper provides patch points to repair defective STL components as needed

### Virtual File System

* Mount points and overlays
* Configuration file format supports any UNIX file/directory, even the ridiculous ones

### Reflection

Implementation split into multiple layers to allow flexibility in architecture integration:

* Tier 1: Value-type helpers
* Tier 2: Static reflection helpers
* Tier 3: Meta-info helpers
* Tier 4: Meta-info declaration machinery
* Tier 5: Run-time type database
* Tier 6: Type-traversal machinery
* Tier 7: Scripting/serialization/etc

Fixes for common engine deficiencies:

* Linker COMDAT stripping won't remove code that meta-info relies on, even in isolated files
* Linker COMDAT stripping won't remove meta-info, even in isolated files
* Can access static class info with just a forward-declaration
* Can access static class info across DLL boundaries without needing headers or run-time lookup
* Meta-info doesn't require invasive modifications to class definitions (except if you want it to do polymorphism via a virtual table)
* Container support isn't limited to a specific list, all containers are treated with equal fairness
* Container support is done at template-time, can be deferred to downstream compilation units
* Container support allows for containers of containers, without requiring a class to act as an in-between
* Container support allows for exotic containers like doubly-variant maps

### Logging

* Log4j figured out most of the best-practices over a decade ago, RetroFit's logging follows them as well

### Testing

* Yes, it has unit tests

### Build Machinery

* Doesn't require CMake, uses normal MSBuild and vcxproj files (Yes, this is actually possible, but community knowledge is still lacking, so YMMV if you try this in your own engine)
* x86 (32 & 64 bit) support
* Limited Arm (32 & 64 bit) support
* Windows support
* MSVC support
* LLVM support

### Platform Considerations

* Platform-specific headers are aggressively quarantined
* Platform-related surface area is kept as small as practically possible
* x86-specific code is avoided wherever possible

----

## What's the license?

MIT-style, super permissive, but you can't make lawsuits or advertise your usage of it as a selling point. See LICENSE file for details.

Even if you don't want the code, it's commented pretty heavily so you should be able to adapt the useful techniques to your own work.

----

## Will this become a game?

Don't hold your breath. But some fun gameplay protoypes may fall out of it.

----

## Can I contribute code or assets?

Public contributions will not be accepted at this time.
