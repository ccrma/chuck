//-----------------------------------------------------------------------------
// file: dsl_codegen.cpp
// desc: convert Piece proto into ChucK source code
//-----------------------------------------------------------------------------

#include "dsl_codegen.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace dsl {
namespace {

std::string FormatDouble(double v)
{
    std::ostringstream oss;
    oss << std::setprecision(12);
    oss << v;
    return oss.str();
}

std::string OscClass(Instrument_Osc osc)
{
    switch (osc)
    {
    case Instrument_Osc_SINE: return "SinOsc";
    case Instrument_Osc_SAW: return "SawOsc";
    case Instrument_Osc_SQUARE: return "SqrOsc";
    case Instrument_Osc_TRIANGLE: return "TriOsc";
    case Instrument_Osc_NOISE: return "Noise";
    case Instrument_Osc_BLIT: return "BlitSaw";
    default: return "SinOsc";
    }
}

std::string FilterClass(Filter_Type type)
{
    switch (type)
    {
    case Filter_Type_LOWPASS: return "LPF";
    case Filter_Type_HIGHPASS: return "HPF";
    case Filter_Type_BANDPASS: return "BPF";
    default: return "";
    }
}

std::string MakeVoiceFunction(const Voice &voice, int index)
{
    const Instrument &instr = voice.instrument();
    const Pattern &pattern = voice.pattern();

    std::ostringstream out;
    const std::string idx = std::to_string(index);
    const std::string func = "voice_" + idx;
    const std::string osc = "osc_" + idx;
    const std::string env = "env_" + idx;
    const std::string pan = "pan_" + idx;
    const std::string filt = "filt_" + idx;
    const std::string rev = "rev_" + idx;

    out << "fun void " << func << "() {\n";
    out << "  " << FormatDouble(voice.offset()) << " * beat => now;\n";
    out << "  " << OscClass(instr.osc()) << " " << osc << ";\n";
    out << "  ADSR " << env << ";\n";
    const bool has_filter = instr.filter().type() != Filter_Type_FILTER_NONE;
    if (has_filter) out << "  " << FilterClass(instr.filter().type()) << " " << filt << ";\n";
    const bool use_reverb = instr.reverb();
    if (use_reverb) out << "  NRev " << rev << ";\n";
    out << "  Pan2 " << pan << ";\n";

    out << "  " << osc << " => ";
    if (has_filter) out << filt << " => ";
    out << env << " => ";
    if (use_reverb) out << rev << " => ";
    out << pan << " => dac;\n";

    out << "  " << FormatDouble(instr.gain()) << " => " << osc << ".gain;\n";
    out << "  " << FormatDouble(voice.pan()) << " => " << pan << ".pan;\n";
    out << "  " << env << ".set(" << FormatDouble(instr.env().attack()) << ", "
        << FormatDouble(instr.env().decay()) << ", "
        << FormatDouble(instr.env().sustain()) << ", "
        << FormatDouble(instr.env().release()) << ");\n";

    if (has_filter)
    {
        if (instr.filter().cutoff() > 0.0)
            out << "  " << FormatDouble(instr.filter().cutoff()) << " => " << filt << ".freq;\n";
        if (instr.filter().q() > 0.0)
            out << "  " << FormatDouble(instr.filter().q()) << " => " << filt << ".Q;\n";
    }

    if (use_reverb)
    {
        out << "  0.1 => " << rev << ".mix;\n";
    }

    const int repeat = pattern.repeat() > 0 ? pattern.repeat() : 1;
    const double density = pattern.density() > 0.0 ? pattern.density() : 1.0;
    const int scale = pattern.scale();

    out << "  for( int rep = 0; rep < " << repeat << "; rep++ ) {\n";
    for (int i = 0; i < pattern.notes_size(); ++i)
    {
        const Note &note = pattern.notes(i);
        const std::string gateVar = "gate_" + idx + "_" + std::to_string(i);
        out << "    // note " << i << "\n";
        if (density < 1.0)
        {
            out << "    if( Math.random2f(0.0, 1.0) > " << FormatDouble(density) << " ) { continue; }\n";
        }
        out << "    Std.mtof(applyScale(" << note.pitch() << ", " << scale << ")) + "
            << FormatDouble(instr.detune()) << " => " << osc << ".freq;\n";
        out << "    " << env << ".keyOn();\n";
        out << "    " << FormatDouble(note.dur()) << " * beat => now;\n";
        if (!note.tie())
        {
            out << "    " << env << ".keyOff();\n";
            if (instr.env().release() > 0.0)
                out << "    " << FormatDouble(instr.env().release()) << " :: second => now;\n";
        }
    }
    out << "  }\n";
    out << "}\n\n";
    return out.str();
}

}  // namespace

bool PieceToChuckSource(const Piece &piece, const std::string &source_name, std::string *out_source,
                        std::string *error)
{
    if (!out_source)
    {
        if (error) *error = "out_source is null";
        return false;
    }

    const double tempo = piece.tempo() > 0.0 ? piece.tempo() : 120.0;

    std::ostringstream out;
    out << "// Generated from " << source_name << "\n";
    out << "60.0 / " << FormatDouble(tempo) << " => float spb;\n";
    out << "spb :: second => dur beat;\n";
    out << FormatDouble(piece.complexity()) << " => float g_complexity;\n\n";

    out << "// Placeholder scale helper (currently pass-through)\n";
    out << "fun int applyScale(int midi, int scale) { return midi; }\n\n";

    for (int i = 0; i < piece.voices_size(); ++i)
    {
        out << MakeVoiceFunction(piece.voices(i), i);
    }

    out << "// spawn voices\n";
    for (int i = 0; i < piece.voices_size(); ++i)
    {
        out << "spork ~ voice_" << i << "();\n";
    }
    out << "// keep alive (could be tightened with computed piece length)\n";
    out << "while(true) 1::second => now;\n";

    *out_source = out.str();
    return true;
}

}  // namespace dsl

