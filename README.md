# Multitimeout
Single thread multiple timeout handler

C++ Standard: C++20 (uses <semaphore>)

This mini-library can handle arbitrary number of timeouts (measured in milliseconds) using a single thread, which always sleeps and wakes up only when some timeout expired.
  
Simple and easily modifyable library with thread-safety and all sleeping logic taken care of. Configure & forget.
  
Useful properties:
- Has a hardcoded number of timeouts, but it's easily modifyable to support any reasonable quantity
- Single expiry timeouts or repeating timeouts
- Single thread handles all timeouts, always sleeps between timeout events
