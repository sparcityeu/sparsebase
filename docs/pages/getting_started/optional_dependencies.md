# Optional Dependencies

Optional dependencies in the context of sparsebase are libraries that for one reason 
or another can not be directly bundled with sparsebase. 

## Optional Dependency List

- [METIS](https://github.com/KarypisLab/METIS)
  - If your code is compiled without CMake, make sure to link against metis.  
- [Rabbit order](https://github.com/araij/rabbit_order)
  - If your code is compiled without CMake, make sure to link against [numa](https://man7.org/linux/man-pages/man3/numa.3.html).
- [PULP](https://github.com/HPCGraphAnalysis/PuLP)
  - If your code is compiled without CMake, make sure to link against pulp.
- [PATOH](https://faculty.cc.gatech.edu/~umit/software.html)
  - If your code is compiled without CMake, make sure to link against patoh.

## Compiling With An Optional Dependency

To enable an optional dependency the user must either pass `-DUSE_<dependency>=ON` parameter to cmake, 
or set it afterwards in the `CMakeCache.txt` file and rerun cmake.

Once this is done cmake will automatically search for the library. It will look at the following locations by default:
- User defined `CMAKE_PREFIX_PATH` variable.
- System defined `CMAKE_SYSTEM_PREFIX_PATH` variable.

If the library is located in a usual place (like `/usr/local` in Linux) then this should be sufficient.
However, if cmake fails to find the library the following options could also be set:
- `<dependency>_LIB_DIR` : Path to the directory containing the `.so`, `.a`, `.dll` file.
- `<dependency>_INC_DIR` : Path to the directory containing the header files.

```{warning}
When passing paths to cmake, we suggest using absolute paths and avoiding symbols like `~` and `*`
```

These are defined both as cmake variables and environment variables, 
meaning one can set these in `CMakeCache.txt` or using `export` UNIX command.

Example for METIS using environment variables:
```bash
mkdir build && cd build
export METIS_LIB_DIR=/home/user/lib/metis/lib
export METIS_INC_DIR=/home/user/lib/metis/include
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_METIS=ON
```

Example for METIS using cmake variables:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_METIS=ON \
  -DMETIS_LIB_DIR=/home/user/lib/metis/lib -DMETIS_INC_DIR=/home/user/lib/metis/include
```


CMake will search in the following order:
- CMake variables.
- Environment variables.
- User defined prefix.
- System defined prefix.

As a result of this, the environment and cmake variables can be used to shadow a default installation.

```{warning}
Static libraries (.a) can cause issues in the linking stage due to name collisions. 
For example, we observed this behaviour with METIS and PATOH when they are used together.
As a result, we suggest using shared libraries (.so) whenever possible.
In the aforementioned example, the problem disappears when METIS is compiled with `shared=1`
```