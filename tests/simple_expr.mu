Foo: struct {
    x bool,
    y f32
}

Bar: struct {
    foo Foo,
    y u8
}

add: (x f32, y f32) f32 = x + y

// addFloat: (x f32, y f32) = add(x, y)

// sample printf foreign function
// @abi("C")
// printf: (fmt *mut u8, args ...) i32 // this function would be linked as libc's printf function
// map: [A, B](fn (A) B, [x:xs] [A]) [B] = fn(x) | map(fn, xs)
// map: [A, B](fn (A) B, [x] [A]) [B] = fn(x)
// foo: (a, b) = a + b
// foo = (a T, b U) T.Add[U] = a.+(b)
