//
//  Zinnia: Online hand recognition system with machine learning
//
//  $Id: feature.h 17 2009-04-05 11:40:32Z taku-ku $;
//
//  Copyright(C) 2008 Taku Kudo <taku@chasen.org>
//
#ifndef zinnia_FEATURE_H__
#define zinnia_FEATURE_H__

#include <vector>

namespace zinnia {

class Character;

struct FeatureNode
{
    int index;
    float value;
};

inline double dot(const FeatureNode *x1, const FeatureNode *x2)
{
    double sum = 0;
    while (x1->index >= 0 && x2->index >= 0) {
        if (x1->index == x2->index) {
            sum += (x1->value * x2->value);
            ++x1;
            ++x2;
        } else if (x1->index < x2->index) {
            ++x1;
        } else {
            ++x2;
        }
    }
    return sum;
}

struct Node
{
    float x;
    float y;
};

class Features
{
private:
    std::vector<FeatureNode> features_;

    struct NodePair
    {
        const Node *first;
        const Node *last;
        NodePair()
            : first(0)
            , last(0)
        {}
    };

    void makeBasicFeature(int id, const Node *first, const Node *last);
    void makeMoveFeature(int id, const Node *first, const Node *last);
    void makeVertexFeature(int id, std::vector<NodePair> *node_pairs);
    void getVertex(const Node *first,
                   const Node *last,
                   int id,
                   std::vector<NodePair> *node_pairs) const;
    void addFeature(int index, float value);

public:
    bool read(const Character &character);
    const FeatureNode *get() const { return &features_[0]; }
};
} // namespace zinnia
#endif
