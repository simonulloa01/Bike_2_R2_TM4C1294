#include "decode.h"

void convert2compact(uint32_t *out, const uint8_t *in)
{
    uint32_t idx = 0;

    for (uint32_t i = 0; i < R_SIZE; i++)
    {
        for (uint32_t j = 0; j < 8ULL; j++)
        {
            if ((i * 8 + j) == R_BITS)
            {
                break;
            }

            if ((in[i] >> j) & 1)
            {
                out[idx++] = i * 8 + j;
            }
        }
    }
}

uint32_t get_predefined_threshold_var(const uint8_t s[R_BITS])
{
    // compute syndrome weight:
    uint32_t syndromeWeight = get_hamming_weight(s, R_BITS);

    // set threshold according to syndrome weight:
    uint32_t threshold = ceil(VAR_TH_FCT(syndromeWeight));

    return threshold;
}

/* $lnbino(n, t) = \ln {n \choose t}$ */
static double lnbino(size_t n, size_t t)
{
    if ((t == 0) || (n == t))
        return 0.0;
    else
        return lgamma(n + 1) - lgamma(t + 1) - lgamma(n - t + 1);
}

static double xlny(double x, double y)
{
    if (x == 0.)
        return 0.;
    else
        return x * log(y);
}

/* Log of the probability mass function of a binomial distribution:
 * $lnbinomial(n, k, p, q)) = \ln({n \choose k} p^k q^{n-k})$ */
static double lnbinomialpmf(size_t n, size_t k, double p, double q)
{
    return lnbino(n, k) + xlny(k, p) + xlny(n - k, q);
}

static double Euh_log(size_t n, size_t w, size_t t, size_t i)
{
    return lnbino(w, i) + lnbino(n - w, t - i) - lnbino(n, t);
}

/* iks = X = sum((l - 1) * E_l, l odd) */
static double iks(size_t r, size_t n, size_t w, size_t t)
{
    size_t i;
    double x;
    double denom = 0.;

    // Euh_log(n, w, t, i) decreases fast when 'i' varies.
    // For $i = 10$ it is very likely to be negligible.
    for (x = 0, i = 1; (i < 10) && (i < t); i += 2)
    {
        x += (i - 1) * exp(Euh_log(n, w, t, i));
        denom += exp(Euh_log(n, w, t, i));
    }

    if (denom == 0.)
        return 0.;
    return x / denom;
}

/* Probability for a bit of the syndrome to be zero, knowing the syndrome
 * weight 'S' and 'X' */
static double counters_C0(size_t n, size_t d, size_t w, size_t S, size_t t,
                          double x)
{
    return ((w - 1) * S - x) / (n - t) / d;
}

/* Probability for a bit of the syndrome to be non-zero, knowing the syndrome
 * weight 'S' and 'X' */
static double counters_C1(size_t n, size_t d, size_t w, size_t S, size_t t,
                          double x)
{
    return (S + x) / t / d;
}

size_t compute_threshold(size_t r, size_t n, size_t d, size_t w, size_t S,
                         size_t t)
{
    double p, q;

    double x = iks(r, n, w, t) * S;
    p = counters_C0(n, d, w, S, t, x);
    q = counters_C1(n, d, w, S, t, x);
    p = (p > 1.0) ? 1.0 : p;
    q = (q > 1.0) ? 1.0 : q;

    size_t threshold;
    if (q == 1.0 || p == 1.0 || p > q)
    {
        threshold = d;
    }
    else
    {
        threshold = d + 1;
        double diff = 0.;
        do
        {
            threshold--;
            diff = (-exp(lnbinomialpmf(d, threshold, p, 1. - p)) * (n - t) +
                    exp(lnbinomialpmf(d, threshold, q, 1. - q)) * t);
        } while (diff >= 0. && threshold > (d + 1) / 2);
        threshold = threshold < d ? (threshold + 1) : d;
    }

    return threshold;
}

