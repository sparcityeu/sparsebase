#include "sparsebase/io/tns_reader.h"

#include <sstream>
#include <string>

#include "sparsebase/config.h"

namespace sparsebase::io {

template <typename IDType, typename NNZType, typename ValueType>
TNSReader<IDType, NNZType, ValueType>::TNSReader(std::string filename,
                                                 bool convert_to_zero_index)
    : filename_(filename), convert_to_zero_index_(convert_to_zero_index) {
  std::ifstream fin(filename_);

  if (!fin.is_open())
    throw utils::ReaderException("Wrong matrix market file name\n");
}

template <typename IDType, typename NNZType, typename ValueType>
template <bool weighted>
format::COO<IDType, NNZType, ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadArrayIntoCOO() const {
  std::ifstream fin(filename_);
  // Ignore headers and comments:
  while (fin.peek() == '%')
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  // Declare variables: (check the types here)

  format::DimensionType M, N;

  fin >> M >> N;

  format::DimensionType total_values = M * N;
  IDType *long_rows = new IDType[total_values];
  IDType *long_cols = new IDType[total_values];
  ValueType *long_vals = nullptr;
  if constexpr (weighted) long_vals = new ValueType[total_values];
  NNZType num_nnz = 0;
  for (format::DimensionType l = 0; l < total_values; l++) {
    ValueType w;
    fin >> w;

    if (w != 0) {
      long_cols[num_nnz] = l / N;
      long_rows[num_nnz] = l % N;
      if constexpr (store_values) long_vals[num_nnz] = w;
      num_nnz++;
    }
    // nnz_counter += w != 0;
  }

  IDType *row = new IDType[num_nnz];
  IDType *col = new IDType[num_nnz];
  std::copy(long_rows, long_rows + num_nnz, row);
  std::copy(long_cols, long_cols + num_nnz, col);
  ValueType *vals = nullptr;

  if constexpr (true) { //change true to store_values boolean.
    
    vals = new ValueType[num_nnz];
    std::copy(long_vals, long_vals + num_nnz, vals);
  }

  return new format::COO<IDType, NNZType, ValueType>(N, M, num_nnz, row, col,
                                                     vals, format::kOwned);
}

template <typename IDType, typename NNZType, typename ValueType>
format::COO<IDType, NNZType, ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadHigherOrderCOO() const {
  std::ifstream fin(filename_);

  if (fin.is_open()) {

    NNZType L = 0;
    format::DimensionType N = 0;

    while (fin.peek() == '#')
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string line;
    std::getline(fin, line);

    std::stringstream ss(line);
    std::string z;
    while (ss >> z) N++;
    N--;

    while (fin.peek() != EOF) {
      if (fin.peek() != '#') {
        L++;
      }
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    L++;

    format::DimensionType *dimension = new format::DimensionType[N];

    IDType **indices = new IDType *[N];
    for (format::DimensionType i = 0; i < N; i++) indices[i] = new IDType[L];

    fin.clear();
    fin.seekg(0);

    ValueType *vals = nullptr;

    if (store_values_) {
      if constexpr (std::is_same_v<void, ValueType>) {
        throw ReaderException(
            "Value type can not be void if store_values option is true.");
      }

      vals = new ValueType[L];

      for (NNZType l = 0; l < L; l++) {
        while (fin.peek() == '#')
          fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (format::DimensionType j = 0; j < N; j++) {
          fin >> indices[j][l];

          if (dimension[j] < indices[j][l]) dimension[j] = indices[j][l];

          indices[j][l] -= convert_to_zero_index_;
        }

        fin >> vals[l];
      }
    } else {
      for (NNZType l = 0; l < L; l++) {
        while (fin.peek() == '#')
          fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (format::DimensionType j = 0; j < N; j++) {
          fin >> indices[j][l];

          if (dimension[j] < indices[j][l]) dimension[j] = indices[j][l];

          indices[j][l] -= convert_to_zero_index_;
        }

        fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
    }
    fin.close();

    auto higher_order_coo = new format::HigherOrderCOO<IDType, NNZType, ValueType>(N, dimension, L, indices, vals, format::kOwned);
    
    return higher_order_coo;
  }
}

template <typename IDType, typename NNZType, typename ValueType>
format::Array<ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadCoordinateIntoArray() const {
  if constexpr (std::is_same_v<ValueType, void>)
    throw utils::ReaderException(
        "Cannot read a matrix market file into an Array with void ValueType");
  else {
    std::ifstream fin(filename_);

    // Ignore headers and comments:
    while (fin.peek() == '%')
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Declare variables: (check the types here)

    // check that it has 1 row/column
    format::DimensionType M, N, L;

    fin >> M >> N >> L;
    if (M != 1 && N != 1) {
      throw utils::ReaderException(
          "Trying to read a 2D matrix with multiple rows "
          "and multiple columns into dense array");
    }
    fin.close();
    auto coo = ReadCOO();
    auto n = coo->get_dimensions()[0];
    auto m = coo->get_dimensions()[1];
    auto coo_col = coo->get_col();
    auto coo_row = coo->get_row();
    auto coo_vals = coo->get_vals();
    auto num_nnz = coo->get_num_nnz();
    ValueType *vals = new ValueType[std::max<IDType>(n, m)]();
    IDType curr_row = 0;
    IDType curr_col = 0;
    for (IDType nnz = 0; nnz < num_nnz; nnz++) {
      vals[coo_col[nnz] + coo_row[nnz]] = coo_vals[nnz];
    }
    return new format::Array<ValueType>(std::max(n, m), vals,
                                        sparsebase::format::kOwned);
  }
}

template <typename IDType, typename NNZType, typename ValueType>
template <bool weighted, int symm, bool conv_to_zero>
format::COO<IDType, NNZType, ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadCoordinateIntoCOO() const {
  // Open the file:
  std::ifstream fin(filename_);

  if (fin.is_open()) {
    // Declare variables: (check the types here)
    format::DimensionType M, N, L;

    // Ignore headers and comments:
    while (fin.peek() == '%')
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    fin >> M >> N >> L;
    ValueType *vals = nullptr;
    if constexpr (symm == (int)MTXSymmetryOptions::general) {
      IDType *row = new IDType[L];
      IDType *col = new IDType[L];
      if constexpr (weighted) {
        if constexpr (!std::is_same_v<void, ValueType>) {
          vals = new ValueType[L];
          for (NNZType l = 0; l < L; l++) {
            IDType m, n;
            ValueType w;
            fin >> m >> n >> w;

            if constexpr (conv_to_zero) {
              n--;
              m--;
            }

            row[l] = m;
            col[l] = n;
            vals[l] = w;
          }

          auto coo = new format::COO<IDType, NNZType, ValueType>(
              M, N, L, row, col, vals, format::kOwned);
          return coo;
        } else {
          // TODO: Add an exception class for this
          throw utils::ReaderException(
              "Weight type for weighted graphs can not be void");
        }
      } else {
        for (NNZType l = 0; l < L; l++) {
          IDType m, n;
          fin >> m >> n;

          if constexpr (conv_to_zero) {
            n--;
            m--;
          }

          row[l] = m;
          col[l] = n;
        }

        auto coo = new format::COO<IDType, NNZType, ValueType>(
            M, N, L, row, col, nullptr, format::kOwned);
        return coo;
      }
    } else if constexpr (symm == (int)MTXSymmetryOptions::symmetric ||
                         symm == (int)MTXSymmetryOptions::skew_symmetric) {
      IDType *row = new IDType[L * 2];
      IDType *col = new IDType[L * 2];
      NNZType actual_nnzs = 0;
      IDType m, n;
      if constexpr (weighted) {
        if constexpr (!std::is_same_v<void, ValueType>) {
          vals = new ValueType[L * 2];
        } else {
          throw utils::ReaderException(
              "Weight type for weighted graphs can not be void");
        }
      }
      for (NNZType l = 0; l < L; l++) {
        fin >> m >> n;

        if constexpr (conv_to_zero) {
          n--;
          m--;
        }
        row[actual_nnzs] = m;
        col[actual_nnzs] = n;
        if constexpr (weighted && !std::is_same_v<void, ValueType>) {
          fin >> vals[actual_nnzs];
        }
        actual_nnzs++;
        bool check_diagonal;
        if constexpr (symm == (int)MTXSymmetryOptions::skew_symmetric)
          check_diagonal = false;
        else
          check_diagonal = true;
        if (check_diagonal && m != n) {
          row[actual_nnzs] = n;
          col[actual_nnzs] = m;
          if constexpr (weighted && !std::is_same_v<void, ValueType> &&
                        weighted)
            vals[actual_nnzs] = vals[actual_nnzs - 1];
          actual_nnzs++;
        }
      }
      IDType *actual_rows = row;
      IDType *actual_cols = col;
      ValueType *actual_vals = vals;
      if (symm == (int)MTXSymmetryOptions::symmetric && actual_nnzs != L * 2) {
        actual_rows = new IDType[actual_nnzs];
        actual_cols = new IDType[actual_nnzs];
        std::copy(row, row + actual_nnzs, actual_rows);
        std::copy(col, col + actual_nnzs, actual_cols);
        delete[] row;
        delete[] col;
        if constexpr (weighted && !std::is_same_v<void, ValueType>) {
          actual_vals = new ValueType[actual_nnzs];
          std::copy(vals, vals + actual_nnzs, actual_vals);
          delete[] vals;
        }
      }
      auto coo = new format::COO<IDType, NNZType, ValueType>(
          M, N, actual_nnzs, actual_rows, actual_cols, actual_vals,
          format::kOwned);
      return coo;
    } else {
      throw utils::ReaderException(
          "Reader only supports general, symmetric, and skew-symmetric "
          "symmetry options");
    }
  } else {
    throw utils::ReaderException("file does not exists!!");
  }
}
template <typename IDType, typename NNZType, typename ValueType>
format::Array<ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadArrayIntoArray() const {
  if constexpr (!std::is_same_v<void, ValueType>) {
    std::ifstream fin(filename_);
    // Ignore headers and comments:
    while (fin.peek() == '%')
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // Declare variables: (check the types here)

    // check that it has 1 row/column
    format::DimensionType M, N;

    fin >> M >> N;

    if (M != 1 && N != 1) {
      throw utils::ReaderException(
          "Trying to read a 2D matrix with multiple rows "
          "and multiple columns into dense array");
    }
    format::DimensionType total_values = M * N;
    // T ODO
    // Currently num_nnz is defined all wrong. Once it's fixed add back
    // nnz_counter
    // NNZType nnz_counter = 0;
    ValueType *vals = new ValueType[total_values];
    for (format::DimensionType l = 0; l < total_values; l++) {
      ValueType w;
      fin >> w;

      vals[l] = w;
      // nnz_counter += w != 0;
    }

    auto array = new format::Array<ValueType>(/*nnz_counter*/ total_values,
                                              vals, format::kOwned);
    return array;

  } else {
    throw utils::ReaderException(
        "Cannot read a matrix market file into an Array whose ValueType is "
        "void");
  }
}

template <typename IDType, typename NNZType, typename ValueType>
format::Array<ValueType> *TNSReader<IDType, NNZType, ValueType>::ReadArray()
    const {
  // check object
  if constexpr (std::is_same_v<ValueType, void>) {
    throw utils::ReaderException(
        "Cannot read a matrix market file into an Array whose ValueType is "
        "void");
  } else {
    bool weighted = options_.field != MTXFieldOptions::pattern;
    if (!weighted) {
      throw utils::ReaderException(
          "Cannot read a matrix market file into an Array if it is in pattern "
          "format");
    }
    if (options_.format == MTXFormatOptions::coordinate) {
      if constexpr (!std::is_same_v<ValueType, void>) {
        return ReadCoordinateIntoArray();
      }
    } else if (options_.format == MTXFormatOptions::array) {
      if constexpr (!std::is_same_v<ValueType, void>) {
        return ReadArrayIntoArray();
      }
    } else {
      throw utils::ReaderException(
          "Wrong format value while reading matrix market file\n");
    }
  }
}

template <typename IDType, typename NNZType, typename ValueType>
format::CSR<IDType, NNZType, ValueType>
    *TNSReader<IDType, NNZType, ValueType>::ReadCSR() const {
  auto coo = ReadCOO();
  converter::ConverterOrderTwo<IDType, NNZType, ValueType> converterObj;
  context::CPUContext cpu_context;
  return converterObj.template Convert<format::CSR<IDType, NNZType, ValueType>>(
      coo, &cpu_context);
}

template <typename IDType, typename NNZType, typename ValueType>
TNSReader<IDType, NNZType, ValueType>::~TNSReader(){};
#ifndef _HEADER_ONLY
#include "init/tns_reader.inc"
#endif
}  // namespace sparsebase::io