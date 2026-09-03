//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: svm.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifndef zinnia_SVM_H_
#define zinnia_SVM_H_

#include <cstddef>

namespace zinnia {
struct FeatureNode;

bool svm_train(size_t l,
               size_t n,
               const float *y,
               const FeatureNode **x,
               double C,
               double *w);
} // namespace zinnia

#endif
