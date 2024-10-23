/*
 * PCG Random Number Generation for C++
 *
 * Copyright 2014-2022 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 *
 * Licensed under the Apache License, Version 2.0 (provided in
 * LICENSE-APACHE.txt and at http://www.apache.org/licenses/LICENSE-2.0)
 * or under the MIT license (provided in LICENSE-MIT.txt and at
 * http://opensource.org/licenses/MIT), at your option. This file may not
 * be copied, modified, or distributed except according to those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 * For additional information about the PCG random number generation scheme,
 * visit http://www.pcg-random.org/.
 */

// Bilow is a minimal subset of imneme/pcg-cpp with std-ish interface
// written by @heavywatal

#pragma once
#ifndef WTL_PCG_HPP_
#define WTL_PCG_HPP_

#include <random>
#include <limits>
#include <iosfwd>
#include <array>

namespace wtl {

namespace {

template <class T> inline
constexpr T unsigned_rotr(const T x, const unsigned s) noexcept {
    constexpr unsigned dig = std::numeric_limits<T>::digits;
    return (x >> s) | (x << ((-s) % dig));
}

constexpr unsigned floor_log2(unsigned x) {
    return x == 1 ? 0 : 1 + floor_log2(x >> 1);
}

} // namespace

class pcg32 {
  public:
    using result_type = uint32_t;
    using state_type = uint64_t;

    static constexpr result_type min() {return 0u;}
    static constexpr result_type max() {return std::numeric_limits<result_type>::max();}
    static constexpr state_type default_seed = 5573589319906701683ULL;
    static constexpr state_type default_increment = 1442695040888963407ULL;
    static constexpr state_type multiplier = 6364136223846793005ULL;

    // constructors
    pcg32() = default;
    pcg32(const pcg32&) = default;
    pcg32(pcg32&&) = default;
    explicit pcg32(state_type s) {seed(s);}
    explicit pcg32(state_type s, state_type inc) {seed(s, inc);}
    explicit pcg32(std::seed_seq& q) {seed(q);}

    void seed(state_type s) {
        state_ = s;
        state_ += increment_;
        bump();
    }
    void seed(state_type s, state_type inc) {
        increment_ = (inc << 1u) | 1u;
        seed(s);
    }
    void seed(std::seed_seq& q) {
        std::array<state_type, 2u> data;
        q.generate(data.begin(), data.end());
        seed(data[0], data[1]);
    }

    result_type operator()() {
        auto res = output();
        bump();
        return res;
    }

    void discard(unsigned long long n) {
        state_ = advance(n);
    }

  private:
    state_type increment_ = default_increment;
    state_type state_ = default_seed;

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

  public: // for pcg-test.cpp
    result_type operator()(result_type upper_bound) {
        const result_type threshold = (max() - min() - upper_bound + 1u) % upper_bound;
        while (true) {
            result_type r = (*this)() - min();
            if (r >= threshold) return r % upper_bound;
        }
    }

    void backstep(state_type delta) {
        discard(-delta);
    }

    state_type operator-(const pcg32& old) const {
        return distance(old.state_);
    }

    static constexpr unsigned period_pow2() {return std::numeric_limits<state_type>::digits;}
    static constexpr unsigned streams_pow2() {return period_pow2() - 1u;}

  private:
    result_type output() const {
        constexpr unsigned st_digits = std::numeric_limits<state_type>::digits;
        constexpr unsigned res_digits = std::numeric_limits<result_type>::digits;
        constexpr unsigned log2_res_digits = floor_log2(res_digits);
        constexpr unsigned bottom_spare = st_digits - res_digits - log2_res_digits;
        constexpr unsigned xshift = (res_digits + log2_res_digits) / 2u;
        state_type internal = state_;
        internal ^= (internal >> xshift);
        result_type result = internal >> bottom_spare;
        const unsigned rot = state_ >> (st_digits - log2_res_digits);
        return unsigned_rotr(result, rot);
    }

    state_type advance(state_type delta) const {
        state_type cur_mult = multiplier;
        state_type cur_plus = increment_;
        state_type acc_mult = 1u;
        state_type acc_plus = 0u;
        while (delta > 0u) {
            if (delta & 1u) {
                acc_mult *= cur_mult;
                acc_plus *= cur_mult;
                acc_plus += cur_plus;
            }
            cur_plus *= (cur_mult + 1u);
            cur_mult *= cur_mult;
            delta >>= 1u;
        }
        return acc_mult * state_ + acc_plus;
    }

    state_type distance(state_type cur_state) const {
        state_type cur_mult = multiplier;
        state_type cur_plus = increment_;
        state_type the_bit = 1u;
        state_type delta = 0u;
        while (state_ != cur_state) {
            if ((state_ & the_bit) != (cur_state & the_bit)) {
                cur_state *= cur_mult;
                cur_state += cur_plus;
                delta |= the_bit;
            }
            cur_plus *= (cur_mult + 1u);
            cur_mult *= cur_mult;
            the_bit <<= 1u;
        }
        return delta;
    }

    void bump() {
        state_ *= multiplier;
        state_ += increment_;
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& ost, const wtl::pcg32& x) {
        return ost << x.multiplier << " "
                   << x.increment_ << " "
                   << x.state_;
    }
};

} // namespace wtl

#endif // WTL_PCG_HPP_
