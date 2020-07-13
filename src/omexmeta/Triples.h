//
// Created by Ciaran on 4/29/2020.
//

#ifndef LIBOMEXMETA_TRIPLES_H
#define LIBOMEXMETA_TRIPLES_H

#include "omexmeta/Triple.h"
#include "omexmeta/Predicate.h"
#include "omexmeta/Subject.h"
#include "omexmeta/Resource.h"
#include "RedlandAPIWrapper.h"
#include "omexmeta/Error.h"
#include <vector>

#include "filesystem"

using namespace redland;

namespace omexmeta {

//    class Triple;

    typedef std::vector<std::shared_ptr<Triple>> SharedTripleVector;
    typedef std::vector<Triple> TripleVector;

    class Triples {
    private:

        TripleVector triples_;

    public:
        Triples();

        /*
         * @brief create an empty Triples object big enough
         * for @param size Triple objects
         */
        explicit Triples(int size);

        /*
         * @brief construct a Triples object from a single triple
         * @param triple The triple to put into triples
         * @details The triple is moved into element 0 of a new
         * Triples object. The Triple must be passed by reference
         * so that the triple is moved directly, instead of copied
         * and then moved into the Triples object (which is a memory leak).
         */
        explicit Triples(Triple &triple);

        explicit Triples(std::vector<Triple> triples);

        bool operator==(const Triples &rhs) const;

        bool operator!=(const Triples &rhs) const;

        void move_back(Triple &triple);

        void emplace_back(Subject subject, const PredicatePtr &predicatePtr, const Resource &resource);

        void emplace_back(Subject subject, const Predicate &predicate, const Resource &resource);

        void emplace_back(Subject subject, BiomodelsBiologyQualifier predicate, const Resource &resource);

        void emplace_back(Subject subject, BiomodelsModelQualifier predicate, const Resource &resource);

        void emplace_back(Subject subject, DCTerm predicate, const Resource &resource);

        void emplace_back(Subject subject, SemSim predicate, const Resource &resource);

        void emplace_back(librdf_node *subject, librdf_node *predicate, librdf_node *resource);

        std::vector<std::string> getSubjectsStr();

        std::vector<std::string> getPredicates();

        std::vector<std::string> getResources();

        int size() const;

        TripleVector::iterator begin();

        TripleVector::iterator end();

        std::string str(
                const std::string &format = "rdfxml-abbrev",
                std::string base = (std::filesystem::current_path() /= "annotations.rdf").string(),
                std::string omex_name = "NewOmex.omex", std::string model_name = "NewModel.xml#"
                );

        void freeTriples();

        /*
         * @brief moves one triple of the end of triples
         * @returns the triple from the end of the triples vector
         * @details No copies are made. This is deliberate as it reduces
         * risk of memory issues. When you pop, you remove from the end of
         * the Triples object and return it. This means that responsibility
         * for freeing the returned Triple switches to the caller.
         */
        Triple pop();

        /*
         * @brief getter operator for Triples object
         * @return a reference to the Triple at index `index`
         * @details A copy is made of the Triple you want to return.
         * To get a reference without copying see Triples::pop(). Remember
         * that iterating over a Triples object and making copies
         * will cause complications with freeing the objects.
         */
        const Triple &operator[](int index) const;

        /*
         * @brief indicates whether Triples object is empty.
         * @return true if the Triples object doesn't contain any Triple objects
         * @details Useful for iterating over Triples in a while loop
         * without using the `[]` operator, which makes unwanted copies
         * that causes memory issues.
         */
        bool isEmpty();

        Triple pop_front();

        int capacity();

        void freeTriplesAndUris();

        void freeTriples2();
    };

}

#endif //LIBOMEXMETA_TRIPLES_H
