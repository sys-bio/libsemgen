//
// Created by Ciaran on 5/17/2020.
//

#ifndef LIBOMEXMETA_LIBRDFMODEL_H
#define LIBOMEXMETA_LIBRDFMODEL_H

#include "LibrdfQuery.h"
#include "LibrdfQueryResults.h"
#include "LibrdfStatement.h"
#include "LibrdfStorage.h"
#include "LibrdfStream.h"
#include "LibrdfWorld.h"
#include "librdf.h"
#include <iostream>
#include <memory>
#include <string>


namespace redland {

    /**
     * @brief std::function signature of librdf_free_model
     */
    using model_free_func = std::function<void(librdf_model *)>;

    /**
     * Instantiation of templated superclass
     */
    using RefCounted_librdf_model = RefCounted<librdf_model, model_free_func>;

    /**
     * @brief RAII abstraction around librdf_model
     */
    class LibrdfModel : public RefCounted_librdf_model {

    public:
        using RefCounted_librdf_model::RefCounted_librdf_model;

        explicit LibrdfModel(librdf_model *model);

        explicit LibrdfModel(LibrdfStorage& storage, const char *options = nullptr);

        bool operator==(const LibrdfModel &rhs) const;

        bool operator!=(const LibrdfModel &rhs) const;


        [[nodiscard]] LibrdfQueryResults query(const LibrdfQuery& query) const;

        librdf_stream* toStream();

        int size() const;

        void addStatement(librdf_statement *statement) const;

//        void freeModel();

        void removeStatement(librdf_statement *statement) const;

        void removeStatement(const LibrdfStatement &statement) const;

        librdf_storage* getStorage() const;

        int commitTransaction() const;

        int startTransaction() const;

        void *getTransactionHandle() const;

        int startTransactionWithHandle(void *handle) const;

        int getTransactionRollback() const;

        int supportsContexts() const;

        bool containsStatement(librdf_statement* statement) const;

        bool containsStatement(const LibrdfStatement &statement) const;

        void addStatement(const LibrdfStatement &statement) const;
    };
}

#endif //LIBOMEXMETA_LIBRDFMODEL_H
