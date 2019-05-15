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
let x = foo()      // x is constant
mut x = foo()      // x is mutable
```
#### Pattern Matching Declaration
```
mut (x, y) = foo()  // there is destructable types.
                    // This expects foo() to return a tuple
                    // of 2 elements where the first element
                    // is assigned to the x and the second
                    // element is assigned to the y.

let [x:xs] = mu()   // Array pattern matching. The first element
                    // of the list goes to x and the rest is assigned to xs
                    // This could be implemented as an element copy and a
                    // Slice assignment.
```

### Function Declaration
All functions are lambdas. They can be bounded to a name or used anonomously.
```
add: (x f32, y f32) f32 = x + y

apply: (f (f32, f32)f32, x f32, y f32) f32 = f(x, y)

apply(add, 1.0, 2.0)
```
### Type Declaration

#### Structures or Product Types
```
Vector: struct {
    x f32,
    y f32
}


// This is more traditional procuct types syntax

Vector: struct {f32, f32}

// The fields can be accessed like a tuple
let vec: Vector {1.0, 2.0}
vec.0 // 1.0
vec.1 // 2.0


```

#### Enumerations or Sum Types
```
Expr: type {
    Name(string),
    IntConstant(i32),
    FloatConstant(f32),
    StringConstant(string),
    Unary(Op, *Expr),
    Binary(Op, *Expr, *Expr),
}
```

#### Type Class (Type Attributes)
```
Read: trait {
    read fn(self, buffer *u32, len u32) u32
}

Write: trait {
    write fn(self, buffer *u32, src *u32, len u32)
}
```

A struct can subtype of a type class by the following syntax:

```
ReadFile: struct < Read {
}

read: (self ReadFile, buffer* u32, len u32) u32 {
    ...
}

WriteFile: struct < Write {
}

write: (self WriteFile, buffer *u32, src *u32, len u32) u32 {
    ...
}

File: struct < Write, Read {
    fd u32,
}

read: (self ReadFile, buffer* u32, len u32) u32 {
    read(self.fd, buffer, len)
}

write: (self WriteFile, buffer *u32, src *u32, len u32) u32 {
    write(self.id, buffer, src, len)
}
```

### Generics
The functions below are using generics. The first one is using an explicit
generics where the type T can be restricted by the programming. While
example 2, will assign type variable to both x, y, and the return. It
will use an inferencing engine (Minly-Milner) to infer the bounds on the
types. This would allow add to be even more generic then the first add, depending
on the explicit bounds on T.

```
add: [T](x T, y T) T {
}

add: (x, y) {
}

```

### Types
The types can have references and pointers. By default, parameters are constant and can be mutable by adding the 'mut' keyword to the type signature.

Type Name | Syntax
--- | ---
Pointer | *T
Reference| &T
Tuple | (T1, T2,...)
Mutable | mut
Name | Identifier

Baukus Naur Form for type specification.
``` bnf
<type> :=  * <type>
         | & <type>
         | mut <type>
         | <name_type>
 
<name_type> := <identifier> <generics>

<generics> := [<type_list_and_bounds>]

<type_list_and_bounds> := <type_list_and_bounds>, <type_bounds>
                        | <type_bounds>

<type_bounds> := <id>: <bounds>

<bounds> :=  <bounds> '|' <name_type>
           | <name_type>
```

