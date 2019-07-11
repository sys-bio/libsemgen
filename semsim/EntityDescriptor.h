# ifndef SEMSIM_ENTITY_DESCRIPTOR_H_
# define SEMSIM_ENTITY_DESCRIPTOR_H_

# include "semsim/Preproc.h"
# include "semsim/DescriptorTerm.h"
# include "semsim/BiomodelsQualifiers.h"

# include <raptor2.h>

# include <vector>
# include <sstream>
# include <iostream>

namespace semsim {
    /**
     * An @ref EntityDescriptor describes "what" a model element is and "where" it is located in the physical world.
     */
    class SEMSIM_PUBLIC EntityDescriptor {
      public:
        typedef std::vector<DescriptorTerm> DescriptorTerms;

        /// Empty constructor
        EntityDescriptor() {}

        # if __cplusplus >= 201103L
        /**
         * Variadic move constructor.
         * This constructor allows you to create
         * a entity descriptor from a list of terms
         * passed in as arguments.
         *
         * @code{.cpp}
         * EntityDescriptor mydesc = EntityDescriptor(DescriptorTerm(),
         *                                            DescriptorTerm());
         * @endcode
         */
        // http://cpptruths.blogspot.com/2012/06/perfect-forwarding-of-parameter-groups.html
        template <class ...T>
        EntityDescriptor(T&&... args)
          : terms_(std::forward(args)...) {}
        # endif

        /// @return @p true if this descriptor is empty
        bool isEmpty() const {
          return !terms_.size();
        }

        /// Add a descriptor term to the sequence of terms
        void addTerm(const DescriptorTerm& t) {
          terms_.push_back(t);
        }

        # if __cplusplus >= 201103L
        /// Add a descriptor term to the sequence of terms
        void addTerm(DescriptorTerm&& t) {
          terms_.emplace_back(std::move(t));
        }
        # endif

        /**
         * Serialize this entity descriptor to RDF using the Raptor library.
         * This function should only be called from the @ref Entity class.
         * The RDF serialization format is chosen when initializing the
         * @c raptor_serializer, and must be done before calling this function.
         * @param sbml_base_uri   The base URI of the SBML model (usu. a relative path in a COMBINE archive).
         * @param metaid   The meta id of the entity this descriptor is attached to.
         * @param world      Raptor world object. Must be initialized prior to calling this function.
         * @param serializer Raptor serializer object. Must be initialized prior to calling this function.
         * @return the URI for this entity.
         */
        void serializeToRDF(const URI& sbml_base_uri, const std::string& metaid, raptor_world* world, raptor_serializer* serializer) const {
          unsigned int k=0;
          URI last_uri=sbml_base_uri.withFrag(metaid);
          for (DescriptorTerms::const_iterator i(terms_.begin()); i!=terms_.end(); ++i) {
            if (!i->getResource().isLocal()) {
              std::stringstream ss_this;
              ss_this << metaid << "_term" << ++k;
              URI next_uri=sbml_base_uri.withFrag(ss_this.str());
              serializeDescriptorTermToRDF(*i, last_uri, next_uri, world, serializer);
              last_uri = next_uri;
            } else {
              std::cerr << "term uri: " << i->getResource().getURI().encode() << "\n";
              serializeDescriptorTermToRDF(*i, last_uri, i->getResource().getURI(), world, serializer);
            }
          }
        }

      protected:
        void serializeDescriptorTermToRDF(
              const DescriptorTerm& term,
              const URI& linked_uri,
              const URI& term_uri,
              raptor_world* world,
              raptor_serializer* serializer) const {
          // term structural relation triple
          raptor_statement* s = raptor_new_statement(world);
          s->subject = raptor_new_term_from_uri_string(world, (const unsigned char*)linked_uri.encode().c_str());
          s->predicate = raptor_new_term_from_uri_string(world, (const unsigned char*)term.getRelation().getURI().encode().c_str());
          s->object = raptor_new_term_from_uri_string(world, (const unsigned char*)term_uri.encode().c_str());
          raptor_serializer_serialize_statement(serializer, s);
          raptor_free_statement(s);

          // term definition triple
          if (!term.getResource().isLocal()) {
            s = raptor_new_statement(world);
            s->subject = raptor_new_term_from_uri_string(world, (const unsigned char*)term_uri.encode().c_str());
            s->predicate = raptor_new_term_from_uri_string(world, (const unsigned char*)bqb::is.getURI().encode().c_str());
            s->object = raptor_new_term_from_uri_string(world, (const unsigned char*)term.getResource().getURI().encode().c_str());
            raptor_serializer_serialize_statement(serializer, s);
            raptor_free_statement(s);
          }
        }

        /// A sequence of descriptor terms joined by structural relations
        DescriptorTerms terms_;
    };
}

# endif
