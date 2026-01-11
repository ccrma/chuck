//-----------------------------------------------------------------------------
// file: dsl_opt.cpp
// desc: middle-end optimizer over Piece proto
//-----------------------------------------------------------------------------

#include "dsl_opt.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <vector>

namespace dsl {
namespace {

struct RNG {
    std::mt19937 rng;
    std::uniform_real_distribution<double> uni{0.0, 1.0};
    RNG() : rng(42) {} // deterministic for now
    double next() { return uni(rng); }
};

bool IsTurningPoint(const std::vector<Note> &notes, size_t idx)
{
    if (idx == 0 || idx + 1 >= notes.size()) return false;
    int prev = notes[idx - 1].pitch();
    int cur = notes[idx].pitch();
    int next = notes[idx + 1].pitch();
    int d1 = cur - prev;
    int d2 = next - cur;
    return (d1 == 0 || d2 == 0) ? false : (d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0);
}

//---------------- Pattern Density Pass ----------------
Piece ApplyNoteDensityPass(const Piece &in, double complexity)
{
    Piece out = in;
    double simplifyAmt = complexity < 0.5 ? (0.5 - complexity) * 2.0 : 0.0;
    double enrichAmt = complexity > 0.5 ? (complexity - 0.5) * 2.0 : 0.0;
    RNG rng;

    for (int v = 0; v < out.voices_size(); ++v)
    {
        Pattern *pat = out.mutable_voices(v)->mutable_pattern();
        std::vector<Note> orig(pat->notes().begin(), pat->notes().end());
        std::vector<Note> rewritten;

        if (orig.empty()) continue;

        if (simplifyAmt > 0.0)
        {
            double keep_ratio = 1.0 - simplifyAmt; // complexity close to 0 keeps little
            double carried_dur = 0.0;
            for (size_t i = 0; i < orig.size(); ++i)
            {
                bool keep = (i == 0) || IsTurningPoint(orig, i) || rng.next() < keep_ratio;
                carried_dur += orig[i].dur();
                if (keep)
                {
                    Note kept = orig[i];
                    kept.set_dur(carried_dur);
                    rewritten.push_back(kept);
                    carried_dur = 0.0;
                }
            }
            if (rewritten.empty())
            {
                rewritten.push_back(orig.front());
                rewritten.back().set_dur(orig.front().dur());
            }
        }
        else // enrich
        {
            rewritten.reserve(orig.size() * 2);
            for (size_t i = 0; i < orig.size(); ++i)
            {
                const Note &n = orig[i];
                rewritten.push_back(n);
                // insert neighbor or approach notes
                if (enrichAmt > 0.0)
                {
                    bool insert_neighbor = rng.next() < enrichAmt * 0.5;
                    bool subdivide = rng.next() < enrichAmt * 0.4;
                    if (insert_neighbor)
                    {
                        Note neighbor = n;
                        int dir = (i + 1 < orig.size() && orig[i + 1].pitch() != n.pitch())
                                      ? ((orig[i + 1].pitch() > n.pitch()) ? 1 : -1)
                                      : (rng.next() < 0.5 ? -1 : 1);
                        neighbor.set_pitch(n.pitch() + dir);
                        neighbor.set_dur(n.dur() * 0.5);
                        // shorten original if we add neighbor
                        rewritten.back().set_dur(n.dur() * 0.5);
                        rewritten.push_back(neighbor);
                    }
                    else if (subdivide && n.dur() > 0.5)
                    {
                        Note split = n;
                        split.set_dur(n.dur() * 0.5);
                        rewritten.back().set_dur(n.dur() * 0.5);
                        rewritten.push_back(split);
                    }
                }
            }
        }

        pat->clear_notes();
        for (const auto &n : rewritten) *pat->add_notes() = n;
    }
    return out;
}

//---------------- Pitch Simplification Pass ----------------
int QuantizeToScale(int midi, Scale scale)
{
    static const int major[7] = {0, 2, 4, 5, 7, 9, 11};
    static const int minor[7] = {0, 2, 3, 5, 7, 8, 10};
    static const int dorian[7] = {0, 2, 3, 5, 7, 9, 10};
    const int *steps = nullptr;
    switch (scale)
    {
    case SCALE_MAJOR: steps = major; break;
    case SCALE_MINOR: steps = minor; break;
    case SCALE_DORIAN: steps = dorian; break;
    default: return midi;
    }
    int octave = midi / 12;
    int degree = midi % 12;
    int best = steps[0];
    int bestDiff = std::abs(degree - best);
    for (int i = 1; i < 7; ++i)
    {
        int d = steps[i];
        int diff = std::abs(degree - d);
        if (diff < bestDiff)
        {
            best = d;
            bestDiff = diff;
        }
    }
    return octave * 12 + best;
}

Piece ApplyPitchSimplificationPass(const Piece &in, double complexity)
{
    Piece out = in;
    double simplifyAmt = complexity < 0.5 ? (0.5 - complexity) * 2.0 : 0.0;
    double enrichAmt = complexity > 0.5 ? (complexity - 0.5) * 2.0 : 0.0;
    RNG rng;

    for (int v = 0; v < out.voices_size(); ++v)
    {
        Pattern *pat = out.mutable_voices(v)->mutable_pattern();
        std::vector<Note> orig(pat->notes().begin(), pat->notes().end());
        std::vector<Note> rewritten;
        if (orig.empty()) continue;
        Scale scale = pat->scale();

        if (simplifyAmt > 0.0)
        {
            Note prev = orig.front();
            rewritten.push_back(prev);
            for (size_t i = 1; i < orig.size(); ++i)
            {
                Note n = orig[i];
                int interval = n.pitch() - prev.pitch();
                if (std::abs(interval) > 7) // larger than perfect fifth
                {
                    int dir = interval > 0 ? 1 : -1;
                    n.set_pitch(prev.pitch() + dir * 5);
                }
                if (scale != SCALE_NONE)
                    n.set_pitch(QuantizeToScale(n.pitch(), scale));
                rewritten.push_back(n);
                prev = n;
            }
        }
        else // enrich
        {
            for (size_t i = 0; i < orig.size(); ++i)
            {
                const Note &n = orig[i];
                rewritten.push_back(n);
                int next_pitch = (i + 1 < orig.size()) ? orig[i + 1].pitch() : n.pitch();
                int interval = next_pitch - n.pitch();
                if (std::abs(interval) >= 2 && enrichAmt > 0.0)
                {
                    if (rng.next() < enrichAmt * 0.6)
                    {
                        Note passing = n;
                        int dir = interval > 0 ? 1 : -1;
                        passing.set_pitch(n.pitch() + dir);
                        passing.set_dur(n.dur() * 0.5);
                        rewritten.back().set_dur(n.dur() * 0.5);
                        rewritten.push_back(passing);
                    }
                    if (rng.next() < enrichAmt * 0.2)
                    {
                        Note octave = n;
                        octave.set_pitch(n.pitch() + (rng.next() < 0.5 ? 12 : -12));
                        rewritten.push_back(octave);
                    }
                }
            }
        }

        pat->clear_notes();
        for (const auto &n : rewritten) *pat->add_notes() = n;
    }
    return out;
}

//---------------- Rhythmic Quantization Pass ----------------
double RoundDur(double dur)
{
    const double targets[] = {1.0, 0.5, 0.25};
    double best = targets[0];
    double bestDiff = std::abs(dur - best);
    for (int i = 1; i < 3; ++i)
    {
        double diff = std::abs(dur - targets[i]);
        if (diff < bestDiff)
        {
            best = targets[i];
            bestDiff = diff;
        }
    }
    return best;
}

Piece ApplyRhythmicQuantizationPass(const Piece &in, double complexity)
{
    Piece out = in;
    double simplifyAmt = complexity < 0.5 ? (0.5 - complexity) * 2.0 : 0.0;
    double enrichAmt = complexity > 0.5 ? (complexity - 0.5) * 2.0 : 0.0;
    RNG rng;

    for (int v = 0; v < out.voices_size(); ++v)
    {
        Pattern *pat = out.mutable_voices(v)->mutable_pattern();
        std::vector<Note> orig(pat->notes().begin(), pat->notes().end());
        std::vector<Note> rewritten;
        if (orig.empty()) continue;

        if (simplifyAmt > 0.0)
        {
            double t = 0.0;
            for (size_t i = 0; i < orig.size(); ++i)
            {
                Note n = orig[i];
                n.set_dur(RoundDur(n.dur()));

                // remove syncopation occasionally
                double onset_mod = std::fmod(t, 1.0);
                t += n.dur();
                if (onset_mod > 0.01 && rng.next() < simplifyAmt * 0.5)
                    continue;

                if (!rewritten.empty() && rewritten.back().pitch() == n.pitch())
                {
                    rewritten.back().set_dur(rewritten.back().dur() + n.dur());
                }
                else
                {
                    rewritten.push_back(n);
                }
            }
        }
        else // enrich
        {
            for (size_t i = 0; i < orig.size(); ++i)
            {
                Note n = orig[i];
                rewritten.push_back(n);
                if (n.dur() > 1.0 && rng.next() < enrichAmt * 0.6)
                {
                    Note sub = n;
                    sub.set_dur(n.dur() * 0.5);
                    rewritten.back().set_dur(n.dur() * 0.5);
                    rewritten.push_back(sub);
                }
                if (rng.next() < enrichAmt * 0.3)
                {
                    Note off = n;
                    off.set_dur(std::max(0.25, n.dur() * 0.5));
                    rewritten.push_back(off);
                }
            }
        }

        pat->clear_notes();
        for (const auto &n : rewritten) *pat->add_notes() = n;
    }
    return out;
}

//---------------- Timbre Complexity Pass ----------------
Instrument_Osc SimplifyOsc(Instrument_Osc osc)
{
    switch (osc)
    {
    case Instrument_Osc_SAW: return Instrument_Osc_TRIANGLE;
    case Instrument_Osc_SQUARE: return Instrument_Osc_TRIANGLE;
    default: return Instrument_Osc_SINE;
    }
}

Instrument_Osc EnrichOsc(Instrument_Osc osc)
{
    switch (osc)
    {
    case Instrument_Osc_SINE: return Instrument_Osc_SAW;
    case Instrument_Osc_TRIANGLE: return Instrument_Osc_SAW;
    default: return osc;
    }
}

Piece ApplyTimbreComplexityPass(const Piece &in, double complexity)
{
    Piece out = in;
    double simplifyAmt = complexity < 0.5 ? (0.5 - complexity) * 2.0 : 0.0;
    double enrichAmt = complexity > 0.5 ? (complexity - 0.5) * 2.0 : 0.0;

    for (int v = 0; v < out.voices_size(); ++v)
    {
        Instrument *inst = out.mutable_voices(v)->mutable_instrument();
        Envelope *env = inst->mutable_env();
        Filter *filter = inst->mutable_filter();
        LFO *lfo = inst->mutable_lfo();

        if (simplifyAmt > 0.0)
        {
            inst->set_osc(SimplifyOsc(inst->osc()));
            inst->set_detune(inst->detune() * (1.0 - 0.7 * simplifyAmt));
            inst->set_reverb(false);
            lfo->set_depth(lfo->depth() * (1.0 - simplifyAmt));
            lfo->set_rate(lfo->rate() * (1.0 - simplifyAmt));
            env->set_attack(env->attack() + 0.05 * simplifyAmt);
            env->set_decay(env->decay() + 0.02 * simplifyAmt);
            env->set_sustain(std::min(1.0, env->sustain() + 0.1 * simplifyAmt));
            env->set_release(env->release() + 0.05 * simplifyAmt);
            if (filter->type() != Filter_Type_FILTER_NONE)
                filter->set_q(filter->q() * (1.0 - 0.5 * simplifyAmt));
        }
        else
        {
            inst->set_osc(EnrichOsc(inst->osc()));
            inst->set_detune(inst->detune() + 0.2 * enrichAmt);
            inst->set_reverb(true);
            if (lfo->target().empty()) lfo->set_target("freq");
            lfo->set_depth(std::max(lfo->depth(), 0.05 * enrichAmt));
            lfo->set_rate(std::max(lfo->rate(), 0.5 * enrichAmt));
            env->set_attack(std::max(0.01, env->attack() * (1.0 - 0.5 * enrichAmt)));
            env->set_decay(env->decay() + 0.01 * enrichAmt);
            env->set_release(env->release() * (1.0 - 0.3 * enrichAmt));
            if (filter->type() == Filter_Type_FILTER_NONE)
                filter->set_type(Filter_Type_LOWPASS);
            filter->set_q(std::max(filter->q(), 0.7 * enrichAmt));
        }
    }

    return out;
}

//---------------- Top-level ----------------
}  // namespace

Piece OptimizePiece(const Piece &in, double complexity)
{
    double c = std::clamp(complexity, 0.0, 1.0);
    Piece t1 = ApplyNoteDensityPass(in, c);
    Piece t2 = ApplyPitchSimplificationPass(t1, c);
    Piece t3 = ApplyRhythmicQuantizationPass(t2, c);
    Piece t4 = ApplyTimbreComplexityPass(t3, c);
    return t4;
}

}  // namespace dsl
