#include <iterator>
#include "generators.h"

void generate_bubble(SortingNetworkBuilder *builder, int n) {
    for(int i = n - 1; i > 0; -- i) {
        for(int j = 0; j < i; ++j) {
            builder->addComparator(j, j + 1);
        }
    }
}

void generate_transposition(SortingNetworkBuilder *builder, int n) {
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

void generate_3smooth_comb(SortingNetworkBuilder *builder, int n) {
    for(auto ig = comb_3smooth::find(n) - 1; ig >= 0; --ig) {
        auto gap = comb_3smooth::list[ig], l = n - gap;
        for(int i = 0; i < l; ++i) {
            builder->addComparator(i, i + gap);
        }
        // builder->addSynchronizer(0, n);
        // this synchronizer makes the picture too big
    }
}

void generate_bitonic(SortingNetworkBuilder *builder, int n) {
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

void generate_odd_even(SortingNetworkBuilder *builder, int n) {
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

void generate_merge_exchange(SortingNetworkBuilder *builder, int n) {
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

void generate_pairwise(SortingNetworkBuilder *builder, int n) {
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
            builder->addSynchronizer(0, n);
        }
    }
}


namespace bose_nelson {
    void merge(SortingNetworkBuilder *builder, \
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

    void split(SortingNetworkBuilder *builder, int i, int length) {
        if(length >= 2) {
            const int right = length >> 1;
            const int left = length - right;
            split(builder, i, left);
            split(builder, i + left, right);
            builder->addSynchronizer(i, length);
            merge(builder, i, left, i + left, right);
        }
    }
}

void generate_bose_nelson(SortingNetworkBuilder *builder, int n) {
    bose_nelson::split(builder, 0, n);
}

void generate_hibbard(SortingNetworkBuilder *builder, int n) {
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

static void build_given(SortingNetworkBuilder *builder, int n, const int (*comp)[2], int nsync, const int *sync) {
    for(int i = 0; i < sync[0]; ++i) {
        if(comp[i][1] < n) {
            builder->addComparator(comp[i][0], comp[i][1]);
        }
    }
    for(int j = 0; j < nsync - 1; ++j) {
        builder->addSynchronizer(0, n);
        for(int i = sync[j]; i < sync[j+1]; ++i) {
            if(comp[i][1] < n) {
                builder->addComparator(comp[i][0], comp[i][1]);
            }
        }
    }
}

namespace optimal6 {
    static const int comp[][2] = {{0,1},{2,3},{4,5},{0,2},{3,5},{1,4},{0,1},{2,3},{4,5},{1,2},{3,4},{2,3}};
    static const int sync[] = {3,6,9,11,12};
}

namespace optimal9 {
    static const int comp_s[][2] = {
        {0,1},{3,4},{6,7},{1,2},{4,5},{7,8},{0,1},{3,4},{6,7},{2,5},{1,4},{0,3},{5,8},{2,5},{3,6},{4,7},\
        {0,3},{1,4},{5,7},{2,6},{1,3},{2,4},{5,6},{2,3},{4,5}};
    static const int sync_s[] = {3,6,10,13,16,20,23,25};
}

namespace optimal10 {
    static const int comp_s[][2] = {
        {0,5},{1,6},{2,7},{3,8},{4,9},{0,3},{1,4},{5,8},{6,9},{0,2},{3,6},{7,9},{0,1},{2,4},{5,7},{8,9},\
        {1,2},{3,5},{4,6},{7,8},{1,3},{2,5},{4,7},{6,8},{2,3},{6,7},{3,4},{5,6},{4,5}};
    static const int sync_s[] = {5,9,12,16,20,24,26,28,29};
    static const int comp_l[][2] = {
        {0,9},{1,2},{3,4},{5,6},{7,8},{1,8},{0,2},{3,5},{4,6},{7,9},{0,1},{2,6},{3,7},{4,5},{8,9},{0,3},\
        {6,9},{1,5},{4,8},{2,7},{1,2},{3,4},{5,6},{7,8},{1,3},{2,4},{5,7},{6,8},{2,3},{4,5},{6,7}};
    static const int sync_l[] = {5,10,15,20,24,28,31};
}

namespace optimal12 {
    static const int comp_c[][2] = {{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{0,2},{1,3},{4,6},{5,7},{8,10},{9,11}};
    static const int sync_c[] = {6,12};
    static const int comp_s[][2] = {
        {1,2},{5,6},{9,10},{0,4},{7,11},{1,5},{6,10},{3,7},{4,8},{2,6},{5,9},{0,4},{7,11},{3,8},{1,5},{6,10},\
        {1,4},{7,10},{2,3},{8,9},{2,4},{7,9},{3,5},{6,8},{3,4},{5,6},{7,8}};
    static const int sync_s[] = {5,9,14,16,20,24,27};
    static const int comp_l[][2] = {
        {2,6},{1,5},{0,4},{9,10},{7,11},{1,2},{6,10},{5,9},{4,8},{3,7},{2,6},{1,5},{0,4},{9,10},{7,11},{3,8},\
        {1,4},{2,3},{5,6},{7,10},{8,9},{3,5},{2,4},{6,8},{7,9},{3,4},{5,6},{7,8}};
    static const int sync_l[] = {5,10,16,21,25,28};
}

namespace optimal13 {
    static const int comp_s[][2] = {
        {0,12},{1,10},{2,11},{3,6},{4,7},{8,9},{0,1},{2,8},{3,4},{6,7},{9,11},{10,12},{0,3},{1,4},{5,11},\
        {6,10},{7,12},{4,9},{5,10},{7,8},{11,12},{2,5},{3,7},{4,6},{8,10},{9,11},{0,2},{3,4},{1,5},{8,9},\
        {10,11},{1,2},{5,7},{6,8},{1,3},{2,4},{5,6},{7,8},{9,10},{2,3},{4,5},{6,7},{8,9},{3,4},{5,6}};
    static const int sync_s[] = {6, 12, 17, 21, 26, 31, 34, 39, 43, 45};
}

namespace optimal16 {
    static const int comp_c[][2] = {
        {0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{0,2},{1,3},{4,6},{5,7},{8,10},{9,11},{12,14},{13,15},\
        {0,4},{1,5},{2,6},{3,7},{8,12},{9,13},{10,14},{11,15},{0,8},{1,9},{2,10},{3,11},{4,12},{5,13},{6,14},{7,15},\
        {1,2},{13,14},{3,12},{4,8},{7,11},{5,10},{6,9}};
    static const int sync_c[] = {8,16,24,32,39};
    static const int comp_s[][2] = {
        {1,4},{7,13},{2,8},{11,14},{2,4},{5,6},{9,10},{11,13},{3,8},{7,12},{3,5},{6,8},{7,9},{10,12},{3,4},\
        {5,6},{7,8},{9,10},{11,12},{6,7},{8,9}};
    static const int sync_s[] = {4,10,14,19,21};
    static const int comp_l[][2] = {
        {1,4},{2,8},{3,10},{5,9},{6,12},{7,13},{11,14},{2,4},{3,5},{6,8},{7,9},{10,12},{11,13},{3,6},{5,8},\
        {7,10},{9,12},{3,4},{5,6},{7,8},{9,10},{11,12}};
    static const int sync_l[] = {7,13,17,22};
}

template<typename T, int N>
inline constexpr int array_size(T (&)[N]) { return N; }

void generate_optimal_size(SortingNetworkBuilder *builder, int n) {
    switch (n) {
    case 6:
        build_given(builder, 6, optimal6::comp, array_size(optimal6::sync), optimal6::sync);
        break;
    case 9:
        build_given(builder, 9, optimal9::comp_s, array_size(optimal9::sync_s), optimal9::sync_s);
        break;
    case 10:
        build_given(builder, 10, optimal10::comp_s, array_size(optimal10::sync_s), optimal10::sync_s);
        break;
    case 11:
    case 12:
        build_given(builder, n, optimal12::comp_c, array_size(optimal12::sync_c), optimal12::sync_c);
        builder->addSynchronizer(0, n);
        build_given(builder, n, optimal12::comp_s, array_size(optimal12::sync_s), optimal12::sync_s);
        break;
    case 13:
        build_given(builder, 13, optimal13::comp_s, array_size(optimal13::sync_s), optimal13::sync_s);
        break;
    case 14:
    case 15:
    case 16:
        build_given(builder, n, optimal16::comp_c, array_size(optimal16::sync_c), optimal16::sync_c);
        builder->addSynchronizer(0, n);
        build_given(builder, n, optimal16::comp_s, array_size(optimal16::sync_s), optimal16::sync_s);
        break;
    
    default:
        generate_odd_even(builder, n);
        break;
    }
}

void generate_optimal_latency(SortingNetworkBuilder *builder, int n) {
    switch (n) {
    case 6:
        build_given(builder, 6, optimal6::comp, array_size(optimal6::sync), optimal6::sync);
        break;
    case 9:
    case 10:
        build_given(builder, n, optimal10::comp_l, array_size(optimal10::sync_l), optimal10::sync_l);
        break;
    case 11:
    case 12:
        build_given(builder, n, optimal12::comp_c, array_size(optimal12::sync_c), optimal12::sync_c);
        builder->addSynchronizer(0, n);
        build_given(builder, n, optimal12::comp_l, array_size(optimal12::sync_l), optimal12::sync_l);
        break;
    case 13:
    case 14:
    case 15:
    case 16:
        build_given(builder, n, optimal16::comp_c, array_size(optimal16::sync_c), optimal16::sync_c);
        builder->addSynchronizer(0, n);
        build_given(builder, n, optimal16::comp_l, array_size(optimal16::sync_l), optimal16::sync_l);
        break;
    
    default:
        generate_odd_even(builder, n);
        break;
    }
}


int estimate_bubble(int n, int) {
    return n * 2 + 1;
}

int estimate_transpostion(int n, int) {
    return n + 4;
}

int estimate_3smooth_comb(int n, int) {
    return n * comb_3smooth::find(n);
}

int estimate_bitonic_and_odd_even(int, int l) {
    return (1 << (l + 1)) + (l - 1) * l / 2 + l;
}

int estimate_pairwise(int, int l) {
    return (1 << (l - 1)) * l + (l - 1) * l / 2 + l + 4;
}

int estimate_bose_nelson(int, int l) {
    int x = 3;
    int i;
    for(i = 1; i * 2 <= l; i = i * 2) {
        x = x * x;
    }
    for(; i <= l; ++i) {
        x = x * 3;
    }
    return (x - 1) / 2;
}

int estimate_optimal(int n, int l) {
    if(n > 16) return -1;
    return (1 << (l - 1)) * l + (l - 1) * l / 2 + l + 4;
}

typedef void (*Generator)(SortingNetworkBuilder *builder, int n);

static const Generator generators[] = {
    generate_bubble,
    generate_transposition,
    generate_3smooth_comb,
    generate_bitonic,
    generate_odd_even,
    generate_merge_exchange,
    generate_pairwise,
    generate_bose_nelson,
    generate_hibbard,
    generate_optimal_size,
    generate_optimal_latency,
};

void generate_network(int index, int n, SortingNetworkBuilder *builder) {
    generators[index](builder, n);
}

typedef int (*Estimator)(int n, int l);

static const Estimator estimators[] = {
    estimate_bubble,
    estimate_transpostion,
    estimate_3smooth_comb,
    estimate_bitonic_and_odd_even,
    estimate_bitonic_and_odd_even,
    estimate_pairwise,
    estimate_pairwise,
    estimate_bose_nelson,
    estimate_bose_nelson,
    estimate_optimal,
    estimate_optimal
};

int estimate_columns(int index, int n) {
    return estimators[index](n, ceil_pow_2(n));
}