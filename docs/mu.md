# Mu Programming Language
Expression-based, multi-paradigm language with a focus on expressability of concurrent, distributed, and sequential applications.

The primary langauge will be similar to Rust and the type classes (traits), borrow checking, type inferencing using hindly milner. 
An end goal is to make the language such that it works well with robots.

# Main Features:
* Powerful type system. Based on Haskell similar to Rust.
* borrow checking,
* type inference,
* async/await,
* built-in or standard library support for message passing,
* Immutable data by default.


## Language Syntax
### Variable Declaration

```
let x = foo();      // x is constant
mut x = foo();      // x is mutable
```
#### Pattern Matching Declaration
```
mut (x, y) = foo(); // there is destructable types.
                    // This expects foo() to return a tuple
                    // of 2 elements where the first element
                    // is assigned to the x and the second
                    // element is assigned to the y.

let [x:xs] = mu();  // Array pattern matching. The first element
                    // of the list goes to x and the rest is assigned to xs
                    // This could be implemented as an element copy and a
                    // Slice assignment.
```

### Function Declaration
All functions are lambdas. They can be bounded to a name or used anonomously.
```
let add = (x: i32, y: i32) -> i32 {
}
```
### Type Declaration

#### Structures
```
```
#### Algebraic Data Types
```
```
#### Type Class (Type Attributes)
```
```

### Generics
The functions below are using generics. The first one is using an explicit
generics where the type T can be restricted by the programming. While
example 2, will assign type variable to both x, y, and the return. It
will use an inferencing engine (Minly-Milner) to infer the bounds on the
types. This would allow add to be even more generic then the first add, depending
on the explicit bounds on T.

```
let add = [T](x: T, y: T) -> T {
}

let add = (x, y) {
}

```

