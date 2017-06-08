darkmacro: darkmacro.c
	clang $^ -o $@ -I../realtalk/src -L../realtalk -lrealtalk
