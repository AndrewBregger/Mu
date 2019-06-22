Foo: struct {
    x bool,
    y f64
}

Bar: struct {
    foo Foo,
    y u8
}

mut x = (true, 1.0)
