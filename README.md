# Multitimeout
Single thread multiple timeout handler

C++ Standard: C++20 (uses \<semaphore\>)

This mini-library can handle arbitrary number of timeouts (measured in milliseconds) using a single thread, which always sleeps and wakes up only when some timeout expired.
  
Simple and easily modifyable library with thread-safety and all sleeping logic taken care of. Configure & forget.
  
Useful properties:
- Uses externally provided memory for arbitrary number of timeouts
- Single expiry timeouts or repeating timeouts
- Single thread handles all timeouts, always sleeps between timeout events
- Was originally designed to handle all TCP connection timeouts using a single thread

License: Unlicense. Do whatever you want with it.
