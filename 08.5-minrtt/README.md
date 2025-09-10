# `08.5-minrtt`

### tl;dr

we modify the SEGGER RTT source such that it uses our simple libc replacement functions, `my_memcpy`, `my_memset` and `my_strlen`. 

this way we can continue linking without any libraries.

### from scratch

