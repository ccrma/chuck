// Simple CLI to load a DSL TOML file and emit ChucK source.
#include "dsl_codegen.h"
#include "dsl_toml.h"

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::string in_path;
    std::string out_path;

    // simple flag parser: dsl_codegen <input.toml> [-o output.ck]
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "missing argument for " << arg << "\n";
                return 1;
            }
            out_path = argv[++i];
        }
        else if (arg.rfind("-o", 0) == 0 && arg.size() > 2)
        {
            out_path = arg.substr(2);
        }
        else if (arg.rfind("--output=", 0) == 0)
        {
            out_path = arg.substr(9);
        }
        else if (arg[0] == '-')
        {
            std::cerr << "unknown flag: " << arg << "\n";
            return 1;
        }
        else if (in_path.empty())
        {
            in_path = arg;
        }
        else
        {
            std::cerr << "unexpected extra argument: " << arg << "\n";
            return 1;
        }
    }

    if (in_path.empty())
    {
        std::cerr << "usage: " << argv[0] << " <input.toml> [-o output.ck]\n";
        return 1;
    }

    dsl::Piece piece;
    dsl::TomlError err;
    if (!dsl::PieceFromTomlFile(in_path, &piece, &err))
    {
        std::cerr << "parse failed: " << err.message;
        if (!err.path.empty())
            std::cerr << " (" << err.path << ":" << err.line << ":" << err.column << ")";
        std::cerr << "\n";
        return 1;
    }

    std::string code;
    std::string cg_err;
    if (!dsl::PieceToChuckSource(piece, in_path, &code, &cg_err))
    {
        std::cerr << "codegen failed: " << cg_err << "\n";
        return 1;
    }

    // default output path: basename.toml -> basename.ck in CWD
    std::string out = out_path;
    if (out.empty())
    {
        std::string fname = in_path;
        // strip directory
        std::size_t slash = fname.find_last_of("/\\");
        if (slash != std::string::npos) fname = fname.substr(slash + 1);
        // drop extension
        std::size_t dot = fname.find_last_of('.');
        if (dot != std::string::npos) fname = fname.substr(0, dot);
        out = fname + ".ck";
    }

    std::ofstream ofs(out.c_str(), std::ios::out | std::ios::trunc);
    if (!ofs.good())
    {
        std::cerr << "could not open output file '" << out << "'\n";
        return 1;
    }
    ofs << code;
    ofs.close();
    std::cout << "wrote " << out << "\n";
    return 0;
}
