#pragma once
#include <iostream>

template<typename It>
class IteratorRange
{
public:
    IteratorRange(It begin, It end) : it_beg_(begin), it_end_(end), size_(distance(begin, end)) {};
    It begin() const { return it_beg_; }
    It end() const { return it_end_; }
    int size() const { return static_cast<int>(size_); }
private:
    It it_beg_, it_end_;
    size_t size_;

};

template<typename It>
std::ostream& operator<<(std::ostream& os, const IteratorRange<It>& it)
{
    for (It i = it.begin(); i != it.end(); ++i)
    {
        os << *i;
    }
    return os;
}

template <typename It>
class Paginator {

public:
    Paginator(It begin, It end, int paginate)
    {
        int pg = distance(begin, end);
        It it_beg = begin;
        It it_end = begin;

        while (pg > 0) {

            if (pg <= paginate) {
                storage_.push_back(IteratorRange(it_beg, end));
                break;
            }
            else {
                advance(it_end, paginate);
                storage_.push_back(IteratorRange(it_beg, it_end));
                pg -= paginate;
                it_beg = it_end;
            }
        }
    }

    auto begin() const {
        return storage_.begin();
    }
    auto end() const {
        return storage_.end();
    }

private:
    vector<IteratorRange<It>> storage_;
};


template <typename Container>
const auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
