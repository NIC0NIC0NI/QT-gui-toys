#include <iterator>
#include "generators.h"

namespace sorting_network {
namespace algorithm {

void generate_bubble(Builder *builder, int n) {
    for(int i = n - 1; i > 0; -- i) {
        for(int j = 0; j < i; ++j) {
            builder->addComparator(j, j + 1);
        }
    }
}

void generate_odd_even_transposition(Builder *builder, int n) {
    for(int i = 0; i < n; ++i) {
        for(int j = i & 1; j < n - 1; j += 2) {
            builder->addComparator(j, j + 1);
        }
    }
}

namespace comb_3smooth {
    static const int list[] = {1, 2, 3, 4, 6, 8, 9, 12, 16, 18, 24, 27, 32, \
        36, 48, 54, 64, 72, 81, 96, 108, 128};

    static int find(int n) {
        auto b = std::begin(list), e = std::end(list);
        return std::lower_bound(b, e, n) - b;
    }
}

void generate_3smooth_comb(Builder *builder, int n) {
    for(auto ig = comb_3smooth::find(n) - 1; ig >= 0; --ig) {
        auto gap = comb_3smooth::list[ig], l = n - gap;
        for(int i = 0; i < l; ++i) {
            builder->addComparator(i, i + gap);
        }
    }
}

void generate_bitonic(Builder *builder, int n) {
    const int l = ceil_pow_2(n), nfull = 1 << l, res = (nfull - n) >> 1, nres = n + res;
    // truncate from both sides

    for(int d_1 = 0; d_1 < l; ++d_1) {
        const int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = length_1 * 2 - j - 1;
            const int left = j - res, right = partner - res;
            int i;
            for(i = 0; i < - left; i += length_1 * 2);
            for(; i < nres - partner; i += length_1 * 2) {
                builder->addComparator(i + left, i + right);
            }
        }
        builder->addSynchronizer(0, n);
        for(int d_2 = d_1 - 1; d_2 >= 0; --d_2) {
            const int length_2 = 1 << d_2;
            for(int j = 0; j < length_2; ++j) {
                const int partner = j + length_2;
                const int left = j - res, right = partner - res;
                int i;
                for(i = 0; i < - left; i += length_2 * 2);
                for(; i < nres - partner; i += length_2 * 2) {
                    builder->addComparator(i + left, i + right);
                }
            }
            builder->addSynchronizer(0, n);
        }
    }
}

void generate_odd_even_merge(Builder *builder, int n) {
    const int l = ceil_pow_2(n);

    for(int d_1 = 0; d_1 < l; ++d_1) {
        const int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = j + length_1;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                builder->addComparator(i + j, i + partner);
            }
        }
        builder->addSynchronizer(0, n);
        for(int d_2 = d_1 - 1; d_2 >= 0; --d_2) {
            const int length_2 = 1 << d_2;
            for(int k = 0; k < length_2; ++k) {
                const int partner = k + length_2;
                for(int j = 0; j < n; j += length_1 * 2) {
                    const int end = std::min(n, j + length_1 * 2);
                    for(int i = j + length_2; i < end - partner; i += length_2 * 2) {
                        builder->addComparator(i + k, i + partner);
                    }
                }
            }
            builder->addSynchronizer(0, n);
        }
    }
}

void generate_merge_exchange(Builder *builder, int n) {
    const int l = ceil_pow_2(n);

    for(int p = 1 << (l - 1); p > 0; p >>= 1) {
        int q = 1 << (l - 1);
        int r = 0;
        int d = p;
        while(d > 0) {
            for(int i = 0; i < n - d; ++i) {
                if((i & p) == r){
                    builder->addComparator(i, i + d);
                }
            }
            builder->addSynchronizer(0, n);
            d = q - p;
            q >>= 1;
            r = p;
        }
    }
}

void generate_pairwise(Builder *builder, int n) {
    const int l = ceil_pow_2(n);

    for(int d_1 = 0; d_1 < l; ++d_1) {
        const int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = j + length_1;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                builder->addComparator(i + j, i + partner);
            }
        }
    }
        
    for(int d_1 = l - 2; d_1 >= 0; --d_1) {
        const int length_1 = 1 << d_1;
        for(int d_2 = l - 1; d_2 > d_1; --d_2) {
            const int length_2 = 1 << d_2;
            const int leap = length_2 - length_1;

            for(int k = length_1; k < length_2; k += length_1 * 2) {
                for(int j = 0; j < length_1; ++j) {
                    for(int i = k + j; i < n - leap; i += length_2) {
                        builder->addComparator(i, i + leap);
                    }
                }
            }
        }
    }
}


