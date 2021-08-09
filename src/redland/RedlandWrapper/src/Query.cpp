//
// Created by Ciaran on 4/24/2020.
//

#include "redland/Query.h"


// todo: Is laqrs a valid query language?

namespace omexmeta {

//    Query::Query(LibrdfModel &model, std::string query)
//        : model_(model.get()), query_(std::move(query)) {
//        q_ = librdf_new_query(
//                LibrdfWorld::getWorld(), (const char *) "sparql",
//                nullptr, (const unsigned char *) query_.c_str(),
//                nullptr);
//        if (!q_) {
//            std::ostringstream qerr;
//            throw NullPointerException("NullPointerException: Query::runQuery(): q_");
//        }
//        runQuery();
//    }
//
//    Query::~Query() {
//        if (!q_)
//            return;
//        int usage = getUsage();
//        librdf_free_query(q_);
//        if ((usage - 1) == 0) {
//            query_ = nullptr;
//        }
//    }
//
//
//    Query::Query(const Query &query) {
//        if (q_) {
//            librdf_free_query(q_);
//            query_ = nullptr;
//        }
//        query_ = query.get();// automatically increments the reference count
//    };
//
//    Query::Query(Query &&query) noexcept {
//        if (query) {
//            librdf_free_statement(q_);
//            q_ = nullptr;
//        }
//        q_ = query.getWithoutIncrement();
//        query.q_ = nullptr;
//    };
//
//    Query::Query(Query &query) noexcept {
//        if (q_) {
//            librdf_free_query(q_);
//            q_ = nullptr;
//        }
//        q_ = query.get();
//        query.q_ = nullptr;
//    };
//
//    Query &Query::operator=(const Query &query) {
//        // note: equality operators only test for equality between
//        // the query pointers because librdf does not support equality operators
//        // for queries
//        if (q_) {
//            librdf_free_query(q_);
//            q_ = nullptr;
//        }
//        q_ = query.get(); // increments the internal ref counter for us
//        return *this;
//    }
//
//    Query &Query::operator=(Query &&query) noexcept {
//        if (*this != query) {
//            if (q_) {
//                librdf_free_query(q_);
//                q_ = nullptr;
//            }
//            q_ = query.getWithoutIncrement();
//            query.q_ = nullptr;
//        }
//        return *this;
//    }
//
//    int Query::getUsage() {
//        return q_->usage;
//    }
//
//    librdf_query *Query::get() const {
//        q_->usage++;
//        return q_;
//    }
//
//    librdf_query *Query::getWithoutIncrement() const {
//        return q_;
//    }
//
////    void Query::freeQuery() {
////        if (query_results_ != nullptr) {
////            librdf_free_query_results(query_results_);
////            query_results_ = nullptr;
////        }
////        if (q_ != nullptr) {
////            librdf_free_query(q_);
////            q_ = nullptr;
////        }
////    }
//
//    void Query::runQuery() {
//        // When we runQuery twice, we have memory leak if we do
//        //  not free previous query results.
//        if (query_results_ != nullptr) {
//            librdf_free_query_results(query_results_);
//            query_results_ = nullptr;
//        }
//
//        query_results_ = librdf_query_execute(q_, model_);
//        if (!query_results_) {
//            throw NullPointerException("NullPointerException: Query::runQuery(): query_results_");
//        }
//    }
//
//    bool Query::isBoolean() {
//        return librdf_query_results_is_boolean(query_results_);
//    }
//
//    bool Query::isBindings() {
//        return librdf_query_results_is_bindings(query_results_);
//    }
//
//    int Query::getBoolean() {
//        return librdf_query_results_get_boolean(query_results_);
//    }
//
//    librdf_stream *Query::resultsAsLibRdfStream() {
//        return librdf_query_results_as_stream(query_results_);
//    }
//
//    //    RDF Query::resultsAsRDF() {
//    //        RDF rdf = RDF::fromString(resultsAsStr("rdfxml"), "rdfxml");
//    //        return rdf;
//    //    }
//
//    int Query::getCount() {
//        return librdf_query_results_get_count(query_results_);
//    }
//
//    std::string Query::getBindingValueByName(const std::string &name) {
//        librdf_node* n = librdf_query_results_get_binding_value_by_name(
//                query_results_, (const char *) name.c_str());
//        if (n){
//            LibrdfNode node(n);
//            std::string s = node.str();
//            return s;
//        }
//        return {};
//    }
//
//    int Query::getBindingsCount() {
//        return librdf_query_results_get_bindings_count(query_results_);
//    }
//
//    std::string Query::resultsAsStr(const std::string &output_format, std::string baseuri) const {
//        if (std::find(valid_output_formats_.begin(), valid_output_formats_.end(), output_format) ==
//            valid_output_formats_.end()) {
//            std::ostringstream err;
//            err << __FILE__ << ":" << __LINE__ << ": requested output format \"" << output_format << "\"";
//            err << "is not valid. These formats are valid: ";
//            for (auto &it : valid_output_formats_) {
//                err << it << ", ";
//            }
//            err << std::endl;
//            throw std::invalid_argument(err.str());
//        }
//        baseuri = OmexMetaUtils::prepareBaseUri(baseuri);
//        LibrdfUri uri(baseuri);
//        unsigned char *s = librdf_query_results_to_string2(
//                query_results_, output_format.c_str(),
//                nullptr, nullptr, uri.get());
//        std::string res = (const char *) s;// makes a copy
//        // the above string using \r\n for line endings. Convert to \n like any sane program should.
//        res = OmexMetaUtils::stringReplace(res, "\r\n", "\n");
//        free(s);
//        return res;
//    }
//
//
//    int Query::next() {
//        return librdf_query_results_next(query_results_);
//    }
//
//    ResultsMap Query::resultsAsMap() {
//        ResultsMap map;
//        // configure a data structure for storing the results
//        // binding name is x, y, z of sparql query for example.
//        for (int i = 0; i < getBindingsCount(); i++) {
//            std::string binding_name = getBindingsName(i);
//            map[binding_name] = std::vector<std::string>();
//        }
//
//        // iterate over bindings until no more left
//        bool done = false;
//        while (!done) {
//            for (auto &key : map) {
//                map[key.first].push_back(getBindingValueByName(key.first));
//            }
//            int failed = next();
//            if (failed) {
//                done = true;
//            }
//        }
//
//        // we rerun the query to overwrite the serializer_
//        // variable with a fresh object (since once you've
//        // hit the end you can't seem to go back).
//        // todo look more into this.
//        runQuery();
//        return map;
//    }
//
//
//    std::string Query::getBindingsName(int index) {
//        return librdf_query_results_get_binding_name(query_results_, index);
//    }
//
//    void Query::printQueryResults() {
//        auto results = resultsAsMap();
//        for (auto [name, list] : results) {
//            std::cout << name << std::endl;
//            for (auto x : list) {
//                std::cout << "\t" << x << std::endl;
//            }
//        }
//    }


}// namespace omexmeta