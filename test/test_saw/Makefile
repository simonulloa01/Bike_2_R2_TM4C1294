all: prove

prove: bike2_test.bc
	saw bike2_test.saw

# Be sure to have the .bc file, the command to generate it is: 
# clang-8 -emit-llvm -c bike2_test.c -o bike2_test.bc
# Beware that the clang version may be different, check your 
# version and run accordingly. 
%ref.bc: %ref.c
	clang-8 $< -o $@ -c -emit-llvm -O0 -std=c90

clean:
	rm -rf bike2_test.bc

.PHONY: all clean prove