namespace bose_nelson {
    void merge(Builder *builder, \
                        int i, int length_i, int j, int length_j) {
        if(length_i == 1 && length_j == 1) {
            builder->addComparator(i, j);
        }
        else if (length_i == 1 && length_j == 2) {
            builder->addComparator(i, j + 1);
            builder->addComparator(i, j);
        }
        else if (length_i == 2 && length_j == 1) {
            builder->addComparator(i, j);
            builder->addComparator(i + 1, j);
        }
        else {
            const int i_mid = length_i >> 1;
            const int j_mid = (length_i & 1) ? (length_j >> 1): ((length_j + 1) >> 1);
            merge(builder, i, i_mid, j, j_mid);
            merge(builder, i + i_mid, length_i - i_mid, j + j_mid, length_j - j_mid);
            merge(builder, i + i_mid, length_i - i_mid, j, j_mid);
        }
    }

    void split(Builder *builder, int i, int length) {
        if(length >= 2) {
            const int right = length >> 1;
            const int left = length - right;
            split(builder, i, left);
            split(builder, i + left, right);
            merge(builder, i, left, i + left, right);
        }
    }
}

void generate_bose_nelson(Builder *builder, int n) {
    bose_nelson::split(builder, 0, n);
}

void generate_hibbard(Builder *builder, int n) {
    int x = 0, y = 1, lastbit = 1 << ceil_pow_2(n - 1);
start:
    for(;;) {
        int bit = 1;
        int xbit = x & bit;
        int ybit = y & bit;
        builder->addComparator(x, y);
            
        while(xbit != 0 && ybit == 0) { /* two */
            x &= ~bit;
            bit <<= 1;
            xbit = x & bit;
            ybit = y & bit;
        }
        if(ybit == 0) { /* zero */
            x |= bit;
            y |= bit;
            if(y > n - 1) {
                y &= ~bit;
            }
            continue;
        }
        if(xbit != 0) { /* one */
            y &= ~bit;
            continue;
        }
        do {  /* first_two */
            if(bit == lastbit) {
                return;
            }
            x &= ~bit;
            y &= ~bit;
            bit <<= 1;
            if(y & bit) {
                x &= ~bit;
                goto start;
            }
            x |= bit;
            y |= bit;
        } while(y > n - 1);

        if(y < n - 1) {
            bit = 1;
        }
        x &= ~bit;
        y |= bit;
    }
}


static void build_given(Builder *builder, int n, int ncomp, const int (*comp)[2]) {
    for(int i = 0; i < ncomp; ++i) {
        if(comp[i][1] < n) {
            builder->addComparator(comp[i][0], comp[i][1]);
        }
    }
}

namespace optimal6 {
    static const int comp[][2] = {{0,1},{2,3},{4,5},{0,2},{3,5},{1,4},{0,1},{2,3},{4,5},{1,2},{3,4},{2,3}};
}

namespace optimal9 {
    static const int comp_s[][2] = {
        {0,1},{3,4},{6,7},{1,2},{4,5},{7,8},{0,1},{3,4},{6,7},{2,5},{1,4},{0,3},{5,8},{2,5},{3,6},{4,7},\
        {0,3},{1,4},{5,7},{2,6},{1,3},{2,4},{5,6},{2,3},{4,5}};
}

namespace optimal10 {
    static const int comp_s[][2] = {
        {0,5},{1,6},{2,7},{3,8},{4,9},{0,3},{1,4},{5,8},{6,9},{0,2},{3,6},{7,9},{0,1},{2,4},{5,7},{8,9},\
        {1,2},{3,5},{4,6},{7,8},{1,3},{2,5},{4,7},{6,8},{2,3},{6,7},{3,4},{5,6},{4,5}};
    static const int comp_l[][2] = {
        {0,9},{1,2},{3,4},{5,6},{7,8},{1,8},{0,2},{3,5},{4,6},{7,9},{0,1},{2,6},{3,7},{4,5},{8,9},{0,3},\
        {6,9},{1,5},{4,8},{2,7},{1,2},{3,4},{5,6},{7,8},{1,3},{2,4},{5,7},{6,8},{2,3},{4,5},{6,7}};
}

