#include "request_queue.h"


RequestQueue::RequestQueue(const SearchServer& search_server) {
        search_s = &search_server;
        empty_ = 0;
    }
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        return AddFindRequest(
            raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                return document_status == status;
            });
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
    }

    int RequestQueue::GetNoResultRequests() const {
        return empty_;
    }

    bool RequestQueue::IsEmptyRequest(const vector<Document>& vctr) const{
        return vctr.empty();
    }

