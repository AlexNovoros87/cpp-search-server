
//библиотеки в алф.порядке
#include <algorithm>
#include<cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>



using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;

    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        if (!document.empty()) {
            ++document_count_;
            const vector<string> words = SplitIntoWordsNoStop(document);
            double one_word = 1. / words.size();
            for (auto word : words) {
                word_to_document_freqs_[word][document_id] += one_word;
            }
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const set<string> query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    //поля 
    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;
    int document_count_ = 0;

    //////////////////////////ВСПОМОГАТЕЛЬНЫЕ//////////////////////////////////////////////////////////////
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    bool IsInDocument(const string& value) const
    {
        return word_to_document_freqs_.count(value) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    set<string> ParseQuery(const string& text) const {
        set<string> query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query_words.insert(word);
        }
        return query_words;
    }


    vector<Document> FindAllDocuments(const set<string>& query_words) const {
        //элементы
        vector<Document> matched_documents;
        map<int, double>to_check;
        set<string>minus_words;  //минус-слова
        set<string>plus_words;   //плюс-слова


        for (const string& word : query_words)
        {
            if (!IsStopWord(word)) {
                if (word[0] == '-')
                {
                    string i = word;
                    i.erase(0, 1);
                    minus_words.insert(i); //зполняем минус слова
                }
                else if (IsInDocument(word)) { plus_words.insert(word); }
            }
        }

        //удаляем минус-слова из коллеции плюс-слов
        for (auto i : minus_words) {
            if (plus_words.count(i) > 0)plus_words.erase(i);
        }

        for (const string& word : plus_words) {
            //определяем idf не создавая отдельной фукции
            const double idf = (double)log(static_cast<double>(document_count_)
                / static_cast<double>(word_to_document_freqs_.at(word).size()));

            for (const auto& [id, sum] : word_to_document_freqs_.at(word)) {
                to_check[id] += sum * idf;
            }
        };


        for (const auto& [id, rel] : to_check) {
            matched_documents.push_back({ id,rel });
        }
        return matched_documents;
    }
};


SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
            << "relevance = "s << relevance << " }"s << endl;
    }
}