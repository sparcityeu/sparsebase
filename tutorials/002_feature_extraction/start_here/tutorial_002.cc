#include "sparsebase/feature/feature_extractor.h"
#include "sparsebase/feature/degrees.h"
#include "sparsebase/feature/degree_distribution.h"
#include "sparsebase/format/format.h"
#include "sparsebase/bases/iobase.h"
#include <iostream>

using namespace std;
using namespace sparsebase;
using namespace bases;
using namespace format;

using vertex_type = unsigned int;
using edge_type = unsigned int;
using value_type = float;
using feature_type = double;

using degrees = feature::Degrees<vertex_type, edge_type, value_type>;
using degree_dist = feature::DegreeDistribution<vertex_type, edge_type,
    value_type, feature_type>;

int main(int argc, char * argv[]){

  if (argc < 2) {
    cout << "Usage: ./sparse_feature <matrix_market_format>\n";
    cout << "Hint: You can use the matrix market file: "
            "examples/data/ash958.mtx\n";
    return 1;
  }

  /////// YOUR CODE GOES HERE ///////

  return 0;
}
