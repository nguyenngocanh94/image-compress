// inspired from http://rosettacode.org/wiki/Huffman_coding

#pragma once

#include <iostream>
#include <queue>
#include <map>
#include <climits> // for CHAR_BIT
#include <iterator>
#include <algorithm>

#define UNIQUE_SYMBOLS (1<<CHAR_BIT)

typedef std::vector<bool> HuffCode;

template<typename T>
struct HuffOutput {
    HuffCode code;
    std::map<T,HuffCode> map;
};

class INode {
public:
    const int f;
    virtual ~INode() {}
protected:
    INode(int f_init) : f(f_init) {}
};

class InternalNode : public INode {
public:
    INode *const left;
    INode *const right;
    InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode() {
        delete left;
        delete right;
    }
};

template<typename T>
class LeafNode : public INode {
public:
    const T c;
    LeafNode(int f_init, T c_init) : INode(f_init), c(c_init) {}
};

struct NodeCmp {
    bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};

template<typename T>
inline INode* BuildTree(const std::vector<int> frequencies) {
    std::priority_queue<INode*, std::vector<INode*>, NodeCmp> trees;
    for(size_t i = 0; i < frequencies.size(); ++i) {
        if(frequencies[i] != 0)
            trees.push(new LeafNode<T>(frequencies[i], (T)i));
    }
    while(trees.size() > 1) {
        INode* childR = trees.top();
        trees.pop();
        INode* childL = trees.top();
        trees.pop();
        INode* parent = new InternalNode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}

template<typename T>
inline void GenerateCodes(const INode* node, const HuffCode& prefix, std::map<T,HuffCode>& outCodes) {
    if(const LeafNode<T>* lf = dynamic_cast<const LeafNode<T>*>(node))
        outCodes[lf->c] = prefix;
    else if(const InternalNode* in = dynamic_cast<const InternalNode*>(node)) {
        HuffCode leftPrefix = prefix;
        leftPrefix.push_back(false);
        GenerateCodes(in->left, leftPrefix, outCodes);
        HuffCode rightPrefix = prefix;
        rightPrefix.push_back(true);
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}

template<typename T=int16_t>
inline HuffOutput<T> mat2huff(std::vector<T> vec) {
    static_assert(sizeof(T)<4,"Do you really have 16GB to spend?");
    std::vector<int> anSymbolFrequencies(((size_t)1)<<sizeof(T)*8,0);
    for(size_t i=0; i<vec.size(); ++i)
        ++anSymbolFrequencies[(std::make_unsigned<T>::type)vec[i]];
    INode* pRoot = BuildTree<T>(anSymbolFrequencies);
    HuffOutput<T> oOutput;
    GenerateCodes<T>(pRoot,HuffCode(),oOutput.map);
    delete pRoot;
    for(size_t i=0; i<vec.size(); ++i) {
        const HuffCode& vbCurrCode = oOutput.map[vec[i]];
        oOutput.code.insert(oOutput.code.begin(),vbCurrCode.begin(),vbCurrCode.end());
    }
    return oOutput;
}
