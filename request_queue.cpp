#include "request_queue.h"
template <typename DocumentPredicate>
vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
    auto documents = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(documents.empty());
    return documents;
}



vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}



int RequestQueue::GetNoResultRequests() const {
    return no_result_requests_;
}

void RequestQueue::AddRequest(bool has_results) {
    ++current_time_;
    requests_.push_back({has_results, current_time_});
    RemoveOldRequests();
}

void RequestQueue::RemoveOldRequests() {
    while (!requests_.empty() && current_time_ - requests_.front().timestamp >= min_in_day_) {
        if (!requests_.front().has_results) {
            --no_result_requests_;
        }
        requests_.pop_front();
    }
}
