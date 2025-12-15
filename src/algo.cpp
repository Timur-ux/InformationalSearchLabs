#include "algo.hpp"

size_t len(long n) {
	if(n == 0)
		return 1;
	if(n < 0) 
		n = -n;

	size_t result = 0;
	while(n > 0) 
		++result, n /= 10;

	return result;
}

namespace IR::algo {
const char * lltostring(long n) {
	static char buffer[30];
	size_t nLen = len(n);
	if(nLen > 29 || n < 0)
		return NULL;
		
	for(size_t i = 0; i < nLen; ++i) 
		buffer[nLen - 1 - i] = n % 10 + '0', n /= 10;

	buffer[nLen] = 0;
	return buffer;
}
} // namespace IR::algo
