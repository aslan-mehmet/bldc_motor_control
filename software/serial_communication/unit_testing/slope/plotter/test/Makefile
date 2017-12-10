pre:
	gcc -E preprocess.c
# feature test macro
# http://man7.org/linux/man-pages/man7/feature_test_macros.7.html
test:
	gcc -E -D_POSIX_C_SOURCE=199309L preprocess.c
# libc version
ver:
	ldd --version
sleep:
	gcc -D_POSIX_C_SOURCE=199309L sleep.c -o sleep

	
