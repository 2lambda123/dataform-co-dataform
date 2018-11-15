---
layout: documentation
title: JS API
---

Generally the easiest way to define [materializations](/guides/materializations), [assertions](/guides/assertions), and [operations](/guides/operations) is to create a new file with the appropriate extension, one of `.sql .assert.sql .ops.sql`.

For advanced use cases, all of the above can be defined Dataform's a JavaScript API.

To use the JS API, create a `.js` file anywhere in the `models/` folder of your project.

The JS API provides three primary functions that correspond to the types of objects above:
- [`materialize()`](#materialize)
- [`operate()`](#operate)
- [`assert()`](#assert)

These are regular JavaScript (ES6) files that can contain arbitrary code, for loops, functions, constants etc.

## `materialize()`

Defines a materialization.

Arguments: `materialization-name, query (optional)`

```js
// models/example.js
materialize("example")
  .type("table")
  .query("select 1 as test");
```

To use [built in functions](/built-in-functions), for some of the methods we can provide a function as an argument. The function will be called with a context object that has all the built-in functions available on it.

For example, to use the `self()` built-in as part of a [`where()`](/built-in-functions#where) method call, we pass a function to the method that uses ES5 template strings:

```js
// models/example_incremental.js
materialize("example_incremental")
  .type("incremental")
  .query("select 1 as ts")
  .where(context => `ts > (select max(ts) from ${context.self()})`);
```
## `operate()`

## `assert()`
