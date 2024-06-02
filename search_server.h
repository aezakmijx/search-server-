#pragma once
#include "document.h"
#include "string_processing.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>
using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;
const int inaccurasy = 1e-6;

class SearchServer {
public:

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
            : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
    {
        if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw invalid_argument("Some of stop words are invalid"s);
        }
    }

    // Конструктор, принимающий текст со стоп-словами
    explicit SearchServer(const std::string& stop_words_text);



    // Функция для создания уникальных и непустых строк из контейнера
    template<typename StringContainer>
    std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings){
        set<string> non_empty_strings;
        for (const string& str : strings) {
            if (!str.empty()) {
                non_empty_strings.insert(str);
            }
        }
        return non_empty_strings;
    }






    // Добавление документа в поисковый сервер
    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);

    // Поиск документов по запросу с предикатом

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query,
                                                    DocumentPredicate document_predicate) const{

        const auto query = ParseQuery(raw_query);


        auto matched_documents = FindAllDocuments(query, document_predicate);

        std::sort(matched_documents.begin(), matched_documents.end(),
                  [](const Document& lhs, const Document& rhs) {
                      if (std::abs(lhs.relevance - rhs.relevance) < inaccurasy) {
                          return lhs.rating > rhs.rating;
                      } else {
                          return lhs.relevance > rhs.relevance;
                      }
                  });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;

    }
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const;
    vector<Document> FindTopDocuments(const string& raw_query) const;



    // Получение количества документов в поисковом сервере
    [[nodiscard]] int GetDocumentCount() const;

    // Получение идентификатора документа по индексу
    int GetDocumentId(int index) const;

    // Сопоставление документа с запросом
    [[nodiscard]] std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

    // Разбиение текста на слова с исключением стоп-слов
    [[nodiscard]] std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;



private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::vector<int> document_ids_;
    const std::set<std::string> stop_words_;

    // Проверка, является ли слово стоп-словом
    bool IsStopWord(const std::string& word) const;

    // Валидация слова
    static bool IsValidWord(const std::string& word);


    // Вычисление среднего рейтинга документов
    static int ComputeAverageRating(const std::vector<int>& ratings);

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;

    };

    // Анализ слова запроса
    QueryWord ParseQueryWord(const std::string& text) const;

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    double ComputeWordInverseDocumentFreq(const std::string& word) const;



        // Анализ запроса
    Query ParseQuery(const std::string& text) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const Query& query,
                                                         DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto& [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                    {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }

};


