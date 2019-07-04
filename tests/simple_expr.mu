Foo: struct {
    x bool = true,
    y f32 = 1.0
}

// Bar: struct {
//     foo Foo,
//     y u8
// }

Foo: impl {
    new: () Foo = Foo { true, 1.0 }

    get_x: (self) = self.x
    get_y: (self) = self.y

	as_ref: (self) = self
	as_mut: (mut self) = self

	clone: (self) = Foo {self.x, self.y}
}

main: () {
    // let foo = Foo.new()
    let y = Foo.new()
    let x = foo.get_x()
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