void getCol(
    uint32_t *h_compact_col,
    const uint32_t *h_compact_row, uint32_t DV)
{
    if (h_compact_row[0] == 0)
    {
        h_compact_col[0] = 0;

        for (uint32_t i = 1; i < DV; i++)
        {
            // set indices in increasing order:
            h_compact_col[i] = R_BITS - h_compact_row[DV - i];
        }
    }
    else
    {
        for (uint32_t i = 0; i < DV; i++)
        {
            // set indices in increasing order:
            h_compact_col[i] = R_BITS - h_compact_row[DV - 1 - i];
        }
    }
}

// The position in e is adjusted because syndrome is transposed.
void flipAdjustedErrorPosition(uint8_t e[R_BITS * 2], uint32_t position)
{
    uint32_t adjustedPosition = position;
    if (position != 0 && position != R_BITS)
    {
        adjustedPosition = (position > R_BITS) ? ((N_BITS - position) + R_BITS) : (R_BITS - position);
    }
    e[adjustedPosition] ^= 1;
}

uint32_t ctr(
    uint32_t *h_compact_col,
    int position,
    uint8_t syndrome[R_BITS],
    const bike2_params_t *params)
{
    size_t DV = params->row_weight / 2;
    uint32_t count = 0;
    for (uint32_t i = 0; i < DV; i++)
    {
        if (syndrome[(h_compact_col[i] + position) % R_BITS])
            count++;
    }
    return count;
}

void compute_counter_of_unsat(uint8_t unsat_counter[N_BITS],
                              const uint8_t syndrome[R_BITS],
                              const uint32_t *h0_compact,
                              const uint32_t *h1_compact, const bike2_params_t *params)
{
    size_t DV = params->row_weight / 2;
    uint8_t unsat_counter2[N_BITS * 2] = {0};
    uint32_t h1_compact2[DV];

    for (uint32_t i = 0; i < DV; i++)
    {
        h1_compact2[i] = N_BITS + h1_compact[i];
    }

    for (uint32_t i = 0; i < R_BITS; i++)
    {
        if (!syndrome[i])
        {
            continue;
        }

        for (uint32_t j = 0; j < DV; j++)
        {
            unsat_counter2[h0_compact[j] + i]++;
            unsat_counter2[h1_compact2[j] + i]++;
        }
    }

    for (uint32_t i = 0; i < R_BITS; i++)
    {
        unsat_counter[i] = unsat_counter2[i] + unsat_counter2[R_BITS + i];
        unsat_counter[R_BITS + i] =
            unsat_counter2[N_BITS + i] + unsat_counter2[N_BITS + R_BITS + i];
    }
}

void recompute_syndrome(uint8_t syndrome[R_BITS],
                        const uint32_t numPositions,
                        const uint32_t positions[N_BITS],
                        const uint32_t *h0_compact,
                        const uint32_t *h1_compact,
                        const bike2_params_t *params)
{
    size_t DV = params->row_weight / 2;
    for (uint32_t i = 0; i < numPositions; i++)
    {
        uint32_t pos = positions[i];
        if (pos < R_BITS)
        {
            for (uint32_t j = 0; j < DV; j++)
            {
                if (h0_compact[j] <= pos)
                {
                    syndrome[pos - h0_compact[j]] ^= 1;
                }
                else
                {
                    syndrome[R_BITS - h0_compact[j] + pos] ^= 1;
                }
            }
        }
        else
        {
            pos = pos - R_BITS;
            for (uint32_t j = 0; j < DV; j++)
            {
                if (h1_compact[j] <= pos)
                    syndrome[pos - h1_compact[j]] ^= 1;
                else
                    syndrome[R_BITS - h1_compact[j] + pos] ^= 1;
            }
        }
    }
}