namespace optimal12 {
    static const int comp_c[][2] = {{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{0,2},{1,3},{4,6},{5,7},{8,10},\
        {9,11},{9,10},{0,4},{7,11}};
    static const int comp_s[][2] = {
        {1,2},{5,6},{1,5},{6,10},{3,7},{4,8},{2,6},{5,9},{0,4},{7,11},{3,8},{1,5},{6,10},\
        {1,4},{7,10},{2,3},{8,9},{2,4},{7,9},{3,5},{6,8},{3,4},{5,6},{7,8}};
    static const int comp_l[][2] = {
        {2,6},{1,5},{1,2},{6,10},{5,9},{4,8},{3,7},{2,6},{1,5},{0,4},{9,10},{7,11},{3,8},\
        {1,4},{2,3},{5,6},{7,10},{8,9},{3,5},{2,4},{6,8},{7,9},{3,4},{5,6},{7,8}};
}

namespace optimal13 {
    static const int comp_s[][2] = {
        {0,12},{1,10},{2,11},{3,6},{4,7},{8,9},{0,1},{2,8},{3,4},{6,7},{9,11},{10,12},{0,3},{1,4},{5,11},\
        {6,10},{7,12},{4,9},{5,10},{7,8},{11,12},{2,5},{3,7},{4,6},{8,10},{9,11},{0,2},{3,4},{1,5},{8,9},\
        {10,11},{1,2},{5,7},{6,8},{1,3},{2,4},{5,6},{7,8},{9,10},{2,3},{4,5},{6,7},{8,9},{3,4},{5,6}};
}

namespace optimal16 {
    static const int comp_c[][2] = {{1,2},{13,14},{3,12},{4,8},{7,11},{5,10},{6,9},{1,4},{2,8},{7,13},{11,14}};
    static const int comp_s[][2] = {
        {2,4},{5,6},{9,10},{11,13},{3,8},{7,12},{3,5},{6,8},{7,9},{10,12},{3,4},\
        {5,6},{7,8},{9,10},{11,12},{6,7},{8,9}};
    static const int comp_l[][2] = {
        {3,10},{5,9},{6,12},{2,4},{3,5},{6,8},{7,9},{10,12},{11,13},{3,6},{5,8},\
        {7,10},{9,12},{3,4},{5,6},{7,8},{9,10},{11,12}};

    static void generate_common(Builder *builder, int n) {
        for(int d_1 = 0; d_1 < 4; ++d_1) {
            const int length_1 = 1 << d_1;
            for(int j = 0; j < length_1; ++j) {
                const int partner = j + length_1;
                for(int i = 0; i < n - partner; i += length_1 * 2) {
                    builder->addComparator(i + j, i + partner);
                }
            }
        }
    }
}

template<typename T, int N>
inline constexpr int array_size(T (&)[N]) { return N; }

void generate_optimal_size(Builder *builder, int n) {
    switch (n) {
    case 6:
        build_given(builder, 6, array_size(optimal6::comp), optimal6::comp);
        break;
    case 9:
        build_given(builder, 9, array_size(optimal9::comp_s), optimal9::comp_s);
        break;
    case 10:
        build_given(builder, 10, array_size(optimal10::comp_s), optimal10::comp_s);
        break;
    case 11:
    case 12:
        build_given(builder, n, array_size(optimal12::comp_c), optimal12::comp_c);
        build_given(builder, n, array_size(optimal12::comp_s), optimal12::comp_s);
        break;
    case 13:
        build_given(builder, 13, array_size(optimal13::comp_s), optimal13::comp_s);
        break;
    case 14:
    case 15:
    case 16:
        optimal16::generate_common(builder, n);
        build_given(builder, n, array_size(optimal16::comp_c), optimal16::comp_c);
        build_given(builder, n, array_size(optimal16::comp_s), optimal16::comp_s);
        break;
    default:
        generate_odd_even_merge(builder, n);
        break;
    }
}

void generate_optimal_latency(Builder *builder, int n) {
    switch (n) {
    case 6:
        build_given(builder, 6, array_size(optimal6::comp), optimal6::comp);
        break;
    case 9:
    case 10:
        build_given(builder, n, array_size(optimal10::comp_l), optimal10::comp_l);
        break;
    case 11:
    case 12:
        build_given(builder, n, array_size(optimal12::comp_c), optimal12::comp_c);
        build_given(builder, n, array_size(optimal12::comp_l), optimal12::comp_l);
        break;
    case 13:
    case 14:
    case 15:
    case 16:
        optimal16::generate_common(builder, n);
        build_given(builder, n, array_size(optimal16::comp_c), optimal16::comp_c);
        build_given(builder, n, array_size(optimal16::comp_l), optimal16::comp_l);
        break;
    default:
        generate_odd_even_merge(builder, n);
        break;
    }
}

typedef void (*Generator)(Builder *builder, int n);

static const Generator generators[] = {
    generate_bubble,
    generate_odd_even_transposition,
    generate_3smooth_comb,
    generate_bitonic,
    generate_odd_even_merge,
    generate_merge_exchange,
    generate_pairwise,
    generate_bose_nelson,
    generate_hibbard,
    generate_optimal_size,
    generate_optimal_latency,
};

void generate_network(int index, int n, Builder *builder) {
    generators[index](builder, n);
}

// we only know optimal networks for 0 ~ 16
int maximum_n(int index) {
    return (index >= 9) ? 16 : SORTING_NETWORK_MAX_INPUT;
}

}
}
