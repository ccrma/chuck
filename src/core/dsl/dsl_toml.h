//-----------------------------------------------------------------------------
// file: dsl_toml.h
// desc: parse TOML-based DSL into Piece proto
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <string_view>

#include "piece.pb.h"

namespace dsl {

// Populated when parsing fails to carry basic source information.
struct TomlError {
    std::string message;
    int line = 0;
    int column = 0;
    std::string path;
};

// Parse a TOML DSL file into a Piece proto.
// Returns true on success; on failure populates |error| if provided.
bool PieceFromTomlFile(const std::string &path, Piece *piece, TomlError *error = nullptr);

// Parse TOML content (with an optional source name for diagnostics) into a Piece proto.
bool PieceFromTomlString(std::string_view content, const std::string &source_name, Piece *piece,
                         TomlError *error = nullptr);

}  // namespace dsl

