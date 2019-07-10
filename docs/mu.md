# Mu Programming Language
Expression-based, multi-paradigm language with a focus on expressability
of concurrent, distributed, and sequential applications.

The primary langauge will be similar to Rust and the type classes
(traits), borrow checking, type inferencing using hindly milner. 
An end goal is to make the language such that it works well with robots.

# Main Features:
* Powerful type system. Based on Haskell similar to Rust.
* type inference,
* async/await, futures
* built-in or standard library support for message passing,
* Immutable data by default.


## Language Syntax
### Variable Declaration

```code
let x = foo()      // x is constant
mut x = foo()      // x is mutable
```

#### Pattern Matching Declaration
```code
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
```code
add: (x f32, y f32) f32 = x + y

apply: (f (f32, f32)->f32, x f32, y f32) f32 = f(x, y)
apply(add, 1.0, 2.0)
```
#### Function Attributes

```code
@foreign("printf"), abi("C")
printf: (fmt *u8, args ...)

// Attributes can be stacked (Not implemented by the parser at the moment)
@foreign("printf")
@abi("C")
printf: (fmt *u8, args ...)
```
#### Function Modifiers

A function can be a modifier before the signiture. For now, this is limited to just inlining. In this future this could
be used to implement the parallel features, but this could also be done the attributes.

```code
foo: inline () = 0.0
```

#### Variadic Parameters

Mu will support C-like variadics to allow easy interfacing with C. This will be done with '...' as a parameter type
in the function signiture. See the printf example above.

The language will also have native variadics. There are two obvious flavors of this, variadic of the same type and
variadic with different types. The syntax is shown below.

```code
// Same type variadics.

foo: (args int...) = ....

// polymorphic variadics.
foo: [T...](args T...) = ....

```


### Type Declaration

#### Structures or Product Types
```code
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
```code
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
```code
Read: trait {
    Elem: Type // this is an alias to a known type (placeholder in Rust)
    
    read fn(self, buffer *u32, len u32) u32
}

Write: trait {
    write fn(self, buffer *u32, src *u32, len u32)
}
```

A struct can subtype of a type class by the following syntax:
```code
ReadFile: struct < Read {
}

WriteFile: struct < Write {
}

File: struct < Write, Read {
    fd u32,
}
```

#### Methods

```code
File: impl {
    read: (self, buffer *u32, len u32)u32 {
        read(self.fd, buffer, len)
    }
    
    write: (self, buffer *u32, src *u32, len u32) u32 {
        write(self.id, buffer, src, len)
    }
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

add: [T](x T, y T) T = x + y

apply: [T](f (T, T)T, x T, y T) T = f(x, y)

apply[f32](add, 1.0, 1.0)
```

#### Bounds

Similarly to most modern languages with generics, the generic parameters can be bounded to some subtype. In object oriented langauges this mean the type parameter must be a subtype/subclass or instance of some interface (Java) inorder to satisfy the generic contraints. Since this language is not object oriented, the only subtyping available are traits

```
add: [T < Arithmetic](x T, y T) T {

}

// Arithmetic is a bounds for type T. When the function is instantiated the type of the type parameters will be checked with their bounds and cause a type error when the bounds are not satisfied.

// standard dyanamic array.
Vec3: struct < Arithmetic {
    x f32,
    y f32,
    z f32
}

let v1 = Vec3{1.0, 2.0, 3.0}
let v2 = Vec3{1.0, 2.0, 3.0}

add(v1, v2)

Vec: struct[T < Copy + Clone] {}

let v1 = Vec[f32].new()
let v2 = Vec[f32].new()

add(v1, v2) // this will cause a type errow because Vec[f32] doesn't satisfies Arithmetic trait.


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
            | 

<type_list_and_bounds> := <type_list_and_bounds>, <type_bounds>
                        | <type_bounds>

<type_bounds> := <id>: <bounds>

<bounds> :=  <bounds> '|' <name_type>
           | <name_type>
```

