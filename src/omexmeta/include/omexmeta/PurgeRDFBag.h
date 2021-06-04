//
// Created by Ciaran on 21/04/2021.
//

#ifndef LIBOMEXMETA_PURGERDFBAG_H
#define LIBOMEXMETA_PURGERDFBAG_H

#include "omexmeta/IRDF.h"

namespace omexmeta {

    class IRDF;

    class PurgeRDFBag {
    public:
        /**
         * @brief constructor for PurgeRDFBag
         * @param rdf* non owning pointer to RDF object.
         * @details Since this class is designed to be used
         * by the rdf*, the private member variables rdf_ will
         * always live for longer than the lifetime of this class
         */
        explicit PurgeRDFBag(IRDF *rdf);

        static std::string rdfBagQueryString() ;

        void purge();

        /**
         * @brief Remove rdf:bag entries from the current
         * rdf graph. The rdf:bag entries targeted are not
         * in a rdf container (and so no rdf:_1, rdf:_2, ..., rdf:_n)
         * which is covered in @see purgeListBagEntries
         */
        void purgeNonListBagEntries();

        /**
         * @brief Remove rdf:bag entries from the current
         * rdf graph. The rdf:bag entries targeted are
         * in a rdf container rdf:_1, rdf:_2, ..., rdf:_n
         */
        void purgeListBagEntries();

    private:
        IRDF* rdf_;
        unsigned int currentBlankId_ = 0;
    };
}// namespace omexmeta

#endif//LIBOMEXMETA_PURGERDFBAG_H
