Foo: struct {
    pub x bool,
    y f32
}

Bar: struct {
    foo Foo,
    y u8
}

impl Foo {

}

main: () {
}

// addFloat: (x f32, y f32) = add2(x, 1)
// addFloat: (f32, f32) = add // this would be cool i guess, it would be more similar to haskell then rust/c++
// I guess if this is really desired then just make it a global; let addFloat = add

// add2: (f32) = add(2)
// I have no idea how to achieve this right now.
// add2: (f32) = (+2)

// sample printf foreign function
// @abi("C")
// printf: (fmt *mut u8, args ...) i32 // this function would be linked as libc's printf function
// map: [A, B](fn (A) B, [x:xs] [A]) [B] = fn(x) | map(fn, xs)
// map: [A, B](fn (A) B, [x] [A]) [B] = fn(x)
// foo: (a, b) = a + b
// foo = (a T, b U) T.Add[U] = a.+(b)
