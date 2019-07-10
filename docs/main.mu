Vec: struct[T, N] {
	data [T; N],
} such
	T < Arithmetic + Copy + Clone,
	N < u8

Vec: impl[T, N] {
	new: (d: &[T; N]) -> Self = Self {
		data: d
	}
}

Vec[T, N]: impl Add[Self] {
	Other = Self

	add: (self, other Self) -> Self {
	
	}
}

main: () {

}
