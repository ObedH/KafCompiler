export var: i32 thing = 500;
var: i32 last_fib_num = 1;
var: i32 second_last_fib_num = 0;

export fn: i32 dude() {
	for(var: i32 i = 0; i < 50; i = i + 1) {
		if(i % 2 == 0) {
			return i + thing;
		}
	}
}
export fn: i32 next_fib_num() {
	var: i32 next = last_fib_num + second_last_fib_num;
	second_last_fib_num = last_fib_num;
	last_fib_num = next;
	return next;
}
