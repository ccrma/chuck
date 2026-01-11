// Simple CLI to load a DSL TOML file and print the Piece proto.
#include "dsl_toml.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <path/to/file.toml>\n";
        return 1;
    }

    const std::string path = argv[1];
    dsl::Piece piece;
    dsl::TomlError err;
    if (!dsl::PieceFromTomlFile(path, &piece, &err))
    {
        std::cerr << "parse failed: " << err.message;
        if (!err.path.empty()) std::cerr << " (" << err.path << ":" << err.line << ":" << err.column << ")";
        std::cerr << "\n";
        return 1;
    }

    std::cout << piece.DebugString();
    return 0;
}

