#include <string>

#include "sparsebase/config.h"
#include "sparsebase/io/writer.h"

#ifndef SPARSEBASE_PROJECT_MTX_WRITER_H
#define SPARSEBASE_PROJECT_MTX_WRITER_H

namespace sparsebase::io {

//! Writer for the Matrix Market File Format
/*!
 * Detailed explanations of the MTX format can be found in these links:
 * - https://networkrepository.com/mtx-matrix-market-format.html
 * - https://math.nist.gov/MatrixMarket/formats.html
 */
template <typename IDType, typename NNZType, typename ValueType>
class MTXWriter : public Writer,
                             public WritesCOO<IDType, NNZType, ValueType>,
                             public WritesCSR<IDType, NNZType, ValueType> {
 public:
  explicit MTXWriter(std::string filename);
  ~MTXWriter() override = default;
  void WriteCOO(format::COO<IDType, NNZType, ValueType> *coo) const override;

 private:
  std::string filename_;
};

}  // namespace sparsebase::io
#ifdef _HEADER_ONLY
#include "mtx_writer.cc"
#endif
#endif  // SPARSEBASE_PROJECT_MTX_WRITER_H
