//-----------------------------------------------------------------------------
// file: dsl_toml.cpp
// desc: parse TOML-based DSL into Piece proto
//-----------------------------------------------------------------------------

#include "dsl_toml.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>

#include "toml.hpp"

namespace dsl {
namespace {

std::string ToUpper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

void FillError(const std::string &message, const toml::source_region *region, TomlError *error)
{
    if (!error) return;
    error->message = message;
    error->line = 0;
    error->column = 0;
    error->path.clear();
    if (region)
    {
        error->line = static_cast<int>(region->begin.line);
        error->column = static_cast<int>(region->begin.column);
        if (region->path) error->path = *region->path;
    }
}

bool Fail(const std::string &message, TomlError *error)
{
    FillError(message, nullptr, error);
    return false;
}

bool Fail(const toml::node &node, const std::string &message, TomlError *error)
{
    FillError(message, &node.source(), error);
    return false;
}

template <typename T>
std::optional<T> ReadValue(const toml::node &node, const std::string &label, const char *expected,
                           TomlError *error)
{
    if (auto value = node.value<T>())
    {
        return value;
    }

    std::ostringstream oss;
    oss << label << " must be a " << expected;
    Fail(node, oss.str(), error);
    return std::nullopt;
}

bool ParseScale(const toml::node &node, const std::string &raw, Scale *out, TomlError *error)
{
    std::string key = ToUpper(raw);
    if (key == "NONE")
        *out = SCALE_NONE;
    else if (key == "MAJOR")
        *out = SCALE_MAJOR;
    else if (key == "MINOR")
        *out = SCALE_MINOR;
    else if (key == "PENTATONIC")
        *out = SCALE_PENTATONIC;
    else if (key == "CHROMATIC")
        *out = SCALE_CHROMATIC;
    else if (key == "DORIAN")
        *out = SCALE_DORIAN;
    else if (key == "LYDIAN")
        *out = SCALE_LYDIAN;
    else
        return Fail(node, "unknown scale '" + raw + "'", error);
    return true;
}

bool ParseOrnament(const toml::node &node, const std::string &raw, Ornament *out, TomlError *error)
{
    std::string key = ToUpper(raw);
    if (key == "NONE")
        *out = ORNAMENT_NONE;
    else if (key == "GRACE")
        *out = ORNAMENT_GRACE;
    else if (key == "TRILL")
        *out = ORNAMENT_TRILL;
    else
        return Fail(node, "unknown ornament '" + raw + "'", error);
    return true;
}

bool ParseOsc(const toml::node &node, const std::string &raw, Instrument *instrument, TomlError *error)
{
    std::string key = ToUpper(raw);
    if (key == "SINE")
        instrument->set_osc(Instrument_Osc_SINE);
    else if (key == "SAW")
        instrument->set_osc(Instrument_Osc_SAW);
    else if (key == "SQUARE")
        instrument->set_osc(Instrument_Osc_SQUARE);
    else if (key == "TRIANGLE")
        instrument->set_osc(Instrument_Osc_TRIANGLE);
    else if (key == "NOISE")
        instrument->set_osc(Instrument_Osc_NOISE);
    else if (key == "BLIT")
        instrument->set_osc(Instrument_Osc_BLIT);
    else
        return Fail(node, "unknown osc '" + raw + "'", error);
    return true;
}

bool ParseFilter(const toml::table &table, Filter *filter, TomlError *error)
{
    if (const toml::node *type_node = table.get("type"))
    {
        auto type = ReadValue<std::string>(*type_node, "filter.type", "string", error);
        if (!type) return false;
        std::string key = ToUpper(*type);
        if (key == "LOWPASS")
            filter->set_type(Filter_Type_LOWPASS);
        else if (key == "HIGHPASS")
            filter->set_type(Filter_Type_HIGHPASS);
        else if (key == "BANDPASS")
            filter->set_type(Filter_Type_BANDPASS);
        else
            return Fail(*type_node, "unknown filter type '" + *type + "'", error);
    }

    if (const toml::node *cutoff_node = table.get("cutoff"))
    {
        auto cutoff = ReadValue<double>(*cutoff_node, "filter.cutoff", "number", error);
        if (!cutoff) return false;
        filter->set_cutoff(*cutoff);
    }

    if (const toml::node *q_node = table.get("q"))
    {
        auto q = ReadValue<double>(*q_node, "filter.q", "number", error);
        if (!q) return false;
        filter->set_q(*q);
    }

    return true;
}

bool ParseEnvelope(const toml::table &table, Envelope *env, TomlError *error)
{
    if (const toml::node *attack_node = table.get("attack"))
    {
        auto attack = ReadValue<double>(*attack_node, "env.attack", "number", error);
        if (!attack) return false;
        env->set_attack(*attack);
    }

    if (const toml::node *decay_node = table.get("decay"))
    {
        auto decay = ReadValue<double>(*decay_node, "env.decay", "number", error);
        if (!decay) return false;
        env->set_decay(*decay);
    }

    if (const toml::node *sustain_node = table.get("sustain"))
    {
        auto sustain = ReadValue<double>(*sustain_node, "env.sustain", "number", error);
        if (!sustain) return false;
        env->set_sustain(*sustain);
    }

    if (const toml::node *release_node = table.get("release"))
    {
        auto release = ReadValue<double>(*release_node, "env.release", "number", error);
        if (!release) return false;
        env->set_release(*release);
    }

    return true;
}

bool ParseLFO(const toml::table &table, LFO *lfo, TomlError *error)
{
    if (const toml::node *target_node = table.get("target"))
    {
        auto target = ReadValue<std::string>(*target_node, "lfo.target", "string", error);
        if (!target) return false;
        lfo->set_target(*target);
    }

    if (const toml::node *rate_node = table.get("rate"))
    {
        auto rate = ReadValue<double>(*rate_node, "lfo.rate", "number", error);
        if (!rate) return false;
        lfo->set_rate(*rate);
    }

    if (const toml::node *depth_node = table.get("depth"))
    {
        auto depth = ReadValue<double>(*depth_node, "lfo.depth", "number", error);
        if (!depth) return false;
        lfo->set_depth(*depth);
    }

    return true;
}

bool ParseNote(const toml::table &table, Note *note, TomlError *error)
{
    const toml::node *pitch_node = table.get("pitch");
    if (!pitch_node) return Fail(table, "note requires 'pitch'", error);
    auto pitch = ReadValue<int64_t>(*pitch_node, "note.pitch", "integer", error);
    if (!pitch) return false;
    if (*pitch < 0 || *pitch > 127)
        return Fail(*pitch_node, "note.pitch must be between 0 and 127", error);
    note->set_pitch(static_cast<int32_t>(*pitch));

    const toml::node *dur_node = table.get("dur");
    if (!dur_node) return Fail(table, "note requires 'dur'", error);
    auto dur = ReadValue<double>(*dur_node, "note.dur", "number", error);
    if (!dur) return false;
    note->set_dur(*dur);

    if (const toml::node *tie_node = table.get("tie"))
    {
        auto tie = ReadValue<bool>(*tie_node, "note.tie", "boolean", error);
        if (!tie) return false;
        note->set_tie(*tie);
    }

    return true;
}

bool ParsePattern(const toml::table &table, Pattern *pattern, TomlError *error)
{
    if (const toml::node *repeat_node = table.get("repeat"))
    {
        auto repeat = ReadValue<int64_t>(*repeat_node, "pattern.repeat", "integer", error);
        if (!repeat) return false;
        pattern->set_repeat(static_cast<int32_t>(*repeat));
    }

    if (const toml::node *density_node = table.get("density"))
    {
        auto density = ReadValue<double>(*density_node, "pattern.density", "number", error);
        if (!density) return false;
        pattern->set_density(*density);
    }

    if (const toml::node *ornament_node = table.get("ornament"))
    {
        auto ornament = ReadValue<std::string>(*ornament_node, "pattern.ornament", "string", error);
        if (!ornament) return false;
        Ornament parsed = ORNAMENT_NONE;
        if (!ParseOrnament(*ornament_node, *ornament, &parsed, error)) return false;
        pattern->set_ornament(parsed);
    }

    if (const toml::node *scale_node = table.get("scale"))
    {
        auto scale = ReadValue<std::string>(*scale_node, "pattern.scale", "string", error);
        if (!scale) return false;
        Scale parsed = SCALE_NONE;
        if (!ParseScale(*scale_node, *scale, &parsed, error)) return false;
        pattern->set_scale(parsed);
    }

    if (const toml::node *notes_node = table.get("notes"))
    {
        const toml::array *notes = notes_node->as_array();
        if (!notes) return Fail(*notes_node, "pattern.notes must be an array", error);
        for (size_t i = 0; i < notes->size(); ++i)
        {
            const toml::node &note_node = (*notes)[i];
            const toml::table *note_table = note_node.as_table();
            if (!note_table) return Fail(note_node, "pattern.notes entries must be tables", error);
            if (!ParseNote(*note_table, pattern->add_notes(), error)) return false;
        }
    }

    return true;
}

bool ParseInstrument(const toml::table &table, Instrument *instrument, TomlError *error)
{
    const toml::node *osc_node = table.get("osc");
    if (!osc_node) return Fail(table, "instrument requires 'osc'", error);
    auto osc = ReadValue<std::string>(*osc_node, "instrument.osc", "string", error);
    if (!osc) return false;
    if (!ParseOsc(*osc_node, *osc, instrument, error)) return false;

    if (const toml::node *detune_node = table.get("detune"))
    {
        auto detune = ReadValue<double>(*detune_node, "instrument.detune", "number", error);
        if (!detune) return false;
        instrument->set_detune(*detune);
    }

    if (const toml::node *reverb_node = table.get("reverb"))
    {
        auto reverb = ReadValue<bool>(*reverb_node, "instrument.reverb", "boolean", error);
        if (!reverb) return false;
        instrument->set_reverb(*reverb);
    }

    if (const toml::node *gain_node = table.get("gain"))
    {
        auto gain = ReadValue<double>(*gain_node, "instrument.gain", "number", error);
        if (!gain) return false;
        instrument->set_gain(*gain);
    }

    if (const toml::node *complexity_node = table.get("complexity"))
    {
        auto complexity = ReadValue<double>(*complexity_node, "instrument.complexity", "number", error);
        if (!complexity) return false;
        instrument->set_complexity(*complexity);
    }

    if (const toml::node *env_node = table.get("env"))
    {
        const toml::table *env_table = env_node->as_table();
        if (!env_table) return Fail(*env_node, "instrument.env must be a table", error);
        if (!ParseEnvelope(*env_table, instrument->mutable_env(), error)) return false;
    }

    if (const toml::node *filter_node = table.get("filter"))
    {
        const toml::table *filter_table = filter_node->as_table();
        if (!filter_table) return Fail(*filter_node, "instrument.filter must be a table", error);
        if (!ParseFilter(*filter_table, instrument->mutable_filter(), error)) return false;
    }

    if (const toml::node *lfo_node = table.get("lfo"))
    {
        const toml::table *lfo_table = lfo_node->as_table();
        if (!lfo_table) return Fail(*lfo_node, "instrument.lfo must be a table", error);
        if (!ParseLFO(*lfo_table, instrument->mutable_lfo(), error)) return false;
    }

    return true;
}

bool ParseVoice(const toml::table &table, Voice *voice, TomlError *error)
{
    if (const toml::node *offset_node = table.get("offset"))
    {
        auto offset = ReadValue<double>(*offset_node, "voice.offset", "number", error);
        if (!offset) return false;
        voice->set_offset(*offset);
    }

    if (const toml::node *pan_node = table.get("pan"))
    {
        auto pan = ReadValue<double>(*pan_node, "voice.pan", "number", error);
        if (!pan) return false;
        voice->set_pan(*pan);
    }

    const toml::node *instrument_node = table.get("instrument");
    if (!instrument_node) return Fail(table, "voice requires 'instrument' table", error);
    const toml::table *instrument_table = instrument_node->as_table();
    if (!instrument_table) return Fail(*instrument_node, "voice.instrument must be a table", error);
    if (!ParseInstrument(*instrument_table, voice->mutable_instrument(), error)) return false;

    if (const toml::node *pattern_node = table.get("pattern"))
    {
        const toml::table *pattern_table = pattern_node->as_table();
        if (!pattern_table) return Fail(*pattern_node, "voice.pattern must be a table", error);
        if (!ParsePattern(*pattern_table, voice->mutable_pattern(), error)) return false;
    }

    return true;
}

bool ParsePiece(const toml::table &table, Piece *piece, TomlError *error)
{
    piece->Clear();
    piece->set_tempo(120.0);

    if (const toml::node *tempo_node = table.get("tempo"))
    {
        auto tempo = ReadValue<double>(*tempo_node, "tempo", "number", error);
        if (!tempo) return false;
        piece->set_tempo(*tempo);
    }

    if (const toml::node *complexity_node = table.get("complexity"))
    {
        auto complexity = ReadValue<double>(*complexity_node, "complexity", "number", error);
        if (!complexity) return false;
        piece->set_complexity(*complexity);
    }

    if (const toml::node *voices_node = table.get("voices"))
    {
        const toml::array *voices = voices_node->as_array();
        if (!voices) return Fail(*voices_node, "voices must be an array of tables", error);
        for (size_t i = 0; i < voices->size(); ++i)
        {
            const toml::node &voice_node = (*voices)[i];
            const toml::table *voice_table = voice_node.as_table();
            if (!voice_table) return Fail(voice_node, "voice entry must be a table", error);
            if (!ParseVoice(*voice_table, piece->add_voices(), error)) return false;
        }
    }

    return true;
}

}  // namespace

bool PieceFromTomlString(std::string_view content, const std::string &source_name, Piece *piece,
                         TomlError *error)
{
    if (!piece) return Fail("output Piece pointer is null", error);
#if TOML_EXCEPTIONS
    try
    {
        toml::table table = toml::parse(content, source_name);
        return ParsePiece(table, piece, error);
    }
    catch (const toml::parse_error &err)
    {
        FillError(std::string(err.description()), &err.source(), error);
        return false;
    }
#else
    toml::parse_result result = toml::parse(content, source_name);
    if (!result)
    {
        FillError(std::string(result.error().description()), &result.error().source(), error);
        return false;
    }

    return ParsePiece(result.table(), piece, error);
#endif
}

bool PieceFromTomlFile(const std::string &path, Piece *piece, TomlError *error)
{
    if (!piece) return Fail("output Piece pointer is null", error);
#if TOML_EXCEPTIONS
    try
    {
        toml::table table = toml::parse_file(path);
        return ParsePiece(table, piece, error);
    }
    catch (const toml::parse_error &err)
    {
        FillError(std::string(err.description()), &err.source(), error);
        return false;
    }
#else
    toml::parse_result result = toml::parse_file(path);
    if (!result)
    {
        FillError(std::string(result.error().description()), &result.error().source(), error);
        return false;
    }

    return ParsePiece(result.table(), piece, error);
#endif
}

}  // namespace dsl
