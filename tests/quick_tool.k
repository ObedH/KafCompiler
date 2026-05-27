
fn: void write_char(val: char) {

}
fn: char u8_to_hex_char(val: u8) {
	if(val < 10) {
		return val + 0x30;
	}
	else if(val < 16){
		return val - 10 + 0x41;
	}
	else {
		return 0;
	}
}
fn: void write_var() {
	write_char('v');
	write_char('a');
	write_char('r');
}
fn: void write_type() {
	write_char(':');
	write_char(' ');
	write_char('u');
	write_char('8');
	write_char(' ');
}
fn: void write_var_name(index: u8) {
	write_char('S');
	write_char('T');
	write_char('U');
	write_char('P');
	write_char('I');
	write_char('D');
	write_char('_');
	write_char('A');
	write_char('R');
	write_char('R');
	write_char('A');
	write_char('Y');
	write_char('_');
	write_char(u8_to_hex_char((index & 0xF0) >> 4));
	write_char(u8_to_hex_char((index & 0x0F) >> 0));
	write_char(' ');
}
fn: void write_var_initializer() {
	write_char('=');
	write_char(' ');
	write_char('0');
	write_char(';');
}

export fn: void main() {
	for(var: u32 i = 0; i < 256; i = i + 1) {
		write_var();
		write_type();
		write_var_name(i);
		write_var_initializer();
	}
}
