#pragma once

#include "search_server.h"
#include<deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    vector<Document> AddFindRequest(const string& raw_query, DocumentPredicate document_predicate) {
        vector<Document> result = (*search_s).FindTopDocuments(raw_query, document_predicate);
        if (result.empty()) ++empty_;

        if (requests_.size() < min_in_day_) {
            requests_.push_back({ IsEmptyRequest(result), raw_query });
        }
        else {
            if (requests_.front().is_empty_request == true) --empty_;
            requests_.pop_front();
            requests_.push_back({ IsEmptyRequest(result), raw_query });
        }
        return result;
    }

    vector<Document> AddFindRequest(const string& raw_query, DocumentStatus status);
    vector<Document> AddFindRequest(const string& raw_query);
    int GetNoResultRequests() const;

private:

    struct QueryResult {
        bool is_empty_request;
        string query;
    };
    deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer* search_s;
    int empty_;
    bool IsEmptyRequest(const vector<Document>& vctr) const;

};
