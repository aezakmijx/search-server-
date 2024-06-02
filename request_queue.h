#pragma once
#include <deque>
#include "search_server.h"
using namespace std;

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server)
            : search_server_(search_server) {}

    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate);
    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status) {
        return AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
    }    vector<Document> AddFindRequest(const string &raw_query);


    int GetNoResultRequests() const;

private:
    const SearchServer& search_server_;
    int no_result_requests_ = 0;
    static constexpr int min_in_day_ = 1440;
    int current_time_ = 0;

    void AddRequest(bool has_results);
    void RemoveOldRequests();

    struct QueryResult {
        bool has_results;
        int timestamp;
    };
    deque<QueryResult> requests_;

};
