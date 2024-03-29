#include <string>

#include "sparsebase/config.h"
#include "sparsebase/io/reader.h"

#ifndef SPARSEBASE_PROJECT_BINARY_READER_ORDER_ONE_H
#define SPARSEBASE_PROJECT_BINARY_READER_ORDER_ONE_H

namespace sparsebase::io {

//! Reads files encoded in SparseBase's custom binary format (Array)
template <typename T>
class BinaryReaderOrderOne : public Reader, public ReadsArray<T> {
 public:
  explicit BinaryReaderOrderOne(std::string filename);
  ~BinaryReaderOrderOne() override = default;
  format::Array<T> *ReadArray() const override;

 private:
  std::string filename_;
};
}  // namespace sparsebase::io
#ifdef _HEADER_ONLY
#include "binary_reader_order_two.cc"
#endif
#endif  // SPARSEBASE_PROJECT_BINARY_READER_ORDER_ONE_H
