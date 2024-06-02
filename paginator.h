#pragma once
#include "document.h"
#include <iterator> 
#include <algorithm>
#include <vector> 

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
            : first_(begin), last_(end), size_(std::distance(begin, end)) {}

    Iterator begin() const {
        return first_;
    }

    Iterator end() const {
        return last_;
    }

    size_t size() const {
        return size_;
    }

private:
    Iterator first_, last_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (size_t left = std::distance(begin, end); left > 0;) {
            size_t current_page_size = std::min(page_size, left);
            Iterator current_end = std::next(begin, current_page_size);
            pages_.push_back({begin, current_end});

            left -= current_page_size;
            begin = current_end;
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    size_t size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    using Iterator = typename Container::const_iterator;
    return Paginator<Iterator>(begin(c), end(c), page_size);
}

void PrintDocument(const Document& document) {
    std::cout << "{ "
              << "document_id = " << document.id << ", "
              << "relevance = " << document.relevance << ", "
              << "rating = " << document.rating << " }" << std::endl;
}

template <typename Iterator>
std::ostream& operator<<(std::ostream& out, const IteratorRange<Iterator>& range) {
    for (const auto& document : range) {
        out << document;
    }
    return out;
}
