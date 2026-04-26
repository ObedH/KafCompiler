var: i32 number = 10;

fn: i32 add(a: i32, b: i32) {
	return a + b;
}

fn: void do_something() {
	add(number, number);
}

export fn: i32 not_do(a: u8) {
	return add(a, number) << 8;
}

fn: f64 bad(a: u32) {
	return 0;
}
