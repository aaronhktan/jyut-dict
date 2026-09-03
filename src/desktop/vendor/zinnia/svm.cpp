//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: svm.cpp 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#include "feature.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

namespace zinnia {

namespace {
static const double kEPS = 0.1;
static const double kINF = 1e+37;
} // namespace

bool svm_train(size_t l,
               size_t n,
               const float *y,
               const FeatureNode **x,
               double C,
               double *w)
{
    size_t active_size = l;
    double PGmax_old = kINF;
    double PGmin_old = -kINF;
    std::vector<double> QD(l);
    std::vector<size_t> index(l);
    std::vector<double> alpha(l);

    std::fill(w, w + n, 0.0);
    std::fill(alpha.begin(), alpha.end(), 0.0);

    for (size_t i = 0; i < l; ++i) {
        index[i] = i;
        QD[i] = 0;
        for (const FeatureNode *f = x[i]; f->index >= 0; ++f) {
            QD[i] += (f->value * f->value);
        }
    }

    static const size_t kMaxIteration = 2000;
    for (size_t iter = 0; iter < kMaxIteration; ++iter) {
        std::random_device rd;
        std::mt19937 rng(rd());

        double PGmax_new = -kINF;
        double PGmin_new = kINF;

        std::ranges::shuffle(index.begin(), index.begin() + active_size, rng);

        for (size_t s = 0; s < active_size; ++s) {
            const size_t i = index[s];
            double G = 0;

            for (const FeatureNode *f = x[i]; f->index >= 0; ++f) {
                G += w[f->index] * f->value;
            }

            G = G * y[i] - 1;
            double PG = 0.0;

            if (alpha[i] == 0.0) {
                if (G > PGmax_old) {
                    active_size--;
                    std::swap(index[s], index[active_size]);
                    s--;
                    continue;
                } else if (G < 0.0) {
                    PG = G;
                }
            } else if (alpha[i] == C) {
                if (G < PGmin_old) {
                    active_size--;
                    std::swap(index[s], index[active_size]);
                    s--;
                    continue;
                } else if (G > 0.0) {
                    PG = G;
                }
            } else {
                PG = G;
            }

            PGmax_new = std::max(PGmax_new, PG);
            PGmin_new = std::min(PGmin_new, PG);

            if (std::abs(PG) > 1.0e-12) {
                const double alpha_old = alpha[i];
                alpha[i] = std::min(std::max(alpha[i] - G / QD[i], 0.0), C);
                const double d = (alpha[i] - alpha_old) * y[i];
                for (const FeatureNode *f = x[i]; f->index >= 0; ++f) {
                    w[f->index] += d * f->value;
                }
            }
        }

        if (iter % 4 == 0) {
            std::cout << "." << std::flush;
        }

        if ((PGmax_new - PGmin_new) <= kEPS) {
            if (active_size == l) {
                break;
            } else {
                active_size = l;
                PGmax_old = kINF;
                PGmin_old = -kINF;
                continue;
            }
        }

        PGmax_old = PGmax_new;
        PGmin_old = PGmin_new;
        if (PGmax_old <= 0) {
            PGmax_old = kINF;
        }
        if (PGmin_old >= 0) {
            PGmin_old = -kINF;
        }
    }

    std::cout << std::endl;

    return true;
}
} // namespace zinnia
