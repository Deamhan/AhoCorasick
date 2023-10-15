# Introducing
**AhoCorasick** is a flexible header only implementation of Aho-Corasick pattern searching algorithm (see https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm) that can be build for a great variety of platforms (*cmake* and *C++ 14* compatible compiler is required).

## License
Implementation is distributed under the **MPL v2.0** License. See [LICENSE](http://mozilla.org/MPL/2.0/) for details.

## Implementation interface
The implementation interface is very simple: there is a *AhoCorasick::Scanner* template class you need to create, push patterns to find and a callback to receive results :-) The library can be used for serch text, byte chains, even custom objects sequences.
Apart from that the implementation can be perfomance efficient (parent to child access with *O(1)*) or memory efficient biased (parent to child access with *O(log(n))*) by use of *PerformanceStrategy* template parameter of *Scanner*.

## How to build
Just generate project you want using *CMake* and enjoy :-)

## How to run tests
The tests provided can be run manually of using *CTest* (*CMake* provided test driver utility).
