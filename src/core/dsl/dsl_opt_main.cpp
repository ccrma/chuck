// CLI: optimize a TOML piece and emit TOML.
#include "dsl_opt.h"
#include "dsl_toml.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {

void usage(const char *prog)
{
    std::cerr << "usage: " << prog << " <input.toml> [-o output.toml] [--complexity X]\n";
}

std::string PieceToToml(const dsl::Piece &piece)
{
    auto oscStr = [](dsl::Instrument_Osc osc) {
        std::string s = dsl::Instrument_Osc_Name(osc);
        const std::string prefix = "Instrument_Osc_";
        if (s.rfind(prefix, 0) == 0) s = s.substr(prefix.size());
        return s;
    };
    auto filtStr = [](dsl::Filter_Type t) {
        std::string s = dsl::Filter_Type_Name(t);
        const std::string prefix = "Filter_Type_";
        if (s.rfind(prefix, 0) == 0) s = s.substr(prefix.size());
        return s;
    };
    auto scaleStr = [](dsl::Scale s) {
        std::string str = dsl::Scale_Name(s);
        const std::string prefix = "SCALE_";
        if (str.rfind(prefix, 0) == 0) str = str.substr(prefix.size());
        return str;
    };

    std::ostringstream out;
    out << "tempo = " << piece.tempo() << "\n";
    out << "complexity = " << piece.complexity() << "\n\n";

    for (int v = 0; v < piece.voices_size(); ++v)
    {
        const dsl::Voice &voice = piece.voices(v);
        out << "[[voices]]\n";
        out << "offset = " << voice.offset() << "\n";
        out << "pan = " << voice.pan() << "\n\n";

        const dsl::Instrument &ins = voice.instrument();
        out << "[voices.instrument]\n";
        out << "osc = \"" << oscStr(ins.osc()) << "\"\n";
        out << "gain = " << ins.gain() << "\n";
        out << "reverb = " << (ins.reverb() ? "true" : "false") << "\n";
        out << "detune = " << ins.detune() << "\n";
        out << "complexity = " << ins.complexity() << "\n\n";

        const dsl::Envelope &env = ins.env();
        out << "[voices.instrument.env]\n";
        out << "attack = " << env.attack() << "\n";
        out << "decay = " << env.decay() << "\n";
        out << "sustain = " << env.sustain() << "\n";
        out << "release = " << env.release() << "\n\n";

        const dsl::Filter &filt = ins.filter();
        out << "[voices.instrument.filter]\n";
        out << "type = \"" << filtStr(filt.type()) << "\"\n";
        out << "cutoff = " << filt.cutoff() << "\n";
        out << "q = " << filt.q() << "\n\n";

        const dsl::LFO &lfo = ins.lfo();
        out << "[voices.instrument.lfo]\n";
        out << "target = \"" << lfo.target() << "\"\n";
        out << "rate = " << lfo.rate() << "\n";
        out << "depth = " << lfo.depth() << "\n\n";

        const dsl::Pattern &pat = voice.pattern();
        out << "[voices.pattern]\n";
        out << "density = " << pat.density() << "\n";
        out << "repeat = " << pat.repeat() << "\n";
        out << "scale = \"" << scaleStr(pat.scale()) << "\"\n\n";

        for (int n = 0; n < pat.notes_size(); ++n)
        {
            const dsl::Note &note = pat.notes(n);
            out << "[[voices.pattern.notes]]\n";
            out << "pitch = " << note.pitch() << "\n";
            out << "dur = " << note.dur() << "\n";
            out << "tie = " << (note.tie() ? "true" : "false") << "\n\n";
        }
    }

    return out.str();
}

}  // namespace

int main(int argc, char **argv)
{
    std::string in_path;
    std::string out_path;
    double complexity = -1.0;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output")
        {
            if (i + 1 >= argc) { usage(argv[0]); return 1; }
            out_path = argv[++i];
        }
        else if (arg.rfind("--output=", 0) == 0)
        {
            out_path = arg.substr(9);
        }
        else if (arg == "--complexity")
        {
            if (i + 1 >= argc) { usage(argv[0]); return 1; }
            complexity = atof(argv[++i]);
        }
        else if (arg.rfind("--complexity=", 0) == 0)
        {
            complexity = atof(arg.c_str() + 13);
        }
        else if (!arg.empty() && arg[0] != '-' && in_path.empty())
        {
            in_path = arg;
        }
        else
        {
            usage(argv[0]);
            return 1;
        }
    }

    if (in_path.empty())
    {
        usage(argv[0]);
        return 1;
    }

    dsl::Piece piece;
    dsl::TomlError err;
    if (!dsl::PieceFromTomlFile(in_path, &piece, &err))
    {
        std::cerr << "parse failed: " << err.message;
        if (!err.path.empty()) std::cerr << " (" << err.path << ":" << err.line << ":" << err.column << ")";
        std::cerr << "\n";
        return 1;
    }

    if (complexity >= 0.0) piece.set_complexity(complexity);

    dsl::Piece opt = dsl::OptimizePiece(piece, piece.complexity());
    std::string toml_out = PieceToToml(opt);

    if (out_path.empty())
    {
        std::cout << toml_out;
        return 0;
    }

    std::ofstream ofs(out_path.c_str(), std::ios::out | std::ios::trunc);
    if (!ofs.good())
    {
        std::cerr << "could not open output file '" << out_path << "'\n";
        return 1;
    }
    ofs << toml_out;
    ofs.close();
    return 0;
}
