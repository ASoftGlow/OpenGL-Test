#pragma once

int mod(int k, int n) {
	return ((k %= n) < 0) ? k + n : k;
}
int mod(char k, char n) {
	return ((k %= n) < 0) ? k + n : k;
}
int mod(char k, int n) {
	return ((k %= n) < 0) ? k + n : k;
}