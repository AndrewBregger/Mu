// Foo: struct {
//     x bool,
//     y f32
// }
//
// Bar: struct {
//     foo Foo,
//     y u8
// }

main: (x f32, y f32) = <>

// map: [A, B](fn (A) B, [x:xs] [A]) [B] = fn(x) | map(fn, xs)
// map: [A, B](fn (A) B, [x] [A]) [B] = fn(x)
// foo: (a, b) = a + b
// foo = (a T, b U) T.Add[U] = a.+(b)
