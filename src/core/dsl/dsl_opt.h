//-----------------------------------------------------------------------------
// file: dsl_opt.h
// desc: middle-end optimizer over Piece proto
//-----------------------------------------------------------------------------

#pragma once

#include "piece.pb.h"

namespace dsl {

// Apply the sequence of passes described in the DSL middle-end. Complexity in [0,1].
Piece OptimizePiece(const Piece &in, double complexity);

}  // namespace dsl