void check(
    uint8_t e[R_BITS * 2],
    uint32_t *h0_compact_col,
    uint32_t *h1_compact_col,
    uint32_t *h0_compact,
    uint32_t *h1_compact,
    uint8_t s[R_BITS],
    uint32_t *Jl,
    uint32_t sizeJl,
    int threshold,
    const bike2_params_t *params)
{
    size_t DV = params->row_weight / 2;
    for (uint32_t j = 0; j < sizeJl; j++)
    {
        uint32_t pos = Jl[j];
        if (pos < R_BITS)
        {
            uint32_t counter_unsat_pos = ctr(h0_compact_col, pos, s, params);
            if (counter_unsat_pos > (DV / 2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact, params);
            }
        }
        else
        {
            uint32_t counter_unsat_pos = ctr(h1_compact_col, pos - R_BITS, s, params);
            if (counter_unsat_pos > (DV / 2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact, params);
            }
        }
    }
}

int decode_backflip(uint8_t *e, uint8_t *syndrome, const uint8_t *h0, const uint8_t *h1, const bike2_params_t *params)

{
    int DV = PUBLIC_KEY_WEIGHT;
    uint32_t h0_compact[DV];
    uint32_t h1_compact[DV];
    size_t index = 2; // number of cyclic blocks
    size_t block_length = R_BITS;
    size_t block_weight = DV;
    size_t syndrome_stop = 0;

    // convert h0 and h1 to compact
    convert2compact(h0_compact, h0);
    convert2compact(h1_compact, h1);

    // sparse_t *Hcolumns = dec->Hcolumns;
    // sparse_t *Hrows = dec->Hrows;
    //  computing the first column of each parity-check block:
    uint32_t h0_compact_col[DV];
    uint32_t h1_compact_col[DV];
    getCol(h0_compact_col, h0_compact, DV);
    getCol(h1_compact_col, h1_compact, DV);

    // creating ring buffer data structure:
    ring_buffer_t flips = rb_alloc(2 * params->target_error);
    flips->length = 0;

    // dense_t *bits = dec->bits;
    uint8_t bits[2][R_BITS] = {0x00};

    // dense_t syndrome = dec->syndrome;
    // bit_t **counters = dec->counters;
    uint8_t unsat_counter[N_BITS] = {0};
    size_t threshold;
    int syndrome_weight = get_hamming_weight(syndrome, R_BITS);

    // dec->iter = 0;
    int iterations = 0;
    // TODO: check if max_iter needs to be 100:
    while (iterations < max_iter && syndrome_weight != syndrome_stop)
    {
        ++iterations;
        // compute_counters(index, block_length, block_weight, Hcolumns, Hrows, syndrome, counters);
        compute_counter_of_unsat(unsat_counter, syndrome, h0_compact, h1_compact, params);

        // int t = param->error_weight - dec->flips->length;
        int t = params->target_error - flips->length;
        t = (t > 0) ? t : 1;
        // size_t threshold = compute_threshold(block_length, index * block_length, block_weight, index * block_weight, dec->syndrome_weight, t);
        size_t threshold = compute_threshold(block_length, index * block_length, block_weight, index * block_weight, syndrome_weight, t);

        for (size_t k = 0; k < index; ++k)
        {
            for (size_t j = 0; j < block_length; ++j)
            {
                // if (counters[k][j] >= threshold) {
                if (unsat_counter[k * R_BITS + j] >= threshold)
                {
                    /* If the position was previously flipped, make sure to
                     * remove it from the flip queue */
                    if (bits[k][j])
                    {
                        size_t a = 0;
                        uint16_t index, position;
                        while (1)
                        {
                            rb_get(flips, a, &index, &position, NULL);
                            if (index == k && position == j)
                                break;
                            ++a;
                        };
                        rb_remove(flips, a);
                    }
                    else if (flips->length < flips->size)
                    {
                        int ttl = compute_ttl(unsat_counter[k * R_BITS + j] - threshold);

                        rb_append(flips, k, j,
                                  (iterations + ttl) % TTL_MAX);
                    }
                    // size_t counter = single_counter(param, Hcolumns[k], j, syndrome);
                    size_t counter = 0;
                    uint32_t pos = j;
                    if (k == 0)
                    {
                        counter = ctr(h0_compact_col, pos, syndrome, params);
                    }
                    else
                    {
                        counter = ctr(h1_compact_col, pos, syndrome, params);
                        pos += R_BITS;
                    }

                    // single_flip(param, syndrome, Hcolumns[k], j);
                    recompute_syndrome(syndrome, 1, &pos, h0_compact, h1_compact, params);

                    bits[k][j] ^= 1;
                    syndrome_weight += block_weight - 2 * counter;
                }
            }
        }
        /* Undo the flips that have reached their end of life */
        if (syndrome_weight != syndrome_stop)
        {
            size_t flips_len = flips->length;
            size_t deleted = 0;
            size_t current_iter = iterations % TTL_MAX;
            for (size_t i = 0; i < flips_len; ++i)
            {
                int i2 = i - deleted;
                uint16_t k, j;
                int iter_cancel;
                rb_get(flips, i2, &k, &j, &iter_cancel);
                if (iter_cancel == current_iter)
                {
                    rb_remove(flips, i2);
                    ++deleted;
                    // size_t counter = single_counter(param, Hcolumns[k], j, syndrome);
                    size_t counter = 0;
                    uint32_t pos = j;
                    if (k == 0)
                    {
                        counter = ctr(h0_compact_col, pos, syndrome, params);
                    }
                    else
                    {
                        counter = ctr(h1_compact_col, pos, syndrome, params);
                        pos += R_BITS;
                    }

                    // single_flip(param, syndrome, Hcolumns[k], j);
                    recompute_syndrome(syndrome, 1, &pos, h0_compact, h1_compact, params);

                    bits[k][j] ^= 1;
                    syndrome_weight += block_weight - 2 * counter;
                }
            }
        }
    }
    for (size_t k = 0; k < index; ++k)
    {
        for (size_t j = 0; j < block_length; ++j)
        {
            if (bits[k][j])
            {
                flipAdjustedErrorPosition(e, k * R_BITS + j);
            }
        }
    }
    return !(syndrome_weight == syndrome_stop);
}

