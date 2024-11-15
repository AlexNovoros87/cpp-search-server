
#include <algorithm>
#include <cmath>
#include <map>
#include<numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "search_server.h"


SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
 {
 }

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
        const vector<int>& ratings) {
        if (document_id < 0 ||
            documents_.count(document_id) > 0) {
            throw invalid_argument("Add document: Incorrect ID");
        }
        if (!IsAcceptQuery(document)) {
            throw invalid_argument("Add document: Incorrect input string");
        }
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();

        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
        order_ids_[document_counter_] = document_id;
        ++document_counter_;
    }

    

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {

        return FindTopDocuments(
            raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                return document_status == status;
            });
    }

    vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }


    tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {

        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }

        return tie(matched_words, documents_.at(document_id).status);
    }

    int SearchServer::GetDocumentCount() const {
        return documents_.size();
    }

    int SearchServer::GetDocumentId(int index) const {
        return order_ids_.at(index);
    }



    bool SearchServer::IsAcceptQuery(const string& query) const
    {
        if (!IsValidWord(query))return false;
        else  if (query[query.size() - 1] == '-')return false;
        else  if (query.find("- ") != query.npos || query.find("--") != query.npos) return false;
        else return true;
    }

     bool SearchServer::IsValidWord(const string& word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
            });
    }


    bool SearchServer::IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

     int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
        return rating_sum / static_cast<int>(ratings.size());
    }

    

     SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
        if (!IsAcceptQuery(text)) throw invalid_argument("Match document: Incorrect input string");
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, IsStopWord(text) };
    }


     SearchServer::Query SearchServer::ParseQuery(const string& text) const {

        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }



    
