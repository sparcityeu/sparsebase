# Feature Extraction from a Matrix Market File

## Objective
Read a matrix-market file and extract features from the data.

## Overview

In this tutorial, you will use SparseBase to do the following:

1. Read a matrix-market file.
2. Create a FeatureExtractor and add features to extract, for this tutorial we use `Degrees` and `DegreeDistribution`.
3. Extract features and collect the results.

## Preliminaries
Start by navigating to the directory `tutorials/002_feature_extraction/start_here/`. Open the file `tutorial_002.cc` using a code editor and follow along with the tutorial. The file contains some boilerplate code that includes the appropriate headers, creates some type definitions, and uses the `sparsebase` and `format` namespace.
In addition to renaming primitive types, we also define aliases for the following feature classes to improve the readability of our code.

```c++
using degrees = feature::Degrees<vertex_type, edge_type, value_type>;
using degree_dist = feature::DegreeDistribution<vertex_type, edge_type,
    value_type, feature_type>;
```

The completed tutorial can be found in `tutorials/002_feature_extraction/solved/solved.cc`. 

For this tutorial, we will use the matrix-market file `examples/data/ash958.mtx`.

## Steps

### 1. Read the graph from disk
Begin your main program by reading the matrix-market file into a `COO` object using `IOBase`.

```c++
// The name of the matrix-market file
string file_name = argv[1];

// Read the matrix in to a COO representation
COO<vertex_type, edge_type, value_type> *coo = IOBase::ReadMTXToCOO<vertex_type, edge_type, value_type>(file_name);
```

The three templated type parameters of the `COO` object and `IOBase::ReadMTXToCOO` function determine the data types that will store the IDs, the number of non-zeros, and the values of the weights of the format, respectively. 

You will find that these three template types are used by most classes of the library.

### 2. Init the Extractor and define the features to be extracted
Next, intialize the extractor and add features to be extracted (note that the class/features must extend the ExtractableType interface):

```c++
// Create an extractor with the correct types of your COO (data) and your expected feature type
sparsebase::feature::Extractor engine =
    sparsebase::feature::FeatureExtractor<vertex_type, edge_type,
        value_type, feature_type>();

//add all the feature you want to extract to the extractor
engine.Add(feature::Feature(degrees{}));
engine.Add(feature::Feature(degree_dist{}));
```
`Add` function first checks if the passed feature is actually a class that calculates more than one feature. The `get_sub_ids()` function returns the list of features that the current class computes as `Extractable` types. Then it adds these classes if they are not already added to the `FeatureExtractor`.


Print out the added features.

```c++
// print features to be extracted
auto fs = engine.GetList();
cout << endl << "Features that will be extracted: " << endl;
for (auto f : fs) {
  cout << utils::demangle(f.name()) << endl;
}
cout << endl;
```

### 3. Extract Features from the Format

Define a `context` that selects the architecture in which the computations will occur.
Then use the `Extract` function of the `FeatureExtractor` to get the features as a `map`.
The map is of type `std::unordered_map<std::type, std::any>`. The features are accessed by calling the respective `::get_id_static()` member function and using the result as a key. 
Last parameter to the `Extract` function states whether the extractor is allowed to convert input data if necessary. If not allowed and a repective function can not be found for the given format and feature, `sparsebase` will throw an exception. 

```c++
// Create a context, CPUcontext for this case.
// The contexts defines the architecture that the computation will take place in.
context::CPUContext cpu_context;
// extract features
auto raws = engine.Extract(coo, {&cpu_context}, true);
```

The extract function, if possible, merges the computation for the features by finding a class that provides the merged version of the calculations.
For this tutorial we will add `Degrees` and `DegreeDistribution` classes separately and observe that the `Degrees_DegreeDistribution` class will be used by the extractor.

After the extraction, cast the features to their raw/original type by first using the `std::type` of the feature to access it from the map. Then cast it to the correct primitive type by using `std::any_cast`.
Note that all the computations takes place using c arrays of the respective primitive type, thanks to the highly templated design of the library.

```c++
cout << "#features extracted: " << raws.size() << endl;
auto dgrs =
    std::any_cast<vertex_type *>(raws[degrees::get_id_static()]);
auto dst = std::any_cast<feature_type *>(
    raws[degree_dist::get_id_static()]);
cout << "vertex 0 => degree: " << dgrs[0] << endl;
cout << "dst[0] " << dst[0] << endl;
```

### 4. Compile the program and execute it
Compile the code using `g++`. We assume SparseBase has already been installed in the compiled setting (as opposed to header-only installation).

While in the directory `tutorials/002_feature_extraction/start_here`, execute the following commands:
```bash
g++ -std=c++17 tutorial_002.cc -lsparsebase -fopenmp -std=c++17 -o feature.out
./feature.out ../../../examples/data/ash958.mtx
```

You should see something similar to the following output:

```
[11/08/22 14:53:51] [WARNING] [sparsebase::format::COO<unsigned int, unsigned int, float>] COO arrays must be sorted. Sorting...

Features that will be extracted:
sparsebase::preprocess::DegreeDistribution<unsigned int, unsigned int, float, double>
sparsebase::preprocess::Degrees<unsigned int, unsigned int, float>

getting converter 0x60000096c018
#features extracted: 2
vertex 0 => degree: 2
dst[0] 0.00104384
```