int decode_1st_round(uint8_t *e,
                     uint8_t *s,
                     uint8_t *h0,
                     uint8_t *h1,
                     bike2_params_t *params)
{

    int DV = PUBLIC_KEY_WEIGHT;
    uint32_t h0_compact[DV];
    uint32_t h1_compact[DV];

    // convert h0 and h1 to compact
    convert2compact(h0_compact, h0);
    convert2compact(h1_compact, h1);

    // computing the first column of each parity-check block:
    uint32_t h0_compact_col[DV];
    uint32_t h1_compact_col[DV];
    getCol(h0_compact_col, h0_compact, DV);
    getCol(h1_compact_col, h1_compact, DV);

    // J: list of positions involved in more than
    // (threshold - delta) unsatisfied p.c. equations:
    uint32_t J[DELTA_BIT_FLIPPING][N_BITS] = {0};
    uint32_t sizeJ[DELTA_BIT_FLIPPING] = {0};

    // count the number of unsatisfied parity-checks:
    uint8_t unsat_counter[N_BITS] = {0};
    compute_counter_of_unsat(unsat_counter, s, h0_compact, h1_compact, params);

    // LINE 1 of One-Round Bit Flipping Algorithm:
    uint32_t threshold = get_predefined_threshold_var(s);

    // LINES 2-4 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; i < N_BITS; i++)
    {
        if (unsat_counter[i] > threshold - DELTA_BIT_FLIPPING)
        {
            uint32_t difference = threshold - MIN(threshold, unsat_counter[i]);
            J[difference][sizeJ[difference]] = i;
            sizeJ[difference]++;
        }
    }

    // LINES 5-6 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; i < sizeJ[0]; i++)
    {
        flipAdjustedErrorPosition(e, J[0][i]);
    }

    recompute_syndrome(s, sizeJ[0], J[0], h0_compact, h1_compact, params);

    // check if decoding finished:
    if (get_hamming_weight(s, R_BITS) <= 0)
    {
        return 0;
    }

    // LINES 7-10 of One-Round Bit Flipping Algorithm:
    for (uint32_t i = 0; get_hamming_weight(s, R_BITS) > S_BIT_FLIPPING && i < N_BITS; i++)
    {
        for (int l = 0; l < DELTA_BIT_FLIPPING; l++)
        {
            check(e, h0_compact_col, h1_compact_col, h0_compact, h1_compact, s,
                  J[l], sizeJ[l], DV / 2, params);
        }
    }

    // check if decoding finished:
    if (get_hamming_weight(s, R_BITS) <= 0)
    {
        return 0;
    }

    // LINES 11-12 of One-Round Bit Flipping Algorithm:
    uint32_t errorPos[R_BITS] = {0};
    int countError = 0;
    for (uint32_t i = 0; i < 2 * R_BITS; i++)
    {
        if (e[i])
        {
            uint32_t posError = i;
            if (i != 0 && i != R_BITS)
            {
                // the position in e is adjusted since syndrome is transposed
                posError = (i > R_BITS) ? ((N_BITS - i) + R_BITS) : (R_BITS - i);
            }
            errorPos[countError++] = posError;
        }
    }
    for (int j = 0; j < countError; j++)
    {
        uint32_t pos = errorPos[j];
        uint32_t counter_unsat_pos;

        if (pos < R_BITS)
        {
            counter_unsat_pos = ctr(h0_compact_col, pos, s, params);
        }
        else
        {
            counter_unsat_pos = ctr(h1_compact_col, pos - R_BITS, s, params);
        }

        if (counter_unsat_pos > (DV / 2))
        {
            flipAdjustedErrorPosition(e, pos);
            recompute_syndrome(s, 1, &pos, h0_compact, h1_compact, params);
        }
    }

    // check if decoding finished:
    if (get_hamming_weight(s, R_BITS) <= 0)
    {
        return 0;
    }

    // LINES 13-15 of One-Round Bit Flipping Algorithm:
    for (uint32_t k = 0; get_hamming_weight(s, R_BITS) > 0 && k < N_BITS; k++)
    {
        // find a random non-zero position in the syndrome:
        uint32_t i = rand() % R_BITS;
        while (!s[i])
            i = (i + 1) % R_BITS;

        int errorFound = 0;
        for (int j = 0; j < DV && !errorFound; j++)
        {
            // finding position of 1 in the i-th row:
            uint32_t pos = (h0_compact[j] + i) % R_BITS;
            int counter_unsat_pos = ctr(h0_compact_col, pos, s, params);
            if (counter_unsat_pos > (DV / 2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact, params);
                errorFound = 1;
            }
        }
        for (int j = 0; j < DV && !errorFound; j++)
        {
            // finding position of 1 in the i-th row:
            uint32_t pos = (h1_compact[j] + i) % R_BITS;
            pos += R_BITS;
            int counter_unsat_pos = ctr(h1_compact_col, pos, s, params);
            if (counter_unsat_pos > (DV / 2))
            {
                flipAdjustedErrorPosition(e, pos);
                recompute_syndrome(s, 1, &pos, h0_compact, h1_compact, params);
                errorFound = 1;
            }
        }
    }

    // check if decoding succeeded:
    if (get_hamming_weight(s, R_BITS) <= 0)
    {
        return 0;
    }

    return -1;
}

int decode(uint8_t *e, uint8_t *syndrome, const uint8_t *h0, const uint8_t *h1, const bike2_params_t *params)
{
    return decode_1st_round(e, syndrome, h0, h1, params);
    //return decode_backflip(e, syndrome, h0, h1, params);
}