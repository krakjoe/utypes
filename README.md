utypes
=====

[![Build Status](https://travis-ci.org/krakjoe/utypes.svg?branch=master)](https://travis-ci.org/krakjoe/utypes)

utypes allows the user to hook into type verification at function entry (params) and exit (return) and handle the verification in user land.

Additionally, utypes allows the verification of a function against an interface (Functional Interfaces).

Hook
====

```
function utypes\handler(callable $handler) : bool;
```

Calling `utypes\handler` shall install the handler for the user, the prototype for the handler should be:

```
function (string $type, $value) : bool;
```

The handler should return `true` if execution should continue (type verification passed), or `false` if the engine should run normal verification.

Functional Interfaces
===================

```
function utypes\verify(callable $value, string $interface) : bool;
```

Should be called by handler to verify that the value conforms to the interface provided: The interface should be functional (have a single method).

Examples
=======

See tests.
