//-----------------------------------------------------------------------------
// file: dsl_codegen.h
// desc: convert Piece proto into ChucK source code
//-----------------------------------------------------------------------------

#pragma once

#include <string>

#include "piece.pb.h"

namespace dsl {

// Translate a Piece proto into a ChucK source string.
// Returns true on success; on failure populates |error| (if non-null) and leaves |out_source| unchanged.
bool PieceToChuckSource(const Piece &piece, const std::string &source_name, std::string *out_source,
                        std::string *error = nullptr);

}  // namespace dsl

