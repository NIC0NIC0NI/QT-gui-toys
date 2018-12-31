#include <algorithm>
#include "generators.h"

inline int ceil_pow_2(int x) {
    int i;
    for(i = 0; (1 << i) < x; ++i);
    return i;
}

void generate_bubble(SortingNetworkBuilder *builder, int n) {
    for(int i = n - 1; i > 0; -- i) {
        for(int j = 0; j < i; ++j) {
            builder->addComparator(j, j + 1);
        }
    }
}

namespace bitonic {
    void merge(SortingNetworkBuilder *builder, int i, int length) {
        if(length > 1) {
            const int right = length >> 1;
            const int left = length - right;
            for(int j = 0; j < right; ++j) {
                builder->addComparator(i + j, i + j + left);
            }
            builder->addSynchronizer(i, length);
            merge(builder, i, left);
            merge(builder, i + left, right);
        }
    }

    void split(SortingNetworkBuilder *builder, int i, int length) {
        if(length > 1) {
            const int right = length >> 1;
            const int left = length - right;
            split(builder, i, left);
            split(builder, i + left, right);
            builder->addSynchronizer(i, length);
            for(int j = 0; j < right; ++j) {
                builder->addComparator(i + j, i + length - j - 1);
            }
            builder->addSynchronizer(i, length);
            merge(builder, i, left);
            merge(builder, i + left, right);
        }
    }
}

void generate_bitonic_top_down(SortingNetworkBuilder *builder, int n) {
    bitonic::split(builder, 0, n);
}

void generate_bitonic_bottom_up(SortingNetworkBuilder *builder, int n) {
    const int l = ceil_pow_2(n);

    for(int d_1 = 0; d_1 < l; ++d_1) {
        const int length_1 = 1 << d_1;
        for(int j = 0; j < length_1; ++j) {
            const int partner = length_1 * 2 - j - 1;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                builder->addComparator(i + j, i + partner);
            }
        }
        builder->addSynchronizer(0, n);
        for(int d_2 = d_1 - 1; d_2 >= 0; --d_2) {
            const int length_2 = 1 << d_2;
            for(int j = 0; j < length_2; ++j) {
                const int partner = j + length_2;
                for(int i = 0; i < n - partner; i += length_2 * 2) {
                    builder->addComparator(i + j, i + partner);
                }
            }
            builder->addSynchronizer(0, n);
        }
    }
}

namespace odd_even {
    void merge(SortingNetworkBuilder *builder, int i, int length, int r) {
        const int m = r << 1;
        if (m < length) {
            merge(builder, i, length, m);
            merge(builder, i + r, length - r, m);
            builder->addSynchronizer(i, length);
            for(int j = i + r; j + r < i + length; j += m) {
                builder->addComparator(j, j + r);
            }
        } else if(r < length) {
            builder->addComparator(i, i + r);
        }
    }
/*
    void merge(SortingNetworkBuilder *builder, int begin, int n) {
        int l = ceil_pow_2(n);
        int length_1 = 1 << (l - 1);
        for(int j = 0; j < length_1; ++j) {
            const int partner = j + length_1;
            for(int i = 0; i < n - partner; i += length_1 * 2) {
                builder->addComparator(begin + i + j, begin + i + partner);
            }
        }
        for(int d_2 = l - 1; d_2 >= 0; --d_2) {
            int length_2 = 1 << d_2;
            for(int k = 0; k < length_2; ++k) {
                const int partner = k + length_2;
                for(int j = 0; j < n; j += length_1 * 2) {
                    const int end = std::min(n, j + length_1 * 2);
                    for(int i = j + length_2; i < end - partner; i += length_2 * 2) {
                        builder->addComparator(begin + i + k, begin + i + partner);
                    }
                }
            }
        }
    }*/

    void split(SortingNetworkBuilder *builder, int i, int length) {
        if (length > 1) {
            const int right = length >> 1;
            const int left = length - right;
            split(builder, i, left);
            split(builder, i + left, right);
            builder->addSynchronizer(i, length);
            merge(builder, i, length, 1);
        }
    }
}

void generate_odd_even_top_down(SortingNetworkBuilder *builder, int n) {
    odd_even::split(builder, 0, n);
}

void generate_odd_even_bottom_up(SortingNetworkBuilder *builder, int n) {
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

void generate_pairwise_bottom_up(SortingNetworkBuilder *builder, int n) {
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

namespace hibbard {
    void generate(SortingNetworkBuilder *builder, int n, int lastbit, int x, int y) {
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
}

void generate_hibbard(SortingNetworkBuilder *builder, int n) {
    hibbard::generate(builder, n, 1 << ceil_pow_2(n - 1), 0, 1);
}

void generate_balanced(SortingNetworkBuilder *builder, int n) {
    const int l = ceil_pow_2(n);
    for(int k = 0; k < l; ++k) {
        for(int curr = 1 << l; curr > 1; curr >>= 1) {
            for(int i = 0; i < (1 << l); i += curr) {
                for(int j = std::max(0, i + curr - n); j < curr >> 1; ++j) {
                    builder->addComparator(i + j, i + curr - j - 1);
                }
            }
            builder->addSynchronizer(0, n);
        }
    }
}


int estimate_bubble(int n, int) {
    return n * 2 + 1;
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

int estimate_balanced(int, int l) {
    return ((1 << l) - 1) * l + 4;
}

typedef void (*Generator)(SortingNetworkBuilder *builder, int n);

static const Generator generators[] = {
    generate_bubble,
    generate_bitonic_bottom_up,
    generate_bitonic_top_down,
    generate_odd_even_bottom_up,
    generate_odd_even_top_down,
    generate_merge_exchange,
    generate_pairwise_bottom_up,
    generate_bose_nelson,
    generate_hibbard,
    generate_balanced
};

void generate_network(int index, int n, SortingNetworkBuilder *builder) {
    generators[index](builder, n);
}

typedef int (*Estimator)(int n, int l);

static const Estimator estimators[] = {
    estimate_bubble,
    estimate_bitonic_and_odd_even,
    estimate_bitonic_and_odd_even,
    estimate_bitonic_and_odd_even,
    estimate_bitonic_and_odd_even,
    estimate_pairwise,
    estimate_pairwise,
    estimate_bose_nelson,
    estimate_bose_nelson,
    estimate_balanced
};

int estimate_columns(int index, int n) {
    return estimators[index](n, ceil_pow_2(n));
}