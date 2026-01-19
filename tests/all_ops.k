fn: u8 all_ops(a: u8, b: u8, c: u8) {
	return !(a == b) + a - b * c / (a * &b) > a >= (a < c <= b) ^^ (a != *b);
}
