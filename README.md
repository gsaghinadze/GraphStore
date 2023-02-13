# GraphStore

The C++ library that handles the labelled directed unweighted graph and provides the following functionality:
* Create vertex in the Graph Store
* Create edges between vertices 
* Add/Remove label to/from the vertex
* Calculate the shortest path between vertices.

## Dependencies

* CMake 3.16 or better
* C++17 or higher

# Build
``` bash
mkdir build
cd build
cmake ..
make
```

# Test
``` bash
cd build && make test
```

# Documentation
To generate the documentation, you need to have [Doxygen](https://www.doxygen.nl/) installed. Run doxygen in the root folder to generate the doumentation. Documentation will be created in the /docs folder.
``` bash
doxygen Doxyfile
```

