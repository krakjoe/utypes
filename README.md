utypes
=====

utypes allows the user to hook into type verification at function entry (params) and exit (return) and handle the verification in user land.

API
===

```
function utypes(callable $handler) : void;
```

Calling `utypes` shall install the handler for the user, the prototype should be:

```
function (string $type, $value) : bool;
```

The `$handler` should return `true` if execution should continue, else should return `false` if the engine should run normal verification.

Examples
=======

See tests.
